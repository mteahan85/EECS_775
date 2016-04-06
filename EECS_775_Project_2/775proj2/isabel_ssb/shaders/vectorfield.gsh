#version 450 core

layout ( points ) in;
layout ( line_strip, max_vertices = 27 ) out; //triangle_strip

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
uniform float ldsVectorLengthScale = 10.0;

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



void getDot(in vec4 a, in vec4 b, out float dot){
  dot = a.x*b.x + a.y*b.y + a.z*b.z;
}

void getLength(in vec4 a, out float l){
  l = a.x*a.x + a.y*a.y + a.z*a.z;
  l = sqrt(l);
}

void getThetaAngle(in vec4 a, in vec4 b, out float angle){

  float dot_prod = 0; 
  float a_l = 0;
  float b_l = 0;
  
  getDot(a, b, dot_prod);
  getLength(a, a_l);
  getLength(b, b_l);
  
  float cos_theta = dot_prod / (a_l * b_l);
  
  angle = acos(cos_theta);

}

void getPhiAngle(in vec4 a,  out float angle){

  float rho = a.x*a.x + a.y*a.y + a.z*a.z;
  rho = sqrt(rho);
  
  
  
  angle = acos(a.z/rho);

}


void drawVector(in float u, in float v, in float w, in float speed)
{
/*
	//float speedIn0To1 = (speed - vectorLengthMin) / (vectorLengthMax - vectorLengthMin);
	// Make (u,v,w) be a unit vector
	u /= speed; v /= speed; w /= speed;
	
	vec4 original = gl_in[0].gl_Position;
	vec4 tip = original;
	
	//gl_in[0].gl_Position +
	//	ldsVectorLengthScale*vec4(scaleTrans[0]*u, scaleTrans[2]*v, w, 0.0);

	
	//rho
	//float rho = (tip.x -original.x)*(tip.x -original.x) + (tip.y -original.y)*(tip.y -original.y) + (tip.z -original.z)*(tip.z -original.z); //shifting center
	float rho = tip.x*tip.x + tip.y*tip.y + tip.z*tip.z;
	rho = sqrt(rho);
	
	float r = sqrt((tip.x*tip.x) + (tip.y*tip.y));
	
	
	//phi
	float phi = asin(r / rho);
	
	
	
	
	
	//float theta = acos((tip.x)/r);
	float theta = acos( (tip.x/ (rho*sin(phi))) ); 
	
	float r_polar = sqrt(original.x*original.x + original.y*original.y);
	float theta_p = atan(original.y/original.x);
	
	vec4 left =  vec4(r_polar*cos(theta_p), r_polar*sin(theta_p), original.z, original.w);
//	vec4 left =  vec4(rho*cos(theta)*sin(phi), rho*sin(theta)*sin(phi), rho*cos(phi), original.w);
	outputPoint(left);

	
	outputPoint(tip);
*/

//	outputPoint(gl_in[0].gl_Position);

	float speedIn0To1 = (speed - vectorLengthMin) / (vectorLengthMax - vectorLengthMin);
	// Make (u,v,w) be a unit vector
	u /= speed; v /= speed; w /= speed;
	vec4 diff = ldsVectorLengthScale*vec4(scaleTrans[0]*u, scaleTrans[2]*v, w, 0.0);
	outputPoint(gl_in[0].gl_Position + diff);
	outputPoint(gl_in[0].gl_Position + vec4(-diff.y, diff.x, 0.0, 0.0));
	outputPoint(gl_in[0].gl_Position + vec4(diff.y, -diff.x, 0.0, 0.0));
	


	
}

void drawCircle(in float u, in float v, in float w, in float speed)
{

	//outputPoint(gl_in[0].gl_Position); //center of circle
	vec4 origin = gl_in[0].gl_Position;
	float speedIn0To1 = (speed - vectorLengthMin) / (vectorLengthMax - vectorLengthMin);
	
	float PI = 3.14;
	float cur_rad = 0.0;
	float prev_rad = 0.0;
	for(int i = 0; i < 9; i++){
	  cur_rad = PI * i * 0.25; //around circle
	  
	  
	  outputPoint(origin); //center of circle
	  outputPoint(origin + 0.05*vec4(cos(prev_rad), sin(prev_rad), origin.z, 0.0));
	  outputPoint(origin + 0.05*vec4(cos(cur_rad), sin(cur_rad), origin.z, 0.0));
	  
	  prev_rad = cur_rad;
	
	}
}


void drawArrow(in float u, in float v, in float w, in float speed)
{

	outputPoint(gl_in[0].gl_Position);

	float speedIn0To1 = (speed - vectorLengthMin) / (vectorLengthMax - vectorLengthMin);
	// Make (u,v,w) be a unit vector
	u /= speed; v /= speed; w /= speed;
	outputPoint(gl_in[0].gl_Position +
		speedIn0To1*ldsVectorLengthScale*vec4(scaleTrans[0]*u, scaleTrans[2]*v, w, 0.0));
	
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
