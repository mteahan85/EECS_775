/** @file Grid.h
*@author Megan Teahan <mteahan@ku.edu>
*@id_number 2617777  
*/

#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Grid
{
public:
 
	/**Give warning if you haven't input any files*/
	Grid();
	~Grid();
	
	/**Takes in the file and then passes on the file into the read method for it to be read and stored in the list*/
	Grid(int cw, int ch, int nw, int nh, int c);
	
	void setFiles(ifstream& i, char* o);
	
	void interpBilinear();
	
	void interpBiCublic();
	
	
private:
 
	double*** original;
	double*** changed;
	double c_cr[4][4];
	char* output;
	//Catmull-Rom and Cardinal matrix
	
	int cur_width, cur_height;
	int new_width, new_height;
	int color;
	double r_width, r_height; //width and height ratio

	double*** initializeMat( int width, int height, int color);
	
	void read(ifstream& isFile) ;

	void deleteMat(double*** mat, int width, int height);
	
	double* spline_point(double t, double* point1, double* point2, double* point3, double* point4 );
	
	void calc_grid();
	
	double calc_bilinear_data(int ri, int ri_1, double rf, int ci, int ci_1, double cf, int rgb, double*** mat);
	
	void bilinear_row(int row_num);
	
	void bilinear_column(int column_num);
	
	void print_mat(double*** mat, int width, int height, int rgb);
		
};

#endif

