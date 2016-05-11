#version 450 core

layout (std430, binding = 0) buffer VoxelGrid
{
  int g[];
} voxelGrid;

const int MAX_NUM_STEPS = 10;

uniform int nRows, nCols, nSheets, nSteps;
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
uniform int tri = 0;

uniform float stepSize = 0.9; // voxels
uniform float preGain = 1.0;
uniform float cellSizeX = 1.0, cellSizeY = 1.0, cellSizeZ = 1.0;

uniform int rgba0Steps[MAX_NUM_STEPS];
uniform int rgba1Steps[MAX_NUM_STEPS];

uniform vec4 rgba0[MAX_NUM_STEPS];
uniform vec4 rgba1[MAX_NUM_STEPS];

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


float getChange(in float d1, in float d2, in float d3, in float d4, in float f1, in float f2){
  
  
  return (1-f1)*((1-f2)*d1 + f2*d2) + f1*((1-f2)*d3 + f2*d4); 
  
  
}


vec3 getNormal(in int ri, in float rf, in int ci, in float cf, in int si, in float sf){
   
  
  float x1 = voxelGrid.g[loc(ri,ci+1,si)] - voxelGrid.g[loc(ri,ci,si)];
  float x2 = voxelGrid.g[loc(ri+1,ci+1,si)] - voxelGrid.g[loc(ri+1,ci,si)];
  float x3 = voxelGrid.g[loc(ri,ci+1,si+1)] - voxelGrid.g[loc(ri,ci,si+1)];
  float x4 = voxelGrid.g[loc(ri+1,ci+1,si+1)] - voxelGrid.g[loc(ri+1,ci,si+1)];
  
  float dx = getChange(x1, x2, x3, x4, sf, rf);
  
  
  float y1 = voxelGrid.g[loc(ri+1,ci,si)] - voxelGrid.g[loc(ri,ci,si)];
  float y2 = voxelGrid.g[loc(ri+1,ci+1,si)] - voxelGrid.g[loc(ri,ci+1,si)];
  float y3 = voxelGrid.g[loc(ri+1,ci,si+1)] - voxelGrid.g[loc(ri,ci,si+1)];
  float y4 = voxelGrid.g[loc(ri+1,ci+1,si+1)] - voxelGrid.g[loc(ri,ci+1,si+1)];
 
  float dy = getChange(y1, y2, y3, y4, sf, cf);
  
  
  float z1 = voxelGrid.g[loc(ri,ci,si+1)] - voxelGrid.g[loc(ri,ci,si)];
  float z2 = voxelGrid.g[loc(ri,ci+1,si+1)] - voxelGrid.g[loc(ri,ci+1,si)];
  float z3 = voxelGrid.g[loc(ri+1,ci,si+1)] - voxelGrid.g[loc(ri+1,ci,si)];
  float z4 = voxelGrid.g[loc(ri+1,ci+1,si+1)] - voxelGrid.g[loc(ri+1,ci+1,si)];
  
  float dz = getChange(z1, z2, z3, z4, rf, cf); //may reverse fractions
  
  float scale = max(dx, max(dy, dz));
  
  
  vec3 gradient = vec3(dx/scale, dy/scale, dz/scale);
  
  return gradient;
  
}


