#version 450 core

layout (std430, binding = 1) buffer UVec
{
	float f[];
} uvec;
layout (std430, binding = 2) buffer VVec
{
	float f[];
} vvec;
layout (std430, binding = 3) buffer WVec
{
	float f[];
} wvec;

layout (std430, binding = 5) buffer TempField
{
	float sf[];
} temp;

uniform float vectorLengthMin, vectorLengthMax;

uniform int nRows, nCols;
uniform int nSheets;
uniform int sheetIndex = 0;
uniform int timestepIndex = 0;
uniform int scalarIndex = 0;

uniform float tempMin, tempMax;

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
		int offset = row*nCols + col + sheetIndex*nRows*nCols + nSheets*nRows*nCols*timestepIndex;//*timestepIndex -- somehow timestepIndex*
		float dx = uvec.f[offset];
		float dy = vvec.f[offset];
		float dz = wvec.f[offset];
		
		if(scalarIndex <= 1){
		
		  if ((dx > vectorLengthMax) || (dy > vectorLengthMax) || (dz > vectorLengthMax))
			  discard; // Missing Data
		  else
		  {
			  float speed = sqrt(dx*dx + dy*dy + dz*dz);
			  // map val to 0..1
			  float valIn0To1 = (speed - vectorLengthMin) / (vectorLengthMax - vectorLengthMin);
			  fragmentColor = vec4(0.0, valIn0To1, 0.0, 1.0);
		  }
		
		}else{
			    float val = temp.sf[offset];
			    if (val > tempMax){ // Missing Data
				   fragmentColor = vec4(0.0,0.0,1.0, 1.0);
			    }	
			    
			    //fragmentColor = vec4(1.0,1.0,1.0, 1.0);
			    
			    float val_new = (val - tempMin) / (tempMax - tempMin);
			    
			    //interpolates from green to red
			    if(val_new < 0.50){
			      fragmentColor = vec4(2.0 * val_new, 1.0, 0.0, 1.0);
			    }else{
			      fragmentColor = vec4(1.0, 2.0 * (1.0 - val_new), 0.0, 1.0);
			    }
		  
		  
		}
		
		
	}
	else
		fragmentColor = vec4(color, 1.0);
}

