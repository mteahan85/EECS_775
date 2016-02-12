/**
 * @file Grid.cpp
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




/***/
void Grid::read(ifstream& isFile) 
{
  
  
  //will read the text file and store it in a matrix
  int width, height, rgb;
  double num;
  isFile >> cur_width;
  isFile >> cur_height;
  isFile >> color;
  
  cout << "width: " << cur_width;
  cout << "height: " << cur_height;
  
  cout << "rgb: " << color << "\n";
  
  
   //initializeMat(original, cur_width, cur_height, 3);
   
   original = new double**[cur_height]; 
  
  for(int i = 0; i < cur_height; i++){
    
    double** row = new double*[cur_width];
    for(int j = 0; j < cur_width; j++){
      row[j] = new double[color];
      
    }
    original[i] = row; 
  }
  //original = new double**[cur_height]; 
  
  // while(isFile >> num){
  for(int j = 0; j < cur_height; j++){
    
    for(int i = 0; i < cur_width; i++) 
    {
   
      for(int k = 0; k < color; k++){
	isFile >> num;
	original[j][i][k] = num;
      }

    }
   
  }
  cout << "I break in calc " ;
  calc_grid();
  //issues not enough space to hold everything.
  //could potetially have a different data structure but not sure what that looks like.
  interpBiCublic();
 cout << "I break in cubic " ;
  
  //   double* thing = new double[4];
  //   thing[0] = 1;
  //   thing[1] = 2;
  //   thing[2] = 3;
  //   thing[3] = 4;
  //   
  //   cout<< "mat calc: " <<  spline_point(2, thing);
  //    
  //    cout << "current height: " << cur_height << "\n";
  //    cout << "new height: " << new_height << "\n";
  //    cout << "current width: " << cur_width << "\n";
  //    cout << "new width: " << new_width << "\n";
  //    
  // //    print_row(original, 0, cur_width);
  // //    bilinear_row(0);
//        for(int k = 0; k < new_width; k++){
//          bilinear_column(k);
//        }
  
//   for(int k = 0; k < new_height; k++){
//         bilinear_row(k);
//      }
//   
//        
      //  print_mat(changed, new_width, new_height, color);
  //       print_mat(changed, new_width, new_height, color);
  //    
  //   // print_row(original, 0, cur_width);
  //    
  //    cout << "\n \n \n";
  
  //print_row(original, 1, cur_width);
  
  //bilinear_row(0);
  //print_mat(original, cur_width, cur_height, color);
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
  //changed = new double**[new_height]; 
  initializeMat(changed, new_width, new_height, 3);
  
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
  
  
  //   {{-0.5, 1.5, -1.5, 0.5},
  // 			     {1, -2.5, 2, -0.5}, 
  // 			     {-0.5, 0, 0.5, 0},
  // 			     {0, 1, 0, 0}};
  
  
  read(intFile);
  
  
}


void Grid::initializeMat(double*** mat, int width, int height, int color){
  
  changed = new double**[height]; 
  
  for(int i = 0; i < height; i++){
    
    double** row = new double*[width];
    for(int j = 0; j < width; j++){
      row[j] = new double[color];
      
    }
    changed[i] = row; 
  }
  
}

void Grid::calc_grid()
{
  r_width = (double) cur_width / (double) new_width;
  r_height = (double) cur_height / (double) new_height;
}


