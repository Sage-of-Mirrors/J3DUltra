#version 330 core

// Input Data
in vec4 ColorNormal;

// Final Output
out vec4 PixelColor;

void main()
{
	vec3 colorOutput = ColorNormal.rgb * 0.5 + vec3(0.5, 0.5, 0.5);
	PixelColor = vec4(colorOutput, 1);
}