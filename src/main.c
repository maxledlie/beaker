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

int main() {
    log_line("Starting scene configuration");

    Mat4D transform;
    Material material;

    // Create a group describing a square made of cylinders and spheres
    material = material_default();
    transform = translation(-5.0, 0.0, 0.0);
    Shape left = cylinder_new(transform, material, "window_left", -5.0, 5.0, 1);

    transform = translation(5.0, 0.0, 0.0);
    Shape right = cylinder_new(transform, material, "window_right", -5.0, 5.0, 1);

    transform = mat4d_mul_mat4d(translation(0.0, 5.0, 0.0), rotation_z(M_PI / 2.0));
    Shape top = cylinder_new(transform, material, "window_top", -5.0, 5.0, 1);

    transform = mat4d_mul_mat4d(translation(0.0, -5.0, 0.0), rotation_z(M_PI / 2.0));
    Shape bottom = cylinder_new(transform, material, "window_top", -5.0, 5.0, 1);

    transform = translation(-5.0, 5.0, 0.0);
    Shape top_left = sphere_new(transform, material, "window_top_left");

    transform = translation(5.0, 5.0, 0.0);
    Shape top_right = sphere_new(transform, material, "window_top_right");

    transform = translation(-5.0, -5.0, 0.0);
    Shape bottom_left = sphere_new(transform, material, "window_bottom_left");

    transform = translation(5.0, -5.0, 0.0);
    Shape bottom_right = sphere_new(transform, material, "window_bottom_right");

    World world = world_new(MAX_GROUPS, MAX_LIGHTS);
    world_add_shape(&world, &left);
    world_add_shape(&world, &right);
    world_add_shape(&world, &top);
    world_add_shape(&world, &bottom);
    world_add_shape(&world, &bottom_left);
    world_add_shape(&world, &top_left);
    world_add_shape(&world, &bottom_right);
    world_add_shape(&world, &top_right);

    Vec4D light_position = d4_point(-2.0, 10.0, -10.0);
    Color light_color = (Color) {1.0, 1.0, 1.0};
    PointLight light = (PointLight) { light_position, light_color };
    world_add_light(&world, light);

    Mat4D view = view_transform(
        d4_point(-2.0, 4.0, -20.0),
        d4_point(0., 0., 0.),
        d4_vector(0., 1., 0.)
    );
    Camera camera = camera_new(1200, 900, M_PI / 3., view);
    Canvas canvas = canvas_create(camera.hsize, camera.vsize);

    log_line("Completed scene configuration");

    if (CFG_SINGLE_PIXEL_DEBUG) {
        printf("Debugging single pixel at (%d, %d)\n", CFG_SINGLE_PIXEL_X, CFG_SINGLE_PIXEL_Y);
        Ray ray = ray_at_pixel(camera, CFG_SINGLE_PIXEL_X, CFG_SINGLE_PIXEL_Y);
        Color c = ray_color(ray, world, CFG_RECURSION_DEPTH);
        printf("Output color: (%f, %f, %f)\n", c.r, c.g, c.b);
        return 0;
    }

    // Render
    log_line("Starting render");
    for (int y = 0; y < camera.vsize; y++) {
        for (int x = 0; x < camera.hsize; x++) {
            Ray ray = ray_at_pixel(camera, x, y);
            Color c = ray_color(ray, world, CFG_RECURSION_DEPTH);
            canvas_pixel_set(canvas, x, y, c);
        }
    }
    log_line("Completed render");
    canvas_save_ppm(canvas, "out.ppm");

    // Free stuff to keep address sanitizer happy
    world_free(&world);
    canvas_destroy(canvas);
}

const cl_uint MAX_PLATFORMS = 8;
const cl_uint MAX_PLATFORM_NAME_LEN = 32;
const cl_uint MAX_DEVICES = 8;
#define ARRAY_SIZE 1024

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

int create_mem_objects(cl_context context, cl_mem mem_objects[3], float *a, float *b) {
    // Create buffer for input a
    mem_objects[0] = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(float) * ARRAY_SIZE,
        a,
        NULL
    );

    // Create buffer for input b
    mem_objects[1] = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(float) * ARRAY_SIZE,
        b,
        NULL
    );

    // Create buffer for result
    mem_objects[2] = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE,
        sizeof(float) * ARRAY_SIZE,
        NULL,
        NULL
    );

    if (mem_objects[0] == NULL || mem_objects[1] == NULL || mem_objects[2] == NULL) {
        fprintf(stderr, "Error creating memory objects.\n");
        return 0;
    }

    return 1;
}

int gpu_main() {
    // Check current working directory
    char cwd[1024];
    if (_getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directoory: %s\n", cwd);
    }
    else {
        fprintf(stderr, "_getcwd() error");
    }
    
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
    cl_kernel kernel = clCreateKernel(program, "hello_kernel", NULL);
    if (kernel == NULL) {
        fprintf(stderr, "Failed to create kernel\n");
        return 1;
    }

    // Create memory objects that will be used as arguments to the kernel.
    // First, create host memory arrays that will be used to store the arguments to the kernel.
    float result[ARRAY_SIZE];
    float a[ARRAY_SIZE];
    float b[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; i++) {
        a[i] = (float)i;
        b[i] = (float)(i * 2);
    }

    cl_mem mem_objects[3] = { 0, 0, 0 };
    if (!create_mem_objects(context, mem_objects, a, b)) {
        return 1;
    }

    // Set the kernel arguments (result, a, b)
    cl_int err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &mem_objects[0]);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &mem_objects[1]);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &mem_objects[2]);

    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error setting kernel arguments.\n");
        return 1;
    }

    size_t global_work_size[1] = { ARRAY_SIZE };
    size_t local_work_size[1] = { 1 };

    // Queue the kernel up for execution across the array
    err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);

    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error queueing kernel for execution\n");
        return 1;
    }

    // Read the output buffer back to the host
    err = clEnqueueReadBuffer(command_queue, mem_objects[2], CL_TRUE, 0, ARRAY_SIZE * sizeof(float), result, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error reading result buffer.\n");
        return 1;
    }

    // Output the result buffer
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%f ", result[i]);
    }
    printf("\n");
    printf("Executed program successfully.\n");

    return 0;
}