double Grid::calc_bilinear_data(int ri, int ri_1,  double rf, int ci, int ci_1, double cf, int rgb, double*** mat)
{
  double fir_part = (1 - rf) * ( ((1-cf) * mat[ri][ci][rgb]) + (cf * mat[ri][ci_1][rgb]) );
  double sec_part = rf * ( ((1-cf) * mat[ri_1][ci][rgb]) + (cf * mat[ri_1][ci_1][rgb]));
  
  int item = fir_part + sec_part; 
  
  
 if(item < 0) { 
  cout << "rf: " << rf << "\n"; 
  cout << "cf: " << cf << "\n";
  cout << "first part: " << fir_part << " second part: " << sec_part << "\n";
  cout << "final part: "  << item << "\n";
}  
  
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
  
  
  //from 0 to new_height - 3
  //from 0 to new_width -3
  //to account for the outside stuff
  //bilinear Interoplation first. Spots will be overwritten with bicublic
//   for(int i = 0; i < new_height; i++){
//    bilinear_row(i); 
//   }
  
  double cf = r_width, rf = r_height;
  double cf_total = 0,  rf_total = 0;
  int ci = 0;
  int ri_1 = 0;
  
  
  int i = 1;
  int j = 1; 
  //don't want to subtract 3 because you don't get the closest
  //you can to the edge
  
  cout << "I break in cubic " ;
  while((ri_1 = floor(rf_total)) < cur_height-2){
    
    if(ri_1 >= 1){
    rf = rf_total - ri_1;
//     cout << "row: " << i << "\n"; 
//    // cout << "rf: " << rf << "\n";
//     cout << "rf_total: " << rf_total << "\n";
   // cout << "cur_height: " << cur_height << "\n";
   // cout << "r_height: " << r_height << "\n";
    
    //don't want to subtract 3 because you don't get the closest
    //you can to the edge
    cf_total = 0;
    j= 1;
    while((ci = floor(cf_total)) < cur_width-2){
      
      if(ci >= 1){
	cf = cf_total - ci;
	/*
	cout << "cf: " << cf << "\n";
	cout << "cf_total: " << cf_total << "\n \n";
	cout << "column: " << j << "\n \n"; */
	
	double** point_info = new double*[4];
	
	for(int k = -1; k < 3; k+=1){

	  double** points = new double*[4];
	  //one row of red
	  points[0] = original[ri_1+k][ci-1];
	 // cout << "points[0]: " << points[0][0] << "\n";
	  points[1]= original[ri_1+k][ci];
	//  cout << "points[1]: " << points[1][0] << "\n";
	  points[2]= original[ri_1+k][ci+1];
	//  cout << "points[2]: " << points[2][0] << "\n";
	  points[3] = original[ri_1+k][ci+2];
	 // cout << "points[3]: " << points[3][0] << "\n";
	  
	
	  //one red x
	  point_info[k+1] = spline_point(cf, points);
// 	  for(int m = 0; m < color; m++){
// 	   cout << point_info[m]; 
// 	    
// 	  }
	}
	
	//interpolates x's to get final point
	double* final_point = spline_point(rf, point_info);
// 	cout << "after? \n";
// 	cout << "ri_1: " << ri_1 << "\n";
// 	cout << "ci: " << ci << "\n";
	changed[i][j] = final_point;
// 	cout << "change? \n";
      }
      cf_total += r_width;
      j++;
      
    }
    
    }
    rf_total += r_height;
    i++;
  }
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
  
  //will have to go two columns and rows in for cubic. cannot go outside that range
  
  
  
}

//will have to do this spline point for each of the colors
//points may be a double** array
double* Grid::spline_point(double t, double** points){
  
  //will check which one is bigger and change multiplcation based off size
  //either 1 x 4 or 4 x 4
  
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
    for(int i = 0; i < 4; i++){
      
      sum[j] += t_mat[i] * points[i][j];
    }
    sum[j] = floor(sum[j]);
    if(sum[j] < 0){
     sum[j] = 0; 
    }
  }
  
  return sum;
  
  
}

//works for reductions, doesn't work for expansions
//will linearing interpolate row
void Grid::bilinear_row(int row_num)
{
  //increment the value based on the cf = r_width (the column)
  //will keep the row ratio the same
  //row ratio will be rf = r_height * row_num - rounded to a certain percent
  //will also need to check to make sure it doesn't go outside the new matrix bounds
  
  
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
    
    //adds rgb list to column in row
    
  }
  //adds the row
  
  
}

