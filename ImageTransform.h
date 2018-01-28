#ifndef _TRANSFORM

#include "CImg.h"
#include "matrix.h"
#include "ImageInterpolation.h"
#include <cmath>

using namespace matrix;
using namespace cimg_library;
using namespace image_interpolation;

namespace image_transform{
	

	//Inverse Wraping based on projection matrix
	CImg<double> image_2D_transform(double **transform_matrix, int tm_row, int tm_col, CImg<double> source_image){
		int image_x = source_image.width();
		int image_y = source_image.height();
		CImg<double> transformed_image(image_x, image_y, 1, 3, 0);
		double** original_coordinates;
		double** homo_coordinates = new double*[3];
		for(int init = 0; init < 3; ++init)
			homo_coordinates[init] = new double[1];
		//Find The Inverse Matrix Relative To New Transformed Image to be formed
		double** inverse_transform_matrix = matrix::inverse_matrix(transform_matrix, tm_row);
		homo_coordinates[2][0] = 1.0;
		for(int outer_loop = 0; outer_loop < image_y; ++outer_loop){
			for(int inner_loop = 0; inner_loop < image_x; ++inner_loop){
				//Fill the x,y
				homo_coordinates[1][0] = (double)outer_loop;
				homo_coordinates[0][0] = (double)inner_loop;
				//Find the x,y in the source image
				original_coordinates = matrix::matrix_multiplication(inverse_transform_matrix, homo_coordinates, tm_row, tm_col, tm_col, 1);
				//Normalize based on w in homogeneous coordinate system
				original_coordinates[0][0] /= original_coordinates[2][0];
				original_coordinates[1][0] /= original_coordinates[2][0];
				//interpolation of the rgb values
				if ((original_coordinates[0][0] >= 0.0) && (original_coordinates[0][0] < image_x) && (original_coordinates[1][0] >= 0.0) && (original_coordinates[1][0] < image_y))
					for(int color_rgb = 0; color_rgb < 3; ++color_rgb)
						transformed_image(inner_loop, outer_loop, 0, color_rgb) = image_interpolation::bicubic_interpolation
							(source_image, original_coordinates[0][0], original_coordinates[1][0], image_x, image_y, color_rgb);
				
				for(int init = 0; init < tm_row; ++init)
					free(original_coordinates[init]);
				free(original_coordinates);
			}
		}
		return transformed_image;
	}


	//Finds the Projection points based on 4 SIFT points provided in array xy_pairs array
	double** find_2D_transformation_matrix(double **xy_pairs){
		double **points = new double*[8]; //8*8
		double **transformed_points = new double*[8]; //8*1
		double **projection_matrix = new double*[3]; //3*3
		for(int init = 0; init < 8; ++init)
			points[init] = new double[8];
		for(int init = 0; init < 8; ++init)
			transformed_points[init] = new double[1];
		for(int init = 0; init < 3; ++init)
			projection_matrix[init] = new double[3];
		//Initialize points array with 0
		for(int row_loop = 0; row_loop < 8; ++row_loop)
			for(int col_loop = 0; col_loop < 8; ++col_loop)
				points[row_loop][col_loop] = 0.0;
		//Inflate the points matrix
		for(int loop = 0; loop < 4; ++loop){
			//Refer 10.1 2D Mapping Function Pg 199 Burger Core Algorithms Image Processing
			//First 6 Columns
			points[loop * 2][0] = xy_pairs[loop][0];
			points[loop * 2][1] = xy_pairs[loop][1];
			points[loop * 2][2] = 1.00;
			points[(loop * 2) + 1][3] = xy_pairs[loop][0];
			points[(loop * 2) + 1][4] = xy_pairs[loop][1];
			points[(loop * 2) + 1][5] = 1.00;
			//Last 2 Columns
			points[loop * 2][6] = -1.0 * xy_pairs[loop][0] * xy_pairs[loop][2];
			points[loop * 2][7] = -1.0 * xy_pairs[loop][1] * xy_pairs[loop][2];
			points[(loop * 2) + 1][6] = -1.0 * xy_pairs[loop][0] * xy_pairs[loop][3];
			points[(loop * 2) + 1][7] = -1.0 * xy_pairs[loop][1] * xy_pairs[loop][3];
		}
		//matrix::print_matrix(points, 8, 8);
		//matrix::print_matrix(transformed_points, 8, 1);
		//Inflate the 8*1 transformed matrix x',y'
		for(int loop = 0; loop < 8; ++loop)
			transformed_points[loop][0] = xy_pairs[(int)floor((loop / 2))][2 + (loop % 2)];
		//Find the inverse of points matrix
		double **inversed_points = matrix::inverse_matrix(points, 8);
		//Find the projection matrix = inv(points) * transformed_points
		double **transformation_matrix_points = matrix::matrix_multiplication(inversed_points, transformed_points, 8, 8, 8, 1);
		//Converting to 3*3 from 8*1 matrix form
		for(int row_loop = 0; row_loop < 3; ++row_loop)
			for(int col_loop = 0; col_loop < 3; ++col_loop)
				if((row_loop != 2) || (col_loop != 2))
					projection_matrix[row_loop][col_loop] = 
						transformation_matrix_points[(row_loop * 3) + col_loop][0];
		projection_matrix[2][2] = 1.0;
		//matrix::print_matrix(points, 8, 8);
		//matrix::print_matrix(transformed_points, 8, 1);
		//matrix::print_matrix(inversed_points, 8, 8);
		for(int init = 0; init < 8; ++init){
			free(points[init]);
			free(inversed_points[init]);
		}
		for(int init = 0; init < 8; ++init){
			free(transformed_points[init]);
			free(transformation_matrix_points[init]);
		}
		free(points);
		free(inversed_points);
		free(transformed_points);
		free(transformation_matrix_points);
		return projection_matrix;
	}
}

#define _TRANSFORM 0
#endif