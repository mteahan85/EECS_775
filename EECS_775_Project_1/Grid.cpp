/**
*@file Grid.cpp
*@author Megan Teahan <mteahan@ku.edu>
*@version 1.0
*
*@section DESCRIPTION
*	Executive.cpp takes in the streams that area passed in through the main.cpp file. It reads those files, passing the information to the corresponding CountList<T> and then having the ability to print the results of storing that information.
*
*
*/

#include "Grid.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>


/***/
void Grid::read(std::ifstream& isFile) 
{
  //will read the text file and store it in a matrix

}
/**
* Constructor tells if nothing has passed in or not.
*/
Grid::Grid()
{

	std::cout << "No text files were inputted to be read. \n";

}


void Grid::calc_grid()
{
  //will need to check for reduction or expansion
  
  r_width = cur_width /  new_width;
  r_height = cur_height / new_height;
  
  
}

double Grid::calc_bilinear_data(int ri, double rf, int ci, double cf)
{

  double fir_part = (1 - rf) * ( ((1-cf) * original[ri][ci]) + (cf * original[ri][ci+1]) );
  double sec_part = rf * ( ((1-cf) * original[ri-1][ci]) + (cf * original[ri-1][ci+1]));
  
  return fir_part + sec_part;
}


//will do Bilinear Interoplation on the Grid
void Grid::interpBilinear()
{

}

//will do BiCublic Interplation on the Grid
void Grid::interpBiCublic()
{

}

//will cublically interpolate row
void Grid::bicublic_row(int row_num)
{
  //will have to start 1 row and 1 column in and stop 1 row and 1 column short
  //the points in the Cardinal Splines or Catmull & Rom splines could potetially correspond to grid vertices
  //what is important is picking the vertices used to calculate the new values.
  //might not be down the line, but rather a circle around the points. 
  
}



//will linearing interpolate row
void Grid::bilinear_row(int row_num)
{

  //increment the value based on the cf = r_width (the column)
  //will keep the row ratio the same
  //row ratio will be rf = r_height * row_num - rounded to a certain percent
  //will also need to check to make sure it doesn't go outside the new matrix bounds
  
  //make sure to reverse the ri and ri_1 -- currently reversed
  double rf = r_height * row_num; 
  int ri_1 = floor(rf);
  int ri = ri_1 + 1;
  
  double cf = r_width; 
  int ci, ci_1;
  double cal_data;
  
  for(int i = 0; i < new_width; i++){
   
   //each increment will create new grid line 
   cf = (i+1) * r_width;
   ci = floor(cf);
    
   //need to make sure that ci + 1 doesn't go outside the original matrix
   //same goes for ri
    changed[row_num][i] = calc_bilinear_data(ri, rf, ci, cf);
   
  }

}

//will linearing interpolate column
void Grid::bilinear_column(int column_num)
{

  //increment the value based on the rf = r_height (the row)
  //will keep the column ratio the same
  //column ratio will be cf = r_width * column_num - rounded to a certain percent
  //will also need to check to make sure it doesn't go outside the new matrix bounds
  
  
  
}




