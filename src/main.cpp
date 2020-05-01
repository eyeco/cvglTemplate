
#include <time.h>

#include <chrono>
#include <iostream>

#include <common.h>
#include <trackball.h>

#include <cxxopts.hpp>

#ifdef _WIN32
#include <gl/glew.h>
#include <Windows.h>
//#include <gl/wglew.h>
#endif

#include <GLFW/glfw3.h>

//#include <imgui/imgui.h>
//#include <imgui/imgui_impl_glfw.h>
//#include <imgui/imgui_impl_opengl3.h>
//
//#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
//#include <GL/gl3w.h>    // Initialize with gl3wInit()
//#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
//#include <GL/glew.h>    // Initialize with glewInit()
//#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
//#include <glad/glad.h>  // Initialize with gladLoadGL()
//#else
//#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
//#endif

#include <opencv2/opencv.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace cvglTemplate
{
	extern double globalTime;

	struct StartupConfig
	{
	public:
		StartupConfig()
		{}

		float sensorDefaultTimeOut;

		static bool readPA( int argc, char **argv, StartupConfig &cfg )
		{
			//generic options: 
			cxxopts::Options options( argv[0], " command line options" );
			options
				.positional_help( "[optional args]" )
				.show_positional_help();

			options.add_options()
				//TODO: incorporate version, run RCStamp tool as pre build step and generate include file with version number constants
				//http://www.codeproject.com/KB/dotnet/build_versioning.aspx
				//		( "version,v",										"print version string" )
				( "h,help", "print help" )
				;

			//positional options:
			//options.parse_positional( { "" } );

			try
			{
				cxxopts::ParseResult result = options.parse( argc, argv );

				//generic options
				if( result.count( "help" ) )
				{
					std::cout << options.help( {
						""//, 
						//"OSC" 
						} ) << std::endl;

					return false;
				}

				//TODO
			}
			catch( std::exception & e )
			{
				std::cerr << e.what() << std::endl;

				std::stringstream sstr;
				sstr << std::endl << "An error occured, processing program options: " << std::endl
					<< e.what() << std::endl;

				std::cout << options.help();

				throw std::runtime_error( sstr.str().c_str() );
			}

			return true;
		}
	};
}

using namespace cvglTemplate;

StartupConfig cfg;

bool quit = false;

GLFWwindow *window = nullptr;

int windowX = 100;
int windowY = 100;

int windowWidth = 1800;
int windowHeight = 1040;

float ar = 1.0f;

double timeAccu = 0.0;


Trackball3D trackball3D( glm::vec3( 0, 0, -5 ), glm::vec3( toRad( 30 ), toRad( -45 ), 0 ) );

glm::mat4 mProj( glm::identity<glm::mat4>() );

glm::vec4 clearColor( 0, 0, 0.2f, 1 );

glm::vec2 oldMousePos;
glm::vec2 mousePos;

std::vector<bool> mousePressed( 10 );

cv::Mat currentFrame;
cv::VideoCapture capture;

void moved( int x, int y )
{}

void sizeChanged( int width, int height)
{
	windowWidth = width;
	windowHeight = height;

	ar = (float) windowWidth / windowHeight;

	mProj = glm::perspectiveFov( (float)toRad( 60.0f ), (float)windowWidth, (float)windowHeight, 0.1f, 100.0f );
}

void keyDown( int key, int scanCode, int action, int mods )
{
	trackball3D.onSpecialDown( key );

	switch( key )
	{
	case GLFW_KEY_ESCAPE:
		quit = true;
		break;
	}
}

void charDown( unsigned char c )
{
	trackball3D.onKeyDown( c );

	switch( c )
	{
	case ' ':
		break;
	}
}


/*
 *  @param[in] error An [error code](@ref errors).
 *  @param[in] description A UTF-8 encoded string describing the error.
 */
void onError( int error, const char *desc )
{
	std::cerr << "<error> GLFW err #" << error << ": \"" << desc << "\"" << std::endl;
}

/*
 *  @param[in] xpos The new x-coordinate, in screen coordinates, of the
 *  upper-left corner of the content area of the window.
 *  @param[in] ypos The new y-coordinate, in screen coordinates, of the
 *  upper-left corner of the content area of the window.
 */
void onWindowMoved( GLFWwindow *window, int x, int y )
{
	moved( x, y );
}

