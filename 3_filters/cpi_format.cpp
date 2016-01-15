#include "cpi_format.h"
#include <stdio.h>
#include <iostream>
#include <fstream>

namespace imaging 
{

	void CPIImageWriter::write(std::string filename, const Image & src)
	{

		ofstream output;
		output.open(filename.c_str(), ios::binary | ios::trunc);

		try{

			if (!output) throw("Error opening file to write");
			addLogEntry("Successfully opened "+filename+" for writing.");

			output << "CPI";

			__int8 edition = 1;

			output.write((char*)&edition, sizeof(edition));

			unsigned short LE_BE = 258;

			output.write((char*)&LE_BE, sizeof(LE_BE));

			unsigned short width = (unsigned short)src.getWidth();
			unsigned short height = (unsigned short)src.getHeight();

			output.write((char*)&width, sizeof(width));

			output.write((char*)&height, sizeof(height));

			Component * imgBuffer = src.getRawDataPtr();

			output.write((char*)imgBuffer, (streamsize)sizeof(Component)*width*height * 3);

			std::cout << "The image was successfully written to " << filename << "\n";
			addLogEntry("The image was successfully written to "+filename);

		}
		catch (char * err){
			fprintf(stderr, "%s\n", err);
			addLogEntry(err);
		}

		output.close();

	}

	Image * CPIImageReader::read(std::string filename)
	{

		ifstream input(filename.c_str(), ios::binary);

		try{
			if (!input) throw("The file you try to open cannot open or does not exist! Please try again");
			addLogEntry("Successfully opened "+filename+" for reading.");

			char cpi[3];

			input.read(cpi, sizeof(char) * 3);

			if ((cpi[0] != 'C') || (cpi[1] != 'P') || (cpi[2] != 'I')) throw("The image you tried to open is not CPI type. Please try again with another image");

			__int8 edition;
			input.read(&edition, sizeof(edition));
			if (edition != 1) throw("The image you tried to open is not of the proper CPI edition (1st). Please try again with another image");

			unsigned short BE_LE;
			input.read((char *)&BE_LE, sizeof(BE_LE));
			if (BE_LE != 258) throw("The image you tried to open cannot be read. Please try again with another image");

			unsigned short width, height;
			input.read((char *)&width, sizeof(width));
			input.read((char*)&height, sizeof(height));

			Component * buff = new Component[width*height * 3];
			input.read((char*)buff, sizeof(Component)*width*height * 3);

			Image * returnedImage = new Image(width, height, buff, false);

			delete[] buff;
			
			input.close(); 
			addLogEntry("The image was successfully read.");
			return returnedImage;

		}
		catch (char * err){
			fprintf(stderr, "%s\n", err);
			addLogEntry(err);
			input.close();
			destroyLogger();
			return NULL;
		}

	}

}//namespace imaging

