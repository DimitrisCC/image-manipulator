#include "Filter.h"
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

namespace imaging
{

	Image* Gray::apply(Image& src)
	{

		const unsigned int height = src.getHeight();
		const unsigned int width = src.getWidth();
		Component mean;  //the mean value of the sub-pixels values
		Color pixel;

		for(unsigned int i = 0; i < width; ++i){
			for(unsigned int j = 0; j < height; ++j){
				pixel = src.getPixel(i, j);
				mean = (Component) (((float)pixel.x + (float)pixel.y + (float)pixel.z) / 3);
				src.setPixel(i, j, Color(mean, mean, mean));
			}
		}

		return &src;
	}

	Image* ColorF::apply(Image& src)
	{

		const unsigned int height = src.getHeight();
		const unsigned int width = src.getWidth();
		Color pixel;

		vec3 cf = vec3(((float)filter_c.x)/255.0f, ((float)filter_c.y)/255.0f, ((float)filter_c.z)/255.0f);

		for(unsigned int i = 0; i < width; ++i){
			for(unsigned int j = 0; j < height; ++j){
				pixel = src.getPixel(i, j);
				pixel.x *= cf.x;
				pixel.y *= cf.y;
				pixel.z *= cf.z;
				src.setPixel(i, j, pixel);
			}
		}

		return &src;
	}

	Image* Blur::apply(Image& src)
	{

		const unsigned int height = src.getHeight();
		const unsigned int width = src.getWidth();
		Color pixel;
		Component *data = new Component[3*height*width];  //the new data buffer of the blurred image
		unsigned int d = 0; //helper counter for data array
		unsigned int add_d = (src.isInterleaved()) ? 1 : width*height; //the added value to d counter to place the pixel values
														 			  //to the right indexed position of the data buffer
		unsigned int dd = (src.isInterleaved()) ? 3 : 1;

		Component num = 0; //total number of valid pixels
		float red = 0;
		float green = 0;
		float blue = 0;

		for(unsigned int j = 0; j < height; j++){

			for(unsigned int i = 0; i < width; i++){
				
				for(short m = -1; m < 2; m++){

					for(short n = -1; n < 2; n++){

						pixel = src.getPixel(i + m, j + n);
						//this extra if is to reduce the number the isValidPixel method is called
						//cause it's only needed when the current pixel is on the very sides of the image
						if ((i == 0) || (j == 0) || (i == width - 1) || (j == height - 1))
							if (!isValidPixel(i + m, j + n, width, height)) continue;
							++num;
						
							red += (float)pixel.x;
							green += (float)pixel.y;
							blue += (float)pixel.z;
					}
				}

				data[d] = (Component)(red/num); data[d + add_d] = (Component)(green/num); data[d + add_d * 2] = (Component)(blue/num);

				d += dd;
				num = 0;
				red = 0;
				green = 0;
				blue = 0;

			}
		}

		Image* blurred = new Image(width, height, data, src.isInterleaved());

		delete[] data;

		return blurred;
	}

	bool NeighborhoodFilter::isValidPixel(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{	
		return (x >= 0) && (x < width) && (y >= 0) && (y < height);
	}

	Image* Median::apply(Image& src)
	{

		const unsigned int height = src.getHeight();
		const unsigned int width = src.getWidth();
		unsigned int d = 0; //helper counter for data array
		unsigned int add_d = (src.isInterleaved()) ? 1 : width*height; //the added value to d counter to place the pixel values
		//to the right indexed position of the data buffer
		Color pixel;
		Component * data = new Component[3 * height*width];  //the new data buffer of the blurred image
		vector<Component> neighborhoodR;
		vector<Component> neighborhoodG;
		vector<Component> neighborhoodB;

		//reserve space from the very beginning to make the push_back operation a little faster
		//and stop making the vector resize itself too frequently
		neighborhoodR.reserve(height*width);
		neighborhoodG.reserve(height*width);
		neighborhoodB.reserve(height*width);

		unsigned int dd = (src.isInterleaved()) ? 3 : 1;


		for (unsigned int j = 0; j < height; ++j){
			for (unsigned int i = 0; i < width; ++i){
				for (short m = -1; m < 2; ++m){
					for (short n = -1; n < 2; ++n){
						if (!isValidPixel(i + m, j + n, width, height)) continue;
						pixel = src.getPixel(i + m, j + n);
						neighborhoodR.push_back(pixel.x);
						neighborhoodG.push_back(pixel.y);
						neighborhoodB.push_back(pixel.z);
					}
				}
				std::sort(neighborhoodR.begin(), neighborhoodR.end());
				std::sort(neighborhoodG.begin(), neighborhoodG.end());
				std::sort(neighborhoodB.begin(), neighborhoodB.end());

				data[d] = neighborhoodR[neighborhoodR.size() / 2];
				data[d + add_d] = neighborhoodG[neighborhoodG.size() / 2];
				data[d + add_d * 2] = neighborhoodB[neighborhoodB.size() / 2];
				d += dd; 

				neighborhoodR.clear();
				neighborhoodG.clear();
				neighborhoodB.clear();
			}
		}

		Image* medianed = new Image(width, height, data, src.isInterleaved());

		delete[] data;

		return medianed;
	}


	Image* Diff::apply(Image& src)
	{

		const unsigned int height = src.getHeight();
		const unsigned int width = src.getWidth();
		unsigned int d = 0; //helper counter for data array
		unsigned int add_d = (src.isInterleaved()) ? 1 : width*height; //the added value to d counter to place the pixel values
																		 //to the right indexed position of the data buffer
		Color pixel;
		Component * data = new Component[3*height*width];  //the new data buffer of the blurred image
		vector<Component> neighborhoodR;
		vector<Component> neighborhoodG;
		vector<Component> neighborhoodB;

		//reserve space from the very beginning to make the push_back operation a little faster
		//and stop making the vector resize itself too frequently
		neighborhoodR.reserve(height*width);
		neighborhoodG.reserve(height*width);
		neighborhoodB.reserve(height*width);

		unsigned int dd = (src.isInterleaved()) ? 3 : 1;


		for(unsigned int j = 0; j < height; ++j){
			for(unsigned int i = 0; i < width; ++i){
				for(short m = -1; m < 2; ++m){
					for(short n = -1; n < 2; ++n){
						if(!isValidPixel(i + m, j + n, width, height)) continue;
						pixel = src.getPixel(i + m, j + n);
						neighborhoodR.push_back(pixel.x);
						neighborhoodG.push_back(pixel.y);
						neighborhoodB.push_back(pixel.z);
					}
				}
				std::sort(neighborhoodR.begin(),neighborhoodR.end());
				std::sort(neighborhoodG.begin(),neighborhoodG.end());
				std::sort(neighborhoodB.begin(),neighborhoodB.end());

				data[d] = neighborhoodR.back() - neighborhoodR.front();
				data[d + add_d] = neighborhoodG.back() - neighborhoodG.front();
				data[d + add_d*2] = neighborhoodB.back() - neighborhoodB.front();

				d += dd;

				neighborhoodR.clear();
				neighborhoodG.clear();
				neighborhoodB.clear();
			}
		}

		Image* diffed = new Image(width, height, data, src.isInterleaved());

		delete[] data;
		return diffed;
	}

}
