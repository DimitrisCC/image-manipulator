
/*	The multiple types of filters were individually implemented conceptually in different classes creating chain inheritance relations 
	so that maintainability, scalabity, functionality, usability, testability, abstractability and extensibility are enhanced.
*/

#ifndef _FILTER
#define _FILTER

#include "Image.h"
#include "Color.h"

namespace imaging
{

	class Filter
	{
	public:
		Filter() {};
		virtual ~Filter() {}; 
		virtual Image* apply(Image& src) = 0;
	};

	class LocalFilter : public Filter
	{
	public:
		LocalFilter() {}
		~LocalFilter() {}
	};

	class NeighborhoodFilter : public Filter 
	{
	public:
		NeighborhoodFilter() {}
		~NeighborhoodFilter() {}

	protected:
		Component * data;

		inline bool isValidPixel(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
	};

	class Gray : public LocalFilter
	{
	public:
		Gray() {};
		~Gray() {};
		Image* apply(Image& src);
	};

	class ColorF : public LocalFilter
	{
		Color filter_c;
	public:
		ColorF(Color c) : filter_c(c) {};
		~ColorF() {};
		Image* apply(Image& src);
	};

	class Blur : public NeighborhoodFilter
	{
	public:
		Blur() {};
		~Blur() {};
		Image* apply(Image& src);
	};

	class Median : public NeighborhoodFilter
	{
	public:
		Median(){};
		~Median(){};
		Image* apply(Image& src);
	};

	class Diff : public NeighborhoodFilter
	{
	public:
		Diff(){};
		~Diff(){};
		Image* apply(Image& src);
	};

}

#endif
