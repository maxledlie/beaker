__constant bool DEBUG = true;
__constant int SHAPE_TYPE_SPHERE = 0;
__constant int SHAPE_TYPE_PLANE = 1;

typedef struct {
    float4 inv_transform[4];
    float field_of_view;
    int hsize;
    int vsize;
    int pad;  // To ensure aligned to 16 bytes
} Camera;

typedef struct {
    float4 color;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
    float reflective;
    float transparency;
    float refractive_index;
    float pad;
} Material;

typedef struct {
    int type;
    float ymin;   // Only relevant for cylinders and cones
    float ymax;   // ditto
    bool closed;  // ditto
    float4 inv_transform[4];
    Material material;
} Shape;

typedef struct {
    float4 position;   // Where is the light located?
    float4 intensity;  // What color is the light?
} PointLight;

typedef struct {
    float4 origin;
    float4 direction;
} Ray;

float4 mat_mul_vec(__constant float4 mat[4], float4 vec) {
    return (float4)(
        dot(mat[0], vec),
        dot(mat[1], vec),
        dot(mat[2], vec),
        dot(mat[3], vec)
    );
}

// Need two versions of this, one for constant data!?
// Surely there's a better way...
float4 _mat_mul_vec(float4 mat[4], float4 vec) {
    return (float4)(
        dot(mat[0], vec),
        dot(mat[1], vec),
        dot(mat[2], vec),
        dot(mat[3], vec)
    );
}

int transpose(float4 mat[4], float4 out[4]) {
    out[0] = (float4)(mat[0].x, mat[1].x, mat[2].x, mat[3].x);
    out[1] = (float4)(mat[0].y, mat[1].y, mat[2].y, mat[3].y);
    out[2] = (float4)(mat[0].z, mat[1].z, mat[2].z, mat[3].z);
    out[3] = (float4)(mat[0].w, mat[1].w, mat[2].w, mat[3].w);
    return 0;
}

bool should_print() {
    return DEBUG && get_global_id(0) == 500 && get_global_id(1) == 400;
}

bool ray_intersect_sphere(Ray ray, float *t) {
    // Vector from sphere's center to ray origin
    float4 sphere_to_ray = ray.origin - (float4)(0.0f, 0.0f, 0.0f, 1.0f);
    
    float a = dot(ray.direction, ray.direction);
    float b = 2.0f * dot(ray.direction, sphere_to_ray);
    float c = dot(sphere_to_ray, sphere_to_ray) - 1.0f;
    float disc = b * b - 4 * a * c;

    if (disc < 0) {
        return false;
    }

    float root = sqrt(disc);
    float t1 = (-b - root) / (2.0f * a);
    float t2 = (-b + root) / (2.0f * a);

    float tmin = min(t1, t2);
    float tmax = max(t1, t2);
    if (tmin >= 0.0f) {
        *t = tmin;
        return true;
    } else if (tmax >= 0.0f) {
        *t = tmax;
        return true;
    }
    return false;
}

bool ray_intersect_plane(Ray ray, float *t) {
    if (fabs(ray.direction.y) < FLT_EPSILON) {
        return false;
    }
    float _t = -ray.origin.y / ray.direction.y;
    if (_t >= 0.0f) {
        *t = _t;
        return true;
    }
    return false;
}

Ray transform_ray(Ray r, __constant float4 transform[4]) {
    return (Ray) {
        mat_mul_vec(transform, r.origin),
        mat_mul_vec(transform, r.direction)
    };
}

bool ray_intersect_shape(Ray ray, __constant Shape *shape, float *t) {
    // Transform the ray into the shape's object space to simplify calculations
    Ray ray_local = transform_ray(ray, shape->inv_transform);

    switch (shape->type) {
        case SHAPE_TYPE_SPHERE:
            return ray_intersect_sphere(ray_local, t);
        case SHAPE_TYPE_PLANE:
            return ray_intersect_plane(ray_local, t);
        default:
            return false;
    }
}

float4 normal_at(Shape *shape, float4 world_point) {
    // First transform the hit point into the shape's object space to simplify the calculation
    float4 intersection_local = _mat_mul_vec(shape->inv_transform, world_point);
    float4 local_normal;
    switch (shape->type) {
        case SHAPE_TYPE_SPHERE:
            local_normal = (float4)(intersection_local.xyz, 0.0f);
            break;
        default:
            local_normal = (float4)(0.0f, 1.0f, 0.0f, 0.0f);
            break;
    }

    // Convert the normal back to world space. Here we have to multiply by the inverse *transpose*.
    // (I worked out why this is once but can't remember so just trust me bro)
    float4 inv_transpose[4];
    transpose(shape->inv_transform, inv_transpose);
    float4 world_normal = _mat_mul_vec(inv_transpose, local_normal);
    world_normal.w = 0.0f;
    return normalize(world_normal);
}


float4 reflect(float4 in, float4 normal) {
    float scale = 2.0f * dot(in, normal);
    return in - normal * scale;
}

