// B657 assignment 2 skeleton code
//
// Compile with: "make"
//
// See assignment handout for command line and project specifications.


//Link to the header file
#include "CImg.h"
#include "matrix.h"
#include "ImageRANSAC.h"
#include "ImageMatching.h"
#include "ImageTransform.h"
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <Sift.h>
#include <cmath>
#include <constants.h>
#include <climits>
#include <algorithm>
#include <time.h>


//Use the cimg namespace to access the functions easily
using namespace cimg_library;
using namespace std;
using namespace matrix;
using namespace image_matching;
using namespace image_transform;
using namespace RANSAC;


//*********************************************
//-----------------------------Custom functions
//*********************************************
//
//http://www.cplusplus.com/reference/ctime/time/
//Code Borrowed

class SiftPoints{
public:
	vector <int> points;
	SiftPoints(int);
};
SiftPoints::SiftPoints(int val){
	for(int loop = 0; loop < val; ++loop){
		points.push_back(0);
	}
}


double time_stamp(){
		time_t timer;
		struct tm y2k;
		double seconds;
		y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
	    y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;
		time(&timer);  /* get current time as from year 2000; same as: timer = time(NULL)  */
		seconds = difftime(timer,mktime(&y2k));
		return seconds;
}
//Code Borrow Reference Finished


//Code Reference Started
//// ---http://www.sanfoundry.com/c-program-integer-to-string-vice-versa/ ---
////Hacked By Gurleen Singh Dhody -gdhody- 12/Feb/2017
string toString(int num)
{
	int rem, len = 0, n;
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    char *value = new char[len + 1];
    for (int i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        value[len - i - 1] = rem + '0';
    }
    value[len] = '\0';
    string result(value);
	return result;
}
//Code Reference Finished



//To draw lines between matching SIFT points in 2 images
void draw_points_and_line(vector<SimilarPoints> &points, CImg<double> &image, const int offset){
		int tx1 = 0, ty1 = 0, tx2 = 0, ty2 = 0;
		SimilarPoints *draw_point;
		const unsigned char color_line[] = {0, 255, 0};
		double color_point[] = {255.0, 255.0, 0};
		for(unsigned int loop = 0; loop < points.size(); ++loop){
				draw_point = &points[loop];
				//Draw a line
				image.draw_line(draw_point->source_c, draw_point->source_r, draw_point->target_c + offset, draw_point->target_r, color_line);
				//Print a crossHair
				for(int x=-2; x<3; x++)
					for(int y=-2; y<3; y++)
						if(x==0 || y==0)
							for(int c=0; c<3; c++){
								//Find if coordinates are in workspace to draw crosshair
								tx1 = (draw_point->source_c + y - 1);
								tx2 = (offset + draw_point->target_c + y - 1);
								ty1 = (draw_point->source_r + x - 1);
								ty2 = (draw_point->target_r + x - 1);
								if (tx1 >= 0 && tx1 < image.width() && ty1 >= 0 && ty1 < image.height())
									image( tx1, ty1, 0, c) = color_point[c];
								if (tx2 >= 0 && tx2 < image.width() && ty2 >= 0 && ty2 < image.height())
									image( tx2, ty2, 0, c) = color_point[c];					
							}
		}

}


//Function not properly tested - prints crosshair
void print_descriptor_image(CImg<double> image, const vector<SiftDescriptor> descriptors, const char *filename){
	for(unsigned int i=0; i < descriptors.size(); i++){
	    //cout << "Descriptor #" << i << ": x=" << descriptors[i].col << " y=" << descriptors[i].row << " descriptor=(";
	    //for(int l=0; l<128; l++)
	    //cout << descriptors[i].descriptor[l] << "," ;
		//cout << ")" << endl;
		int tx1 = 0, ty1 = 0, tx2 = 0, ty2 = 0;
		double color_point[] = {255.0, 255.0, 0};
	   			for(int x=-2; x<3; x++)
					for(int y=-2; y<3; y++)
						if(x==0 || y==0)
							for(int c=0; c<3; c++){
								//Find if coordinates are in workspace to draw crosshair
								tx1 = (descriptors[i].col + y - 1);
								ty1 = (descriptors[i].row + x - 1);
								if (tx1 >= 0 && tx1 < image.width() && ty1 >= 0 && ty1 < image.height())
									image( tx1, ty1, 0, c) = color_point[c];				
							}
	  }
	image.get_normalize(0,255).save(filename);
}


