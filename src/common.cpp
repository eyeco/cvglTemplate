#include <common.h>

#include <vector>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <gl/GLU.h>

namespace cvglTemplate
{
	double globalTime = 0.0f;

	double getGlobalTime()
	{
		return globalTime;
	}

#ifdef _WIN32
	void disableQuickEditMode()
	{
		HANDLE hStdin = GetStdHandle( STD_INPUT_HANDLE );
		unsigned long mode = 0;
		GetConsoleMode( hStdin, &mode );

		mode &= ~ENABLE_QUICK_EDIT_MODE;

		SetConsoleMode( hStdin, mode );
	}
#endif

#ifdef _DEBUG
	bool checkForGLError()
	{
		GLenum errCode;
		if( ( errCode = glGetError() ) != GL_NO_ERROR )
		{
			const GLubyte *str = gluErrorString( errCode );
			if( str )
				std::cerr << "<error> OpenGL error: " << str << std::endl;
			else
				std::cerr << "<error> unknown OpenGL error #" << std::hex << errCode << std::dec << std::endl;
			return false;
		}
		return true;
	}
#else
	bool checkForGLError() { return true; }
#endif //_DEBUG

	namespace Internal
	{
		struct GLModel
		{
			size_t size;

			size_t vertsElemSize;
			size_t colsElemSize;

			std::vector<GLfloat> verts;
			std::vector<GLfloat> cols;
		};

		GLModel createAxes()
		{
			GLModel axes;

			axes.size = 6;
			axes.vertsElemSize = 3;
			axes.colsElemSize = 3;
			axes.verts.resize( axes.size * axes.vertsElemSize );
			axes.cols.resize( axes.size * axes.colsElemSize );

			glm::vec3 *verts = reinterpret_cast<glm::vec3*>( &axes.verts[0] );
			glm::vec3 *cols = reinterpret_cast<glm::vec3*>( &axes.cols[0] );

			verts[0] = glm::vec3( 0, 0, 0 );
			verts[1] = glm::vec3( 1, 0, 0 );
			verts[2] = glm::vec3( 0, 0, 0 );
			verts[3] = glm::vec3( 0, 1, 0 );
			verts[4] = glm::vec3( 0, 0, 0 );
			verts[5] = glm::vec3( 0, 0, 1 );

			cols[0] = red();
			cols[1] = red();
			cols[2] = green();
			cols[3] = green();
			cols[4] = blue();
			cols[5] = blue();

			return axes;
		}

		GLModel createGrid()
		{
			GLModel grid;

			grid.size = 44;
			grid.vertsElemSize = 3;
			grid.colsElemSize = 3;
			grid.verts.resize( grid.size * grid.vertsElemSize );
			grid.cols.resize( grid.size * grid.colsElemSize );

			glm::vec3 *verts = reinterpret_cast<glm::vec3 *>( &grid.verts[0] );
			glm::vec3 *cols = reinterpret_cast<glm::vec3 *>( &grid.cols[0] );

			for( int i = 0; i <= 10; i++ )
			{
				verts[i * 2 + 0] = glm::vec3( i - 5, 0, -5 );
				verts[i * 2 + 1] = glm::vec3( i - 5, 0, 5 );

				verts[( i + 11 ) * 2  + 0] = glm::vec3( -5, 0, i - 5 );
				verts[( i + 11 ) * 2  + 1] = glm::vec3( 5, 0, i - 5 );
			}

			for( int i = 0; i < grid.size; i++ )
				cols[i] = white();

			return grid;
		}

		const GLModel &getAxes()
		{
			static GLModel axes = createAxes();
			return axes;
		}

		const GLModel &getGrid()
		{
			static GLModel grid = createGrid();
			return grid;
		}
	}

	void drawAxes()
	{
		glPushAttrib( GL_ALL_ATTRIB_BITS );
		{
			glEnable( GL_COLOR_MATERIAL );
			glDisable( GL_TEXTURE_2D );

			const Internal::GLModel &axes = Internal::getAxes();

			glEnableClientState( GL_VERTEX_ARRAY );
			glEnableClientState( GL_COLOR_ARRAY );

			glVertexPointer( axes.vertsElemSize, GL_FLOAT, 0, &axes.verts[0] );
			glColorPointer( axes.colsElemSize, GL_FLOAT, 0, &axes.cols[0] );

			glDrawArrays( GL_LINES, 0, axes.size );

			// deactivate vertex arrays after drawing
			glDisableClientState( GL_VERTEX_ARRAY );
			glDisableClientState( GL_COLOR_ARRAY );
		}
		glPopAttrib();
	}

	void drawGrid()
	{
		glPushAttrib( GL_ALL_ATTRIB_BITS );
		{
			glEnable( GL_COLOR_MATERIAL );
			glDisable( GL_TEXTURE_2D );

			const Internal::GLModel &grid = Internal::getGrid();

			glEnableClientState( GL_VERTEX_ARRAY );
			glEnableClientState( GL_COLOR_ARRAY );

			glVertexPointer( grid.vertsElemSize, GL_FLOAT, 0, &grid.verts[0] );
			glColorPointer( grid.colsElemSize, GL_FLOAT, 0, &grid.cols[0] );

			glDrawArrays( GL_LINES, 0, grid.size );

			// deactivate vertex arrays after drawing
			glDisableClientState( GL_VERTEX_ARRAY );
			glDisableClientState( GL_COLOR_ARRAY );
		}
		glPopAttrib();
	}

	template<>
	std::string toString<bool>( const bool &t )
	{
		return std::string( t ? "true" : "false" );
	}
}
