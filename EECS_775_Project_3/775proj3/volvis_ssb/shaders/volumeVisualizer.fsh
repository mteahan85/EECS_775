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
uniform int rayFunctionParameter = 100; // some ray functions need a parameter

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

//takes in two verts and fractional difference
float linearInterp(in float v1, in float v2, in float f){
 
  return (1-f)*v1 + f*v2;
  
}


float trilinearInterp(in int ri, in float rf, in int ci, in float cf, in int si, in float sf){
  
  //interpolate along x
  float c00 = linearInterp(voxelGrid.g[loc(ri,ci,si)], voxelGrid.g[loc(ri+1,ci,si)], rf);
  float c01 = linearInterp(voxelGrid.g[loc(ri,ci,si+1)], voxelGrid.g[loc(ri+1,ci,si+1)], rf);
  float c10 = linearInterp(voxelGrid.g[loc(ri,ci+1,si)], voxelGrid.g[loc(ri+1,ci+1,si)], rf);
  float c11 = linearInterp(voxelGrid.g[loc(ri,ci+1,si+1)], voxelGrid.g[loc(ri+1,ci+1,si+1)], rf);
  
  //interpolate along y
  float c0 = linearInterp(c00, c10, cf);
  float c1 = linearInterp(c01, c11, cf);
  
  //interpolate along z -- final actual point
  float c = linearInterp(c0, c1, sf);
  
  return c;
}

float bilinearInterp(in int v1, in int v2, in int v3, in int v4, in float rf, in float cf){
  
  float fir_part = (1 - rf) * ( ((1-cf) * v1) + (cf * v2) );
  float sec_part = rf * ( ((1-cf) * v3) + (cf * v4));
  
  float item = fir_part + sec_part; 
  return item;
}




float getVal(in int ri, in float rf, in int ci, in float cf, in int si, in float sf)
{
	// This should do trilinear interpolation. As a placeholder:
  
	//trilinear interp -- little smoother?
	//return trilinearInterp(ri,rf,ci,cf,si,sf);
  
	//two bilinear an linear interp to simulate trilinear -- not as smooth
// 	float interp1 = bilinearInterp(voxelGrid.g[loc(ri,ci,si)], voxelGrid.g[loc(ri,ci,si+1)], 
// 				      voxelGrid.g[loc(ri+1,ci,si)], voxelGrid.g[loc(ri+1,ci,si+1)], rf, cf);
// 	float interp2 = bilinearInterp(voxelGrid.g[loc(ri,ci+1,si)], voxelGrid.g[loc(ri,ci+1,si+1)], 
// 				      voxelGrid.g[loc(ri+1,ci+1,si)], voxelGrid.g[loc(ri+1,ci+1,si+1)], rf, cf);
//   
// 	float final_interp = linearInterp(interp1, interp2, sf);
// 	return final_interp;
// 	
//   
	//continue with this or now bc trilienar is slow
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
	float num = 0; 
	float vTotal = 0;
	bool binary = false;
	while (mcToRCS(mcPoint, ri, rf, ci, cf, si, sf)) //what does this mean
	{
		v = getVal(ri, rf, ci, cf, si, sf); //i'm not really sure what's going on here
		
		if (rayFunction == 1){ //binary
			if(v >= rayFunctionParameter){//if hit point in ray that hits this threshold then turns it true
			 binary = true;  
			}
		}if (rayFunction == 2){ //finds overall max
			// BEGIN "MAX", part 1
			if (v > maxVal)
				maxVal = v;
			// END "MAX", part 1
		}else if (rayFunction == 3 || rayFunction == 4){ //averages the values
			num++;
			vTotal += v;
		}
		mcPoint += stepSize*mcLineOfSight;
	}
	vec4 colorToReturn;
	if(rayFunction == 1){ //not quite turning out like expected
	  
	  // BEGIN "BINARY", part 2
	  if(binary){
	    colorToReturn = vec4(1.0, 1.0, 1.0, 1.0);
	  }else{
	    colorToReturn = vec4(0.0, 0.0, 0.0, 1.0);
	  }
	  // END "BINARY", part 2
	}else if (rayFunction == 2){
		// BEGIN "MAX", part 2
		v = maxVal / 255.0;
		colorToReturn = vec4(v, v, v, 1.0);
		// END "MAX", part 2
	}else if (rayFunction == 3){
		// BEGIN "AVERAGE"
		vTotal = vTotal / num;
		vTotal = vTotal / 255.0;
		colorToReturn = vec4(vTotal, vTotal, vTotal, 1.0);	
	}else if (rayFunction == 4){ //currently the same as average. need to learn out to scale sum to 0 to 1 scale
		// BEGIN "SUM" -- scale sum to 0 to 1
		float maxSum = num * 255.0;
		v = vTotal / maxSum; //for scaling
		colorToReturn = vec4(v, v, v, 1.0);	
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