//will linearing interpolate column
void Grid::bilinear_column(int column_num)
{
  
  //increment the value based on the rf = r_height (the row)
  //will keep the column ratio the same
  //column ratio will be cf = r_width * column_num - rounded to a certain percent
  //will also need to check to make sure it doesn't go outside the new matrix bounds
  
  
  double cf_total = r_width;
  cf_total += r_width * (double) column_num;
  cout << "------------------------------section: " << column_num << " ---------------------------------- \n \n\n";
  cout << "cf_total: " << cf_total << "\n \n";
  
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
    	cout << "cf: " << cf << "\n";
	cout << "cf_total: " << cf_total << "\n \n";
	cout << "column: " << column_num << "\n \n";
	cout << "ci: " << ci << "\n";
	cout << "ci_1: " << ci_1 << "\n";
	
  double rf = r_height;
  double rf_total = r_height;
  int ri, ri_1;
  int i = 0; 
  
  while((ri_1 = floor(rf_total)) < cur_height-1){
    ri = ri_1 + 1; 
    rf = rf_total - ri_1;
    
// 	cout << "row: " << i << "\n";
// 	cout << "column: " << column_num  << "\n\n";
// 	cout << "rf: " << rf << "\n";
// 	cout << "rf_total: " << rf_total << "\n ";
// 	cout << "ri: " << ri << "\n";
// 	cout << "ri_1: " << ri_1 << "\n";
    
    
    for(int j = 0; j < color; j++){
      changed[i][column_num][j] = calc_bilinear_data(ri, ri_1, rf, ci, ci_1, cf, j, original);
    }
    rf_total += r_height;
    i++;
    
  }
  
  for(i; i < new_height; i++){
   
    ri_1 = cur_height -2;
    ri = ri_1 + 1; 
    rf = rf_total - ri;
    
//     cout << "final row: " << i << "\n";
// 	cout << "column: " << column_num  << "\n\n";
// 	cout << "rf: " << rf << "\n";
// 	cout << "rf_total: " << rf_total << "\n ";
// 	cout << "ri: " << ri << "\n";
// 	cout << "ri_1: " << ri_1 << "\n";
//     
    for(int j = 0; j < color; j++){
      changed[i][column_num][j] = calc_bilinear_data(ri, ri_1, rf, ci, ci_1, cf, j, original);
    }
    
  }
  
 
  
  
  
//   for(int i = 0; i < new_height; i++){
//     
//     
//     rf = (double) i * r_height;
//     if(i == 0){
//       rf = r_height; 
//     }
//     
//     ri_1 = floor(rf);
//     
//     ri = ri_1 + 1;
//     if(ri == cur_height){
//       ri = ri_1; 
//     }
//     //sets the fraction based off the position to ci_1 and ci
//     rf = rf - ri_1;
//     
//     for(int j = 0; j < color; j++){
//       changed[i][column_num][j] = calc_bilinear_data(ri, ri_1, rf, ci, ci_1, cf, j, original);
//     }
//   }
}

void Grid::print_row(double*** mat, int row, int width){
  
  //    for(int j = 0; j < width; j++){
  //      
  //       cout  << mat[row][j][0] << " " << mat[row][j][1] << " " << mat[row][j][2] << ", ";
  //       
  //     }
  //     
  //      cout << "\n";
}

void Grid::print_mat(double*** mat, int width, int height, int rgb){
  
  for(int i = 0; i < height; i++){
    
    cout << "------------------------------section: " << i << " ---------------------------------- \n \n\n";
    
    for(int j = 0; j < width; j++){
      
      for(int k = 0; k < rgb; k++){
	cout << mat[i][j][k] << " ";
      }
    }
    cout << "\n \n \n";

  }
  
  ofstream myfile;
  myfile.open ("output.txt");
  
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





