/**
 * @file Main.cpp
 *@author Megan Teahan <mteahan@ku.edu>
 *@version 1.0
 *
 *@section DESCRIPTION
 *
 */

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>

using namespace std;

#include "Grid.h"




/**
 * 
 *Constructor that takes in one text file and opens it, then sending it to the Executive class.
 *
 *@param char* argv[] it is an array of text files passed into the program
 *	
 *@return returns an int to terminate the program after it has been completely used. 
 *
 */

int main(int argc, char* argv[ ])
{
  //opens the stream and checks if it opened correctly
  std::ifstream s1;
  s1.open(argv[1]);
  char* s2 = argv[2];
  
  if(!s1.is_open())	
  {
    cout << "Error: the file could not be opened properly. The program will terminate now. \n";
    return -1;	
  }
  
  string current;
  int cur_width =0, cur_height=0;
  int color; 
  int new_width, new_height; 
  char interpolation;
  
  cout << "Current (width, height): ";
  s1 >> cur_width;
  s1 >> cur_height;
  s1 >> color;
 
  cout << "(" << cur_width << ", " << cur_height << ")\n"; 
  cout << "Enter desired resampled width: ";
  cin >> new_width;
  cout << "Enter desired resampled height: ";
  cin >> new_height;
  
  
  Grid grid(cur_width, cur_height, new_width, new_height, color);
  grid.setFiles(s1, s2);	   
 
  cout << "Use Bilinear (l) or bicubic (c) interpolation? ";
  cin >> interpolation;
  
  if(interpolation == 'l' || interpolation == 'L'){
    grid.interpBilinear();
  }else{
   grid.interpBiCublic();
  }
  
 
  s1.close();
  

  
  return 0;
  
}
