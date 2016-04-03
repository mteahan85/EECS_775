#version 450 core

layout (std430, binding = 0) buffer ScalarField
{
	float sf[];
} scalarField;

layout (std430, binding = 1) buffer TempField
{
	float sf[];
} temp;


const vec3 red = vec3(1.0, 0.0, 0.0);
const vec3 green = vec3(0.0, 1.0, .0);



uniform float scalarFieldMin, scalarFieldMax;
uniform float tempMin, tempMax;
uniform float precipMin, precipMax;


uniform int nRows, nCols;
uniform int nSheets;
uniform int sheetIndex = 0;
uniform int timestepIndex = 0;
uniform int scalarIndex = 0;

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
		
		if(scalarIndex == 0){ //--pressure
		    if (val > scalarFieldMax) // Missing Data
			  discard;
		    else
		    {
			    // map val to 0..1
			    float valIn0To1 = (val - scalarFieldMin) / (scalarFieldMax - scalarFieldMin);
			    fragmentColor = vec4(0.0, valIn0To1, 0.0, 1.0);
		    }
		  
		}else if(scalarIndex == 1){ //--temp
		    if (val > tempMax) // Missing Data
			  discard;
		    else
		    {
			    float valIn0To1 = (val - tempMin) / (tempMax - tempMin);
			    //divide temp into thresholds
			    float tempRange = tempMax - tempMin;
			    if(val < tempMin + (tempRange * 0.25 )){
			      fragmentColor = vec4(0.0, 1.0*valIn0To1, 0.0, 1.0);
			      
			    }else if(val < tempMin + (tempRange * 0.5 )){
			      fragmentColor = vec4(1.0*valIn0To1, 1.0*valIn0To1, 0.0, 1.0);
			      
			    }else if(val < tempMin + (tempRange * 0.75 )){
			      fragmentColor = vec4(1.0*valIn0To1, 0.5*valIn0To1, 0.0, 1.0);
			      
			    }else{
			      fragmentColor = vec4(1.0*valIn0To1, 0.0, 0.0, 1.0);
			      
			    }
			    
			    //map val to 0..1
			    //fragmentColor = vec4(0.0, valIn0To1, 0.0, 1.0);
		    }
		  
		}else if(scalarIndex == 2){ //--precip
		    if (val > precipMax) // Missing Data
			  discard;
		    else
		    {
			    // map val to 0..1
			    float valIn0To1 = (val - precipMin) / (precipMax - precipMin);
			    fragmentColor = vec4(0.0, valIn0To1, 0.0, 1.0);
		    }
		  
		}else{
		  
		  if (val > scalarFieldMax) // Missing Data
			discard;
		  else
		  {
			  // map val to 0..1
			  float valIn0To1 = (val - scalarFieldMin) / (scalarFieldMax - scalarFieldMin);
			  fragmentColor = vec4(0.0, valIn0To1, 0.0, 1.0);
		  }
		  
		}
		
	}
	else
		fragmentColor = vec4(color, 1.0);
}

