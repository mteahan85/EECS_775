// GriddedData.h - a concrete 3D subclass of ModelViewWithPhongLighting

#ifndef GRIDDEDDATA_H
#define GRIDDEDDATA_H

#include <string>

#include "ModelView.h"
#include "ShaderIF.h"

class GriddedData : public ModelView
{
public:
	GriddedData(int nRowsIn, int nColsIn, int nSheetsIn, int nTimestepsIn,
		float* attrArrayIn, float* tempIn, float* precipIn,
		float* uCompIn, float* vCompIn, float* wCompIn,
		bool noGeometryShader);
	virtual ~GriddedData();

	void getMCBoundingBox(double* xyzLimits) const;
	void handleCommand(unsigned char key, double ldsX, double ldsY);
	void render();

private:
	GLuint vaoGrid[1];
	GLuint vboGrid[2]; // mcPosition & texCoords

	// Shader Storage Buffers (not placed in VAOs)
	GLuint vboScalarField[1];
	GLuint vboVectorField[3];

	int nRows, nCols, nSheets, nTimesteps;
	int sheetIndex, timestepIndex, scalarIndex; // currently being displayed
	float *attrArray, *temp, *precip, *uComp, *vComp, *wComp;
	double xyzMinMax[6];
	float scalarFieldMin, scalarFieldMax, tempMin, tempMax, precipMin, precipMax;
	float vectorLengthMin, vectorLengthMax;
	int tessLevel;

	// We will have two shader programs - one for the scalar field; one
	// for the vector field:
	static ShaderIF::ShaderSpec scalarPgmSpec[2], vectorPgmSpec[5];
	static ShaderIF* shaderIF[2];
	static GLuint shaderProgram[2];

	// PVAs and PPUs for both scalar field and vector field:
	static GLint pvaLoc_mcCoords[2], pvaLoc_texCoords[2];
	static GLint ppuLoc_scaleTrans[2], ppuLoc_color[2];
	static GLint ppuLoc_nRows[2], ppuLoc_nCols[2], ppuLoc_nSheets[2], ppuLoc_sheetIndex[2], ppuLoc_timestepIndex[2], ppuLoc_scalarIndex[2];

	// PVAs and PPUs just for scalar field:
	static GLint ppuLoc_scalarFieldMin, ppuLoc_scalarFieldMax, ppuLoc_tempMin, ppuLoc_tempMax, ppuLoc_precipMin, ppuLoc_precipMax;

	// PVAs and PPUs just for vector field:
	static GLint ppuLoc_vectorLengthMin, ppuLoc_vectorLengthMax,
	             ppuLoc_tessLevel;

	static int numInstances;

	static void fetchGLSLVariableLocations(int si);
	static void minMax(float* arr, int nVals, float& theMin, float& theMax,
		std::string what);
	static void minMax(float* u, float* v, float* w, int nVals,
		float& theMin, float& theMax, std::string what);

	void defineGridGeometry();
	void sendBufferData();
};

#endif
