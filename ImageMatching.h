#ifndef _MATCHING


#include "CImg.h"
#include <Sift.h>
#include <constants.h>
#include <limits.h>
#include <iomanip>

using namespace cimg_library;

namespace image_matching{

//Part 1 To sort the Images Similarity based on common SIFT points
typedef struct SSD_RATIO_MULTI_IMAGES{
		double number_of_match_points;
		char *file_name;
		struct SSD_RATIO_MULTI_IMAGES *next;
		struct SSD_RATIO_MULTI_IMAGES *prev;
}ssd_images;


//To store points X,Y SIFT points in Image A,B that are same
class SimilarPoints{
		public:
				double source_r, source_c, target_r, target_c, metric;
				SimilarPoints(double, double, double, double, double);
};

SimilarPoints::SimilarPoints(double sr, double sc, double tr, double tc, double r){
	source_r = sr;
	source_c = sc;
	target_r = tr;
	target_c = tc;
	metric = r;
}


//Calculate Eculidian Distance
double sift_vector_distance(const SiftDescriptor sd_1, const SiftDescriptor sd_2, unsigned int dimension){
	double result = 0.0;
	for (unsigned int vector_i = 0; vector_i < dimension; ++vector_i){
			result += ((sd_1.descriptor[vector_i] - sd_2.descriptor[vector_i]) * (sd_1.descriptor[vector_i] - sd_2.descriptor[vector_i]));
	}
	return sqrt(result);
}



//Matches 2 images and finds common SIFT points
vector<SimilarPoints> image_matching(const vector<SiftDescriptor> source, const vector<SiftDescriptor> target){
	vector<SimilarPoints> sift_similar_points;
	double shortest_dist = LONG_MAX, second_shortest_dist = LONG_MAX, result_dist = 0.0;
	int matching_target_index = -1;
	for(unsigned int source_index = 0; source_index < source.size(); ++source_index){
			shortest_dist = LONG_MAX;
			second_shortest_dist = LONG_MAX;
			matching_target_index = -1;
			//SSD ratio finding closest and second closest SIFT points in the query image
			for(unsigned int target_index = 0; target_index < target.size(); ++target_index){
				result_dist = sift_vector_distance(source[source_index], target[target_index], SIFT_VECTOR_SIZE);
				if (result_dist < shortest_dist){
					second_shortest_dist = shortest_dist;
					shortest_dist = result_dist;
					matching_target_index = target_index;
				}
				else if (result_dist < second_shortest_dist){
					second_shortest_dist = result_dist;
				}
			}
			if ((shortest_dist/second_shortest_dist) < SIFT_MATCHING_RATIO_THRESHOLD && shortest_dist < SIFT_MATCHING_SHORTEST_DISTANCE) {
				sift_similar_points.push_back( SimilarPoints(source[source_index].row, source[source_index].col,
					target[matching_target_index].row, target[matching_target_index].col, SIFT_MATCHING_SHORTEST_DISTANCE_SCORE_UPPER_BOUND - shortest_dist));
			}
	}
	//cout << "Total Matching Descriptors Found : " << sift_similar_points.size() << endl;
	//for(unsigned int i = 0; i < sift_similar_points.size(); ++i)
	//	cout << sift_similar_points[i].source_c << " , " << sift_similar_points[i].source_r << " , " << sift_similar_points[i].target_c << " , " << sift_similar_points[i].target_r << " , " << endl;
	return sift_similar_points;
}


vector<SimilarPoints> processedMatches(vector<SimilarPoints> matches, int width) {
	cout << "Optimizing Match Points : Points before : " << matches.size() << endl;	
	vector<SimilarPoints> newMatches;
	double avgSlope = 0;
	for(unsigned int i = 0; i<matches.size(); i++) {
		double run = matches[i].source_c - matches[i].target_c - width;
		double rise = matches[i].source_r - matches[i].target_r;
		avgSlope += rise/run;
	}
	avgSlope = avgSlope/matches.size();
	
	for(unsigned int i = 0; i<matches.size(); i++) {
		double run = matches[i].source_c - matches[i].target_c - width;
		double rise = matches[i].source_r - matches[i].target_r;
		double slope = rise/run;
		if(abs(slope-avgSlope) < SIFT_MATCHING_POINTS_SLOPE_OPTIMIZATION) 
			newMatches.push_back(matches[i]);
	}
	if(newMatches.size() == 0 || newMatches.size() <= 3) {
		return matches;
	}
	matches = newMatches;
	newMatches.clear();
	int avgXDiff = 0, avgYDiff = 0;
	for(unsigned int i = 0; i<matches.size(); i++) {
		avgXDiff += abs(matches[i].source_c - matches[i].target_c);
		avgYDiff += abs(matches[i].source_r - matches[i].target_r);
	}
	avgXDiff = avgXDiff/matches.size();
	avgYDiff = avgYDiff/matches.size();
	
	for(unsigned int i = 0; i<matches.size(); i++) {
		int xDiff = abs(matches[i].source_c - matches[i].target_c);
		int yDiff = abs(matches[i].source_r - matches[i].target_r);
		if(xDiff < avgXDiff*2 && yDiff < avgYDiff*2) 
			newMatches.push_back(matches[i]);
	}
	if(newMatches.size() == 0 || newMatches.size() <= 3) {
		return matches;
	}
	cout << "Optimization Done Best Points Selected : " << newMatches.size() << endl;
	return newMatches;
}



//Sort the Images based on SIFT points and print them
void sort_and_print_descending_order_matched_images(ssd_images *root,const int &files){
	 double numerical_highest = -10.0;
     	 ssd_images *move, *highest, *result, *result_root;
	 //Declaring sorted list head structure
	 result_root = new ssd_images;
	 result_root->number_of_match_points = -10.0;
	 result_root->file_name = NULL;
	 result_root->prev = NULL;
	 //Pointing the result that moves on the result_root list
	 result = result_root;
	 //Initial status of the list
	 highest = move = root;
	 //Print original list
	 /*cout << files << endl;
	 while (true){
		if(move->next == NULL)
				break;
		move = move->next;
	 }
	 move = root;*/
	 //Naive Sorting; the list - removing the highest from the original list and adding to result_root list
	 for(int loop = 0; loop < files; ++loop){
		numerical_highest = -10.0;
		while (true){
			if ((numerical_highest < move->number_of_match_points) && (move->number_of_match_points != LONG_MAX)){
				numerical_highest = move->number_of_match_points;
				highest = move;
			}
			if(move->next == NULL)
				break;
			else
				move = move->next;
		}
		//cout << "Printing Highest Extracted " << highest->number_of_match_points << "," << numerical_highest << endl;
		(highest->prev)->next = highest->next;
		(highest->next)->prev = highest->prev;
		result->next = highest;
		highest->prev = result;
		result = result->next;
		highest = move = root;
	 }
	 result->next = NULL;
	 //Printing the descending order list, the original list is lost
	 const char spaceSeperator = ' ';
	 const char divideSeperator = '-';
	 const char fileWidth = 25;
	 const char numWidth = 3;
	 while(true){
		if (result_root->number_of_match_points != -10.0){
			cout << "Image : ";
			cout << left << setw(fileWidth) << setfill(divideSeperator) << result_root->file_name;
			cout << "-matched by ";
			cout << left << setw(numWidth) << setfill(spaceSeperator) << result_root->number_of_match_points;
			cout << " Eculidian Metric";
			cout << endl;
		}
		if(result_root->next == NULL)
			break;
		else
			result_root = result_root->next;
	 }
}



//Pearson's Coefficient
double p_coeff(vector<SimilarPoints> matches){
	double mean_source_r = 0.0, mean_source_c = 0.0, mean_target_r = 0.0, mean_target_c = 0.0;	
	for (unsigned int loop = 0; loop < matches.size(); ++loop){
		mean_source_r += matches[loop].source_r;
		mean_source_c += matches[loop].source_c;
		mean_target_r += matches[loop].target_r;
		mean_target_c += matches[loop].target_c;
	}
	mean_source_r /= matches.size(); mean_source_c /= matches.size(); mean_target_r /= matches.size(); mean_target_c /= matches.size();
	double p_x_sd_t = 0.0, p_x_sd_s = 0.0, p_x_v = 0.0;
	for (unsigned int loop = 0; loop < matches.size(); ++loop){
		p_x_v += (matches[loop].source_r - mean_source_r) * (matches[loop].target_r - mean_target_r);
		p_x_sd_s += (matches[loop].source_r - mean_source_r) * (matches[loop].source_r - mean_source_r);
		p_x_sd_t += (matches[loop].target_r - mean_target_r) * (matches[loop].target_r - mean_target_r);
	}
	double p_x = p_x_v / sqrt(p_x_sd_s * p_x_sd_t);

	double p_y_sd_t = 0.0, p_y_sd_s = 0.0, p_y_v = 0.0;
	for (unsigned int loop = 0; loop < matches.size(); ++loop){
		p_y_v += (matches[loop].source_c - mean_source_c) * (matches[loop].target_c - mean_target_c);
		p_y_sd_s += (matches[loop].source_c - mean_source_c) * (matches[loop].source_c - mean_source_c);
		p_y_sd_t += (matches[loop].target_c - mean_target_c) * (matches[loop].target_c - mean_target_c);
	}
	double p_y = p_y_v / sqrt(p_y_sd_s * p_y_sd_t);
	return p_x * p_y;
}



double matching_best_points_for_homography(double** homo_matrix, vector<SimilarPoints> points){
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



	double** matching_find_homography_matrix(vector<SimilarPoints> points){
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
			potential_transformed_matched_points = matching_best_points_for_homography(result, points);
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
		//cout << "The Projection Matrix For Above Image" << endl;
		//matrix::print_matrix(best_homo_matrix, 3, 3);
		//cout << "Matched Points " << size_vector << endl << endl;
		return best_homo_matrix;


	}




vector<SimilarPoints> find_inliners(double** homo_matrix, vector<SimilarPoints> points){
		vector<SimilarPoints> inlierSIFTPoints;
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
			distance_bw_points = sqrt((points[loop].target_c - transformed_points[0][0]) * (points[loop].target_c - transformed_points[0][0]) +
				(points[loop].target_r - transformed_points[1][0]) * (points[loop].target_r - transformed_points[1][0]));
			if (distance_bw_points <= RANSAC_POINT_DISTANCE_THRESHOLD)
				inlierSIFTPoints.push_back(points[loop]);
			for(int init = 0; init < 3; ++init)
				free(transformed_points[init]);
			free(transformed_points);
		}
		for(int init = 0; init < 3; ++init)
			free(coordinates[init]);
		free(coordinates);
		//cout << endl;
		return inlierSIFTPoints;
}


//Function for Part 1 to do multi image matching
ssd_images* multi_image_matching(int number_files, char **files, int ransac){
		//cout << (IMAGES_PART1_FOLDER + string(files[2])).c_str();
		CImg<double> input_image((IMAGES_PART1_FOLDER + string(files[2])).c_str());
		CImg<double> input_gray = input_image.get_RGBtoHSI().get_channel(2);	
		//Input Image SIFT descriptor
		vector<SiftDescriptor> input_descriptors = Sift::compute_sift(input_gray);
		//Query Image SIFT descriptor
		vector<SiftDescriptor> query_descriptors;
		//Similar Points coordinates vector for any 2 given images
		vector<SimilarPoints> match_points;
		//Structure to store the matches for all images with base file
		//Creating a dud head with INT_MAX helpful while sorting
		ssd_images *ssd_root, *ssd_move;
		ssd_root = new ssd_images;
		ssd_root->prev = NULL;
		ssd_root->number_of_match_points = LONG_MAX;
		ssd_root->file_name = NULL;
		ssd_root->next = new ssd_images;
		ssd_move = ssd_root->next;
		ssd_move->prev = ssd_root;
		for(int loop = 3; loop < number_files; ++loop){
			//cout << (IMAGES_PART1_FOLDER + string(files[loop])).c_str();
			cout << "Reading " << files[loop] << "..."  << endl;
			CImg<double> query_image((IMAGES_PART1_FOLDER + string(files[loop])).c_str());
			CImg<double> query_gray = query_image.get_RGBtoHSI().get_channel(2);
			query_descriptors = Sift::compute_sift(query_gray);
			match_points = image_matching(input_descriptors,query_descriptors);
			if (ransac == 1){
				if (match_points.size() >= 4){
					//match_points = image_matching::processedMatches(match_points, input_gray.width());
					double **homo_matrix = matching_find_homography_matrix(match_points);
					match_points = find_inliners(homo_matrix, match_points);
				}
			}
			double total = 0;
			for(unsigned int i = 0; i<match_points.size(); i++) {
				total += match_points[i].metric;
			}
			ssd_move->number_of_match_points = total;
			ssd_move->file_name = files[loop];
			ssd_move->next = new ssd_images;
			(ssd_move->next)->prev = ssd_move;
			ssd_move = ssd_move->next;
		}
		//Creating a dud tail helpful for sorting
		ssd_move->number_of_match_points = -10.0;
		ssd_move->file_name = NULL;
		ssd_move->next = NULL;
		return ssd_root;
		//return NULL;
}



}

#define _MATCHING 0
#endif