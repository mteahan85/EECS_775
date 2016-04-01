#version 450 core

in vec2 mcCoords;        // 2D Model Coordinate vertices
in vec2 texCoords;       // 2D Texture Coordinates

out PVA
{
	vec2 texCoords;
} pvaOut;

void main()
{
	pvaOut.texCoords = texCoords;

	// The following gl_Position won't be used. It will be
	// replaced by vertices output by the tessellation shaders.
	gl_Position = vec4(mcCoords, 0, 1);
}

