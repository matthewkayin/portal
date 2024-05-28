#version 410 core

in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_texture_coordinate;

out vec4 frag_color;

uniform vec3 view_position;

uniform sampler2D material_albedo;

void main() {  
    vec3 view_direction = normalize(view_position - frag_position);
    vec3 normal = normalize(frag_normal);
    if (dot(normal, view_direction) < 0) {
        discard;
    }
 
    // Albedo
    vec3 albedo = pow(texture(material_albedo, frag_texture_coordinate).rgb, vec3(2.2));

    vec3 ambient = vec3(1.0) * albedo;
    vec3 color = ambient; 
    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0 / 2.2));

    frag_color = vec4(color, 1.0);
}