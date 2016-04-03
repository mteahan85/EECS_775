// GriddedData.c++ - a concrete 3D subclass of ModelViewWithPhongLighting 
#include <iostream>
#include <fstream>

#include "GriddedData.h"

ShaderIF::ShaderSpec GriddedData::scalarPgmSpec[2] =
	{
		{ "shaders/scalarfield.vsh", GL_VERTEX_SHADER },
		{ "shaders/scalarfield.fsh", GL_FRAGMENT_SHADER }
	};
ShaderIF::ShaderSpec GriddedData::vectorPgmSpec[5] =
	{
		{ "shaders/vectorfield.vsh", GL_VERTEX_SHADER },
		{ "shaders/vectorfield.tcsh", GL_TESS_CONTROL_SHADER },
		{ "shaders/vectorfield.tesh", GL_TESS_EVALUATION_SHADER },
		{ "shaders/vectorfield.gsh", GL_GEOMETRY_SHADER },
		{ "shaders/vectorfield.fsh", GL_FRAGMENT_SHADER }
	};
ShaderIF* GriddedData::shaderIF[2] = { NULL, NULL };
GLuint GriddedData::shaderProgram[2] = { 0, 0 };

// PVAs and PPUs for both scalar field and vector field:
GLint GriddedData::pvaLoc_mcCoords[2] = { -2, -2 };
GLint GriddedData::pvaLoc_texCoords[2] = { -2, -2 };
GLint GriddedData::ppuLoc_scaleTrans[2] = { -2, -2 };
GLint GriddedData::ppuLoc_color[2] = { -2, -2 };
GLint GriddedData::ppuLoc_nRows[2] = { -2, -2 };
GLint GriddedData::ppuLoc_nCols[2] = { -2, -2 };
GLint GriddedData::ppuLoc_nSheets[2] = { -2, -2 };
GLint GriddedData::ppuLoc_sheetIndex[2] = { -2, -2 };
GLint GriddedData::ppuLoc_timestepIndex[2] = { -2, -2 };
GLint GriddedData::ppuLoc_scalarIndex[2] = { -2, -2 };

// PVAs and PPUs just for scalar field:
GLint GriddedData::ppuLoc_scalarFieldMin = -2;
GLint GriddedData::ppuLoc_scalarFieldMax = -2;
GLint GriddedData::ppuLoc_tempMin = -2;
GLint GriddedData::ppuLoc_tempMax = -2;
GLint GriddedData::ppuLoc_precipMin = -2;
GLint GriddedData::ppuLoc_precipMax = -2;

// PVAs and PPUs just for vector field:
GLint GriddedData::ppuLoc_vectorLengthMin = -2;
GLint GriddedData::ppuLoc_vectorLengthMax = -2;
GLint GriddedData::ppuLoc_tessLevel = -2;

int GriddedData::numInstances = 0;

static const float missingDataFlag = 1.0e35;

GriddedData::GriddedData(int nRowsIn, int nColsIn, int nSheetsIn, int nTimestepsIn,
	float* attrArrayIn, float* tempIn, float* precipIn, float* uCompIn, float* vCompIn, float* wCompIn,
	bool noGeometryShader) :
		nRows(nRowsIn), nCols(nColsIn), nSheets(nSheetsIn), nTimesteps(nTimestepsIn), sheetIndex(0), timestepIndex(0), scalarIndex(0),
		attrArray(attrArrayIn), temp(tempIn), precip(precipIn),  uComp(uCompIn), vComp(vCompIn), wComp(wCompIn),
		scalarFieldMin(1.0), scalarFieldMax(0.0), tempMin(1.0), tempMax(0.0), precipMin(1.0), precipMax(0.0),
		vectorLengthMin(1.0), vectorLengthMax(0.0), tessLevel(20)
{
	ShaderIF::ShaderSpec* specs[] = { scalarPgmSpec, vectorPgmSpec };
	int numComponents[] = { 2, 5 };
	if (noGeometryShader)
	{
		vectorPgmSpec[3] = vectorPgmSpec[4];
		numComponents[1] = 4;
	}
	for (int si=0 ; si<2 ; si++)
		if (shaderIF[si] == NULL)
		{
			shaderIF[si] = new ShaderIF(specs[si], numComponents[si]);
			shaderProgram[si] = shaderIF[si]->getShaderPgmID();
			fetchGLSLVariableLocations(si);
		}
	numInstances++;

	// data defined at vertices, hence geometry is, e.g.: 0<=x<=nCols-1
	xyzMinMax[0] = 0.0; xyzMinMax[1] = nCols - 1;
	xyzMinMax[2] = 0.0; xyzMinMax[3] = nRows - 1;
	xyzMinMax[4] = 0.0; xyzMinMax[5] = nSheets - 1;
	defineGridGeometry();
	int nValues = nRows * nCols * nSheets * nTimesteps;
	minMax(attrArray, nValues, scalarFieldMin, scalarFieldMax, "scalarField");
	minMax(temp, nValues, tempMin, tempMax, "Temp");
	minMax(precip, nValues, precipMin, precipMax, "Precip");
	float theUMin = 1.0, theVMin = 1.0, theWMin = 1.0;
	float theUMax = 0.0, theVMax = 0.0, theWMax = 0.0;
	minMax(uComp, nValues, theUMin, theUMax, "U");
	minMax(vComp, nValues, theVMin, theVMax, "V");
	minMax(wComp, nValues, theWMin, theWMax, "W");
	minMax(uComp, vComp, wComp, nValues, vectorLengthMin, vectorLengthMax, "uvwLength");
	sendBufferData();
}

