#version 450 core

layout ( vertices = 4 ) out;

uniform int tessLevel = 20;

in PVA
{
	vec2 texCoords;
} pva_in[];

out PVA
{
	vec2 texCoords;
} pva_out[];

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	pva_out[gl_InvocationID].texCoords = pva_in[gl_InvocationID].texCoords;

	gl_TessLevelOuter[0] = gl_TessLevelOuter[2] = tessLevel;
	gl_TessLevelOuter[1] = gl_TessLevelOuter[3] = tessLevel;
	gl_TessLevelInner[0] = gl_TessLevelInner[1] = tessLevel;
}
