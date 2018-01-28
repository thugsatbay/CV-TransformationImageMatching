
//Link to the header file
#include "CImg.h"
#include <ctime>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <cmath>
#include <climits>
#include <algorithm>
#include <time.h>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>

//Use the cimg namespace to access the functions easily
using namespace cimg_library;
using namespace std;

int main(int argc, char **argv)
{
     string line_info;
     CImg<double> output_image(3840,4600,1,3,0);
     ifstream infile;
     infile.open ("b657-wars.txt");
     char *block_elements;
     int  xy[2];
     int position = 0;
     int start_point = 0;
     while(!infile.eof()){
        getline(infile, line_info);
        //cout << line_info << endl;
        position = 0;
        start_point = 0;
        for(int i=0; i<line_info.length();++i){
          //cout << line_info[i] ;
            if (line_info[i] == ' '){
              xy[position++] = atoi(line_info.substr(start_point, i-start_point).c_str());
              start_point = i+1;
              if (position == 2)
                break;
            }
        }
        output_image(xy[1], xy[0], 0, 0) = 255.0;
        output_image(xy[1], xy[0], 0, 1) = 255.0;
        output_image(xy[1], xy[0], 0, 2) = 255.0;
      }
      infile.close();
      output_image.get_normalize(0,255).save("output.png");
      CImg<double> transform_image(3840,4600,1,3,0);
      double x_transform = 1.5;
      double y_transform = .75;
      double temp_x = 0.0, temp_y = 0.0;
      for (int x=0; x<output_image.height(); ++x){
        for(int y=0; y<output_image.width(); ++y){
            temp_x = ((double)x) /(((double)x)*.0001 + ((double)y)*.0001 + .5);
            temp_y = ((double)y)/(((double)x)*.0001 + ((double)y)*.0001 + .5);

            //cout << temp_x << " " << temp_y << endl;
            if (temp_x < output_image.height() && temp_y < output_image.width()){
                transform_image((temp_y), (temp_x), 0, 0) = output_image(y, x, 0, 0);
                transform_image((temp_y), (temp_x), 0, 1) = output_image(y, x, 0, 1);
                transform_image((temp_y), (temp_x), 0, 2) = output_image(y, x, 0, 2);
            }
        }
      }
      CImg<double> transform_image_1(3840,4600,1,3,0);
      for (int x=0; x<output_image.height(); ++x){
        for(int y=0; y<output_image.width(); ++y){
            temp_x = ((double)x * x_transform) + 200 ;
            temp_y = ((double)y * y_transform) + 200;
            //cout << temp_x << " " << temp_y << endl;
            if (temp_x < output_image.height() && temp_y < output_image.width()){
                transform_image_1((temp_y), (temp_x), 0, 0) = output_image(y, x, 0, 0);
                transform_image_1((temp_y), (temp_x), 0, 1) = output_image(y, x, 0, 1);
                transform_image_1((temp_y), (temp_x), 0, 2) = output_image(y, x, 0, 2);
            }
        }
      }
      /*for (int x=1; x<output_image.height()-1; ++x){
        for(int y=1; y<output_image.width()-1; ++y){
          if  ((transform_image(y-1 ,x , 0, 0) == 255.0) && (transform_image(y ,x-1 , 0, 0) == 255.0) && (transform_image(y +1 ,x , 0, 0) == 255.0) && (transform_image(y ,x+1 , 0, 0) == 255.0)){
                transform_image(y, x, 0, 0) = 255.00;
                transform_image(y, x, 0, 1) = 255.00;
                transform_image(y, x, 0, 2) = 255.00;
              }
        }
      }*/
      transform_image.get_normalize(0,255).save("transformoutput.png");
        /*
        values = line_info;
        block_elements = std::strtok(values, " ");
        temp = 0;
        while(block_elements != 0){
          x[temp++] = atoi(block_elements);
          block_elements = std::strtok(NULL, " ");
          if(temp == 2){
            break;
         }
       }
       output_image(x[1], x[0], 0, 0) = 255;
       output_image(x[1], x[0], 1, 0) = 255;
       output_image(x[1], x[0], 2, 0) = 255;
     }
     output_image.get_normalize(0,255).save("output.png");*/
     return 0;
  }