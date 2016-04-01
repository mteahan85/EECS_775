/**
 * @file Grid.cpp
 *@author Megan Teahan <mteahan@ku.edu>
 *@version 1.0
 *
 *@section DESCRIPTION
 *	Grid does all the computations on the matrices for interpolating the given image
 *
 *
 */

#include "Grid.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>


using namespace std;


/**
 * Constructor tells if nothing has passed in or not.
 */
Grid::Grid()
{
  
  cout << "No text files were inputted to be read. \n";
  
}

/**
 * Constructor with current width and height
 * 			new width and height
 * 			and color outlets
 */
Grid::Grid(int cw, int ch, int nw, int nh, int c){
  
  cur_width = cw;
  cur_height = ch;
  new_width = nw;
  new_height = nh;
  color = c; 
  calc_grid();

  original = initializeMat(cur_width, cur_height, color);
  changed = initializeMat(new_width, new_height, 3);
  
  c_cr[0][0] = -0.5;
  c_cr[0][1] = 1.5;
  c_cr[0][2] = -1.5;
  c_cr[0][3] = 0.5;
  
  c_cr[1][0] = 1;
  c_cr[1][1] = -2.5;
  c_cr[1][2] = 2;
  c_cr[1][3] = -0.5;
  
  c_cr[2][0] = -0.5;
  c_cr[2][1] = 0;
  c_cr[2][2] = 0.5;
  c_cr[2][3] = 0;
  
  c_cr[3][0] = 0;
  c_cr[3][1] = 1;
  c_cr[3][2] = 0;
  c_cr[3][3] = 0;
}



/**
 * Deconstructor - frees up memory
 */
Grid::~Grid(){
 
  deleteMat(original, cur_width, cur_height);
  delete original;
  deleteMat(changed, new_width, new_height);
  delete changed;
  
  
}

/**
 * Helps Deconstructor free up memory from
 * the original and changed mats
 */
void Grid::deleteMat(double*** mat, int width, int height){
 
  for(int i = 0; i < height; i++){
    for(int j = 0; j < width; j++){
     delete mat[i][j]; 
    }
    delete mat[i];
  }
  
}


/**
 * Sets up reading the input file
 * Stores output file (o) name to have
 * information printed to later
 */
void Grid::setFiles(ifstream& i, char* o){
  
  read(i);
  output = o;
  
}

/**Reads the file and stores information in the original matrix*/
void Grid::read(ifstream& isFile) 
{
  //will read the text file and store it in a matrix
  int width, height, rgb;
  double num;
 
  for(int j = 0; j < cur_height; j++){
    
    for(int i = 0; i < cur_width; i++) 
    {
      for(int k = 0; k < color; k++){
	isFile >> num;
	original[j][i][k] = num;
      }
    }
  } 
}

/*
 * Initializes Matrix to given size
 */
double*** Grid::initializeMat( int width, int height, int color){
  
  double*** mat = new double**[height]; 
  
  for(int i = 0; i < height; i++){
    double** row = new double*[width];
    
    for(int j = 0; j < width; j++){
      row[j] = new double[color];
    }
    mat[i] = row; 
  }
  return mat;
}

/**
 * Calculates the size change in the width and height
 * Will typically serve to help calculate the r and c fractions
 */
void Grid::calc_grid()
{
  r_width = (double) cur_width / (double) new_width;
  r_height = (double) cur_height / (double) new_height;
}


/**
 * Bilinearly Interpolates the Original Grid
 * into the new Changed Grid.
 * 
 * Then will print the changed matrix to the 
 * file the user gave
 */ 
void Grid::interpBilinear()
{
  for(int i = 0; i < new_width; i++){
   bilinear_column(i); 
  }

  print_mat(changed, new_width, new_height, color);
}

/**
 * Bilinearly Interpolates the Original Grid
 * into the new Changed Grid for the edges
 * 
 * Bicublicly Interoplates the inner section of 
 * Original Grid into the Changed Grid
 * 
 * Then will print the changed matrix to the 
 * file the user gave
 */ 
void Grid::interpBiCublic()
{
  //bilinear Interoplation first for edges. Spots will be overwritten with bicublic
  for(int i = 0; i < new_width; i++){
   bilinear_column(i); 
  }

  double cf = r_width, rf = r_height;
  double cf_total = 0,  rf_total = 0;
  int ci = 0;
  int ri_1 = 0;
  

   int i = 0;
   int j = 1; 

   while((ri_1 = floor(rf_total)) < cur_height-2){
    
    if(ri_1 >= 1){
    rf = rf_total - ri_1;

    cf_total = 0;
    j= 0;
    while((ci = floor(cf_total)) < cur_width-2){
      
      if(ci >= 1){
	cf = cf_total - ci;
	
	double** point_info = new double*[4];
	
	for(int k = -1; k < 3; k+=1){
	  //one red x
	  point_info[k+1] = spline_point(cf, original[ri_1+k][ci-1], original[ri_1+k][ci], 
					  original[ri_1+k][ci+1],original[ri_1+k][ci+2] );
 	}
	
	//interpolates x's to get final point
	for(int l = 0; l < color; l++){
	  changed[i][j][l] = spline_point(rf, point_info[0],point_info[1],point_info[2],point_info[3])[l];
	}
      }
      cf_total += r_width;
      j++;
    }
    }
    rf_total += r_height;
    i++;
  }
  
  
   print_mat(changed, new_width, new_height, color);
}


