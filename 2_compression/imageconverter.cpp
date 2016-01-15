#include <iostream>
#include "Image.h"
#include "cpi_format.h"
#include "ppm_format.h"
#include "rle_format.h"
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include <sstream>
#include "conio.h"

using namespace std;
using namespace imaging;

bool isValidAnswer(string answer);
bool isNumber(string input);
void errorFunc(const char* err);

int main(int argc, char** argv)
{

	string fileName, newfileName;
	createLogger("log.txt");
	int block_size;
	Component error;

	// Please excuse our if_else-monster for we appreciate an error-proof code.

	if (argc > 1 && argc%2 == 0){

		if (strcmp(argv[1], "-o") == 0){
			fileName = argv[2];
			newfileName = argv[3];

			if (isSuffix(newfileName, "rle")){
				if (argc == 6){ //one of the two other parameters and its value
					if (strcmp(argv[4], "-b") == 0){
						if (isNumber(argv[5]))
							block_size = atoi(argv[5]);
						else
							errorFunc("Invalid values! Try again.");
					}
					else if (strcmp(argv[4], "-e") == 0){
						if (isNumber(argv[5]))
							error = atoi(argv[5]);
						else
							errorFunc("Invalid values! Try again.");
					}
					else
						errorFunc("Invalid values! Try again.");
				}
				else if (argc == 8){ //it has all the parameters

					if ((strcmp(argv[4], "-b") == 0) && (strcmp(argv[6], "-e") == 0)){
						if (!isNumber(argv[5]) || !isNumber(argv[7]))
							errorFunc("Invalid values! Try again.");
						//if not valid parameters were given exits the system befove the above assignments
						block_size = atoi(argv[5]);
						error = atoi(argv[7]);
					}
					else if ((strcmp(argv[4], "-e") == 0) && (strcmp(argv[6], "-b") == 0)){
						if (!isNumber(argv[5]) || !isNumber(argv[7]))
							errorFunc("Invalid values! Try again.");
						//if not valid parameters were given exits the system befove the above assignments
						error = atoi(argv[5]);
						block_size = atoi(argv[7]);
					}
					else
						errorFunc("Invalid values! Try again.");

				}
				else if (argc == 4){ //only the two images are given for the conversion
					//thus default values are given
					error = 2;
					block_size = 32;
				}
				else{
					errorFunc("Too many arguments. Please try again.");
				}

				if ((error < 0) || (block_size <= 0)) //an  invalid integer was given
					errorFunc("Invalid values! Try again.");
			}
			else if (argc > 4){
				errorFunc("Invalid arguments! (-b and -e are not used in this convertion) Please try again!");
			}

			/* Check for:
			 * 1. Same file formats. No need to convert
			 * 2. Unknown file formats
			 */

			//to avoid lot of function calls
			string fileName_format = isSuffix(fileName, "cpi") ? "cpi" : (isSuffix(fileName, "ppm") ? "ppm" : (isSuffix(fileName, "rle") ? "rle" : "unknown"));
			string newfileName_format = isSuffix(newfileName, "cpi") ? "cpi" : (isSuffix(newfileName, "ppm") ? "ppm" : (isSuffix(newfileName, "rle") ? "rle" : "unknown"));

			if ((fileName_format == "unknown") || (newfileName_format == "unknown"))
				errorFunc("Unknown format file! Try again with another file");

			if (fileName_format == newfileName_format)
				errorFunc("The conversion cannot be done to files with same formats. Try again");

		}
		else{

			fileName = argv[1];

			if (!isSuffix(fileName, "rle")){
				if (argc == 4){ //one of the two parameters with its value
					if (strcmp(argv[2], "-b") == 0){
						if (isNumber(argv[3]))
							block_size = atoi(argv[3]);
						else
							errorFunc("Invalid values! Try again.");
					}
					else if (strcmp(argv[2], "-e") == 0){
						if (isNumber(argv[3]))
							error = atoi(argv[3]);
						else
							errorFunc("Invalid values! Try again.");
					}
					else
						errorFunc("Invalid values! Try again.");
				}
				else if (argc == 6){ //both parameters with their values

					if ((strcmp(argv[2], "-b") == 0) && (strcmp(argv[4], "-e") == 0)){

						if (!isNumber(argv[3]) || !isNumber(argv[5]))
							errorFunc("Invalid values! Try again.");

						block_size = atoi(argv[3]);
						error = atoi(argv[5]);
					}
					else if ((strcmp(argv[2], "-e") == 0) && (strcmp(argv[4], "-b") == 0)){

						if (!isNumber(argv[3]) || !isNumber(argv[5]))
							errorFunc("Invalid values! Try again.");
						//if not valid parameters were given exits the system befove the above assignments
						error = atoi(argv[3]);
						block_size = atoi(argv[5]);
					}
					else
						errorFunc("Invalid values! Try again.");
				}
				else if (argc == 2) {
					//only the image to compress is given
					//thus default values are given
					error = 2;
					block_size = 32;
				}
				else{
					errorFunc("Too many arguments. Please try again.");
				}

				if ((error < 0) || (block_size <= 0)) //an  invalid integer was given
					errorFunc("Invalid values! Try again.");
			
			}
			else if (argc > 2) {
				errorFunc("Invalid arguments! (-b and -e are not used in this convertion) Please try again!");
			}

			if (isSuffix(fileName, "cpi") || isSuffix(fileName, "ppm") || isSuffix(fileName, "rle")){

				newfileName = fileName;
				bool isRLE = isSuffix(fileName, "rle");
				replaceSuffix(newfileName, isRLE ? "cpi" : "rle");
				string answer;

				do{
					cout << "Save photo as " << newfileName << " ? (y)es/(n)o \n";
					cin >> answer;
				} while (!isValidAnswer(answer));

				bool timeToWrite = true;

				do{
					if ((answer == "n") || (answer == "N")){
						cout << "Output name (without extension): ";
						cin >> newfileName;
						newfileName.append(isRLE ? ".cpi" : ".rle");
						timeToWrite = true;
					}

					if (ifstream(newfileName.c_str())){
						do{
							cout << "The file exists. Overwrite? (y)es/(n)o \n";
							cin >> answer;
						} while (!isValidAnswer(answer));

						if ((answer == "N") || (answer == "n")) timeToWrite = false;
					}

				} while (!timeToWrite);

			}
			else
				errorFunc("Unknown format file! Try again with another file");
		}

	}
	else
		errorFunc("Invalid arguments! Try again.");


	//Now leeeet's play with that image

	Image * myImage;

	//in case of an  error the execution will automatically stop
	//the lines below will not be executed

	//Open correct reader
	if (isSuffix(fileName, "cpi")){

		CPIImageReader reader;
		myImage = reader.read(fileName);//MA

	}
	else if(isSuffix(fileName, "ppm")){

		PPMImageReader reader;
		myImage = reader.read(fileName);//MA

	}
	else{ //.rle

		RLEImageReader reader;
		myImage = reader.read(fileName);

	}

	if (myImage){
		//Open correct writer
		if (isSuffix(newfileName, "cpi")){

			myImage->convertToColorPlaneImage();
			CPIImageWriter writer;
			writer.write(newfileName, *myImage);

		}
		else if (isSuffix(newfileName, "ppm")){

			myImage->convertToInterleavedImage();
			PPMImageWriter writer;
			writer.write(newfileName, *myImage);

		}
		else{ //.rle

			myImage->convertToColorPlaneImage();
			RLEImageWriter writer;
			writer.setBlockDimension(block_size);
			writer.setThreshold(error);
			writer.write(newfileName, *myImage);

		}

		delete myImage;
		myImage = NULL;
	}

	destroyLogger();
	return 0;
}


bool isValidAnswer(string answer)
{
	return (answer == "Y") || (answer == "y") || (answer == "N") || (answer == "n");
}

void errorFunc(const char* err)
{
	fprintf(stderr, "%s %s\n", "Error!", err);
	destroyLogger();
	exit(EXIT_FAILURE);
}

bool isNumber(string input)
{
	istringstream iss(input);
	int number;
	return ((iss >> number).fail()) ? false : true;
}
