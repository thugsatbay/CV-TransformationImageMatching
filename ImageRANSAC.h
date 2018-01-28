#ifndef _RANSAC

#include "ImageTransform.h"
#include "ImageMatching.h"
#include "matrix.h"
#include <stdlib.h>
#include <cmath>
#include <stdio.h>
#include <time.h>
#include <vector>

using namespace image_transform;
using namespace image_matching;

namespace RANSAC{


	double best_points_for_homography(double** homo_matrix, vector<SimilarPoints> points){
		double distance_bw_points = 0.0;
		double **coordinates = new double*[3];
		for(int init = 0; init < 3; ++init)
			coordinates[init] =  new double[1];
		for(unsigned int loop = 0; loop < points.size(); ++loop){
			coordinates[0][0] = points[loop].source_c;
			coordinates[1][0] = points[loop].source_r;
			coordinates[2][0] = 1.0;
			double **transformed_points = matrix::matrix_multiplication(homo_matrix, coordinates, 3, 3, 3, 1);
			transformed_points[0][0] /= transformed_points[2][0];
			transformed_points[1][0] /= transformed_points[2][0];
			distance_bw_points += sqrt((points[loop].target_c - transformed_points[0][0]) * (points[loop].target_c - transformed_points[0][0]) +
				(points[loop].target_r - transformed_points[1][0]) * (points[loop].target_r - transformed_points[1][0]));
			//cout << distance_bw_points << " , ";
			//if (distance_bw_points <= RANSAC_POINT_DISTANCE_THRESHOLD)
			//	++good_points;
			for(int init = 0; init < 3; ++init)
				free(transformed_points[init]);
			free(transformed_points);
		}
		for(int init = 0; init < 3; ++init)
			free(coordinates[init]);
		free(coordinates);
		//cout << endl;
		return distance_bw_points;
	}



	double** find_homography_matrix(vector<SimilarPoints> points){
		srand(time(NULL));
		bool unique_value = true;
		int size_vector = points.size();
		int random_point = 0;
		double transformed_matched_points = LONG_MAX, potential_transformed_matched_points = 0;
		int *unique_random = new int[4];
		double **result = NULL;
		double **best_homo_matrix = new double*[3];
		for(int init = 0; init < 3; ++init)
			best_homo_matrix[init] = new double[3];
		double **candidate_homography_points = new double*[4];
		for (int init = 0; init < 4; ++init)
			candidate_homography_points[init] = new double[4];
		
		//exit(0);
		//cout << endl<< endl << endl << transformed_matched_points << endl;
		for(int iter = 0;iter < RANSAC_ITERATIONS; ++iter){
			for(int init = 0; init < 4; ++init)
				unique_random[init] = -1;
			for(int loop = 0; loop < 4; ++loop){
				while(unique_value){
					random_point = (rand() % size_vector);
					for(int init = 0; init < 4; ++init)
						if(unique_random[init] == random_point)
							unique_value = false;
					if(unique_value == true)
						break;
					unique_value = true;
				}
				unique_random[loop] = random_point;
				//cout << random_point << endl;
				candidate_homography_points[loop][0] = points[random_point].source_c;
				candidate_homography_points[loop][1] = points[random_point].source_r;
				candidate_homography_points[loop][2] = points[random_point].target_c;
				candidate_homography_points[loop][3] = points[random_point].target_r;
			}
			// int temp[] = {1,3,22,36};
			// for(int loop = 0; loop < 4; ++loop){
			// 	candidate_homography_points[loop][0] = points[temp[loop]].source_c;
			// 	candidate_homography_points[loop][1] = points[temp[loop]].source_r;
			// 	candidate_homography_points[loop][2] = points[temp[loop]].target_c;
			// 	candidate_homography_points[loop][3] = points[temp[loop]].target_r;
			// }

			//exit(0);
			result = image_transform::find_2D_transformation_matrix(candidate_homography_points);
			//matrix::print_matrix(result, 3, 3);
			potential_transformed_matched_points = best_points_for_homography(result, points);
			if (potential_transformed_matched_points <= transformed_matched_points){
				transformed_matched_points = potential_transformed_matched_points;
				for(int row_loop = 0; row_loop < 3; ++row_loop)
					for(int col_loop = 0; col_loop < 3; ++col_loop)
							best_homo_matrix[row_loop][col_loop] = result[row_loop][col_loop];
			}
			//cout << "RANSAC : Candidate Points Extracted " << iter << "," << potential_transformed_matched_points << endl;
			for(int init = 0; init < 3; ++init){
				free(result[init]);
			}
			free(result);
		}
		cout << "The Projection Matrix For Above Image" << endl;
		matrix::print_matrix(best_homo_matrix, 3, 3);
		cout << "Matched Points " << size_vector << endl << endl;
		return best_homo_matrix;


	}

}


#define _RANSAC 0
#endif