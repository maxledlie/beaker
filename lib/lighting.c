#include <lighting.h>

Color lighting_compute(Material material, PointLight light, Vec4D point, Vec4D eye, Vec4D normal) {
    Color ambient, diffuse, specular;

    // Combine the surface color with the light's color/intensity
    Color effective_color = color_hadamard(material.color, light.intensity);
    
    // Find the direction to the light source
    Vec4D lightv = d4_norm(d4_sub(light.position, point));

    // Compute ambient contribution
    ambient = color_mul(effective_color, material.ambient);

    // If negative, light is "inside" the object
    double light_dot_normal = d4_dot(lightv, normal);
    if (light_dot_normal < 0.0) {
        diffuse = color_black();
        specular = color_black();
    } else {
        diffuse = color_mul(effective_color, material.diffuse * light_dot_normal);

        Vec4D reflectv = d4_reflect(d4_neg(lightv), normal);
        double reflect_dot_eye = d4_dot(eye, reflectv);

        if (reflect_dot_eye <= 0.0) {
            specular = color_black();
        } else {
            double factor = pow(reflect_dot_eye, material.shininess);
            specular = color_mul(light.intensity, material.specular * factor);
        }
    }

    return color_add(color_add(specular, diffuse), ambient);
}