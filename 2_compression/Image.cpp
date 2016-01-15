
#include <algorithm>
#include "Image.h"
#include <iostream>
#include <vector>

using namespace std;

namespace imaging
{

//-------------------------------- Image class implementation goes here ---------------------------------------------------

	Component * Image::getRawDataPtr() const
	{
		return buffer;
	}

	Color Image::getPixel(unsigned int x, unsigned int y) const
	{
		Color returnColor;

		if ((x < 0) || (x > width - 1) || (y < 0) || (y > height - 1)){

			returnColor.x = 0;
			returnColor.y = 0;
			returnColor.z = 0;
			return returnColor;

		}


		if(interleaved){

			returnColor.x = buffer[3*width*y + 3*x + RED];
			returnColor.y = buffer[3*width*y + 3*x + GREEN];
			returnColor.z = buffer[3*width*y + 3*x + BLUE];
			return returnColor;

		}else{

			returnColor.x = buffer[RED*width*height + width*y + x];
			returnColor.y = buffer[GREEN*width*height + width*y + x];
			returnColor.z = buffer[BLUE*width*height + width*y + x];
			return returnColor;

		}

	}

	void Image::setPixel(unsigned int x, unsigned int y, Color value)
	{

		if((x < 0)||(x > width-1)||(y < 0)||(y > height-1))
			return;

		if(interleaved){
			buffer[3*width*y + 3*x + RED] = value.x;
			buffer[3*width*y + 3*x + GREEN] = value.y;
			buffer[3*width*y + 3*x + BLUE] = value.z;
		}else{
			buffer[RED*width*height + width*y + x] = value.x;
			buffer[GREEN*width*height + width*y + x] = value.y;
			buffer[BLUE*width*height + width*y +x] = value.z;
		}

	}

	void Image::setData(const Component * & data_ptr)
	{

		for (unsigned int i = 0; i < 3 * width*height; i++){
			buffer[i] = data_ptr[i];
		}

	}

	void Image::convertToInterleavedImage()
	{

		if (!interleaved){

			Component * temp = new Component[width*height * 3];

			for (unsigned int i = 0; i < width*height; i++){
				temp[i*3 + RED] = buffer[i + RED*width*height];
				temp[i*3 + GREEN] = buffer[i + GREEN*width*height];
				temp[i*3 + BLUE] = buffer[i + BLUE*width*height];
			}

			delete[] buffer;
			buffer = temp;

			interleaved = true;
		}

	}

	void Image::convertToColorPlaneImage()
	{

		if (interleaved){

			Component * temp = new Component[width*height * 3];

			for (unsigned int i = 0; i < width*height; i++){
				temp[i + RED*width*height] = buffer[i * 3 + RED];
				temp[i + GREEN*width*height] = buffer[i * 3 + GREEN];
				temp[i + BLUE* width*height] = buffer[i * 3 + BLUE];
			}

			delete[] buffer;

			buffer = temp;

			interleaved = false;
		}

	}

	Image::Image(unsigned int width, unsigned int height, bool interleaved)
			: buffer(new Component[width*height*3]), interleaved(interleaved), width(width), height(height)  { }

	Image::Image(unsigned int width, unsigned int height, const Component * data_ptr, bool interleaved)
			: buffer(new Component[width*height*3]), interleaved(interleaved), width(width), height(height)
	{
		setData(data_ptr);
	}

	Image::Image(const Image &src)
			: buffer(new Component[src.getWidth()*src.getHeight()*3]), interleaved(src.isInterleaved()), width(src.getWidth()), height(src.getHeight())
	{
		Component const * data = getRawDataPtr();
		setData(data);
	}

	Image::~Image()
	{
		delete[] buffer;
	}

//-------------------------------- Helper functions -----------------------------------------------------------------------

	bool isSuffix(std::string & filename, const std::string suffix)
	{
		std::string low_caps_filename = filename;
		std::string dot_suffix = '.' + suffix;

		// make the string lowercase.
		std::transform(low_caps_filename.begin(), low_caps_filename.end(), low_caps_filename.begin(), ::tolower );

		//make the suffix lowercase.
		std::transform(dot_suffix.begin(), dot_suffix.end(), dot_suffix.begin(), ::tolower );

		size_t pos = low_caps_filename.rfind(dot_suffix);

		if (pos == std::string::npos) // did not find it
			return false;

		if (pos < low_caps_filename.length()-dot_suffix.length()) // found it, but not at the end
			return false;

		return true; // now we are positive. :)
	}

	void replaceSuffix(std::string & filename, const std::string suffix)
	{
		size_t pos = filename.rfind('.');
		if (pos==std::string::npos)
			return;

		filename.resize(pos+1);
		filename+=suffix;
	}

} // namespace imaging
