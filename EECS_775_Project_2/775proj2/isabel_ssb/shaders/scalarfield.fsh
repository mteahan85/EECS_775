#version 450 core

layout (std430, binding = 4) buffer PressureField
{
	float sf[];
} pressure;

layout (std430, binding = 5) buffer TempField
{
	float sf[];
} temp;
// layout (std430, binding = 6) buffer PrecipField
// {
// 	float sf[];
// } precip;


const vec3 red = vec3(1.0, 0.0, 0.0);
const vec3 green = vec3(0.0, 1.0, .0);

//will eventually need to find a way to only use certain parts of each data
//like some precip and some temperature not sure how to layer certain parts.
//maybe only track temperature for above a certain threshold

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

void colorPressure(in float val){
  
  //using colorbrewer values to create even steps to show up steps in pressure
			    // map val to 0..1
			    float valIn0To1 = (val - scalarFieldMin) / (scalarFieldMax - scalarFieldMin);
			    if(valIn0To1 > 0.90){
			      fragmentColor = vec4(0.97, 0.98, 1.0, 1.0);
			    }else if(valIn0To1 > 0.80){
			      fragmentColor = vec4(0.87, 0.92, 0.97, 1.0);
			    }else if(valIn0To1 > 0.70){
			      fragmentColor = vec4(0.78, 0.86, 0.94, 1.0);
			    }else if(valIn0To1 > 0.60){
			      fragmentColor = vec4(0.62, 0.79, 0.88, 1.0);
			    }else if(valIn0To1 > 0.50){
			      fragmentColor = vec4(0.42, 0.68, 0.84, 1.0);
			    }else if(valIn0To1 > 0.40){
			      fragmentColor = vec4(0.26, 0.57, 0.78, 1.0);
			    }else if(valIn0To1 > 0.30){
			      fragmentColor = vec4(0.13, 0.44, 0.71, 1.0);
			    }else if(valIn0To1 > 0.20){
			      fragmentColor = vec4(0.03, 0.32, 0.61, 1.0);
			    }else if(valIn0To1 > 0.10){
			      fragmentColor = vec4(0.03, 0.19, 0.42, 1.0);
			    }else{
			      fragmentColor = vec4(0.03, 0.06, 0.17, 1.0); 
			    }
  
}



void main()
{
	if ((pvaIn.texCoords.s >= 0.0) && (pvaIn.texCoords.s <= 1.0) &&
	    (pvaIn.texCoords.t >= 0.0) && (pvaIn.texCoords.t <= 1.0))
	{
		int col = int((1.0-pvaIn.texCoords.t) * (nCols-1) + 0.5);
		int row = int(pvaIn.texCoords.s * (nRows-1) + 0.5);
		int offset = row*nCols + col + sheetIndex*nRows*nCols + nSheets*nRows*nCols*timestepIndex ; //use timestepIndex
		float val = temp.sf[offset];
		
		if(scalarIndex == 0){ //--pressure
		 val = pressure.sf[offset];
		
		    if (val > scalarFieldMax) // Missing Data
			  discard;
		    else
		    {	
			    colorPressure(val);
		    }
		  
		}else if(scalarIndex == 1){ //--temp
		    val = temp.sf[offset];
		    if (val > tempMax) // Missing Data
			  discard;
		    else
		    {
			    float valIn0To1 = (val - tempMin) / (tempMax - tempMin);
			    
			    //interpolates from green to red
			    if(valIn0To1 < 0.50){
			      fragmentColor = vec4(2.0 * valIn0To1, 1.0, 0.0, 1.0);
			    }else{
			      fragmentColor = vec4(1.0, 2.0 * (1.0 - valIn0To1), 0.0, 1.0);
			    }
			    
			    
			    //divide temp into thresholds
// 			    float tempRange = tempMax - tempMin;
// 			    if(val < tempMin + (tempRange * 0.25 )){
// 			      fragmentColor = vec4(0.0, 1.0*valIn0To1, 0.0, 1.0);
// 			      
// 			    }else if(val < tempMin + (tempRange * 0.5 )){
// 			      fragmentColor = vec4(1.0*valIn0To1, 1.0*valIn0To1, 0.0, 1.0);
// 			      
// 			    }else if(val < tempMin + (tempRange * 0.75 )){
// 			      fragmentColor = vec4(1.0*valIn0To1, 0.5*valIn0To1, 0.0, 1.0);
// 			      
// 			    }else{
// 			      fragmentColor = vec4(1.0*valIn0To1, 0.0, 0.0, 1.0);
// 			      
// 			    }
			    
			    //map val to 0..1
			    //fragmentColor = vec4(0.0, valIn0To1, 0.0, 1.0);
		    }
		  
		}/*else if(scalarIndex == 2){ //--precip
		  val = precip.sf[offset];
		    if (val > precipMax) // Missing Data
			  discard;
		    else //--need to find cool visual variable way to do this. so far used color and hue
		    { 
			    // map val to 0..1
			    float valIn0To1 = (val - precipMin) / (precipMax - precipMin);
			    if(valIn0To1 < 0.99){
			      fragmentColor = vec4(2.0 * valIn0To1, 1.0, 0.0, 1.0);
			    }else{
			      fragmentColor = vec4(1.0, 2.0 * (1.0 - valIn0To1), 0.0, 1.0);
			    }
			    
			    
			    
			    //fragmentColor = vec4(0.0, valIn0To1, 0.0, 1.0);
		    }
		  
		}*/else{ //--cummulation of types
		  
		  if (val > scalarFieldMax) // Missing Data
			discard;
		  else
		  {
			  //creates pressure field colors
			  colorPressure(pressure.sf[offset]);
			  val = temp.sf[offset];
			  float valIn0To1 = (val - tempMin) / (tempMax - tempMin);
			 
// 			    //interpolates from green to red
// 			    if(valIn0To1 > 0.85){
// 			      fragmentColor = vec4(2.0 * valIn0To1*.85, 1.0, 0.0, 1.0);
// 			    }
// 			  
// 			    if(valIn0To1 > 0.90){
// 			      fragmentColor = vec4(1.0, 2.0 * (1.0 - valIn0To1*.85), 0.0, 1.0);
// 			    }
			    
			  
		  }
		  
		}
		
	}
	else
		fragmentColor = vec4(color, 1.0);
}

