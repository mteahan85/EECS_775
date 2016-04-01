#version 450 core

layout ( points ) in;
layout ( line_strip, max_vertices = 10 ) out;

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

uniform float vectorLengthMin, vectorLengthMax;
uniform float ldsVectorLengthScale = 50.0;

uniform int nRows, nCols;
uniform int nSheets;
uniform int sheetIndex = 0;
uniform int timestepIndex = 0;

uniform vec4 scaleTrans;

in PVA
{
	vec2 texCoords;
} pva_in[];

out PVA
{
	vec2 texCoords;
} pva_out;

void outputPoint(vec4 p)
{
	pva_out.texCoords = pva_in[0].texCoords;
	gl_Position = p;
	EmitVertex();
}

void drawVector(in float u, in float v, in float w, in float speed)
{


	//will go around setting points at different positions
	//want bigger circle, the creater the speed -- potentially
	//start with just creating even circles
	//will need center point to get thrown inbetween
	//basically make min triangles
	
	float ang = 3.14 * 2.0 / 10.0 * 0;
	float ang2 = 3.14 * 2.0 / 10.0 * 5;
	vec4 offset = vec4(cos(ang) *0.05, -sin(ang) * 0.05, 0.0, 0.0);
	vec4 offset2 = vec4(cos(ang2) *0.05, -sin(ang2) * 0.05, 0.0, 0.0);
	vec4 center = gl_in[0].gl_Position;
	vec4 center2 = gl_in[0].gl_Position + offset2;
	center.x = (center.x + center2.x) /2;
	
	
	//center.x = center.x /2;
	
	for (int i = 0; i <= 1; i++) {
	  // Angle between each side in radians
	  ang = 3.14 * 2.0 / 10.0 * i;
	  offset = vec4(cos(ang) *0.05, -sin(ang) * 0.05, 0.0, 0.0);
	 
	  //first point
	  outputPoint(gl_in[0].gl_Position + offset);
	 
	  //center
	//  outputPoint(center);
	  
	 
	  
	  //second point
	  ang = 3.14 * 2.0 / 10.0 * (i+1);
	  offset = vec4(cos(ang) *0.05, -sin(ang) * 0.05, 0.0, 0.0);
	//  outputPoint(gl_in[0].gl_Position + offset);
	 
	}
	
	
/*	
	outputPoint(gl_in[0].gl_Position);

	float speedIn0To1 = (speed - vectorLengthMin) / (vectorLengthMax - vectorLengthMin);
	// Make (u,v,w) be a unit vector
	u /= speed; v /= speed; w /= speed;
	outputPoint(gl_in[0].gl_Position +
		speedIn0To1*ldsVectorLengthScale*vec4(scaleTrans[0]*u, scaleTrans[2]*v, w, 0.0));

*/

 		
}

void getVector(out float u, out float v, out float w)
{
	if ((pva_in[0].texCoords.s >= 0.0) && (pva_in[0].texCoords.s <= 1.0) &&
	    (pva_in[0].texCoords.t >= 0.0) && (pva_in[0].texCoords.t <= 1.0))
	{
		int col = int((1.0-pva_in[0].texCoords.t) * (nCols-1) + 0.5);
		int row = int(pva_in[0].texCoords.s * (nCols-1) + 0.5);
		int offset = row*nCols + col + sheetIndex*nRows*nCols + nSheets*nRows*nCols*timestepIndex; //need to include *timestepIndex somehow
		u = uvec.f[offset];
		v = vvec.f[offset];
		w = wvec.f[offset];
		if ((u > vectorLengthMax) || (v > vectorLengthMax) || (w > vectorLengthMax))
			u = v = w = 0.0; // missing data
	}
	else
		u = v = w = 0.0;
}

void main()
{
	float u, v, w;
	getVector(u,v,w);
	float speed = sqrt(u*u + v*v + w*w);
	if (speed > 0.0)
		drawVector(u, v, w, speed);
	// else - zero length vector ==> missing data; don't draw
}
