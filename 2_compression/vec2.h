
#pragma once

#include <math.h>
#include <stdlib.h>

#define COORD_EPSILON 10e-6

namespace vecmath 
{
	
	// We declare a new template class for supporting 2D vecrors with
	// int, float or double coordinates.
	//
	// Using a templated code base for the 3 types of vectors, saves a lot
	// of common code repetition. Any methods that hace a different behavior 
	// in each vector type, are "specialized".
	//
	// The class uses friend (non-member) operators, which are specialized 
	// according to these template types
	template <typename T> class vec2_t 
	{
	public:
		T x, y; // data member of vec2 are simple enough to be exposed as public:
		        // no dependent data on them
	
		// we could declare all binary operators as non-member functions
		// but member-functions save a lot of code, since otherwise
		// specialized function should be provided for each template type
		const vec2_t<T> operator+(const vec2_t<T> & right) const;
		const vec2_t<T> operator-(const vec2_t<T> & right) const;
		const vec2_t<T> operator*(const vec2_t<T> & right) const;
		const vec2_t<T> operator/(const vec2_t<T> & right) const;
		const vec2_t<T> operator/(const T & right) const;
		const vec2_t<T> operator*(const T & right) const;
		
		vec2_t<T> & operator=(const vec2_t<T> & right);
		//provide both a const and a non-const version
		T & operator[](unsigned char);
		T operator[](unsigned char) const;
		//bool operator==(const vec2_t<T> & right) const;
		//bool operator!=(const vec2_t<T> & right) const;
	
		double length () const;
		T dot ( vec2_t<T> v ) const;
		static T dot (vec2_t<T> v1, vec2_t<T> v2);
		void normalize ();
		static vec2_t<T> normalize (vec2_t<T> src);
	
		friend const vec2_t<int> operator*(const int & left, const vec2_t<int> & right);
		friend const vec2_t<float> operator*(const float & left, const vec2_t<float> & right);
		friend const vec2_t<double> operator*(const double & left, const vec2_t<double> & right);
		

		// constructors:
		vec2_t(T x, T y);                  // from coordinates
		vec2_t();                          // default
		vec2_t(T * xy);                    // from T array
		vec2_t(const vec2_t<T> & src);     // generic copy constructor 
		vec2_t(const T & val);             // argument-replicating constructor 

		
	};

	// some component-wise logical operations:
	template<typename T> vec2_t<bool> operator>(const vec2_t<T> & left, const vec2_t<T> & right);
	template<typename T> vec2_t<bool> operator<(const vec2_t<T> & left, const vec2_t<T> & right);
	template<typename T> vec2_t<bool> operator>=(const vec2_t<T> & left, const vec2_t<T> & right);
	template<typename T> vec2_t<bool> operator<=(const vec2_t<T> & left, const vec2_t<T> & right);
	template<typename T> vec2_t<bool> operator==(const vec2_t<T> & left, const vec2_t<T> & right);
	template<typename T> vec2_t<bool> operator!=(const vec2_t<T> & left, const vec2_t<T> & right);
	// and some logical aggregators:
	bool any(vec2_t<bool>);
	bool all(vec2_t<bool>);


	//----------------------------------------------------------
	// type and function specialization
	//----------------------------------------------------------

	// Create specialized, non-templated class aliases for convenience:
	typedef vec2_t<float> vec2;
	typedef vec2_t<int> ivec2;
	typedef vec2_t<double> dvec2;

	typedef vec2_t<bool> bvec2; // warning: not all operations make sense with booleans
	                            // exercise caution. Since this is a templated class, 
	                            // the compiler won't even try to compile a member function 
	                            // or operator, unless invoked by the user, so avoid using any
								// unreasonable operator like + or *
	                             

	// implementation of the specialized friend non-member operators:
	
	template <typename T> vec2_t<T>::vec2_t(const vec2_t<T> & src) 
		: x(src.x), y(src.y) 
	{
	}

	template <typename T> vec2_t<T>::vec2_t(const T & val) 
		: x(val), y(val) 
	{
	}

	template<typename T> vec2_t<bool> operator>(const vec2_t<T> & left, const vec2_t<T> & right)
	{
		vec2_t<bool> res;
		res.x = left.x>right.x;
		res.y = left.y>right.y;
		return res;
	}

	template<typename T> vec2_t<bool> operator<(const vec2_t<T> & left, const vec2_t<T> & right)
	{
		vec2_t<bool> res;
		res.x = left.x<right.x;
		res.y = left.y<right.y;
		return res;
	}

