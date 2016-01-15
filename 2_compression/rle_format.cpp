#include "rle_format.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>
#include "Block.h"

using namespace std;

namespace imaging
{
	// The RLE Image Writer breaks up the image into blocks of maximum size 1 X "block_length" (row blocks, i.e. line segments)
	// and encodes each one using run length encoding with an error "threshold" (see algorithm description and format specification
	// in assignment document). Therefore, the class is extended with the setBlockDimension and setThreshold to configure these
	// parameters
	//

	void RLEImageWriter::write(string filename, const Image & src)
	{
		ofstream output;
		output.open(filename.c_str(), ios::binary | ios::trunc);

		try{

			if (!output) throw("Error opening file to write");
			addLogEntry("Successfully opened " + filename + " for writing.");

			//------------------------------------------header-----------------------------------------//
			output << "CPI";

			__int8 edition = 2;
			output.write((char*)&edition, sizeof(edition));

			unsigned short LE_BE = 258;
			output.write((char*)&LE_BE, sizeof(LE_BE));

			unsigned short width = (unsigned short)src.getWidth();
			unsigned short height = (unsigned short)src.getHeight();

			output.write((char*)&width, sizeof(width));
			output.write((char*)&height, sizeof(height));

			output.write((char*)&block_length, sizeof(block_length));

			//---------------------------------- run-length encoding ----------------------------------//

			size_t b_size = block_length;
			size_t n_blocks = (((int)(src.getWidth()) % b_size) != 0) ?
					(((int)(src.getWidth()) / b_size)) + 1 : (((int)(src.getWidth()) / b_size)); //number of blocks
			size_t b_last = src.getWidth() - (n_blocks - 1) * b_size; //last block size


			Component current;
			vector<Component> converted; //rle formatted "buffer"
			converted.reserve(3*height*width);
			//Reserve the maximum possible space from the very beginning to make the push_back operation a little faster
			//and stop making the vector resize itself too frequently.
			//Most of the time the preallocated space will be wasted. Τhat's the nature of the compressing process below.
			//But, we believe it's a good time-space trade-off.
			//After the compression, the empty space is freed.
			vecmath::ivec2 iv(0,0); //current pixel in the image
			Block* block;
			Block::iterator it; //iterator for each new block
			Image::channel_t channel;

			for (unsigned int i = 0; i < 3; i++){ //iterate for each color

				switch (i){
				case 0: channel = Image::RED; break;
				case 1: channel = Image::GREEN; break;
				case 2: channel = Image::BLUE; break;
				}

				//resets iv for the next color
				iv.x = 0;
				iv.y = 0;

				for (; iv.y < (int)src.getHeight(); iv.y++){ //this loop represents each line

					for (size_t b = 0; b < n_blocks; b++){ //iterates to create each block

						if ((b_last != 0) && (b == n_blocks - 1)) b_size = b_last; 

						block = Block::copyFromImage(*(const_cast<Image*>(&src)), channel, iv, b_size);

						it = block->begin();
						Component count = 0;

						while( it != block->end()){

							count = 1;  
							current = *it;
							++it;

							if (threshold != '0'){
								while ((!(it == block->end())) && ((abs((int)*it - (int)current)) < (int) threshold) && (count < 255)){ 
									++it;
									++count;
								}
							}
							else {
								while ((!(it == block->end())) && (*it == current) && (count < 255)){
									++it;
									++count;
								}
							}

							converted.push_back(current);
							converted.push_back(count);

						}

						iv.x += (int)b_size;

						delete block;
					}
					//resets x and b_size for the next line
					iv.x = 0; 
					b_size = block_length;

				}
			}
			//free that wasted space
			converted.resize(converted.size()); // it's not necessary but we use it for a more complete code

			Component* c_buffer = new Component[converted.size()];
			std::copy(converted.begin(), converted.end(), c_buffer);
			output.write((char*)c_buffer, (streamsize) sizeof(Component)*converted.size());

			std::cout << "The image was successfully written to " << filename << "\n";
			addLogEntry("The image was successfully written to " + filename);

		}
		catch (char * err){
			fprintf(stderr, "%s\n", err);
			addLogEntry(err);
		}

		output.close();

	}

	Image* RLEImageReader::read(string filename)
	{
		ifstream input(filename.c_str(), ios::binary);

		try{
			if (!input) throw("The file you try to open cannot open or does not exist! Please try again");
			addLogEntry("Successfully opened "+filename+" for reading.");

			//we need to calculate the size of the image buffer
			//cause it can't be directly calculated by the header

			input.seekg(0, input.end);
			int length = input.tellg();
			input.seekg(0, input.beg);
			length -= 12; //full length of the file minus the header, thus the size of the image buffer

			char cpi[3];

			input.read(cpi, sizeof(char) * 3);

			if ((cpi[0] != 'C') || (cpi[1] != 'P') || (cpi[2] != 'I')) throw("The image you tried to open is not CPI type. Please try again with another image");

			__int8 edition;
			input.read(&edition, sizeof(edition));
			if (edition != 2) throw("The image you tried to open is not of the proper CPI edition (2nd). Please try again with another image");

			unsigned short BE_LE;
			input.read((char *)&BE_LE, sizeof(BE_LE));
			if (BE_LE != 258) throw("The image you tried to open cannot be read. Please try again with another image");

			unsigned short width, height, block_size;
			input.read((char *)&width, sizeof(width));
			input.read((char *)&height, sizeof(height));
			input.read((char *)&block_size, sizeof(block_size));

			Component * buff = new Component[length];
			input.read((char*)buff, sizeof(Component)*length);

			vector<Component> decoded; //contains the decoded-to-cpi image buffer
			for(int i = 0; i < length; i += 2){
				for(unsigned int j = 0; j < buff[i+1]; ++j){
					if(j == block_size) break;
					decoded.push_back(buff[i]);
				}
			}

			Component* dec_buffer = new Component[decoded.size()];
			std::copy(decoded.begin(), decoded.end(), dec_buffer);

			Image * returnedImage = new Image(width, height, dec_buffer, false);

			delete[] buff;
			delete[] dec_buffer;

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
} //namespace imaging
