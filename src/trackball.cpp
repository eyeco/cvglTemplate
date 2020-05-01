#include <trackball.h>

#include <glm/gtx/transform.hpp>

namespace cvglTemplate
{
	Trackball::Trackball() :
		_first( true ),
		_oldPos( 0 ),
		_buttonMask( 0 ),
		_m( 1 )
	{}

	Trackball::~Trackball()
	{}

	void Trackball::onMouseButton( int button, bool down )
	{
		if( down )
			_buttonMask |= ( 0x01 << button );
		else
			_buttonMask &= ~( 0x01 << button );
	}

	void Trackball::onMotion( float x, float y )
	{
		motion( glm::vec2( x, y ) );
	}

	void Trackball::onScroll( float dx, float dy )
	{
		scroll( glm::vec2( dx, dy ) );
	}

	bool Trackball::keyDown( unsigned char key )
	{
		switch( key )
		{
		case 'r':
			reset();
			return true;
		}

		return false;
	}

	bool Trackball::keyUp( unsigned char key )
	{
		return false;
	}

	bool Trackball::specialDown( int key )
	{
		return false;
	}

	bool Trackball::specialUp( int key )
	{
		return false;
	}




	Trackball2D::Trackball2D() :
		Trackball(),
		_relativeMode( false ),
		_trans( 0 ),
		_motionSpeed( 0.001f ),
		_rotation( 0 ),
		_rotationSpeed( 0.01f ),
		_scale( 0 ),
		_scaleSpeed( 0.01f ),
		_wheelSpeed( 0.1f ),
		_scalePow( 2.0f )
	{}

	Trackball2D::Trackball2D( const glm::vec3 &translate, float rotate, float scale ) :
		Trackball(),
		_relativeMode( false ),
		_trans( translate ),
		_motionSpeed( 0.001f ),
		_rotation( rotate ),
		_rotationSpeed( 0.01f ),
		_scale( scale ),
		_scaleSpeed( 0.01f ),
		_wheelSpeed( 0.1f ),
		_scalePow( 2.0f ),
		_defaultTrans( translate ),
		_defaultRotation( rotate ),
		_defaultScale( scale )
	{
		reset();
	}

	Trackball2D::~Trackball2D()
	{}

	void Trackball2D::reset()
	{
		_m = glm::translate( _defaultTrans )
			* glm::rotate( _defaultRotation, unitZ() )
			* glm::scale( glm::vec3( max( 0.000001f, pow( _scalePow, _defaultScale ) ) ) );
	}

	void Trackball2D::motion( const glm::vec2 &pos )
	{
		glm::vec2 d( _first ? glm::vec2( 0 ) : pos - _oldPos );

		if( _buttonMask & ( 0x01 << 0 ) ) //left
		{
			if( _relativeMode )
				_m = glm::rotate( -d.x * _rotationSpeed, unitZ() ) * _m;
			else
				_rotation += -d.x * _rotationSpeed;
		}
		if( _buttonMask & ( 0x01 << 1 ) ) //middle
		{
			if( _relativeMode )
				_m = glm::scale( glm::vec3( max( 0.000001f, pow( _scalePow, -d.y * _scaleSpeed ) ) ) ) * _m;
			else
				_scale += -d.y * _scaleSpeed;
		}
		if( _buttonMask & ( 0x01 << 2 ) ) //right
		{
			if( _relativeMode )
				_m = glm::translate( glm::vec3( d.x, -d.y, 0 ) * _motionSpeed ) * _m;
			else
				_trans += glm::vec3( d.x, -d.y, 0 ) * _motionSpeed;
		}

		if( _buttonMask )
			update();

		_oldPos = pos;
		_first = false;
	}

	void Trackball2D::scroll( const glm::vec2 &delta )
	{
		if( _relativeMode )
			_m = glm::scale( glm::vec3( max( 0.000001f, pow( _scalePow, _wheelSpeed * sign( delta.y ) * ( abs( delta.y ) > std::numeric_limits<float>::epsilon() ? 1.0f : 0.0f ) ) ) ) ) * _m;
		else
			_scale += delta.y * _wheelSpeed;

		update();
		
		motion( _oldPos );
	}

	void Trackball2D::update()
	{
		if( !_relativeMode )
		{
			_m = glm::translate( _trans )
				* glm::rotate( _rotation, unitZ() )
				* glm::scale( glm::vec3( max( 0.000001f, pow( _scalePow, _scale ) ) ) )
				;
		}
	}




	Trackball3D::Trackball3D() :
		Trackball(),
		_relativeMode( false ),
		_trans( 0 ),
		_motionSpeed( 0.01f ),
		_wheelSpeed( 0.1f ),
		_euler( 0 ),
		_rotationSpeed( 0.01f )
	{}

	Trackball3D::Trackball3D( const glm::vec3 &translate, const glm::vec3 &euler ) :
		Trackball(),
		_relativeMode( false ),
		_trans( translate ),
		_motionSpeed( 0.01f ),
		_wheelSpeed( 0.1f ),
		_euler( euler ),
		_rotationSpeed( 0.01f ),
		_defaultTrans( translate ),
		_defaultEuler( euler )
	{
		reset();
	}

	Trackball3D::~Trackball3D()
	{}

	void Trackball3D::reset()
	{
		_m = glm::translate( _defaultTrans )
			* glm::rotate( _defaultEuler.x, unitX() )
			* glm::rotate( _defaultEuler.y, unitY() )
			* glm::rotate( _defaultEuler.z, unitZ() );
	}

	void Trackball3D::motion( const glm::vec2 &pos )
	{
		glm::vec2 d( _first ? glm::vec2( 0 ) : pos - _oldPos );

		if( _buttonMask & ( 0x01 << 0 ) ) //left
		{
			if( _relativeMode )
				_m = glm::rotate( d.x * _rotationSpeed, unitY() ) * glm::rotate( d.y * _rotationSpeed, unitX() ) * _m;
			else
				_euler += glm::vec3( d.y, d.x, 0 ) * _rotationSpeed;
		}
		if( _buttonMask & ( 0x01 << 1 ) ) //middle
		{
			if( _relativeMode )
				_m = glm::translate( glm::vec3( 0, 0, -d.y ) * _motionSpeed ) * _m;
			else
				_trans += glm::vec3( 0, 0, -d.y ) * _motionSpeed;
		}
		if( _buttonMask & ( 0x01 << 2 ) ) //right
		{
			if( _relativeMode )
				_m = glm::translate( glm::vec3( d.x, -d.y, 0 ) * _motionSpeed ) * _m;
			else
				_trans += glm::vec3( d.x, -d.y, 0 ) * _motionSpeed;
		}

		if( _buttonMask )
			update();

		_oldPos = pos;
		_first = false;
	}

	void Trackball3D::scroll( const glm::vec2 &delta )
	{
		if( _relativeMode )
			_m = glm::translate( glm::vec3( sign( delta.x ) * ( abs( delta.x ) > std::numeric_limits<float>::epsilon() ? 1.0f : 0.0f ), 0, sign( delta.y ) * ( abs( delta.y ) > std::numeric_limits<float>::epsilon() ? 1.0f : 0.0f ) ) * _wheelSpeed ) * _m;
		else
			_trans += glm::vec3( delta.x, 0, delta.y ) * _wheelSpeed;
		update();

		motion( _oldPos );
	}

	void Trackball3D::update()
	{
		if( !_relativeMode )
		{
			_m = glm::translate( _trans )
				* glm::rotate( _euler.x, unitX() )
				* glm::rotate( _euler.y, unitY() )
				* glm::rotate( _euler.z, unitZ() )
				;
		}
	}
}