//For Part1. To place 2 images side by side for comparison of matching SIFT points 
void join_images(CImg<double> &target, const CImg<double> &src_1, const CImg<double> &src_2){
	//copy Src Image to the left
	unsigned int width_1 = src_1.width();
	unsigned int height_1 = src_1.height();
	for(unsigned int outer_loop = 0; outer_loop < height_1; ++outer_loop){
			for(unsigned int inner_loop = 0; inner_loop < width_1; ++inner_loop){
					for(unsigned int rgb = 0; rgb < 3; ++rgb)
						target(inner_loop,outer_loop,0,rgb) = src_1(inner_loop,outer_loop,0,rgb);
			}
	}
	//Copy target Image to the right
	unsigned int width_2 = src_2.width();
	unsigned int height_2 = src_2.height();
	for(unsigned int outer_loop = 0; outer_loop < height_2; ++outer_loop){
			for(unsigned int inner_loop = 0; inner_loop < width_2; ++inner_loop){
					for(unsigned int rgb = 0; rgb < 3; ++rgb)
							target(inner_loop + width_1,outer_loop,0,rgb) = src_2(inner_loop,outer_loop,0,rgb);
			}
	}
}



double reduced_dimen_vector_distance(const SiftPoints sd_1, const SiftPoints sd_2, unsigned int dimension){
	double result = 0.0;
	for (unsigned int vector_i = 0; vector_i < dimension; ++vector_i){
			result += ((sd_1.points[vector_i] - sd_2.points[vector_i]) * (sd_1.points[vector_i] - sd_2.points[vector_i]));
	}
	return sqrt(result);
}


