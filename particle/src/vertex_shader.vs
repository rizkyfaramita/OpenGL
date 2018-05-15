#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform bool instanced = false;
//uniform bool billboarded = false;
//uniform mat4 instance_transformation[1000];
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    vec4 vertex_pos;

//    if (billboarded) {
//        vec3 camera_right = vec3(view[0][0], view[1][0], view[2][0]);
//        vec3 camera_up = vec3(view[0][1], view[1][1], view[2][1]);
//        vertex_pos = vec4(normalize(camera_right)*aPos.x+normalize(camera_up) * aPos.y, 1.0);
//    } else
//        vertex_pos = vec4(aPos, 1.0);

//    if (instanced) {
//        mat4 transformation = instance_transformation[gl_InstanceID];
//        vertex_pos = transformation * vertex_pos;
//    }

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
