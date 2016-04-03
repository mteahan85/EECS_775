// main.c++

#include <iostream>
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <string.h>
#include <sstream>

#include "GLFWController.h"
#include "GriddedData.h"
#include "Endian.h"

void initializeViewingInformation(Controller& c)
{
	// determine the center of the scene:
	double xyz[6];
	c.getOverallMCBoundingBox(xyz);
	ModelView::setMCRegionOfInterest(xyz);
	double xmid = 0.5 * (xyz[0] + xyz[1]);
	double ymid = 0.5 * (xyz[2] + xyz[3]);
	double zmid = 0.5 * (xyz[4] + xyz[5]);

	// a heuristic: arrange the eye and center points so that the distance
	// between them is (2*max scene dimension)
	double maxDelta = xyz[1] - xyz[0];
	double delta = xyz[3] - xyz[2];
	if (delta > maxDelta)
		maxDelta = delta;
	delta = xyz[5] - xyz[4];
	if (delta > maxDelta)
		maxDelta = delta;
	double distEyeCenter = 2.0 * maxDelta;

	cryph::AffPoint center(xmid, ymid, zmid);
	cryph::AffPoint eye(xmid, ymid, zmid + distEyeCenter);
	cryph::AffVector up = cryph::AffVector::yu;

	ModelView::setEyeCenterUp(eye, center, up);

	// Place the projection plane roughly at the front of scene
	// and set eye coordinate zmin/zmax planes relative to it.
	double zpp = -(distEyeCenter - 0.5*maxDelta);
	ModelView::setProjectionPlaneZ(zpp);
	ModelView::setECZminZmax(zpp - maxDelta, zpp+0.5*maxDelta);

	ModelView::setProjection(ORTHOGONAL);
}

// Parameters for the isabel data files:
const int nRows = 500;
const int nCols = 500;
const int nSheets = 3; //-- mine -- only reads 3 sheets
//const int nSheets = 100; //-- Miller's 
const int nTimeSteps = 48;

float* readAttributeFile(const char* fName)
{
	
	std::ifstream is(fName);
	if (is.good())
		std::cout << "Reading " << fName << "...";
	else
	{
		std::cerr << "Could not open " << fName << " for reading.\n";
		return NULL;
	}
	int nValues = nRows * nCols * nSheets;
	float* buf = new float[nValues];
	if (buf == NULL)
		std::cerr << "Could not allocate memory for " << fName << '\n';
	else
	{
		is.read(reinterpret_cast<char*>(buf), nValues*sizeof(float));
		if (Endian::getEndian() == Endian::LITTLE)
			Endian::swapBytes(buf,nValues);
	}
	std::cout << std::endl;
	return buf;
}

void addToArray(float* total, float* newData, int pos){

  int nValues =  nRows * nCols * nSheets;
  int offset = pos * nValues; 
  for(int i = 0; i < nValues; i++){
    total[i+offset] = newData[i];
  }  
}

float* readFiles(std::string location, std::string file){
  
  
  char fileName[8];
  int nValues = nRows * nCols * nSheets * nTimeSteps;
  float* total = new float[nValues];
  std::string fileNum = "", path;
  for(int i = 0; i < nTimeSteps; i++){
    
    std::stringstream convert;
     convert << i+1; 
    if(i+1 < 10){
     fileNum =  "0" + convert.str();
    }else{
     fileNum = "" + convert.str() ; 
    }
    path = location + file + fileNum + ".bin";
    std::cout << path << "\n";
    const char* cstr = path.c_str();
    float* newData = readAttributeFile(cstr);
    addToArray(total, newData, i);
    
    
   // delete [] cstr;
  }
  
  return total;
  
}


int main(int argc, char* argv[])
{
//   std::cout << "Read all the files \n";
//   float* total = readFiles("/home/miller/pub/Pf/", "Pf");
//   std::cout << "Done Read all the files \n";
//   int nValues = nRows * nCols * nSheets * nTimeSteps;
//   for (int i = 0; i < nValues; i++){
//    std::cout << total[i] << " ";
//    
//    if(i % 500 == 0){
//     std::cout << " \n"; 
//    }
//    
//    
//   }
  
  
  
  
	GLFWController c("Gridded Isabel Data");
	c.reportVersions(std::cout);

	//float* attrArray = readFiles("/home/miller/pub/PRECIPf/", "PRECIPf");
	//float* attrArray = readFiles("/home/miller/pub/TCf/", "TCf");
	float* temp = readFiles("/home/miller/pub/TCf/", "TCf"); 
	if (temp == NULL)
		exit(1);
	float* precip = readFiles("/home/miller/pub/PRECIPf/", "PRECIPf");
	if (precip == NULL)
		exit(1);
	float* pressure = readFiles("/home/miller/pub/Pf/", "Pf"); //= readAttributeFile("/home/miller/pub/Pf/Pf01.bin");
	if (pressure == NULL)
		exit(1);
	float* uComponent = readFiles("/home/miller/pub/UVWf/", "Uf"); //readAttributeFile("/home/miller/pub/UVWf/Uf01.bin");
	if (uComponent == NULL)
		exit(1);
	float* vComponent = readFiles("/home/miller/pub/UVWf/", "Vf");//readAttributeFile("/home/miller/pub/UVWf/Vf01.bin");
	if (vComponent == NULL)
		exit(1);
	float* wComponent = readFiles("/home/miller/pub/UVWf/", "Wf");//readAttributeFile("/home/miller/pub/UVWf/Wf01.bin");
	if (wComponent == NULL)
		exit(1);

	bool noGeometryShader = false;
	if (argc == 2)
		if (strcmp(argv[1], "-nogs") == 0)
			noGeometryShader = true;
	GriddedData* gd = new GriddedData(nRows, nCols, nSheets, nTimeSteps,
		pressure, temp, precip, uComponent, vComponent, wComponent, noGeometryShader);
	c.addModel(gd);

	initializeViewingInformation(c);

	glClearColor(1.0, 1.0, 1.0, 1.0);

	c.run();

	return 0;
}
