/**
*@file Grid.cpp
*@author Megan Teahan <mteahan@ku.edu>
*@version 1.0
*
*@section DESCRIPTION
*	
*
*
*/

#include "Grid.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>


using namespace std;
typedef double vec3[3];
/***/
void Grid::read(ifstream& isFile) 
{
  typedef double vec3[3];
  
  //will read the text file and store it in a matrix
  int width, height, rgb;
  double num;
  isFile >> cur_width;
  isFile >> cur_height;
  isFile >> rgb;
  
  
  
   original = new vec3*[cur_height]; 
//    vec3* row = new vec3[height];
   
   int j = 0;
   
  while(isFile >> num){
    cout << j << "\n" ;
    vec3* row = new vec3[cur_width];
    for(int i = 0; i < cur_width; i++) 
    {
	row[i][0] = num;
	isFile >> num;
	row[i][1] = num;
	isFile >> num;
	row[i][2] = num;
	if(i != cur_width-1){
	  isFile >> num;
	}
	  
    }
    
    original[j] = row;
    j++;

   }
   calc_grid();
   
   cout << "current height: " << cur_height << "\n";
   cout << "new height: " << new_height << "\n";
   cout << "current width: " << cur_width << "\n";
   cout << "new width: " << new_width << "\n";
   
//    print_row(original, 0, cur_width);
//    bilinear_row(0);
   for(int k = 0; k < new_height; k++){
     bilinear_row(k);
   }
   print_mat(changed, new_width, new_height);
   
  // print_row(original, 0, cur_width);
   
   cout << "\n \n \n";
   
   //print_row(original, 1, cur_width);
   
   //bilinear_row(0);
   //print_mat(original, cur_width, cur_height);
}

/**
* Constructor tells if nothing has passed in or not.
*/
Grid::Grid()
{

	cout << "No text files were inputted to be read. \n";

}

Grid::Grid(int cw, int ch, int nw, int nh, ifstream& intFile){
 
  cur_width = cw;
  cur_height = ch;
  new_width = nw;
  new_height = nh;
  calc_grid();
  changed = new vec3*[new_height]; 
  read(intFile);
  
  
}


void Grid::calc_grid()
{
  //will need to check for reduction or expansion
  
  r_width = (double) cur_width / (double) new_width;
  r_height = (double) cur_height / (double) new_height;
  
  
}

 
double Grid::calc_bilinear_data(int ri, double rf, int ci, double cf, int rgb, vec3** mat)
{
  
  //something potentially wrong with this equation. getting odd numbers
  //need to normalize cf to be inbetween the grid lines
//   cout << "calc1 " << mat[ri][ci][rgb] << "\n";
//   cout << "calc2 " << mat[ri][ci+1][rgb] << "\n";
//   cout << "calc3 " << mat[ri-1][ci][rgb] << "\n";
//   cout << "calc4 " << mat[ri-1][ci+1][rgb] << "\n";
  double fir_part = (1 - rf) * ( ((1-cf) * mat[ri][ci][rgb]) + (cf * mat[ri][ci+1][rgb]) );
  double sec_part = rf * ( ((1-cf) * mat[ri-1][ci][rgb]) + (cf * mat[ri-1][ci+1][rgb]));
  
  
//   cout << "mat item: "  << mat[ri][ci][rgb] << "\n"; 
//    cout << "first part: " << fir_part << " second part: " << sec_part << "\n";
  int item = fir_part + sec_part;
  
  
  return (double) item;

  
}


//will do Bilinear Interoplation on the Grid
void Grid::interpBilinear()
{

}

//will do BiCublic Interplation on the Grid
void Grid::interpBiCublic()
{
  //will do all rows and use the columns

}

//will cublically interpolate row
void Grid::bicublic_row(int row_num)
{
  //will have to start 1 row and 1 column in and stop 1 row and 1 column short
  //the points in the Cardinal Splines or Catmull & Rom splines could potetially correspond to grid vertices
  //what is important is picking the vertices used to calculate the new values.
  //might not be down the line, but rather a circle around the points. 
  
  //will have fraction matrix
  //t = cf when in one row going across via columns
  //t = rf when in one column going across via rows
  //most likely will get cf stuff and then use calculated points to
  //rf
  //catmull& rom matrix
  //point matrix
  
  
  
  
}


