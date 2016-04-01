#version 450 core

in vec2 mcCoords;        // 2D Model Coordinate vertices
in vec2 texCoords;       // 2D Texture Coordinates
uniform vec4 scaleTrans; // for mapping coords into Logical Device Space

out PVA
{
	vec2 texCoords;
} pvaOut;

void main()
{
	pvaOut.texCoords = texCoords;

	float ldsX = scaleTrans[0]*mcCoords.x + scaleTrans[1];
	float ldsY = scaleTrans[2]*mcCoords.y + scaleTrans[3];

	gl_Position = vec4(ldsX, ldsY, 0, 1);
}

