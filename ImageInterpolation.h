#ifndef _INTERPOLATION

#include <cmath>
#include "CImg.h"


using namespace cimg_library;

#define bicubic_2D_x_a 1.0
#define bicubic_2D_y_a 1.0

namespace image_interpolation{


double bicubic_interpolation_convolution_kernel(const char _2D, double v){
	if (v < 0.0)
		v = -1.0 * v;
	if ((v >= 0.0) && (v < 1.0)){
		if (_2D == 'x')
			return (((2.0 - bicubic_2D_x_a) * v * v * v) + 
				((bicubic_2D_x_a - 3.0) * v * v) + 1.00);
		else if(_2D == 'y')
			return (((2.0 - bicubic_2D_y_a) * v * v * v) + 
				((bicubic_2D_y_a - 3.0) * v * v) + 1.00);
	}
	else if ((v >= 1.0) && (v < 2.0)){
		if (_2D == 'x')
			return (((- bicubic_2D_x_a) * v * v * v) + ((5.0 * bicubic_2D_x_a) * v * v) + 
				(4.0 * bicubic_2D_x_a) - (8.0 * v * bicubic_2D_x_a));
		else if(_2D == 'y')
			return (((- bicubic_2D_y_a) * v * v * v) + ((5.0 * bicubic_2D_y_a) * v * v) + 
				(4.0 * bicubic_2D_y_a) - (8.0 * v * bicubic_2D_y_a));
	}
	return 0.0;
}


double bicubic_interpolation(const CImg<double> &grid, double x0, double y0, int width, int height, int color_spectrum){
	double interpolated_value = 0.0, intermediate_interpolated_value = 0.0;
	int floored_y = 0, floored_x = 0;
	for (int loop_y = 0; loop_y < 4; ++loop_y){
		floored_y = ((int)floor(y0)) + loop_y - 1;
		intermediate_interpolated_value = 0.0;
		for(int loop_x = 0; loop_x < 4; ++loop_x){
			floored_x = ((int)floor(x0)) + loop_x - 1;
			if ((floored_x >= 0) && (floored_x < width) && (floored_y >= 0) && (floored_y < height))
				intermediate_interpolated_value += grid(floored_x, floored_y, 0, color_spectrum) * 
					bicubic_interpolation_convolution_kernel('x', (x0 - (double)floored_x));
		}
		interpolated_value += intermediate_interpolated_value * 
			bicubic_interpolation_convolution_kernel('y', (y0 - (double)floored_y));
	}
	return interpolated_value;
} 


}

#define _INTERPOLATION 0
#endif