/*
 *  @param[in] width The new width, in screen coordinates, of the window.
 *  @param[in] height The new height, in screen coordinates, of the window.
 */
void onWindowResized( GLFWwindow *window, int width, int height )
{
	sizeChanged( width, height );
}

/*
 *  @param[in] window The window that received the event.
 *  @param[in] key The [keyboard key](@ref keys) that was pressed or released.
 *  @param[in] scancode The system-specific scancode of the key.
 *  @param[in] action `GLFW_PRESS`, `GLFW_RELEASE` or `GLFW_REPEAT`.
 *  @param[in] mods Bit field describing which [modifier keys](@ref mods) were
 *  held down.
 */
void onKey( GLFWwindow *window, int key, int scanCode, int action, int mods )
{
	//bool imGuiHandled = ( ImGui::GetIO().WantCaptureKeyboard );

	//if( !imGuiHandled )
		keyDown( key, scanCode, action, mods );

	//ImGui_ImplGlfw_KeyCallback( window, key, scanCode, action, mods );
}

/*
 *@param[in] codepoint The Unicode code point of the character.
 */
void onChar( GLFWwindow *window, unsigned int c )
{
	//bool imGuiHandled = ( ImGui::GetIO().WantCaptureKeyboard );

	//if( !imGuiHandled )
		charDown( c );

	//ImGui_ImplGlfw_CharCallback( window, c );
}

/*
 *  @param[in] codepoint The Unicode code point of the character.
 *  @param[in] mods Bit field describing which [modifier keys](@ref mods) were
 */
void onCharMods( GLFWwindow *window, unsigned int c, int mods )
{
}

/*
 *  @param[in] button The [mouse button](@ref buttons) that was pressed or
 *  released.
 *  @param[in] action One of `GLFW_PRESS` or `GLFW_RELEASE`.
 *  @param[in] mods Bit field describing which [modifier keys](@ref mods) were
 *  held down.
 */
void onMouseButton( GLFWwindow *window, int button, int action, int mods )
{
	//bool imGuiHandled = ( ImGui::GetIO().WantCaptureMouse );

	trackball3D.onMouseButton( button, action == GLFW_PRESS );

	if( button < mousePressed.size() )
	{
		if( action == GLFW_PRESS )
		{
			//if( !imGuiHandled )
				mousePressed[button] = true;
		}
		else
			mousePressed[button] = false;
	}
}

/*
 *  @param[in] xpos The new cursor x-coordinate, relative to the left edge of
 *  the content area.
 *  @param[in] ypos The new cursor y-coordinate, relative to the top edge of the
 *  content area.
 */
void onCursorMoved( GLFWwindow *window, double x, double y )
{
	static bool firstFrame = true;

	trackball3D.onMotion( x, y );

	glm::vec2 newMousePos( x, y );

	if( firstFrame )
		firstFrame = false;
	else
		oldMousePos = newMousePos;

	mousePos = newMousePos;
}

/*
 *  @param[in] xoffset The scroll offset along the x-axis.
 *  @param[in] yoffset The scroll offset along the y-axis.
 */
void onScroll( GLFWwindow *window, double xoffset, double yoffset )
{
	//bool imGuiHandled = ( ImGui::GetIO().WantCaptureMouse );

	trackball3D.onScroll( xoffset, yoffset );

	//if( !imGuiHandled )
	{
	}

	//ImGui_ImplGlfw_ScrollCallback( window, xoffset, yoffset );
}

void update()
{
	static auto prevTime = std::chrono::system_clock::now();
	auto currentTime = std::chrono::system_clock::now();
	std::chrono::duration<double> diff = currentTime - prevTime;
	prevTime = currentTime;

	double dt = diff.count();
	::globalTime += dt;

	if( capture.isOpened() )
		capture >> currentFrame;

	//TODO
}

void display()
{
	if( !currentFrame.empty() )
		cv::imshow( "camera", currentFrame );

	glViewport( 0, 0, windowWidth, windowHeight );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMultMatrixf( glm::value_ptr( mProj ) );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	//glMultMatrixf( glm::value_ptr( mView ) );
	glMultMatrixf( glm::value_ptr( trackball3D.mat() ) );

	glPushAttrib( GL_ALL_ATTRIB_BITS );
	{
		drawAxes();
		drawGrid();

		//TODO
	}
	glPopAttrib();

	
	glfwMakeContextCurrent( window );
	glfwSwapBuffers( window );

	checkForGLError();
}

