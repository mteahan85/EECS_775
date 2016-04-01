#version 450 core

layout (std430, binding = 0) buffer ScalarField
{
	float sf[];
} scalarField;
uniform float scalarFieldMin, scalarFieldMax;

uniform int nRows, nCols;
uniform int nSheets;
uniform int sheetIndex = 0;
uniform int timestepIndex = 0;

uniform vec3 color = vec3(1.0, 0.0, 0.0);

in PVA
{
	vec2 texCoords;
} pvaIn;

out vec4 fragmentColor;

void main()
{
	if ((pvaIn.texCoords.s >= 0.0) && (pvaIn.texCoords.s <= 1.0) &&
	    (pvaIn.texCoords.t >= 0.0) && (pvaIn.texCoords.t <= 1.0))
	{
		int col = int((1.0-pvaIn.texCoords.t) * (nCols-1) + 0.5);
		int row = int(pvaIn.texCoords.s * (nRows-1) + 0.5);
		int offset = row*nCols + col + sheetIndex*nRows*nCols + nSheets*nRows*nCols*timestepIndex ; //use timestepIndex
		float val = scalarField.sf[offset];
		if (val > scalarFieldMax) // Missing Data
			discard;
		else
		{
			// map val to 0..1
			float valIn0To1 = (val - scalarFieldMin) / (scalarFieldMax - scalarFieldMin);
			fragmentColor = vec4(0.0, valIn0To1, 0.0, 1.0);
		}
	}
	else
		fragmentColor = vec4(color, 1.0);
}

