#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNrm;

// Output
out vec4 ColorNormal;

uniform mat4 ModelMtx;
uniform mat4 ViewMtx;
uniform mat4 ProjMtx;


void main()
{
    ColorNormal = vec4(aNrm.rgb, 1);

    mat4 MVP = ProjMtx * ViewMtx * ModelMtx;
    gl_Position = MVP * vec4(aPos, 1);
}