__kernel void raytrace_kernel(
    __constant Camera *camera,
    int num_shapes,
    __constant Shape *shapes,
    int num_lights,
    __constant PointLight *lights,
    __write_only image2d_t result_img
) {
    if (should_print()) {
        printf("num_shapes: %d\n", num_shapes);
        printf("shapes[0].material.color.r: %f\n", shapes[0].material.color.x);
        printf("shapes[1].material.color.r: %f\n", shapes[1].material.color.x);
        printf("shapes[2].material.color.r: %f\n", shapes[2].material.color.x);
    }

    // Get pixel coordinates
    int2 pixel = (int2)(get_global_id(0), get_global_id(1));
    float2 pixelf = convert_float2(pixel);

    // Compute camera properties (common to every pixel)
    float half_view = tan(0.5f * camera->field_of_view);
    float aspect = (float)camera->hsize / (float)camera->vsize;
    float2 camera_half_size = aspect >= 1.0f
        ? (float2)(half_view, half_view / aspect)
        : (float2)(half_view * aspect, half_view); 
    float camera_pixel_size = camera_half_size.x * 2.0f / (float)camera->hsize;

    // Compute ray at this pixel
    float2 offset = (pixelf + 0.5f) * camera_pixel_size;   // Offset from edge of camera to pixel's center
    float4 pixel_center_view = (float4)(camera_half_size - offset, -1.0f, 1.0f); // Untransformed coordinates of pixel center in view space
    float4 pixel_center_world = mat_mul_vec(camera->inv_transform, pixel_center_view);
    float4 ray_origin_world = mat_mul_vec(camera->inv_transform, (float4)(0.0f, 0.0f, 0.0f, 1.0f));
    float4 direction = normalize(pixel_center_world - ray_origin_world);
    Ray ray = { ray_origin_world, direction };

    // Find closest intersection of ray with any shape.
    float tmin = INFINITY;
    int hit_index = -1;
    for (int i = 0; i < num_shapes; i++) {
        __constant Shape *shape = &shapes[i];
        float t;
        if (ray_intersect_shape(ray, shape, &t) && t < tmin) {
            tmin = t;
            hit_index = i;
        }
    }

    if (hit_index == -1) {
        // Ray missed everything - pixel is black
        write_imagef(result_img, pixel, (float4)(0.0f, 0.0f, 0.0f, 1.0f));
        return;
    }
    Shape hit_shape = shapes[hit_index];

    // Find the point t units along the ray - this is where the intersection occured
    float4 intersection_point = ray.origin + tmin * ray.direction;

    // Compute normal vector at the hit point.
    float4 normalv = normal_at(&hit_shape, intersection_point);


    // ----------------
    // Lighting!
    // Start with the ambient color of the material and add contributions from each light source in the world.
    // We use the Phong reflection model to get reasonably good looking shading and specular highlights.
    // ----------------

    float3 combined_color = (float3)(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < num_lights; i++) {
        PointLight light = lights[i];

        // We use the elementwise product to combine the light color and the material color.
        float3 effective_color = light.intensity.xyz * hit_shape.material.color.xyz;

        // Add ambient contribution. This doesn't depend at all on the position of the light.
        combined_color += effective_color * hit_shape.material.ambient;

        // Add diffuse contribution.
        float4 lightv = normalize(light.position - intersection_point);
        float light_dot_normal = dot(lightv, normalv);
        if (light_dot_normal < 0.0f) {
            continue;
        }
        combined_color += effective_color * hit_shape.material.diffuse * light_dot_normal;

        // Add specular contribution.
        float4 eyev = -ray.direction;
        float4 reflectv = reflect(-lightv, normalv);
        float reflect_dot_eye = dot(eyev, reflectv);
        if (reflect_dot_eye <= 0.0f) {
            continue;
        }
        float factor = pow(reflect_dot_eye, hit_shape.material.shininess);
        combined_color += light.intensity.xyz * hit_shape.material.specular * factor;
    }

    float4 color = (float4)(combined_color, 1.0f);

    if (should_print()) {
        printf("hit_shape.inv_transform:\n");
        printf("%v4hlf\n", hit_shape.inv_transform[0]);
        printf("%v4hlf\n", hit_shape.inv_transform[1]);
        printf("%v4hlf\n", hit_shape.inv_transform[2]);
        printf("%v4hlf\n", hit_shape.inv_transform[3]);
        printf("\n");
        printf("ray_origin_world:\n");
        printf("%v4hlf\n\n", ray_origin_world);
        printf("pixel_center_world:\n");
        printf("%v4hlf\n\n", pixel_center_world);
        printf("ray direction:\n");
        printf("%v4hlf\n\n", direction);
        printf("intersection t-value:\n");
        printf("%f\n\n", tmin);
        printf("intersection_point:\n");
        printf("%v4hlf\n\n", intersection_point);
        printf("normalv:\n");
        printf("%v4hlf\n\n", normalv);
        printf("hit_shape.material.color:\n");
        printf("%v4hlf\n\n", hit_shape.material.color);
    }

    write_imagef(result_img, pixel, color);
}