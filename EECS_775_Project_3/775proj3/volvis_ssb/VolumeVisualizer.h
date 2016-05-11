// VolumeVisualizer.h - a concrete 3D subclass of ModelView

#ifndef VOLUMEVISUALIZER_H
#define VOLUMEVISUALIZER_H

#include <string>

#include "ModelView3D.h"
#include "BasicShapeModelView.h"

class VolumeVisualizer : public ModelView3D
{
public:
	VolumeVisualizer(int nRowsIn, int nColsIn, int nSheetsIn,
		double rowScaleFactor, double colScaleFactor, double sheetScaleFactor, int* attrArrayIn,
		float* rgba0In, float* rgba1In, int nStepsIn, int* rgba0StepsIn, int* rgba1StepsIn);
	virtual ~VolumeVisualizer();

	void getMCBoundingBox(double* xyzLimits) const;
	void handleCommand(unsigned char key, double ldsX, double ldsY);
	void handleFunctionKey(int whichFunctionKey, double ldsX, double ldsY,
		int mods);
	void handleSpecialKey(Controller::SpecialKey key,
		double ldsX, double ldsY, int mods);
	void render();

private:
	BasicShapeModelView* voxelGridCubeMV;
	// Shader Storage Buffers (not placed in VAOs)
	GLuint vboVoxelGrid[1];

	int nRows, nCols, nSheets, nSteps;
	float cellSizeX, cellSizeY, cellSizeZ;
	int* attrArray;
	int* rgba0Steps;
	int* rgba1Steps;
	float* rgba0;
	float* rgba1;
	double xyzMinMax[6];
	int rayFunction, rayFunctionParameter, tri;
	float stepSize, preGain;

	// PPUs for voxel grid
	static GLint ppuLoc_nRows, ppuLoc_nCols, ppuLoc_nSheets;
	static GLint ppuLoc_cellSizeX, ppuLoc_cellSizeY, ppuLoc_cellSizeZ;
	// PPUs for rendering options
	static GLint ppuLoc_rayFunction, ppuLoc_rayFunctionParameter, ppuLoc_tri; 
	static GLint ppuLoc_stepSize, ppuLoc_preGain;
	static GLint ppuLoc_rgba0, ppuLoc_rgba1, ppuLoc_rgba0Steps, ppuLoc_rgba1Steps, ppuLoc_nSteps;
	

	static void fetchGLSLVariableLocations();

	void sendBufferData();
};

#endif
