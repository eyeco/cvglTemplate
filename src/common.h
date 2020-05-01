#pragma once

#include <sstream>

#include <glm/glm.hpp>


namespace cvglTemplate
{
#ifdef _WIN32
	void disableQuickEditMode();
#endif

	template<typename T>
	inline void safeDelete( T *&ptr )
	{
		if( ptr )
		{
			delete ptr;
			ptr = nullptr;
		}
	}

	template<typename T>
	inline void safeDeleteArray( T *&ptr )
	{
		if( ptr )
		{
			delete[] ptr;
			ptr = nullptr;
		}
	}

	inline float sign( float f )
	{
		return ( f < 0.0f ? -1.0f : 1.0f );
	}

	//custom min function since std::min produced slow code on vc++
	// http://randomascii.wordpress.com/2013/11/24/stdmin-causing-three-times-slowdown-on-vc/
	template<typename T>
	inline const T min( const T left, const T right )
	{
		return ( right < left ? right : left );
	}

	//custom max function since std::max produced slow code on vc++
	// http://randomascii.wordpress.com/2013/11/24/stdmin-causing-three-times-slowdown-on-vc/
	template<typename T>
	inline const T max( const T left, const T right )
	{
		return ( left < right ? right : left );
	}

	template<typename T>
	inline T clamp( T value, T minValue, T maxValue )
	{
		return TextileUX::min( TextileUX::max( value, minValue ), maxValue );
	}

	template<typename T>
	inline T clamp01( T value )
	{
		return TextileUX::min( TextileUX::max( value, (T) 0 ), (T) 1 );
	}

	inline const glm::vec3 &zero()
	{
		static const glm::vec3 z( 0 );
		return z;
	}

	inline const glm::vec3 &one()
	{
		static const glm::vec3 o( 1 );
		return o;
	}

	inline const glm::vec3 &unitX()
	{
		static const glm::vec3 x( 1, 0, 0 );
		return x;
	}

	inline const glm::vec3 &unitY()
	{
		static const glm::vec3 y( 0, 1, 0 );
		return y;
	}

	inline const glm::vec3 &unitZ()
	{
		static const glm::vec3 z( 0, 0, 1 );
		return z;
	}

	inline const glm::vec3 &black()
	{
		static const glm::vec3 b( 0 );
		return b;
	}

	inline const glm::vec3 &white()
	{
		static const glm::vec3 w( 1 );
		return w;
	}

	inline const glm::vec3 &red()
	{
		static const glm::vec3 r( 1, 0, 0 );
		return r;
	}

	inline const glm::vec3 &green()
	{
		static const glm::vec3 g( 0, 1, 0 );
		return g;
	}

	inline const glm::vec3 &blue()
	{
		static const glm::vec3 b( 0, 0, 1 );
		return b;
	}

	template<typename T> size_t arraySize( const T &t )
	{
		return sizeof( t ) / sizeof( *t );
	}

	inline unsigned int nextPo2( unsigned int x )
	{
		unsigned int power = 1;
		while( power < x && power )
			power <<= 1;

		if( !power )
			throw std::runtime_error( "nextpo2 overflow" );

		return power;
	}

	inline long double pi()
	{
		return 3.141592653589793238462643383279502884L;
	}

	inline long double pi2()
	{
		return 2 * 3.141592653589793238462643383279502884L;
	}

	inline double toRad( double deg )
	{
		static double s = pi2() / 360.0;
		return deg * s;
	}

	inline double toDeg( double rad )
	{
		static double s = 360.0 / pi2();
		return rad * s;
	}


	bool checkForGLError();

	void drawAxes();
	void drawGrid();

	double getGlobalTime();

	template<typename T>
	std::string toString( const T &t )
	{
		std::stringstream sstr;
		sstr << t;

		return sstr.str();
	}

	template<> std::string toString<bool>( const bool &t );
}