/** @file Grid.h
*@author Megan Teahan <mteahan@ku.edu>
*@id_number 2617777  
*/

#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <fstream>
#include <string>

class Grid
{

private:

	double** original;
	double** changed;
	
	int cur_width, cur_height;
	int new_width, new_height;
	int r_width, r_height; //width and height ratio

	
	static void read(std::ifstream& isFile) ;

	void calc_grid();
	
	double calc_bilinear_data(int ri, double rf, int ci, double cf);
	
	void bicublic_row(int row_num);
	
	void bilinear_row(int row_num);
	
	void bilinear_column(int column_num);
	
public:
	/**Give warning if you haven't input any files*/
	Grid();
	
	/**Takes in the file and then passes on the file into the read method for it to be read and stored in the list*/
	Grid(std::ifstream& intFile);

	void interpBilinear();
	
	void interpBiCublic();
};

#endif