	template<typename T> vec2_t<bool> operator>=(const vec2_t<T> & left, const vec2_t<T> & right)
	{
		vec2_t<bool> res;
		res.x = left.x>=right.x;
		res.y = left.y>=right.y;
		return res;
	}

	template<typename T> vec2_t<bool> operator<=(const vec2_t<T> & left, const vec2_t<T> & right)
	{
		vec2_t<bool> res;
		res.x = left.x<=right.x;
		res.y = left.y<=right.y;
		return res;
	}

	template<typename T> vec2_t<bool> operator==(const vec2_t<T> & left, const vec2_t<T> & right)
	{
		vec2_t<bool> res;
		res.x = left.x==right.x;
		res.y = left.y==right.y;
		return res;
	}

	template<typename T> vec2_t<bool> operator!=(const vec2_t<T> & left, const vec2_t<T> & right)
	{
		vec2_t<bool> res;
		res = left==right;
		return vec2_t<bool>(!res.x,!res.y);
	}

	inline bool any(vec2_t<bool> vec)
	{
		return vec.x || vec.y;
	}

	inline bool all(vec2_t<bool> vec)
	{
		return vec.x && vec.y;
	}

	// specialized template implementation for certain types and functions:
	inline vec2_t<bool> operator==(const vec2_t<float> & left, const vec2_t<float> & right)
	{
		vec2_t<bool> res;
		res.x = fabs(left.x-right.x) < COORD_EPSILON;
		res.y = fabs(left.y-right.y) < COORD_EPSILON;
		return res;
	}

	inline vec2_t<bool> operator==(const vec2_t<double> & left, const vec2_t<double> & right)
	{
		vec2_t<bool> res;
		res.x = abs(left.x-right.x) < COORD_EPSILON;
		res.y = abs(left.y-right.y) < COORD_EPSILON;
		return res;
	}

	//----------------------------------------------------------
	// templated operator definitions 
	//----------------------------------------------------------

	template <typename T> const vec2_t<T> vec2_t<T>::operator+(const vec2_t<T> & right) const 
	{
		return vec2_t<T>(x+right.x,y+right.y);
	}

	template <typename T> const vec2_t<T> vec2_t<T>::operator-(const vec2_t<T> & right) const 
	{
		return vec2_t<T>(x-right.x,y-right.y);
	}

	template <typename T> const vec2_t<T> vec2_t<T>::operator*(const vec2_t<T> & right) const 
	{
		return vec2_t<T>(x*right.x,y*right.y);
	}

	template <typename T> const vec2_t<T> vec2_t<T>::operator*(const T & right) const 
	{
		return vec2_t<T>(x*right,y*right);
	}

	template <typename T> const vec2_t<T> vec2_t<T>::operator/(const vec2_t<T> & right) const 
	{
		return vec2_t<T>(x/right.x,y/right.y);
	}


	template <typename T> const vec2_t<T> vec2_t<T>::operator/(const T & right) const 
	{
		return vec2_t<T>(x/right,y/right);
	}

	template <typename T> vec2_t<T> & vec2_t<T>::operator=(const vec2_t<T> & right) 
	{
		x = right.x;
		y = right.y;
		return *this;
	}

	template <typename T> T & vec2_t<T>::operator[](unsigned char index) 
	{
		return *(&x+index);
	}

	template <typename T> T vec2_t<T>::operator[](unsigned char index) const 
	{
		return (&x)[index];
	}

	template <typename T> vec2_t<T>::vec2_t(T a, T b)
		: x(a), y(b) 
	{
	}

	template <typename T> vec2_t<T>::vec2_t(T * v) 
		: x(0), y(0) 
	{
		if (v!=NULL)
		{
			x=v->x; 
			y=v->y;
		}
	}

	template <typename T> vec2_t<T>::vec2_t() 
		: x(0), y(0) 
	{
	}
	
	template <typename T> T vec2_t<T>::dot ( vec2_t<T> v ) const 
	{
		return x*v.x + y*v.y; 
	}
	
	template <typename T> double vec2_t<T>::length () const 
	{
		T res2 = dot(*this);
		return res2>0? sqrt(double(res2)) : 0.0;
	}	

	template <typename T> T vec2_t<T>::dot (vec2_t<T> v1, vec2_t<T> v2)
	{
		return v1.x*v2.x + v1.y*v2.y;
	}
	
	template <typename T> void vec2_t<T>::normalize () 
	{
		double len = length();
		if (len==0.0)
			return;
		x= T(x/len);
		y= T(y/len);
	}
	
	template <typename T> vec2_t<T> vec2_t<T>::normalize (vec2_t<T> src)
	{
		vec2_t res(src);
		res.normalize();
		return res;
	}
	

} // namespace vecmath
