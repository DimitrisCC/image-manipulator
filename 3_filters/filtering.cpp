#include <iostream>
#include <queue>
#include "Image.h"
#include "Filter.h"
#include "ppm_format.h"
#include "rle_format.h"
#include <cstring>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace imaging;

bool isValidFilter(string& f);
bool isValidColorValue(int v);
void errorFunc(const char* err);
bool isNumber(string input);

int main(int argc, char** argv){

	createLogger("log.txt");

	enum filter_t{ GRAY=0, COLOR, BLUR, MEDIAN, DIFF};

	queue<int> filters; 

	if(argc < 2){
		errorFunc("Not enough arguments given!"); 
	}

	string filename = argv[argc - 1];
	
	
	if(!(isSuffix(filename, "ppm") || isSuffix(filename, "rle"))){
		errorFunc("Not valid image format given (ppm/rle). Please try again!");      
	}
	
	
	if(argc == 2){
		/*if the execution is here then 2 arguments were given, the .exe file and the image file
		  therefore the execution must stop otherwise a .filtered image file will be created with no filtered image
		*/
		return 0;
	}
	
	if (!ifstream(filename.c_str())){
		errorFunc("The image you wanted to convert does not exist in the same path as your program! Please try again with an existing image!");
	}

	//a helper counter to check the input arguments validity
	int i = 1; //index 0 is the .exe file and index 1 is the filename
	
	string filt; //for ease of use

	while(i < argc -1){
		if(strcmp(argv[i], "-f") != 0){ //first check for wrong input
			errorFunc("Wrong argument given for filter! Please try again (-f filter1, -f filter2, ...)"); 
		}

		i++;
		filt = argv[i];
		
		if(!isValidFilter(filt)){
			errorFunc("You gave wrong filter name. Please try again! (gray/color/blur/median/diff)");
		}
		
		if(strcmp(filt.c_str(), "gray") == 0){
			filters.push(GRAY);
		}else if(strcmp(filt.c_str(), "blur") == 0){
			filters.push(BLUR);
		}else if(strcmp(filt.c_str(), "median") == 0){
			filters.push(MEDIAN);
		}else if(strcmp(filt.c_str(), "diff") == 0){
			filters.push(DIFF);
		}else{ //case it is "color"
			//now let's buffer the color values for the color filter, if valid
			if(isNumber(string(argv[i + 1])) && isNumber(string(argv[i + 2])) && isNumber(string(argv[i + 3]))){

				if(isValidColorValue(atoi(argv[i + 1])) && isValidColorValue(atoi(argv[i + 2])) && isValidColorValue(atoi(argv[i + 3]))){

					filters.push(COLOR);
					filters.push(atoi(argv[i + 1]));
					filters.push(atoi(argv[i + 2]));
					filters.push(atoi(argv[i + 3]));

					i += 3;

				} else
					errorFunc("The values you gave for the filter color must be between 0 and 254! Please try again with other values!");

			}else
				errorFunc("Error! Color Filter must be followed by its three color values (in numbers)! Please try again! (values 0-255)"); 
		}

		i++;

	}
	
	Image * myImage;
	//in case of an  error the execution will automatically stop
	//the lines below will not be executed
	
	if(isSuffix(filename, "ppm")){
		PPMImageReader reader;
		myImage = reader.read(filename);//MA

	}
	else{ //.rle]
		RLEImageReader reader;
		myImage = reader.read(filename);
		myImage->convertToColorPlaneImage();
	} 
	
	Filter * filterToApply;//MA  
	Color * given = new Color();//MA

	while(!filters.empty()){

		filter_t currentF = (filter_t)filters.front();
		filters.pop();

		switch(currentF){
			case GRAY:
				filterToApply = new Gray();
				break;
			case COLOR:
				given->x = (Component)filters.front();

				filters.pop();
				given->y = filters.front();

				filters.pop();
				given->z = filters.front();

				filters.pop();
				filterToApply = new ColorF(*given);

				//reinitialise given to be ready for the next use
				*given = Color(0, 0, 0);
				break;
			case BLUR:
				filterToApply = new Blur();
				break;
			case MEDIAN:
				filterToApply = new Median();
				break;
			case DIFF:
				filterToApply = new Diff();
				break;
			default:
				errorFunc("Unknown problem with filtering occurred. Please try again!");
		}//end of switch

		myImage = filterToApply->apply(*myImage);

		delete filterToApply;
		filterToApply = NULL;//MDA

		//now filterToApply is ready for the next use

	}

	delete given;//MDA
	
	if (myImage){

		string newfileName = filename.insert(filename.find("."), ".filtered");
		
		//Open correct writer
		
		if (isSuffix(newfileName, "ppm")){

			PPMImageWriter writer;
			writer.write(newfileName, *myImage);

		}
		else{ //.rle

			RLEImageWriter writer;
			writer.write(newfileName, *myImage);

		}

		delete myImage;//MDA
	}
	
	return 0;
}

bool isValidColorValue(int v){ return (v >= 0) && (v <= 255); } 

bool isValidFilter(string& f){ 

	transform(f.begin(), f.end(), f.begin(), ::tolower);
	
	const char* cs = f.c_str();
	return !strcmp(cs, "gray") || !strcmp(cs, "color") || !strcmp(cs, "blur") || !strcmp(cs, "median") || !strcmp(cs, "diff");

}

void errorFunc(const char* err)
{
	fprintf(stderr, "%s %s\n", "Error!", err);
	destroyLogger();
	system("PAUSE");
	exit(EXIT_FAILURE);
}

bool isNumber(string input)
{
	istringstream iss(input);
	int number;
	return ((iss >> number).fail()) ? false : true;
}