void idle()
{
	glfwPollEvents();

	update();
	display();
}

void initGL( int argc, char **argv )
{
	glfwSetErrorCallback( onError );

	glfwInit();

	const char *glsl_version = "#version 130";
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

	glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );
	glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_API );

	window = glfwCreateWindow( windowWidth, windowHeight, "cvglTemplate", nullptr, nullptr );
	if( !window )
		throw std::runtime_error( "failed to init glfw" );

	glfwMakeContextCurrent( window );
	glfwSwapInterval( 1 ); // Enable vsync

	glfwSetWindowAttrib( window, GLFW_RESIZABLE, GLFW_TRUE );

	glfwSetWindowPos( window, windowX, windowY );
	glfwSetWindowSize( window, windowWidth, windowHeight );


	glfwSetWindowPosCallback( window, onWindowMoved );
	glfwSetWindowSizeCallback( window, onWindowResized );
	glfwSetKeyCallback( window, onKey );
	glfwSetCharCallback( window, onChar );
	glfwSetCharModsCallback( window, onCharMods );
	glfwSetMouseButtonCallback( window, onMouseButton );
	glfwSetCursorPosCallback( window, onCursorMoved );
	glfwSetScrollCallback( window, onScroll );

//#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
//	bool err = gl3wInit() != 0;
//#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
//	bool err = glewInit() != GLEW_OK;
//#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
//	bool err = gladLoadGL() == 0;
//#else
//	bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
//#endif
//	if( err )
//		throw std::runtime_error( "Failed to initialize OpenGL loader!" );

	std::cout << "GL version: " << glGetString( GL_VERSION ) << std::endl;
	//TODO: print some more gl infos

#ifdef _WIN32
	//enable v-sync
	//wglSwapIntervalEXT( 1 );	// -> seems to be null?
#endif

	glPointSize( 4 );
	glLineWidth( 1 );

	glClearColor( clearColor[0], clearColor[1], clearColor[2], clearColor[3] );

	glDisable( GL_TEXTURE_2D );
	glDisable( GL_LIGHTING );
	glEnable( GL_COLOR_MATERIAL );
	glEnable( GL_CULL_FACE );
	glEnable( GL_DEPTH_TEST );

	//initImGui();

	moved( windowX, windowY );
	sizeChanged( windowWidth, windowHeight );

	//mView = glm::rotate(
	//	glm::rotate( 
	//			glm::translate(
	//				glm::identity<glm::mat4>(),
	//				glm::vec3( 0.0f, 0.0f, -5.0f ) ),
	//			(float)toRad( 30.0f ), unitX() ), 
	//	(float)toRad( -45.0f ), unitY() );

	std::cout << "initialized GL" << std::endl;

	//if( !initFont( "resources/courier.ttf", 12, windowWidth, windowHeight ) )
	//	std::cerr << "<error> loading font file failed" << std::endl;
}


bool ctrlHandler( DWORD fdwCtrlType )
{
	switch( fdwCtrlType )
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		quit = true;
		return true;
	}

	return false;
}

static void cleanup()
{
}


void __cdecl onExit()
{
	std::cerr << "on exit called" << std::endl;

	cleanup();
}

void __cdecl onTerminate()
{
	std::cerr << "process terminated" << std::endl;
	abort();
}

int main( int argc, char **argv )
{
	atexit( onExit );
	std::set_terminate( onTerminate );

	srand( time( 0 ) );

	try
	{
		if( !SetConsoleCtrlHandler( (PHANDLER_ROUTINE) ctrlHandler, true ) )
			throw std::runtime_error( "registering ctrl-handler failed" );

		disableQuickEditMode();

		if( StartupConfig::readPA( argc, argv, cfg ) )
		{
			initGL( argc, argv );

			if( !capture.open( 0 ) )
				std::cerr << "<error> failed opening camera" << std::endl;

			while( !quit && !glfwWindowShouldClose( window ) )
			{
				idle();
			}

			cleanup();
		}
	}
	catch( std::exception & e )
	{
		std::cerr << "caught exception: " << e.what() << std::endl;

		cleanup();

#ifdef _DEBUG
#ifdef _WIN32
		std::cout << "press ENTER" << std::endl;
		getchar();
#endif
#endif
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