vector<SimilarPoints> projection_trials(const vector<SiftDescriptor> source, const vector<SiftDescriptor> target, int k, double w){
	vector<double> weight;
	srand(time(NULL));
	for(int loop = 0; loop < k; ++loop){
		weight.push_back(((double)rand() / (double)RAND_MAX));
	}

	vector<SiftPoints> source_dim;
	for(unsigned int loop = 0; loop < source.size(); ++loop){
		source_dim.push_back(SiftPoints(k));
		for(int desc = 0; desc < k; ++desc){
			source_dim[loop].points[desc] = (int)( ((double)((double)source[loop].descriptor[desc] * weight[desc])) / w);
		}
	}


	vector<SiftPoints> target_dim;
	for(unsigned int loop = 0; loop < target.size(); ++loop){
		target_dim.push_back(SiftPoints(k));
		for(int desc = 0; desc < k; ++desc){
			target_dim[loop].points[desc] = (int)( ((double)((double)target[loop].descriptor[desc] * weight[desc])) / w);
		}
	}


	vector<int> target_dim_matches;
	vector<SimilarPoints> sift_similar_points;
	double shortest_dist = LONG_MAX, second_shortest_dist = LONG_MAX, result_dist = 0.0;
	int matching_target_index = -1;
	for(unsigned int source_index = 0; source_index < source_dim.size(); ++source_index){
			target_dim_matches.clear();
			for(unsigned int target_index = 0; target_index < target_dim.size(); ++target_index){
					if(reduced_dimen_vector_distance(source_dim[source_index], target_dim[target_index], k) == 0)
						target_dim_matches.push_back(target_index);
			}
			//cout << target_dim_matches.size() << endl;
			shortest_dist = LONG_MAX;
			second_shortest_dist = LONG_MAX;
			matching_target_index = -1;
			for(unsigned int target_index = 0; target_index < target_dim_matches.size(); ++target_index){
					result_dist = sift_vector_distance(source[source_index], target[target_dim_matches[target_index]], SIFT_VECTOR_SIZE);
					if (result_dist < shortest_dist){
						second_shortest_dist = shortest_dist;
						shortest_dist = result_dist;
						matching_target_index = target_dim_matches[target_index];
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
	return sift_similar_points;
}



int main(int argc, char **argv)
{
  try {

    if(argc <= 2)
      {
	cout << "Insufficent number of arguments; correct usage:" << endl;
	cout << "    a2 part_id ..." << endl;
	return -1;
      }

    string part = argv[1];
    string inputFile = argv[2];

    if(part == "part1"){

    	if(argc <= 3)
      	{
			cout << "Insufficent number of arguments; correct usage:" << endl;
			cout << "    a2 part1 query_image ..." << endl;
			return -1;
      	}
    	string compareFile = argv[3];
		//Part 1 - 1
		if(argc == 4){		
			CImg<double> input_image((IMAGES_PART1_FOLDER + inputFile).c_str());
			CImg<double> query_image((IMAGES_PART1_FOLDER + compareFile).c_str());
			//Hue-Saturation-Intensity Color Conversion
			CImg<double> input_gray = input_image.get_RGBtoHSI().get_channel(2);
			CImg<double> query_gray = query_image.get_RGBtoHSI().get_channel(2);
			CImg<double> result_image(input_gray.width() + query_gray.width(), max(input_gray.height() , query_gray.height()), 1, 3, 0);
			vector<SiftDescriptor> input_descriptors = Sift::compute_sift(input_gray);
			vector<SiftDescriptor> query_descriptors = Sift::compute_sift(query_gray);
			cout << "Input Image Descriptors Found : " << input_descriptors.size() << endl;
			cout << "Query Image Descriptors Found : " << query_descriptors.size() << endl;
			vector<SimilarPoints> match_points = image_matching::image_matching(input_descriptors,query_descriptors);
			match_points = image_matching::processedMatches(match_points, input_gray.width());
			print_descriptor_image(input_image, input_descriptors, "input_image.png");
			print_descriptor_image(query_image, query_descriptors, "query_image.png");
			join_images(result_image,input_image,query_image);
			string join_filename = string(SIFT_MATCH_RESULT_WINDOW_PATH) + toString(((int)time_stamp())) + string("-") + 
					inputFile.replace(inputFile.end() - 4, inputFile.end(), "") + string("-") + 
						compareFile.replace(compareFile.end() - 4, compareFile.end(), "") + string("-") +
							toString((int)match_points.size()) + string(".png");
			draw_points_and_line(match_points, result_image, input_image.width());
			result_image.get_normalize(0,255).save(join_filename.c_str());	
			cout << join_filename.c_str() << " Matches SIFT points" << endl;
			//printf("Descriptor Size %d\n",descriptors.size());
		}
		//Part 1 - 2
		else{
			ssd_images *output;
			//arguments and RANSAC
			output = image_matching::multi_image_matching(argc, argv, 0);
			cout << "SIFT Matching Points found for all images" << endl;
			cout << endl << "***************------------------RESULT------------------***************" << endl;
			cout << "Query Image " << inputFile.c_str() << " has following matches:" << endl << endl; 			
			//Minus 3 becasue of a2 part1 <query_image>
			image_matching::sort_and_print_descending_order_matched_images(output, argc - 3);
		}
//return 1;
//	input_image.get_normalize(0,255).save("sift.png");
      }
    else if(part == "HMpart2")
      {
      		if(argc <= 3)
      		{
				cout << "Insufficent number of arguments; correct usage:" << endl;
				cout << "    a2 part1 query_image ..." << endl;
				return -1;
      		}
      		string compareFile = argv[3];
			CImg<double> input_image((IMAGES_PART1_FOLDER + inputFile).c_str());
			CImg<double> query_image((IMAGES_PART1_FOLDER + compareFile).c_str());
			//Hue-Saturation-Intensity Color Conversion
			CImg<double> input_gray = input_image.get_RGBtoHSI().get_channel(2);
			CImg<double> query_gray = query_image.get_RGBtoHSI().get_channel(2);
			vector<SiftDescriptor> input_descriptors = Sift::compute_sift(input_gray);
			vector<SiftDescriptor> query_descriptors = Sift::compute_sift(query_gray);
			cout << "Input Image Descriptors Found : " << input_descriptors.size() << endl;
			cout << "Query Image Descriptors Found : " << query_descriptors.size() << endl;
			vector<SimilarPoints> match_points = image_matching::image_matching(input_descriptors,query_descriptors);
			match_points = image_matching::processedMatches(match_points, input_gray.width());
			double **homo_matrix = RANSAC::find_homography_matrix(match_points);
			match_points = find_inliners(homo_matrix, match_points);
			cout << "Approx Inliers Found : " << match_points.size() << endl;
			/*CImg<double> output_part_2 = image_transform::image_2D_transform(homo_matrix, 3, 3, input_image);
			string join_filename = string(TRANSFORM_IMAGE_RESULT) + toString(((int)time_stamp())) + string("-") + 
					inputFile.replace(inputFile.end() - 4, inputFile.end(), "") + string("-transformation") + string(".png");
			output_part_2.get_normalize(0,255).save(join_filename.c_str());
			cout << "Lincoln Transformed Image Output : " << join_filename.c_str() << endl;*/


			CImg<double> result_image(input_gray.width() + query_gray.width(), max(input_gray.height() , query_gray.height()), 1, 3, 0);
			join_images(result_image,input_image,query_image);
			string join_filename = string(SIFT_MATCH_RESULT_WINDOW_PATH) + toString(((int)time_stamp())) + string("-") + 
					//inputFile.replace(inputFile.end() - 4, inputFile.end(), "") + string("-") + 
						//compareFile.replace(compareFile.end() - 4, compareFile.end(), "") + string("-") +
							toString((int)match_points.size()) + string(".png");
			draw_points_and_line(match_points, result_image, input_image.width());
			result_image.get_normalize(0,255).save(join_filename.c_str());	
			cout << endl << join_filename.c_str() << " Matches SIFT points" << endl;
      }
    else if(part == "part2"){
    		string compareFile = argv[3];
    		CImg<double> input_image((IMAGES_PART1_FOLDER + inputFile).c_str());
	  		CImg<double> query_image((IMAGES_PART1_FOLDER + compareFile).c_str());
	  		// rgb to HSI for query and input image
	  		CImg<double> query_gray = query_image.get_RGBtoHSI().get_channel(2);
	  		CImg<double> input_gray = input_image.get_RGBtoHSI().get_channel(2);
	  		CImg<double> result_image(input_gray.width() + query_gray.width(), max(input_gray.height() , query_gray.height()), 1, 3, 0);
	  		// Computing the SIFT descriptors for each image
	  		vector<SiftDescriptor> query_descriptors = Sift::compute_sift(query_gray);
	  		vector<SiftDescriptor> input_descriptors = Sift::compute_sift(input_gray);
	  		// Matching query and input image
	  		// To measure time
	  		clock_t t;
	  		t = clock();
	  		vector<SimilarPoints> match_points = image_matching::image_matching(query_descriptors, input_descriptors);
	  		cout << "Time For Naive Matching : " << (clock() - t) << endl;
	  		t = clock();
	  		vector<SimilarPoints> dimen_match_points = projection_trials(input_descriptors, query_descriptors, 10, 50.0);
	  		cout << "Time For Fast Matching (Less Is Better) : " << clock() - t << endl;
	  		cout << "Normal SIFT Matching Found " << match_points.size() << endl;
	  		cout << "Fast SIFT Matching Found " << dimen_match_points.size() << endl;
	  
	  join_images(result_image,input_image,query_image);
	  string join_filename = string(RANSAC_MATCH_RESULT_WINDOW_PATH) + toString(((int)time_stamp())) + string("-") + 
	    inputFile.replace(inputFile.end() - 4, inputFile.end(), "") + string("-") + 
	    compareFile.replace(compareFile.end() - 4, compareFile.end(), "") + string("-") +
	    toString((int)dimen_match_points.size()) + string(".png");
	  draw_points_and_line(dimen_match_points, result_image, input_image.width());
	  result_image.get_normalize(0,255).save(join_filename.c_str());
	  cout << "Fast SIFT Points Discovery showcased in file : " <<  join_filename.c_str() << endl;
    }
    else if(part == "part3")
      {
      	if(argc == 3){	 
      		CImg<double> input_image((IMAGES_PART3_FOLDER + inputFile).c_str());
      		double** constant_lincoln_matrix = new double*[3];
      		for(int init = 0; init < 3; ++init)
      			constant_lincoln_matrix[init] = new double[3];
      		constant_lincoln_matrix[0][0] = 0.907;
      		constant_lincoln_matrix[0][1] = 0.258;
      		constant_lincoln_matrix[0][2] = -182.0;
      		constant_lincoln_matrix[1][0] = -0.153;
      		constant_lincoln_matrix[1][1] = 1.44;
      		constant_lincoln_matrix[1][2] = 58.0;
      		constant_lincoln_matrix[2][0] = -0.000306;
      		constant_lincoln_matrix[2][1] = 0.000731;
      		constant_lincoln_matrix[2][2] = 1.0;
      		CImg<double> output_part_3 = image_transform::image_2D_transform(constant_lincoln_matrix, 3, 3, input_image);
			string join_filename = string(TRANSFORM_IMAGE_RESULT) + toString(((int)time_stamp())) + string("-") + 
					inputFile.replace(inputFile.end() - 4, inputFile.end(), "") + string("-transformation") + string(".png");
			output_part_3.get_normalize(0,255).save(join_filename.c_str());
			cout << "Lincoln Transformed Image Output : " << join_filename.c_str() << endl;
		}
		else{
			cout << "Image Wrapping Application Started With Base Image as : " << inputFile.c_str() << endl;
			CImg<double> input_image((IMAGES_PART3_FOLDER + inputFile).c_str());
			CImg<double> input_gray = input_image.get_RGBtoHSI().get_channel(2);	
			//Input Image SIFT descriptor
			vector<SiftDescriptor> input_descriptors = Sift::compute_sift(input_gray);
			vector<SiftDescriptor> query_descriptors;
			vector<SimilarPoints> match_points;
			CImg<double> output_part_3;
			double **homo_matrix;
			string output_image_name, join_filename;
			for(int loop = 3; loop < (argc); ++loop){
				//cout << (IMAGES_PART1_FOLDER + string(files[loop])).c_str();
				cout << "Reading " << argv[loop] << "..." << endl;
				CImg<double> query_image((IMAGES_PART3_FOLDER + string(argv[loop])).c_str());
				CImg<double> query_gray = query_image.get_RGBtoHSI().get_channel(2);
				query_descriptors = Sift::compute_sift(query_gray);
				match_points = image_matching::image_matching(query_descriptors,input_descriptors);
				match_points = image_matching::processedMatches(match_points, query_gray.width());				
				homo_matrix = RANSAC::find_homography_matrix(match_points);
				output_part_3 = image_transform::image_2D_transform(homo_matrix, 3, 3, query_image);
				output_image_name = string(argv[loop]);
				join_filename = string(TRANSFORM_IMAGE_RESULT) + toString(((int)time_stamp())) + string("-") + 
					output_image_name.replace(output_image_name.end() - 4, output_image_name.end(), "") + string("-transformation") + string(".png");
				output_part_3.get_normalize(0,255).save(join_filename.c_str());
				cout << join_filename.c_str() << " Wrapped Image According To Source Image : " << inputFile.c_str() << endl;
				for(int init = 0; init < 3; ++init)
					free(homo_matrix[init]);
				free(homo_matrix);


				CImg<double> result_image(input_gray.width() + query_gray.width(), max(input_gray.height() , query_gray.height()), 1, 3, 0);
				join_images(result_image,query_image,input_image);
				join_filename = string(SIFT_MATCH_RESULT_WINDOW_PATH) + toString(((int)time_stamp())) + string("-") + 
						//inputFile.replace(inputFile.end() - 4, inputFile.end(), "") + string("-") + 
							//compareFile.replace(compareFile.end() - 4, compareFile.end(), "") + string("-") +
								toString((int)match_points.size()) + string(".png");
				draw_points_and_line(match_points, result_image, query_gray.width());
				result_image.get_normalize(0,255).save(join_filename.c_str());	
				cout << join_filename.c_str() << " Matches SIFT points" << endl << endl << endl;
			}

		}
			  
      }
    else
      throw std::string("unknown part!");

    // feel free to add more conditions for other parts (e.g. more specific)
    //  parts, for debugging, etc.
  }
  catch(const string &err) {
    cerr << "Error: " << err << endl;
  }
}
