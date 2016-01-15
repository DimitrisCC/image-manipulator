#pragma once

#include <math.h>
#include <stdlib.h>

#define COORD_EPSILON 10e-6

namespace vecmath 
{
	
	
	// We declare a new template class for supporting 3D vectors with
	// int, float or double coordinates.
	//
	// Using a templated code base for the 3 types of vectors, saves a lot
	// of common code repetition. Any methods that hace a different behavior 
	// in each vector type, are "specialized".
	//
	// The class uses friend (non-member) operators, which are specialized 
	// according to these template types
	template <typename T> class vec3_t 
	{
	public:
		T x, y, z; // data member of vec3 are simple enough to be exposed as public:
		           // no dependent data on them
	
		// we could declare all binary operators as non-member functions
		// but member-functions save a lot of code, since otherwise
		// specialized function should be provided for each template type
		const vec3_t<T> operator+(const vec3_t<T> & right) const;
		const vec3_t<T> operator-(const vec3_t<T> & right) const;
		const vec3_t<T> operator*(const vec3_t<T> & right) const;
		const vec3_t<T> operator/(const vec3_t<T> & right) const;
		const vec3_t<T> operator/(const T & right) const;
		const vec3_t<T> operator*(const T & right) const;
		
		vec3_t<T> & operator=(const vec3_t<T> & right);
		//provide both a const and a non-const version
		T & operator[](unsigned char);
		T operator[](unsigned char) const;
			
		double length () const;
		T dot ( vec3_t<T> v ) const;
		void normalize ();
		vec3_t<T> cross(vec3_t<T> right) const;
		static T dot (vec3_t<T> v1, vec3_t<T> v2);
		static vec3_t<T> normalize (vec3_t<T> src);
		static vec3_t<T> cross(vec3_t<T> v1, vec3_t<T> v2);
	
		friend const vec3_t<int> operator*(const int & left, const vec3_t<int> & right);
		friend const vec3_t<float> operator*(const float & left, const vec3_t<float> & right);
		friend const vec3_t<double> operator*(const double & left, const vec3_t<double> & right);
		

		// constructors:
		vec3_t(T x, T y, T z);             // from coordinates
		vec3_t();                          // default
		vec3_t(T * xyz);                   // from T array
		vec3_t(const vec3_t<T> & src);     // generic copy constructor 
		vec3_t(const T & val);             // argument-replicating constructor 

		
	};

	// some component-wise logical operations:
	template<typename T> vec3_t<bool> operator>(const vec3_t<T> & left, const vec3_t<T> & right);
	template<typename T> vec3_t<bool> operator<(const vec3_t<T> & left, const vec3_t<T> & right);
	template<typename T> vec3_t<bool> operator>=(const vec3_t<T> & left, const vec3_t<T> & right);
	template<typename T> vec3_t<bool> operator<=(const vec3_t<T> & left, const vec3_t<T> & right);
	template<typename T> vec3_t<bool> operator==(const vec3_t<T> & left, const vec3_t<T> & right);
	template<typename T> vec3_t<bool> operator!=(const vec3_t<T> & left, const vec3_t<T> & right);
	// and some logical aggregators:
	bool any(vec3_t<bool>);
	bool all(vec3_t<bool>);

	//----------------------------------------------------------
	// type and function specialization
	//----------------------------------------------------------

	// Create specialized, non-templated class aliases for convenience:
	typedef vec3_t<float> vec3;
	typedef vec3_t<int> ivec3;
	typedef vec3_t<double> dvec3;

	typedef vec3_t<bool> bvec3; // warning: not all operations make sense with booleans
	                            // exercise caution. Since this is a templated class, 
	                            // the compiler won't even try to compile a member function 
	                            // or operator, unless invoked by the user, so avoid using any
								// unreasonable operator like + or *

	// implementation of the specialized friend non-member operators:

	inline const vec3_t<int> operator*(const int & left, const vec3_t<int> & right) 
	{
		return vec3_t<int>(right.x*left,right.y*left,right.z*left);
	}

	inline const vec3_t<float> operator*(const float & left, const vec3_t<float> & right) 
	{
		return vec3_t<float>(right.x*left,right.y*left,right.z*left);
	}

	inline const vec3_t<double> operator*(const double & left, const vec3_t<double> & right) 
	{
		return vec3_t<double>(right.x*left,right.y*left,right.z*left);
	}

	//----------------------------------------------------------
	// templated operator definitions 
	//----------------------------------------------------------

	template <typename T> const vec3_t<T> vec3_t<T>::operator+(const vec3_t<T> & right) const 
	{
		return vec3_t<T>(x+right.x,y+right.y,z+right.z);
	}

	template <typename T> const vec3_t<T> vec3_t<T>::operator-(const vec3_t<T> & right) const 
	{
		return vec3_t<T>(x-right.x,y-right.y,z-right.z);
	}

	template <typename T> const vec3_t<T> vec3_t<T>::operator*(const vec3_t<T> & right) const 
	{
		return vec3_t<T>(x*right.x,y*right.y,z*right.z);
	}

	template <typename T> const vec3_t<T> vec3_t<T>::operator*(const T & right) const 
	{
		return vec3_t<T>(x*right,y*right,z*right.z);
	}

	template <typename T> const vec3_t<T> vec3_t<T>::operator/(const vec3_t<T> & right) const 
	{
		return vec3_t<T>(x/right.x,y/right.y,z/right.z);
	}


	template <typename T> const vec3_t<T> vec3_t<T>::operator/(const T & right) const 
	{
		return vec3_t<T>(x/right,y/right,z/right);
	}

