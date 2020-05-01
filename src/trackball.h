#pragma once

#include <common.h>

namespace cvglTemplate
{
	class Trackball
	{
	protected:
		bool _first;
		glm::vec2 _oldPos;
		unsigned int _buttonMask;

		glm::mat4 _m;

		virtual void update() = 0;
		virtual void motion( const glm::vec2 &pos ) {}
		virtual void scroll( const glm::vec2 &delta ) {}

		virtual bool keyDown( unsigned char key );
		virtual bool keyUp( unsigned char key );
		virtual bool specialDown( int key );
		virtual bool specialUp( int key );

	public:
		Trackball();
		~Trackball();

		virtual void reset() = 0;

		void onMouseButton( int button, bool down );
		void onScroll( float dx, float dy );

		void onMotion( float x, float y );

		bool onKeyDown( unsigned char key ) { return keyDown( key ); }
		bool onKeyUp( unsigned char key ) { return keyUp( key ); }
		bool onSpecialDown( int key ) { return specialDown( key ); }
		bool onSpecialUp( int key ) { return specialUp( key ); }

		const glm::mat4 &mat() const { return _m; }
	};

	class Trackball2D : public Trackball
	{
	private:
		bool _relativeMode;

		glm::vec3 _trans;
		float _motionSpeed;

		float _rotation;
		float _rotationSpeed;

		float _scale;
		float _scaleSpeed;
		float _wheelSpeed;
		float _scalePow;

		glm::vec3 _defaultTrans;
		float _defaultRotation;
		float _defaultScale;

		virtual void update();
		virtual void motion( const glm::vec2 &pos );
		virtual void scroll( const glm::vec2 &delta );

	public:
		Trackball2D();
		Trackball2D( const glm::vec3 &translate, float rotate, float scale );
		~Trackball2D();

		virtual void reset();
	};

	class Trackball3D : public Trackball
	{
	private:
		bool _relativeMode;

		glm::vec3 _trans;

		float _motionSpeed;
		float _wheelSpeed;

		glm::vec3 _euler;
		float _rotationSpeed;

		glm::vec3 _defaultTrans;
		glm::vec3 _defaultEuler;

		virtual void update();
		virtual void motion( const glm::vec2 &pos );
		virtual void scroll( const glm::vec2 &delta );

	public:
		Trackball3D();
		Trackball3D( const glm::vec3 &translate, const glm::vec3 &euler );
		~Trackball3D();

		virtual void reset();
	};
}