double** Grid::mat_mult(double** mat_1, double** mat_2){
  
 //will check which one is bigger and change multiplcation based off size
 //either 1 x 4 or 4 x 4
  
  
}

//works for reductions, doesn't work for expansions
//will linearing interpolate row
void Grid::bilinear_row(int row_num)
{

  //increment the value based on the cf = r_width (the column)
  //will keep the row ratio the same
  //row ratio will be rf = r_height * row_num - rounded to a certain percent
  //will also need to check to make sure it doesn't go outside the new matrix bounds
//   cout << "row : " << row_num << "\n";
//   cout << "rwidth: " << r_width << "\n";
//   cout << "rheight: " << r_height << "\n";
  
  //make sure to reverse the ri and ri_1 -- currently reversed
  double rf = r_height * (double) row_num; 
  if(row_num == 0){
    rf = r_height;
  }
   
//   cout << "rf: " << rf << "\n";
  int ri_1 = floor(rf);
  int ri = ri_1 + 1;
  
  if(ri_1 == cur_height-1){
   ri = ri_1;
    
  }
  
//   if(ri_1 == cur_height){
//     ri_1 = cur_height-1;
//     ri = cur_height-1; 
//   }
  
  rf = rf - ri_1;
  
  cout << "ri_1: " << ri_1 << "\n";
  cout << "ri: " << ri << "\n";
  cout << "rf: " << rf << "\n";
  
  double cf = r_width; 
  int ci, ci_1;
  
  vec3* row = new vec3[new_width];
  
  for(int i = 0; i < new_width; i++){
   
   
   //each increment will create new grid line 
   cf = i * r_width;
  if(i == 0){
    cf = r_width;
  }
   
   
   ci = floor(cf);
   ci_1 = ci + 1;
   cf = cf - ci; //need to double check this and rf are being done correctly
   
//    cout << "cf: " << cf << "\n";
//    cout << "ci_1: " << ci_1 << "\n";
//    cout << "ci: " << ci << "\n";
//     
   //need to make sure that ci + 1 doesn't go outside the original matrix
   //same goes for ri
   
//    cout << "first row: ci "   << original[ri_1][ci][0] << "\n";
//    cout << "first row: ci_1 "   << original[ri_1][ci_1][0] << "\n";
//    cout << "second row: ci "   << original[ri][ci][0] << "\n";
//    cout << "second row: ci_1 "   << original[ri][ci_1][0] << "\n";
   
   row[i][0] = calc_bilinear_data(ri, rf, ci, cf, 0, original);
   row[i][1] = calc_bilinear_data(ri, rf, ci, cf, 1, original);
   row[i][2] = calc_bilinear_data(ri, rf, ci, cf, 2, original);
   
  
   
   cout <<"column: " << i << " f: " << row[i][0] << ", " << "s: " << row[i][1] << /*", " << "t: " << third <<*/ "\n "; 
    //changed[row_num][i] = calc_bilinear_data(ri, rf, ci, cf, 0, original);
  }
  
  changed[row_num] = row;

}

//will linearing interpolate column
void Grid::bilinear_column(int column_num)
{

  //increment the value based on the rf = r_height (the row)
  //will keep the column ratio the same
  //column ratio will be cf = r_width * column_num - rounded to a certain percent
  //will also need to check to make sure it doesn't go outside the new matrix bounds
  
  
  
}

void Grid::print_row(vec3** mat, int row, int width){
  
   for(int j = 0; j < width; j++){
     
      cout  << mat[row][j][0] << " " << mat[row][j][1] << " " << mat[row][j][2] << ", ";
      
    }
    
     cout << "\n";
}

void Grid::print_mat(vec3** mat, int width, int height){
  
  for(int i = 0; i < height; i++){
   
    for(int j = 0; j < width; j++){
     
      cout  << mat[i][j][0] << " " << mat[i][j][1] << " " << mat[i][j][2] << ", ";
    }
    cout << "\n \n \n";
  }
  
  ofstream myfile;
  myfile.open ("output.txt");
  
  myfile << new_width  << " " << new_height << " 3 \n";
  
  
  for(int i = 0; i < height; i++){
   
    for(int j = 0; j < width; j++){
     
      myfile  << mat[i][j][0] << " " << mat[i][j][1] << " " << mat[i][j][2] << " ";
    }
    myfile << "\n";
  }
  
  myfile.close();
  
}




