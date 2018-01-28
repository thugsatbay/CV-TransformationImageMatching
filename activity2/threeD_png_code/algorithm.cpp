// B657 assignment 2 skeleton code
//
// Compile with: "make"
//
// See assignment handout for command line and project specifications.


//Link to the header file
#define cimg_use_jpeg
#include "CImg.h"
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include "Sift.h"

#include <fstream>
#include <sstream>
#include <list>

//Use the cimg namespace to access the functions easily
using namespace cimg_library;
using namespace std;


vector< vector<int> > hashSift(vector<SiftDescriptor>* descriptors) {
	
	// hash table that will contain indexes of the above table located at their least squares value
	vector< vector<int> >  descLookup (pow(128,2)*128*64,vector<int>());

	//calc least square value of descriptor, and puts its index in hash table
	for(long i=0; i<descriptors->size(); i++) {
		long total = 0;
		for(int l=0; l<128; l++) {
			total += (*descriptors)[i].descriptor[l] * pow(l,2);
		}
		total = sqrt(total);
		while(descLookup.size()-1 < total) {
			vector<int> row;
			descLookup.push_back(row);
		}
		descLookup[total].push_back(i);
	}
	return descLookup;
}

double calcDistance(SiftDescriptor s1, SiftDescriptor s2) {
	double total = 0;
	for(int l=0; l<128; l++) total += abs(s1.descriptor[l] - s2.descriptor[l]);
	return sqrt(total);
}


vector< vector<int> > findMatches(vector< vector<int> >* hash1, vector<SiftDescriptor>* sift1, vector< vector<int> >* hash2, vector<SiftDescriptor>* sift2, double threshold) {
	vector< vector<int> > matches;
	
	for(int i = 0; i<hash1->size(); i++) {
		for(int ii = 0; ii<(*hash1)[i].size(); ii++) {
			double closestDist = 100000, secondClosestDist = 100001;
			int closestIndex = -1;
			for(int j = i-5; j<i+5; j++) {
				if(j>=0 && j<hash2->size()) {
					for(int jj = 0; jj<(*hash2)[j].size(); jj++) {
						double distance = calcDistance((*sift1)[(*hash1)[i][ii]], (*sift2)[(*hash2)[j][jj]]);
						if(distance < closestDist) {
							secondClosestDist = closestDist;
							closestDist = distance;
							closestIndex = (*hash2)[j][jj];
						}
					}
				}
			}
			if(closestDist < threshold && (secondClosestDist - closestDist)/closestDist > closestDist/4) {
				vector<int> match;
				match.push_back((*hash1)[i][ii]);
				match.push_back(closestIndex);
				matches.push_back(match);
			}
		}
	}
	return matches;
}


vector< vector<int> > calcLine(int x1, int y1, int x2, int y2) {
	vector< vector<int> > output;
	double slope = (double)(y2-y1)/(double)(x2-x1);
	int startX = x1, startY = y1;
	if(x2<startX) {
		startX = x2;
		startY = y2;
	}
	for(int i = 1; i<abs(x2-x1); i+=2) {
		int diff = i*slope;
		vector<int> point;
		point.push_back(startX+i);
		point.push_back(diff+startY);
		output.push_back(point);
	}
	return output;
}


CImg<double> mergeImages(CImg<double>* p1, CImg<double>* p2, vector< vector<int> >* matches, vector<SiftDescriptor>* d1, vector<SiftDescriptor>* d2) {
	int width = p1->width() + p2->width();
	int height = p1->height();
	if(p2->height() > height) height = p2->height();
	CImg<double> output;
	output.assign(width, height, 1, 3, 255);
	
	for(int x = 0; x<p1->width(); x++)
		for(int y = 0; y<p1->height(); y++) {
			for(int p=0; p<3; p++)
		    	output(x, y, 0, p)=(*p1)(x, y, 0, p);
		}
	for(int x = 0; x<p2->width(); x++)
		for(int y = 0; y<p2->height(); y++) {
			for(int p=0; p<3; p++)
		    	output(x+p1->width(), y, 0, p)=(*p2)(x, y, 0, p);
		}
	for(int i = 0; i<matches->size(); i++) {
		int r = rand() % 100 + 100;
		int b = rand() % 100 + 100;
		int g = rand() % 100 + 100;
		vector< vector<int> > line = calcLine((*d1)[(*matches)[i][0]].col, (*d1)[(*matches)[i][0]].row, (*d2)[(*matches)[i][1]].col + p1->width(), (*d2)[(*matches)[i][1]].row);
		for(int ii = 0; ii<line.size(); ii++) {
			output(line[ii][0], line[ii][1], 0, 0)=r;
			output(line[ii][0], line[ii][1], 0, 1)=b;
			output(line[ii][0], line[ii][1], 0, 2)=g;
		}
	}
	return output;
}

vector<string> readDirectory(string path) {
	vector<string> output;
	dirent* de;
  	DIR* dp;
  	errno = 0;
  	dp = opendir(path.empty() ? "." : path.c_str() );
  	if (dp) {
    	while(true) {
      	errno = 0;
      	de = readdir( dp );
      	if (de == NULL) break;
      	string name = de->d_name;
      	if(name.length() > 5) output.push_back(path + "/" + de->d_name);
    	}
    	closedir( dp );
    }
    return output;
}





