#define _USE_MATH_DEFINES  // For M_PI on Windows
#define _DEFAULT_SOURCE    // For M_PI on Unix
#include <math.h>

#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <direct.h>

#include <CL/cl.h>

#include <config.h>
#include <color.h>
#include <canvas.h>
#include <matrix.h>
#include <ray.h>
#include <lighting.h>

// Config
void log_line(char *msg) {
    time_t raw_time = time(NULL);
    struct tm *utc_time = gmtime(&raw_time);
    printf("[%i:%i:%i] %s\n", 
        utc_time->tm_hour,
        utc_time->tm_min,
        utc_time->tm_sec,
        msg
    );
}

#define CANVAS_WIDTH 1000
#define CANVAS_HEIGHT 800
#define NUM_SPHERES 2

const cl_uint MAX_PLATFORMS = 8;
const cl_uint MAX_PLATFORM_NAME_LEN = 32;
const cl_uint MAX_DEVICES = 8;

cl_context create_context() {
    // Select an OpenCL platform to run on. For now, just use the default.
    cl_platform_id first_platform_id;
    cl_uint num_platforms;
    cl_int err = clGetPlatformIDs(1, &first_platform_id, &num_platforms);
    if (err != CL_SUCCESS || num_platforms <= 0) {
        printf_s("Failed to find any OpenCL platforms.\n");
        return NULL;
    }

    // Create an OpenCL context on the platform.
    // Attempt to create a GPU-based context. If that fails, try to create a CPU-based context.
    cl_context_properties contextProperties[] = {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties)first_platform_id,
        0
    };

    cl_context context = clCreateContextFromType(
        contextProperties,
        CL_DEVICE_TYPE_GPU,
        NULL,
        NULL,
        &err
    );

    if (err != CL_SUCCESS) {
        printf("Could not create GPU context. Trying CPU...\n");
        context = clCreateContextFromType(
            contextProperties,
            CL_DEVICE_TYPE_CPU,
            NULL,
            NULL,
            &err
        );
        if (err != CL_SUCCESS) {
            fprintf(stderr, "Failed to create an OpenCL GPU or CPU context.\n");
            return NULL;
        }
    }

    return context;
}

/// @brief Selects a device and creates a command queue on it.
cl_command_queue create_command_queue(cl_context context, cl_device_id *device) {
    size_t device_buffer_size;
    cl_int err = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &device_buffer_size);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Failed call to clGetContextInfo(...,GL_CONTEXT_DEVICES,...)\n");
        return NULL;
    }
    if (device_buffer_size <= 0) {
        fprintf(stderr, "No devices available.\n");
        return NULL;
    }

    // Allocate memory for the devices buffer
    cl_device_id *devices = malloc(device_buffer_size * sizeof(cl_device_id));
    err = clGetContextInfo(context, CL_CONTEXT_DEVICES, device_buffer_size, devices, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Failed to get device IDs.\n");
        return NULL;
    }

    // For now, just use the first available device.
    // If we want this to run on a cluster or something, modify to use all available GPUs.
    cl_command_queue command_queue = clCreateCommandQueue(context, devices[0], 0, NULL);
    if (command_queue == NULL) {
        fprintf(stderr, "Failed to create commandQueue for device 0\n");
        return NULL;
    }

    *device = devices[0];
    free(devices);
    return command_queue;
}

cl_program create_program(cl_context context, cl_device_id device, const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Failed to open file %s", filename);
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);
    char *buffer = malloc(size + 1);

    size_t bytes_read = fread(buffer, 1, size, fp);
    buffer[size] = '\0';
    if (bytes_read != (size_t)size) {
        fprintf(stderr, "Short read: expected %ld bytes, got %zu\n", size, bytes_read);
        free(buffer);
        fclose(fp);
        return NULL;
    }
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&buffer, NULL, NULL);

    if (program == NULL) {
        fprintf(stderr, "Failed to create CL program from source.\n");
        return NULL;
    }

    cl_int err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        // Determine the reason for the error
        char build_log[16384];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(build_log), build_log, NULL);
        fprintf(stderr, "Error in kernel: \n");
        fprintf(stderr, "%s\n", build_log);
        clReleaseProgram(program);
        return NULL;
    }
    return program;
}

