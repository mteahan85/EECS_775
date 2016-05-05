#version 450 core

layout (std430, binding = 0) buffer VoxelGrid
{
	int g[];
} voxelGrid;

uniform int nRows, nCols, nSheets;
uniform mat4 mc_ec;

// 0: Just draw the cube; ignore voxel grid

// 1: BINARY (if encounter val > parameter, set white; else black)
// 2: MAX: determine color from maximum sampled value on ray
// 3: AVERAGE: determine color from average of all samples along ray
// 4: SUM: determine color from sum of all samples along ray

// 5: ACCUMRGB_DIV_ACCUM_ALPHA
// 6: ACCUMRGB_ADD_BACKGROUND

// 7: EXPLICIT_ISOSURFACE
// 8: INFER_ISOSURFACE_GRADIENT_SIZE_AT_LEAST
uniform int rayFunction = 0;
uniform float rayFunctionParameter = 100; // some ray functions need a parameter

uniform float stepSize = 0.9; // voxels
uniform float cellSizeX = 1.0, cellSizeY = 1.0, cellSizeZ = 1.0;

in PVA
{
	// Use this if you are ray tracing voxel grid in fragment shader
	vec3 mcPosition;
	// Use this for checking that voxel geometry size looks ok
	vec3 mcNormal;
} pvaIn;

out vec4 fragmentColor;

int loc(int r, int c, int s)
{
	// stored column-major, sheet-by-sheet
	return s*(nRows*nCols) + c*(nRows) + r;
}

float getVal(in int ri, in float rf, in int ci, in float cf, in int si, in float sf)
{
	// This should do trilinear interpolation. As a placeholder:
	return voxelGrid.g[loc(ri, ci, si)];
}

bool mcToRCS(in vec3 mcPoint, out int ri, out float rf,
	 out int ci, out float cf, out int si, out float sf)
{
	float rowF = mcPoint.y/cellSizeY;
	float colF = mcPoint.x/cellSizeX;
	float sheetF = mcPoint.z/cellSizeZ;
	ri = int(rowF);
	rf = rowF - ri;
	ci = int(colF);
	cf = colF - ci;
	si = int(sheetF);
	sf = sheetF - si;
	return (si >= 0) && (si < nSheets) &&
	       (ci >= 0) && (ci < nCols) &&
	       (ri >= 0) && (ri < nRows);
}

vec4 traceRay(in vec3 mcPoint, in vec3 mcLineOfSight)
{
	int ri, ci, si;
	float rf, cf, sf;
	float v, maxVal = 0;
	while (mcToRCS(mcPoint, ri, rf, ci, cf, si, sf))
	{
		v = getVal(ri, rf, ci, cf, si, sf);
		if (rayFunction == 2)
		{
			// BEGIN "MAX", part 1
			if (v > maxVal)
				maxVal = v;
			// END "MAX", part 1
		}
		mcPoint += stepSize*mcLineOfSight;
	}
	vec4 colorToReturn;
	if (rayFunction == 2)
	{
		// BEGIN "MAX", part 2
		v = maxVal / 255.0;
		colorToReturn = vec4(v, v, v, 1.0);
		// END "MAX", part 2
	}
	return colorToReturn;
}

void main()
{
	mat3 mc_ec_inverse = inverse( mat3x3(mc_ec) );
	if (rayFunction == 0)
	{
		// Just render cube with pseudo-color
		mat3 normalMatrix = transpose( mc_ec_inverse );
		vec3 normal = normalMatrix * pvaIn.mcNormal;
		float gray = 0.9 * normal.z; // simulate light at the eye
		fragmentColor = vec4(gray, gray, gray, 1.0);
	}
	else
	{
		vec3 mcLineOfSight = mc_ec_inverse * vec3(0.0, 0.0, -1.0);
		fragmentColor = traceRay(pvaIn.mcPosition, mcLineOfSight);
	}
}

