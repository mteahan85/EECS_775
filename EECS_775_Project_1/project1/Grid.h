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
	
	/**Takes in the file and then passes on the file into the read method for it to be read and stored in the list*/
	Grid(int cw, int ch, int nw, int nh, ifstream& intFile);
	void interpBilinear();
	
	void interpBiCublic();
	
	
private:
 typedef double vec3[3];
	vec3** original;
	vec3** changed;
	double** c_cr; //Catmull-Rom and Cardinal matrix
	
	int cur_width, cur_height;
	int new_width, new_height;
	double r_width, r_height; //width and height ratio

	
	void read(ifstream& isFile) ;

	double** mat_mult(double** mat_1, double** mat_2);
	
	void calc_grid();
	
	double calc_bilinear_data(int ri, double rf, int ci, double cf, int rgb, vec3** mat);
	
	void bicublic_row(int row_num);
	
	void bilinear_row(int row_num);
	
	void bilinear_column(int column_num);
	
	void print_row(vec3** mat, int row, int height);
	
	void print_mat(vec3** mat, int width, int height);
		
};

#endif

