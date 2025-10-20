#include <renderer.h>
#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#define NUM_DIMENSIONS 2
#define NUM_SPHERES 2

const cl_uint MAX_PLATFORMS = 8;
const cl_uint MAX_PLATFORM_NAME_LEN = 32;
const cl_uint MAX_DEVICES = 8;


// Replicating structs expected by the OpenCL code.
// It's pretty tedious to define them all in both places - is there a better way?
int marshall_mat4(Mat4D *in, float out[16]) {
    for (int iRow = 0; iRow < 4; iRow++) {
        for (int jCol = 0; jCol < 4; jCol++) {
            int idx = 4 * iRow + jCol;
            out[idx] = (float)in->m[iRow][jCol];
        }
    }
    return 0;
}

int marshall_vec4(Vec4D *in, float out[4]) {
    out[0] = (float)in->x;
    out[1] = (float)in->y;
    out[2] = (float)in->z;
    out[3] = (float)in->w;
    return 0;
}

int marshall_color(Color *in, float out[4]) {
    out[0] = (float)in->r;
    out[1] = (float)in->g;
    out[2] = (float)in->b;
    out[3] = 1.0f;
    return 0;
}

typedef struct {
    float inv_transform[16];
    float field_of_view;
    int hsize;
    int vsize;
    int pad;  // To ensure aligned to 16 bytes
} CameraCL;

int marshall_camera(Camera camera, CameraCL *out) {
    marshall_mat4(&camera.inv_transform, out->inv_transform);
    out->hsize = camera.hsize;
    out->vsize = camera.vsize;
    out->field_of_view = (float)camera.field_of_view;
    return 0;
}

typedef struct {
    float color[4];
    float ambient;
    float diffuse;
    float specular;
    float shininess;
    float reflective;
    float transparency;
    float refractive_index;
    float pad;
} MaterialCL;

int marshall_material(Material material, MaterialCL *out) {
    // Assuming uniform colors for now
    out->color[0] = (float)material.pattern.a.r;
    out->color[1] = (float)material.pattern.a.g;
    out->color[2] = (float)material.pattern.a.b;
    out->color[3] = 1.0f;
    out->ambient = (float)material.ambient;
    out->diffuse = (float)material.diffuse;
    out->specular = (float)material.specular;
    out->shininess = (float)material.shininess;
    out->reflective = (float)material.reflective;
    out->transparency = (float)material.transparency;
    out->refractive_index = (float)material.refractive_index;
    out->pad = 0.0f;
    return 0;
}

typedef struct {
    int type;
    float ymin;  // Only relevant for cylinders and cones
    float ymax;  // ditto
    int closed;  // ditto
    float inv_transform[16];
    float inv_transpose[16];
    MaterialCL material;
} ShapeCL;

int marshall_shapes(World world, ShapeCL *out)  {
    for (int i = 0; i < world.object_count; i++) {
        Shape *shape = &world.objects[i];
        ShapeCL *shape_cl = &out[i];

        // Precompute transpose
        double m[16];
        for (int jCol = 0; jCol < 4; jCol++) {
            for (int iRow = 0; iRow < 4; iRow++) {
                m[jCol * 4 + iRow] = shape->inv_transform.m[iRow][jCol];
            }
        }
        Mat4D inv_transpose = mat4d_new(m);

        shape_cl->type = shape->type;
        shape_cl->ymin = (float)shape->ymin;
        shape_cl->ymax = (float)shape->ymax;
        shape_cl->closed = shape->closed;
        marshall_mat4(&shape->inv_transform, shape_cl->inv_transform);
        marshall_mat4(&inv_transpose, shape_cl->inv_transpose);
        marshall_material(shape->material, &shape_cl->material);
    }
    return 0;
}

typedef struct {
    float position[4];
    float intensity[4];
} PointLightCL;

int marshall_lights(World world, PointLightCL *out) {
    for (int i = 0; i < world.light_count; i++) {
        PointLight *light = &world.lights[i];
        PointLightCL *light_cl = &out[i];
        marshall_vec4(&light->position, light_cl->position);
        marshall_color(&light->intensity, light_cl->intensity);
    }
    return 0;
}

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
    cl_device_id *devices = calloc(device_buffer_size, sizeof(cl_device_id));
    err = clGetContextInfo(context, CL_CONTEXT_DEVICES, device_buffer_size, devices, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Failed to get device IDs.\n");
        return NULL;
    }

    // Use the first device with image support
    for (int i = 0; i < device_buffer_size; i++) {
        cl_bool supports_images;
        err = clGetDeviceInfo(devices[i], CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &supports_images, NULL);
        if (supports_images) {
            *device = devices[i];
            break;
        }
    }
    if (device == NULL) {
        fprintf(stderr, "No devices support images.\n");
        return NULL;
    }

    // If we want this to run on a cluster or something, modify to use all available GPUs.
    cl_command_queue command_queue = clCreateCommandQueue(context, *device, 0, &err);
    if (command_queue == NULL) {
        fprintf(stderr, "Failed to create commandQueue for device 0. Error code %d.\n", err);
        return NULL;
    }

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
    cl_program program = create_program(context, device, "opencl/raytrace.cl");
    if (program == NULL) {
        return 1;
    }

    // Create OpenCL kernel
    cl_int kernel_err;
    cl_kernel kernel = clCreateKernel(program, "raytrace_kernel", &kernel_err);
    if (kernel == NULL) {
        fprintf(stderr, "Failed to create kernel. Error code %d\n", kernel_err);
        return 1;
    }

    *out_context = context;
    *out_command_queue = command_queue;
    *out_kernel = kernel;
    return 0;
}