	template <typename T> vec3_t<T> & vec3_t<T>::operator=(const vec3_t<T> & right) 
	{
		x = right.x;
		y = right.y;
		z = right.z;
		return *this;
	}

	template <typename T> T & vec3_t<T>::operator[](unsigned char index) 
	{
		return *(&x+index);
	}

	template <typename T> T vec3_t<T>::operator[](unsigned char index) const 
	{
		return (&x)[index];
	}

	template <typename T> vec3_t<T>::vec3_t(T a, T b, T c)
		: x(a), y(b), z(c) 
	{
	}

	template <typename T> vec3_t<T>::vec3_t(T * v) 
		: x(0), y(0), z(0) 
	{
		if (v!=NULL)
		{
			x=v->x; 
			y=v->y;
			z=v->z;
		}
	}

	template <typename T> vec3_t<T>::vec3_t() 
		: x(0), y(0), z(0) 
	{
	}
	
	template <typename T> vec3_t<T>::vec3_t(const vec3_t<T> & src) 
		: x(src.x), y(src.y), z(src.z)  
	{
	}

	template <typename T> vec3_t<T>::vec3_t(const T & val) 
		: x(val), y(val), z(val) 
	{
	}

	template <typename T> T vec3_t<T>::dot ( vec3_t<T> v ) const 
	{
		return x*v.x + y*v.y + z*v.z; 
	}
	
	template <typename T> double vec3_t<T>::length () const 
	{
		T res2 = dot(*this);
		return res2>0? sqrt(double(res2)) : 0.0;
	}	

	template <typename T> T vec3_t<T>::dot (vec3_t<T> v1, vec3_t<T> v2)
	{
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	}
	
	template <typename T> void vec3_t<T>::normalize () 
	{
		double len = length();
		if (len==0.0)
			return;
		x= T(x/len);
		y= T(y/len);
		z= T(z/len);
	}
	
	template <typename T> vec3_t<T> vec3_t<T>::normalize (vec3_t<T> src)
	{
		vec3_t res(src);
		res.normalize();
		return res;
	}

	template <typename T> vec3_t<T> vec3_t<T>::cross(vec3_t<T> right) const
	{
		vec3_t res;
		res.x = y*right.z - z*right.y;
		res.y = z*right.x - x*right.z;
		res.z = x*right.y - y*right.x;
		return res;
	}
	
	template <typename T> vec3_t<T> vec3_t<T>::cross(vec3_t<T> v1, vec3_t<T> v2)
	{
		vec3_t res;
		res.x = v1.y*v2.z - v1.z*v2.y;
		res.y = v1.z*v2.x - v1.x*v2.z;
		res.z = v1.x*v2.y - v1.y*v2.x;
		return res;
	}
	

	template<typename T> vec3_t<bool> operator>(const vec3_t<T> & left, const vec3_t<T> & right)
	{
		vec3_t<bool> res;
		res.x = left.x>right.x;
		res.y = left.y>right.y;
		res.z = left.z>right.z;
		return res;
	}

	template<typename T> vec3_t<bool> operator<(const vec3_t<T> & left, const vec3_t<T> & right)
	{
		vec3_t<bool> res;
		res.x = left.x<right.x;
		res.y = left.y<right.y;
		res.z = left.z<right.z;
		return res;
	}

	template<typename T> vec3_t<bool> operator>=(const vec3_t<T> & left, const vec3_t<T> & right)
	{
		vec3_t<bool> res;
		res.x = left.x>=right.x;
		res.y = left.y>=right.y;
		res.z = left.z>=right.z;
		return res;
	}

	template<typename T> vec3_t<bool> operator<=(const vec3_t<T> & left, const vec3_t<T> & right)
	{
		vec3_t<bool> res;
		res.x = left.x<=right.x;
		res.y = left.y<=right.y;
		res.z = left.z<=right.z;
		return res;
	}

	template<typename T> vec3_t<bool> operator==(const vec3_t<T> & left, const vec3_t<T> & right)
	{
		vec3_t<bool> res;
		res.x = left.x==right.x;
		res.y = left.y==right.y;
		res.z = left.z==right.z;
		return res;
	}

	template<typename T> vec3_t<bool> operator!=(const vec3_t<T> & left, const vec3_t<T> & right)
	{
		vec3_t<bool> res;
		res = left==right;
		return vec3_t<bool>(!res.x,!res.y,!res.z);
	}

	inline bool any(vec3_t<bool> vec)
	{
		return vec.x || vec.y || vec.z;
	}

	inline bool all(vec3_t<bool> vec)
	{
		return vec.x && vec.y && vec.z;
	}

	// specialized template implementation for certain types and functions:
	inline vec3_t<bool> operator==(const vec3_t<float> & left, const vec3_t<float> & right)
	{
		vec3_t<bool> res;
		res.x = fabs(left.x-right.x) < COORD_EPSILON;
		res.y = fabs(left.y-right.y) < COORD_EPSILON;
		res.z = fabs(left.z-right.z) < COORD_EPSILON;
		return res;
	}

	inline vec3_t<bool> operator==(const vec3_t<double> & left, const vec3_t<double> & right)
	{
		vec3_t<bool> res;
		res.x = abs(left.x-right.x) < COORD_EPSILON;
		res.y = abs(left.y-right.y) < COORD_EPSILON;
		res.z = abs(left.z-right.z) < COORD_EPSILON;
		return res;
	}


} // namespace vecmath
