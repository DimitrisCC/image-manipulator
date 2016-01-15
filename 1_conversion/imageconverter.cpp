#include <iostream>
#include "Image.h"
#include "cpi_format.h"
#include "ppm_format.h"
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include "conio.h"

using namespace std;
using namespace imaging;

bool isValidAnswer(string answer);
void errorFunc(const char* err);

int main(int argc, char** argv)
{

	string fileName, newfileName;
	createLogger("log.txt");

	if (argc == 2){
		fileName = argv[1];

		if (isSuffix(fileName, "cpi") || isSuffix(fileName, "ppm")){

			newfileName = fileName;

			bool isCPI = isSuffix(fileName, "cpi");

			replaceSuffix(newfileName, isCPI ? "ppm" : "cpi");

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
					newfileName.append(isCPI ? ".ppm" : ".cpi");
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
		else{
			errorFunc("Unknown format file! Try again with another file");
		}

	}
	else if (argc == 4){

		if (strcmp(argv[1], "-o") == 0){
			fileName = argv[2];
			newfileName = argv[3];

			/* Check for:
				* 1. Same file formats. No need to convert
				* 2. Unknown file formats
				*/

			//to avoid lot of function calls
			string fileName_format = isSuffix(fileName, "cpi") ? "cpi" : (isSuffix(fileName, "ppm") ? "ppm" : "unknown");
			string newfileName_format = isSuffix(newfileName, "cpi") ? "cpi" : (isSuffix(newfileName, "ppm") ? "ppm" : "unknown");

			if ((fileName_format == "unknown") || (newfileName_format == "unknown")){ 
				errorFunc("Unknown format file! Try again with another file");
			}
			if (fileName_format == newfileName_format){
				errorFunc("The conversion cannot be done to files of these formats. Try again");
			}

		}
		else{
			errorFunc("Invalid values! Try again.");
		}

	}
	else{
		errorFunc("Invalid values! Try again.");
	}

	//in case of an  error the execution will automatically stop
	//the lines below will not be executed
	if (isSuffix(fileName, "cpi")){
		CPIImageReader reader;//MA
		Image * myImage = reader.read(fileName);//MA

		if (myImage){
			(*myImage).convertToInterleavedImage();
			PPMImageWriter writer;
			writer.write(newfileName, *myImage);

			delete myImage;//MDA
		}

	}
	else{ //.ppm
		PPMImageReader reader;
		Image * myImage = reader.read(fileName);//MA

		if (myImage){
			(*myImage).convertToColorPlaneImage();
			CPIImageWriter writer;
			writer.write(newfileName, *myImage);

			delete myImage;//MDA
		}

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