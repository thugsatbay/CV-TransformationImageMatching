#ifndef _MATRIX

#include <iomanip>
#include <cmath>
#include <stdlib.h>
#include <iostream>

using namespace std;

namespace matrix{


void print_matrix(double** matrix, int row, int col){
	const int fileWidth = 15;
	const char divideSeperator = ' ';
	for(int matrix_x = 0; matrix_x < row; ++matrix_x){
		for(int matrix_y = 0; matrix_y < col; ++matrix_y){
			cout << left << setw(fileWidth) << setfill(divideSeperator) << matrix[matrix_x][matrix_y];
		}
		cout << endl;
	}
}


double determinant(double **matrix, int size){
	double det_value = 0.0;
	if(size == 1)
		return (double)matrix[0][0];
	else{
		double **sub_matrix = new double*[size - 1];
		for(int init = 0; init < size - 1; ++init)
			sub_matrix[init] = new double[size - 1];
		det_value = 0.0;
		int sub_factor_x = 0, sub_factor_y = 0;
		//Copy Sub-Factor Matrix in sub_matrix and finds its determinent
		for(int top_element = 0; top_element < size; ++top_element){
			sub_factor_x = 0; sub_factor_y = 0;
			for(int move_x = 0; move_x < size; ++move_x){
					for(int move_y = 0; move_y < size; ++move_y){
							if((move_x != 0) && (top_element != move_y)){
								sub_matrix[sub_factor_x][sub_factor_y++] = matrix[move_x][move_y];
								if(sub_factor_y == size-1){
									sub_factor_y = 0;
									sub_factor_x++;
								}
							}
					}
			}
		//Pass the sub_matix for its determinant calculation, -ve sign based on odd even value
		det_value += (double)matrix[0][top_element] * ((double)pow(-1,top_element)) * determinant(sub_matrix, size - 1);
		}
		//Free memory, useful if matrix is large
		for(int init = 0; init < size-1; ++init)
			free(sub_matrix[init]);
		free(sub_matrix);
	}
	return det_value;

}


double** adjacent_matrix(double** matrix,int size){
	double **adj_matrix = new double*[size];
	double **sub_matrix = new double*[size - 1];
	for(int init = 0; init < size; ++init)
			adj_matrix[init] = new double[size];
	for(int init = 0; init < size - 1; ++init)
			sub_matrix[init] = new double[size - 1];
	int sub_matrix_x = 0, sub_matrix_y = 0;
	//Calculate adjoint matrix of size-1 for each position by copying elements to sub-matrix
	for(int adj_pos_x = 0; adj_pos_x < size; ++adj_pos_x){
		for(int adj_pos_y = 0; adj_pos_y < size; ++adj_pos_y){
			sub_matrix_x = 0;
			sub_matrix_y = 0;
			for(int adj_x = 0; adj_x < size; ++adj_x){
				for(int adj_y = 0; adj_y < size; ++adj_y){
					if((adj_x != adj_pos_x) && (adj_y != adj_pos_y)){
						sub_matrix[sub_matrix_x][sub_matrix_y++] = matrix[adj_x][adj_y];
						if(sub_matrix_y == (size - 1)){
							sub_matrix_x++;
							sub_matrix_y = 0;
						}
					}
				}
			}
			//Find the determinant of sub-matrix
			adj_matrix[adj_pos_y][adj_pos_x] = pow(-1, (adj_pos_x + adj_pos_y)) * determinant(sub_matrix,size - 1);
		}
	}
	for(int init = 0; init < size-1; ++init)
		free(sub_matrix[init]);
	free(sub_matrix);
	return adj_matrix;
}


double** inverse_matrix(double** A, int size){
	double ** answer;
	answer = adjacent_matrix(A, size);
	double det_value = determinant(A, size);
	for(int outer_loop = 0; outer_loop < size; ++outer_loop)
		for(int inner_loop = 0; inner_loop < size; ++inner_loop)
			answer[outer_loop][inner_loop] /= det_value;
	return answer;
}


double** matrix_multiplication(double** left_matrix, double** right_matrix, int left_row, int left_column, int right_row, int right_column){
	if (left_column != right_row)
		return NULL;
	double **result_matrix = new double*[left_row];
	for(int init = 0; init < left_row; ++init)
		result_matrix[init] = new double[right_column];
	for(int result_x = 0; result_x < left_row; ++result_x){
		for(int result_y = 0; result_y < right_column; ++result_y){
			result_matrix[result_x][result_y] = 0;
			for(int index_entry = 0; index_entry < left_column; ++index_entry)
				result_matrix[result_x][result_y] += left_matrix[result_x][index_entry] * 
						right_matrix[index_entry][result_y];
		}
	}
	return result_matrix;
}


//Namespace Ends
}

#define _MATRIX 0
#endif