float getVal(in int ri, in float rf, in int ci, in float cf, in int si, in float sf)
{
  //toggles trilinear interpolation
  if(tri == 1){
    return trilinearInterp(ri,rf,ci,cf,si,sf)*preGain;
  }else{
    return voxelGrid.g[loc(ri, ci, si)]*preGain;
  }
  
  
   //two bilinear an linear interp to simulate trilinear -- not as smooth
    // 	float interp1 = bilinearInterp(voxelGrid.g[loc(ri,ci,si)], voxelGrid.g[loc(ri,ci,si+1)], 
    // 				      voxelGrid.g[loc(ri+1,ci,si)], voxelGrid.g[loc(ri+1,ci,si+1)], rf, cf);
    // 	float interp2 = bilinearInterp(voxelGrid.g[loc(ri,ci+1,si)], voxelGrid.g[loc(ri,ci+1,si+1)], 
    // 				      voxelGrid.g[loc(ri+1,ci+1,si)], voxelGrid.g[loc(ri+1,ci+1,si+1)], rf, cf);
    //   
    // 	float final_interp = linearInterp(interp1, interp2, sf);
    // 	return final_interp; 
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





vec4 transferFunction(in float value, in vec4 rgba){
  
  vec4 curr_rgba = vec4(0.0,0.0,0.0,0.0);
  
  //depending on value will make a current rgba value
  //will do that here
  
  //find values to interpolate on
  int step = 0; 
  for(int i = 0; i < nSteps; i++){ //find area to interpolate on
   if(value >= rgba0Steps[i] && value <= rgba1Steps[i]){
    step = i; 
   }
  }
  
  float fraction = (value - rgba0Steps[step]) / (rgba1Steps[step] - rgba0Steps[step]); //percentage 
  curr_rgba = (1-fraction)*rgba0[step] + fraction*rgba1[step]; 
  
  //create scale for color
  float a_ds = curr_rgba.a * stepSize;
  
  curr_rgba = vec4(curr_rgba.x*a_ds, curr_rgba.y*a_ds, curr_rgba.z*a_ds, a_ds);
  
  //then accumulate values
  vec4 color = rgba +(1-rgba.a)*curr_rgba;
  
  return color;
  
}



vec4 traceRay(in vec3 mcPoint, in vec3 mcLineOfSight)
{
  int ri, ci, si;
  float rf, cf, sf;
  float v, maxVal = 0;
  float num = 0; 
  float vTotal = 0;
  bool binary = false;
  bool explicit = false;
  bool cont = true;
  vec3 normal; 
  vec4 accumRGBA = vec4(0.0,0.0,0.0,0.0);
  while (mcToRCS(mcPoint, ri, rf, ci, cf, si, sf) && cont) //what does this mean
  {
    v = getVal(ri, rf, ci, cf, si, sf); //i'm not really sure what's going on here
    
    if (rayFunction == 1){ // BINARY
      if(v >= rayFunctionParameter){//if hit point in ray that hits this threshold then turns it true
	binary = true;
	cont = false; 
      }
    }if (rayFunction == 2){ //finds overall max
      // BEGIN "MAX", part 1
      if (v > maxVal)
	maxVal = v;
      // END "MAX", part 1
    }else if (rayFunction == 3 || rayFunction == 4){ // AVERAGE
      num++;
      vTotal += v;
    }else if (rayFunction == 5 || rayFunction == 6){ //ACCUMRGB
      accumRGBA = transferFunction(v, accumRGBA);
      if(accumRGBA.w >= 1){//fully opaque drop out
	cont = false; 
      }
      
    }else if (rayFunction == 7){ // EXPLICIT_ISOSURFACE
      if(v >= rayFunctionParameter){//if hit point in ray that hits this threshold then turns it true
	normal = getNormal(ri, rf, ci, cf, si, sf);
	accumRGBA = transferFunction(v, accumRGBA);
	explicit = true;
	cont = false; 
      }
      
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
    float maxSum = 50 * 255.0; // this is the same as going: total / num = total ... total/maxValue
    v = vTotal / maxSum; //for scaling
    colorToReturn = vec4(v, v, v, 1.0);	
  }else if (rayFunction == 5){ 
    // BEGIN "ACCUMRGB_DIV_ACCUM_ALPHA" 
    accumRGBA = accumRGBA / accumRGBA.a;
    colorToReturn = accumRGBA;	
  }else if (rayFunction == 6){ 
    // BEGIN "ACCUMRGB_ADD_BACKGROUND" 
    vec4 backgroundColor = vec4(1.0,0.0,0.0,1.0);
    accumRGBA  =  accumRGBA +(1-accumRGBA.a)*backgroundColor;
    colorToReturn = accumRGBA;	
  }else if(rayFunction == 7){
    // BEGIN "EXPLICIT_ISOSURFACE"
    if(explicit){
	//float gray = 0.9 * normal.z;
	accumRGBA = accumRGBA * 0.9 * normal.z;
	float scale = max(accumRGBA.x, max(accumRGBA.y, max(accumRGBA.z, accumRGBA.a)));
	accumRGBA = accumRGBA / scale;
	//colorToReturn = vec4(gray, gray, gray, 1.0);
	colorToReturn = accumRGBA;
    }else{
     colorToReturn = vec4(0.0, 0.0, 0.0, 1.0);
    }
    
    
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