int render_image(World world, Camera camera, Canvas canvas) {
    cl_int err;
    cl_context context;
    cl_command_queue command_queue;
    cl_kernel kernel;
    if (init_opencl(&context, &command_queue, &kernel)) {
        return 1;
    }

    // Create image object we will use for output
    int arg_counter = 0;
    cl_mem buffer;

    // ----------------------------------
    // Set kernel args
    // ----------------------------------

    // Camera
    CameraCL *camera_cl = calloc(1, sizeof(CameraCL));
    marshall_camera(camera, camera_cl);
    buffer = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(CameraCL),
        camera_cl,
        &err
    );
    err |= clSetKernelArg(kernel, arg_counter++, sizeof(cl_mem), &buffer);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error setting camera arg. Error code %d\n", err);
    }

    // Shapes
    ShapeCL *shapes_cl = calloc(world.object_count, sizeof(ShapeCL));
    marshall_shapes(world, shapes_cl);
    buffer = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        world.object_count * sizeof(ShapeCL),
        shapes_cl,
        &err
    );
    err |= clSetKernelArg(kernel, arg_counter++, sizeof(cl_int), &world.object_count);
    err |= clSetKernelArg(kernel, arg_counter++, sizeof(cl_mem), &buffer);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error setting shapes arg. Error code %d\n", err);
    }

    // Lights
    PointLightCL *lights_cl = calloc(world.light_count, sizeof(PointLightCL));
    marshall_lights(world, lights_cl);
    buffer = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        world.light_count * sizeof(PointLightCL),
        lights_cl,
        &err
    );
    err |= clSetKernelArg(kernel, arg_counter++, sizeof(cl_int), &world.light_count);
    err |= clSetKernelArg(kernel, arg_counter++, sizeof(cl_mem), &buffer);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error setting lights arg. Error code %d\n", err);
    }

    // Output image
    cl_image_format image_format;
    image_format.image_channel_order = CL_RGBA;
    image_format.image_channel_data_type = CL_UNORM_INT8;
    cl_mem output_image = clCreateImage2D(
        context,
        CL_MEM_WRITE_ONLY,
        &image_format,
        camera.hsize,
        camera.vsize,
        0,
        NULL,
        &err
    );

    err |= clSetKernelArg(kernel, arg_counter++, sizeof(cl_mem), &output_image);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error setting output image arg. Error code %d\n", err);
        return 1;
    }

    // ----------------------------------------
    // Execute kernel
    // ----------------------------------------

    size_t global_work_size[NUM_DIMENSIONS] = { camera.hsize, camera.vsize };

    // Queue the kernel up for execution across the array
    err = clEnqueueNDRangeKernel(command_queue, kernel, (cl_uint)NUM_DIMENSIONS, NULL, global_work_size, NULL, 0, NULL, NULL);

    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error queueing kernel for execution. Error code %d\n", err);
        return 1;
    }

    // Read the output buffer back to the host. We need 4 bytes per pixel.
    uint8_t *result = calloc(4 * canvas.width * canvas.height, sizeof(uint8_t));
    err = clEnqueueReadImage(
        command_queue,
        output_image,
        CL_TRUE,
        (size_t[]){ 0, 0, 0 },
        (size_t[]){ camera.hsize, camera.vsize, 1 },
        0,
        0,
        result,
        0,
        NULL,
        NULL
    );
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error reading result buffer.\n");
        return 1;
    }

    // Output the result buffer
    for (int y = 0; y < camera.vsize; y++) {
        for (int x = 0; x < camera.hsize; x++) {
            int idx = camera.hsize * y + x;
            int offset = 4 * idx;
            uint8_t r = result[offset];
            uint8_t g = result[offset + 1];
            uint8_t b = result[offset + 2];
            Color c = color_rgb(r / 255.0, g / 255.0, b / 255.0);
            canvas_pixel_set(canvas, x, y, c);
        }
    }
    printf("Executed program successfully.\n");

    return 0;
}