/**
 * Bilinearly Interoplates a given row
 */
void Grid::bilinear_row(int row_num)
{

  //sets the inital fraction to go off of to decide ri_1 and ri
  double rf = r_height * (double) row_num; 
  cout << "rf: " << rf << "\n \n";
  
  if(row_num == 0){
    rf = r_height;
  }
  
  
  int ri_1 = floor(rf);
  int ri = ri_1 + 1;
  
  if(ri_1 == cur_height-1){
    ri = ri_1;    
  }
  
  //sets the fraction based off the position to ri_1 and ri
  rf = rf - ri_1;
  
  double cf = r_width; 
  int ci, ci_1;
  
  
  
  for(int i = 0; i < new_width; i++){
    //each increment will create new grid line 
    cf = i * r_width;
    if(i == 0){
      cf = r_width;
    }
    
    ci = floor(cf);
    ci_1 = ci + 1;
    if(ci_1 == cur_width){
      ci_1 = ci; 
    }
    //sets the fraction based off the position to ci_1 and ci
    cf = cf - ci;
    
    for(int j = 0; j < color; j++){
      //gets all the color values for that particular column in the row
      changed[row_num][i][j] = calc_bilinear_data(ri, ri_1, rf, ci, ci_1, cf, j, original);
    }
  }
 
}

/**
 * Bilinearly Interoplates a given column
 */
void Grid::bilinear_column(int column_num)
{
  
  double cf_total = r_width;
  cf_total += r_width * (double) column_num;
 
  double cf = r_width;
  int ci, ci_1;
  
  ci = floor(cf_total);
  if(ci >= cur_width-1){
    ci = cur_width-2;
  }
  ci_1 = ci + 1; 

  
  cf =  cf_total - ci;
  if(cf > 1){
      cf = 1;
  }
	
  double rf = r_height;
  double rf_total = r_height;
  int ri, ri_1;
  int i = 0; 
  
  while((ri_1 = floor(rf_total)) < cur_height-1){
    ri = ri_1 + 1; 
    rf = rf_total - ri_1;
    
    for(int j = 0; j < color; j++){
      changed[i][column_num][j] = calc_bilinear_data(ri, ri_1, rf, ci, ci_1, cf, j, original);
    }
    rf_total += r_height;
    i++;
    
  }
  
  
  for(int k = i; k < new_height; k++){
   
    ri_1 = cur_height -2;
    ri = ri_1 + 1; 
    rf = rf_total - ri;
    
    for(int j = 0; j < color; j++){
      changed[k][column_num][j] = calc_bilinear_data(ri, ri_1, rf, ci, ci_1, cf, j, original);
    }
    
  }
}

/**
 * Calculates the color with the bilinear interoplation
 * equation
 */
double Grid::calc_bilinear_data(int ri, int ri_1,  double rf, int ci, int ci_1, double cf, int rgb, double*** mat)
{
  double fir_part = (1 - rf) * ( ((1-cf) * mat[ri][ci][rgb]) + (cf * mat[ri][ci_1][rgb]) );
  double sec_part = rf * ( ((1-cf) * mat[ri_1][ci][rgb]) + (cf * mat[ri_1][ci_1][rgb]));
  
  int item = fir_part + sec_part; 
  return (double) item;
  
  
}


/**
 * Bicubic color interpolation matrix multiplication to get the colors
 */
double* Grid::spline_point(double t, double* point1, double* point2, double* point3, double* point4 ){
  
  double ts[4] = {t*t*t, t*t, t, 1};
  double t_mat[4];
  for (int i=0 ; i<4; i++)
  {
    double sum = 0.0;
    for (int j=0 ; j<4 ; j++)
    {
      sum += (c_cr[j][i]*ts[j]);
    }
    t_mat[i] = sum;
  }
  
  double* sum = new double[color];
  for(int j = 0; j < color; j++){
    sum[j] += t_mat[0] * point1[j];
    sum[j] += t_mat[1] * point2[j];
    sum[j] += t_mat[2] * point3[j];
    sum[j] += t_mat[3] * point4[j];
    
    
    sum[j] = floor(sum[j]);
    if(sum[j] < 0){
     sum[j] = 0; 
    }
  }
  
  return sum;
}


/**
 * Prints off the matrix into given file
 */
void Grid::print_mat(double*** mat, int width, int height, int rgb){

  ofstream myfile;
  myfile.open (output);
  
  myfile << new_width  << " " << new_height << " " << color << "\n";
  
  
  for(int i = 0; i < height; i++){
    for(int j = 0; j < width; j++){
      for(int k = 0; k < rgb; k++)
      {
	myfile  << mat[i][j][k] << " " ;
      }
    }
    myfile << "\n";
  }
  myfile.close();
  
}





