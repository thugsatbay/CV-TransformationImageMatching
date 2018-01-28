// B657 assignment 2 skeleton code
//
// Compile with: "make"
//
// See assignment handout for command line and project specifications.


#include "algorithm.cpp"
#include <dirent.h>
#include <sys/types.h>



//Use the cimg namespace to access the functions easily
using namespace cimg_library;
using namespace std;

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


vector<string> parseInput(string fName) {
    std::ifstream inFile(fName);
    string line;
    vector<string> inList = vector<string>();
    if(inFile.is_open()) {
        while(getline(inFile, line)) inList.push_back(line);
    }
    inFile.close();
    return inList;
}


vector<int> convertTo2D(double pX, double pY, double pZ) {

    double proj[] = {1,0,0,0.2,1,0,0.0001,0,1};
    double x = pX*proj[0] + pY*proj[1] + pZ*proj[2];
    double y = pX*proj[3] + pY*proj[4] + pZ*proj[5];
    double z = pX*proj[6] + pY*proj[7] + pZ*proj[8];
	
	vector<int> output;
	output.push_back((int)(x/z));
	output.push_back((int)(y/z));
	return output;
}



int main(int argc, char *argv[]) {
  vector<string> text = parseInput(argv[1]);
  
  CImg<double> output;
  output.assign(5000, 5000, 1, 3, 0);
  
  int greatestX = 0, greatestY = 0;
  
  for(int i = 0; i<text.size(); i++) {
  	vector<string> values = split(text[i], ' ');
  	int x = atoi(values[0].c_str());
  	int y = atoi(values[1].c_str());
  	int z = atoi(values[2].c_str());
  	vector<int> twoD = convertTo2D((double)x,(double)y,1);
  	vector<int> twoD2 = convertTo2D((double)x,(double)y+30,1);
  	if(twoD[0] >= 0 && twoD[0] < 5000 && twoD[1] >= 0 && twoD[1] < 5000) {
  		output(twoD[1], twoD[0], 0, 1)=100;
  		output(twoD[1], twoD[0], 0, 2)=100;
		output(twoD2[1], twoD2[0], 0, 0)=100;
		if(twoD2[1] > greatestX) greatestX = twoD2[1];
		if(twoD2[0] > greatestY) greatestY = twoD2[0];
  	}
  }
  
  CImg<double> output2;
  output2.assign(greatestX+250, greatestY+100, 1, 3, 0);
  
  for(int x = 0; x<greatestX+250; x++)
  	for(int y = 0; y<greatestY+100; y++) {
  		output2(x, y, 0, 0)=output(x, y, 0, 0);
  		output2(x, y, 0, 1)=output(x, y, 0, 1);
  		output2(x, y, 0, 2)=output(x, y, 0, 2);
  	}
  
	output2.get_normalize(0,255).save("threeD.png");
}











