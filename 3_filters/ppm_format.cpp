#include "ppm_format.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <cstdio>
#include <fstream>
#include "rle_format.h"

using namespace std;

namespace imaging
{

	void PPMImageWriter::write(std::string filename, const Image & src)
	{

		ofstream output;
		output.open(filename.c_str(), ios::binary | ios::trunc);

		try{

			if (!output) throw("Error opening file to write!");
			addLogEntry("Successfully opened "+filename+" for writing.");

			output << "P6" << endl;

			unsigned short width = (unsigned short)src.getWidth();
			unsigned short height = (unsigned short)src.getHeight();

			output << width << endl << height << endl << "255" << endl;

			Component * imgBuffer = src.getRawDataPtr();

			output.write((char *)(imgBuffer), width*height * 3);

			cout << "The image was successfully written to " << filename << "\n";//auto??
			addLogEntry("The image was successfully written to "+filename);

		}
		catch (char * err){
			fprintf(stderr, "%s\n", err);
			addLogEntry(err);
		}
	
		output.close();

	}

	Image * PPMImageReader::read(std::string filename)
	{
		
		ifstream input;
		input.open(filename.c_str(), ios::binary);
		string width, height;
		unsigned int depth;

		try{
			if (!input) throw("Can't open the file.");
			addLogEntry("Successfully opened "+filename+" for reading.");
			
			string header;
			input >> header;


			int sharp = header.find("#");
			if (sharp != -1)
				header.erase(sharp, string::npos);

			if (strcmp(header.c_str(), "P6") != 0) throw("Can't read input file. Not P6 format.");
			
			char next = (char)input.peek();
			while ((next == ' ')||(next == '\t')||(next == '\n')||(next == '#')){
				if (next == '#'){
					while (next != '\n'){
						input.get();
						next = (char)input.peek();
					}
				}
				else
					input.get();
				next = (char)input.peek();
			}
			
			input >> width;

			sharp = width.find("#");
			if (sharp != -1)
				width.erase(sharp, string::npos);
			
			next = (char)input.peek();
			while ((next == ' ') || (next == '\t') || (next == '\n') || (next == '#')){
				if (next == '#'){
					while (next != '\n'){
						input.get();
						next = (char)input.peek();
					}
				}
				else
					input.get();
				next = (char)input.peek();
			}

			input >> height; 


			sharp = height.find("#");
			if (sharp != -1)
				height.erase(sharp, string::npos);

			next = (char)input.peek();
			while ((next == ' ') || (next == '\t') || (next == '\n') || (next == '#')){
				if (next == '#'){
					while (next != '\n'){
						input.get();
						next = (char)input.peek();
					}
				}
				else
					input.get();
				next = (char)input.peek();
			}

			input >> depth;
			if (depth > 255) throw("Can't read input file. Not 24bit image.");

			input.get(); 

			int W = atoi(width.c_str());
			int H = atoi(height.c_str());

			Component * imgBuffer = new Component[W*H*3];
			
			input.read((char *)(imgBuffer), W*H*3);

			Image * image = new Image(W, H, imgBuffer, true);

			input.close();

			delete[] imgBuffer;
			addLogEntry("The image was successfully read.");
			return image;

		}
		catch (const char *err) {
			fprintf(stderr, "%s\n", err);
			addLogEntry(err);
			input.close();
			return NULL;
		}
	}

}	