GriddedData::~GriddedData()
{
	glDeleteBuffers(1, vboScalarField);
	glDeleteBuffers(3, vboVectorField);
	glDeleteBuffers(2, vboGrid);
	glDeleteVertexArrays(1, vaoGrid);

	if (--numInstances == 0)
	{
		for (int i=0 ; i<2 ; i++)
		{
			if (shaderIF[i] != NULL)
			{
				shaderIF[i]->destroy();
				delete shaderIF[i];
				shaderIF[i] = NULL;
				shaderProgram[i] = 0;
			}
		}
	}
}

void GriddedData::defineGridGeometry()
{
	typedef float vec2[2];

	glGenVertexArrays(1, vaoGrid);
	glBindVertexArray(vaoGrid[0]);

	glGenBuffers(2, vboGrid);

	// Define a 2D rectangle for the grid. We will display single
	// slices of constant z value. Create it as a 2-triangle
	// GL_TRIANGLE_STRIP.
	vec2 grid[] =
	{
		{ xyzMinMax[0], xyzMinMax[2] },
		{ xyzMinMax[0], xyzMinMax[3] },
		{ xyzMinMax[1], xyzMinMax[2] },
		{ xyzMinMax[1], xyzMinMax[3] }
	};
	// Define the corresponding texture coordinates.
	vec2 tex[] =
	{
		{ 0.0, 0.0 }, { 0.0, 1.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }
	};
	int numBytes = 4 * sizeof(vec2);

	glBindBuffer(GL_ARRAY_BUFFER, vboGrid[0]);
	glBufferStorage(GL_ARRAY_BUFFER, numBytes, grid, 0);
	glVertexAttribPointer(pvaLoc_mcCoords[0], 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(pvaLoc_mcCoords[0]);

	glBindBuffer(GL_ARRAY_BUFFER, vboGrid[1]);
	glBufferStorage(GL_ARRAY_BUFFER, numBytes, tex, 0);
	glVertexAttribPointer(pvaLoc_texCoords[0], 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(pvaLoc_texCoords[0]);
}

void GriddedData::fetchGLSLVariableLocations(int i)
{
	if (shaderProgram[i] > 0)
	{
		pvaLoc_mcCoords[i] = pvAttribLocation(shaderProgram[i],
			"mcCoords");
		pvaLoc_texCoords[i] = pvAttribLocation(shaderProgram[i],
			"texCoords");
		ppuLoc_scaleTrans[i] = ppUniformLocation(shaderProgram[i],
			"scaleTrans");
		ppuLoc_color[i] = ppUniformLocation(shaderProgram[i],
			"color");
		ppuLoc_nRows[i] = ppUniformLocation(shaderProgram[i],
			"nRows");
		ppuLoc_nCols[i] = ppUniformLocation(shaderProgram[i],
			"nCols");
		ppuLoc_nSheets[i] = ppUniformLocation(shaderProgram[i],
			"nSheets");
		ppuLoc_sheetIndex[i] = ppUniformLocation(shaderProgram[i],
			"sheetIndex");
		ppuLoc_timestepIndex[i] = ppUniformLocation(shaderProgram[i],
			"timestepIndex");
		ppuLoc_scalarIndex[i] = ppUniformLocation(shaderProgram[i],
			"scalarIndex");
		if (i == 0) // scalar field program
		{
			ppuLoc_scalarFieldMin = ppUniformLocation(shaderProgram[0],
				"scalarFieldMin");
			ppuLoc_scalarFieldMax = ppUniformLocation(shaderProgram[0],
				"scalarFieldMax");
			ppuLoc_tempMin = ppUniformLocation(shaderProgram[0],
				"tempMin");
			ppuLoc_tempMax = ppUniformLocation(shaderProgram[0],
				"tempMax");
			ppuLoc_precipMin = ppUniformLocation(shaderProgram[0],
				"precipMin");
			ppuLoc_precipMax = ppUniformLocation(shaderProgram[0],
				"precipMax");
		}
		else // vector field program
		{
			ppuLoc_vectorLengthMin = ppUniformLocation(shaderProgram[1],
				"vectorLengthMin");
			ppuLoc_vectorLengthMax = ppUniformLocation(shaderProgram[1],
				"vectorLengthMax");
			ppuLoc_tessLevel = ppUniformLocation(shaderProgram[1],
				"tessLevel");
		}
	}
}

void GriddedData::getMCBoundingBox(double* xyzLimits) const
{
	for (int i=0 ; i<6 ; i++)
		xyzLimits[i] = xyzMinMax[i];
}

void GriddedData::handleCommand(unsigned char key, double ldsX, double ldsY)
{
	if (key == '+')
	{
		if (++sheetIndex >= nSheets)
			sheetIndex = 0;
	}
	else if (key == '-')
	{
		if (--sheetIndex < 0)
			sheetIndex = nSheets - 1;
	}
	else if (key == 't')
	{
		tessLevel -= 2;
		if (tessLevel <= 2)
			tessLevel = 2;
	}
	else if (key == 'T')
	{
		tessLevel += 2;
	}else if (key == 'a'){
	  //will go backwards in timesteps
	    if(--timestepIndex < 0){
		timestepIndex = nTimesteps - 1; 
	    }
	  
	}else if (key == 's'){
	  //will go forward in timesteps
	    if(++timestepIndex >= nTimesteps){
		timestepIndex = 0;
	    }
	}else if (key == 'c'){
	  //will go forward in timesteps
	    if(++scalarIndex >= 4){
		scalarIndex = 0;
	    }
	}
	else
		this->ModelView::handleCommand(key, ldsX, ldsY);
}

void GriddedData::minMax(float* arr, int nVals, float& theMin, float& theMax,
	std::string what)
{
	for (int i=0 ; i<nVals ; i++)
	{
		if (arr[i] == missingDataFlag)
			continue;
		if (theMin > theMax)
			theMin = theMax = arr[i];
		else if (arr[i] < theMin)
			theMin = arr[i];
		else if (arr[i] > theMax)
			theMax = arr[i];
	}
	std::cout << theMin << " <= " << what << " <= " << theMax << '\n';
}

void GriddedData::minMax(float* u, float* v, float* w, int nVals,
	float& theMin, float& theMax, std::string what)
{
	for (int i=0 ; i<nVals ; i++)
	{
		if ((u[i] == missingDataFlag) || (v[i] == missingDataFlag) ||
		    (w[i] == missingDataFlag))
			continue;
		float speedSqr = u[i]*u[i] + v[i]*v[i] + w[i]*w[i];
		float speed = sqrt(speedSqr);
		if (theMin > theMax)
			theMin = theMax = speed;
		else if (speed < theMin)
			theMin = speed;
		else if (speed > theMax)
			theMax = speed;
	}
	std::cout << theMin << " <= " << what << " <= " << theMax << '\n';
}

static GLenum mode[] = { GL_TRIANGLE_STRIP, GL_PATCHES };

void GriddedData::render()
{
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);

	for (int i=0 ; i<2 ; i++)
	{
		if (shaderProgram[i] <= 0)
			continue;

		glUseProgram(shaderProgram[i]);

		float sclTrans[4];
		compute2DScaleTrans(sclTrans);
		glUniform4fv(ppuLoc_scaleTrans[i], 1, sclTrans);
		glUniform1i(ppuLoc_nRows[i], nRows);
		glUniform1i(ppuLoc_nCols[i], nCols);
		glUniform1i(ppuLoc_nSheets[i], nSheets);
		glUniform1i(ppuLoc_sheetIndex[i], sheetIndex);
		glUniform1i(ppuLoc_timestepIndex[i], timestepIndex);
		glUniform1i(ppuLoc_scalarIndex[i], scalarIndex);

		if (i == 0) // scalar field program
		{
			glUniform1f(ppuLoc_scalarFieldMin, scalarFieldMin);
			glUniform1f(ppuLoc_scalarFieldMax, scalarFieldMax);
			glUniform1f(ppuLoc_tempMin, tempMin);
			glUniform1f(ppuLoc_tempMax, tempMax);
			glUniform1f(ppuLoc_precipMin, precipMin);
			glUniform1f(ppuLoc_precipMax, precipMax);
		}
		else // vector field program
		{
			glUniform1f(ppuLoc_vectorLengthMin, vectorLengthMin);
			glUniform1f(ppuLoc_vectorLengthMax, vectorLengthMax);
			glUniform1i(ppuLoc_tessLevel, tessLevel);
			glPointSize(3.0);
		}

		glBindVertexArray(vaoGrid[0]);
		// Draw a 2-triangle square: (0,0)->(nCols-1, nRows-1)
		glDrawArrays(mode[i], 0, 4);
	}

	glUseProgram(pgm);
}

void GriddedData::sendBufferData()
{
	int numBytes = nCols * nRows * nSheets * nTimesteps *  sizeof(float);

	// The scalar field:
	glGenBuffers(1, vboScalarField);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vboScalarField[0]);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, numBytes, temp, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vboScalarField[0]);

	// The vector field:
	glGenBuffers(3, vboVectorField);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vboVectorField[0]);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, numBytes, uComp, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vboVectorField[0]);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vboVectorField[1]);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, numBytes, vComp, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, vboVectorField[1]);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vboVectorField[2]);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, numBytes, wComp, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, vboVectorField[2]);
}