int create_mem_objects(cl_context context, cl_mem mem_objects[4], int *num_circles, float *xs, float *ys, float *rs, int *img_width) {
    mem_objects[0] = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(int),
        num_circles,
        NULL
    );

    // Create buffer for xs
    mem_objects[1] = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(float) * NUM_SPHERES,
        xs,
        NULL
    );

    // Create buffer for ys
    mem_objects[2] = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(float) * NUM_SPHERES,
        ys,
        NULL
    );

    // Create buffer for rs
    mem_objects[3] = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(float) * NUM_SPHERES,
        rs,
        NULL
    );

    mem_objects[4] = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(int),
        img_width,
        NULL
    );

    // Create buffer for result
    mem_objects[5] = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE,
        sizeof(float) * CANVAS_WIDTH * CANVAS_HEIGHT,
        NULL,
        NULL
    );

    if (mem_objects[0] == NULL || mem_objects[1] == NULL || mem_objects[2] == NULL || mem_objects[3] == NULL) {
        fprintf(stderr, "Error creating memory objects.\n");
        return 0;
    }

    return 1;
}

/// @brief Returns an OpenCL context, kernel and command queue.
int init_opencl(cl_context *out_context, cl_command_queue *out_command_queue, cl_kernel *out_kernel) {
    // Create an OpenCL context on first available platform
    cl_context context = create_context();
    if (context == NULL) {
        fprintf(stderr, "Failed to create OpenCL context.\n");
        return 1;
    }

    // Create a command queue on the first available device
    cl_device_id device;
    cl_command_queue command_queue = create_command_queue(context, &device);
    if (command_queue == NULL) {
        return 1;
    }

    // Create OpenCL program from source file
    cl_program program = create_program(context, device, "src/hello.cl");
    if (program == NULL) {
        return 1;
    }

    // Create OpenCL kernel
    cl_int kernel_err;
    cl_kernel kernel = clCreateKernel(program, "circles_kernel", &kernel_err);
    if (kernel == NULL) {
        fprintf(stderr, "Failed to create kernel. Error code %d\n", kernel_err);
        return 1;
    }

    *out_context = context;
    *out_command_queue = command_queue;
    *out_kernel = kernel;
    return 0;
}

int gpu_main(Canvas canvas, float *xs, float *ys, float *rs) {
    cl_context context;
    cl_command_queue command_queue;
    cl_kernel kernel;
    if (init_opencl(&context, &command_queue, &kernel)) {
        return 1;
    }

    // Create memory objects that will be used as arguments to the kernel.
    cl_mem mem_objects[6] = { 0, 0, 0, 0, 0, 0 };
    int ns = NUM_SPHERES;
    int cw = CANVAS_WIDTH;
    if (!create_mem_objects(context, mem_objects, &ns, xs, ys, rs, &cw)) {
        return 1;
    }

    // Set the kernel arguments (result, xs, ys, rs)
    cl_int err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &mem_objects[0]);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &mem_objects[1]);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &mem_objects[2]);
    err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &mem_objects[3]);
    err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &mem_objects[4]);
    err |= clSetKernelArg(kernel, 5, sizeof(cl_mem), &mem_objects[5]);

    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error setting kernel arguments.\n");
        return 1;
    }

    size_t global_work_size[1] = { CANVAS_WIDTH * CANVAS_HEIGHT };
    size_t local_work_size[1] = { 1 };

    // Queue the kernel up for execution across the array
    err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);

    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error queueing kernel for execution. Error code %d\n", err);
        return 1;
    }

    // Read the output buffer back to the host
    float *result = malloc(canvas.width * canvas.height * sizeof(float));
    err = clEnqueueReadBuffer(command_queue, mem_objects[5], CL_TRUE, 0, CANVAS_WIDTH * CANVAS_HEIGHT * sizeof(float), result, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error reading result buffer.\n");
        return 1;
    }

    // Output the result buffer
    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            int idx = CANVAS_WIDTH * y + x;
            if (result[idx] == 1.0) {
                canvas_pixel_set(canvas, x, y, color_rgb(1.0, 0.0, 0.0));
            }
        }
    }
    printf("Executed program successfully.\n");

    return 0;
}


int main() {
    log_line("Starting scene configuration\n");

    float xs[NUM_SPHERES] = { 80.0, 560.0 };
    float ys[NUM_SPHERES] = { 50.0, 600.0 };
    float rs[NUM_SPHERES] = { 300.0, 200.0 };

    Canvas canvas = canvas_create(CANVAS_WIDTH, CANVAS_HEIGHT);

    // Render
    log_line("Starting pixel computations");
    gpu_main(canvas, xs, ys, rs);
    log_line("Completed pixel computations");

    canvas_save_ppm(canvas, "out.ppm");

    // Free stuff to keep address sanitizer happy
    canvas_destroy(canvas);
}