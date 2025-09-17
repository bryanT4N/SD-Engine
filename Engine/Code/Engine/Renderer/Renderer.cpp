#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in VERY few places (and .CPPs only)

#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library

//-----------------------------------------------------------------------------------------------
HGLRC g_openGLRenderingContext = nullptr; 

//-----------------------------------------------------------------------------------------------
Renderer::Renderer()
{
}

//-----------------------------------------------------------------------------------------------
Renderer::~Renderer()
{
}

//-----------------------------------------------------------------------------------------------
void Renderer::Startup()
{
	CreateRenderingContext();
}

//-----------------------------------------------------------------------------------------------
void Renderer::Shutdown()
{
}

//-----------------------------------------------------------------------------------------------
void Renderer::BeginFrame()
{
}

//-----------------------------------------------------------------------------------------------
void Renderer::EndFrame()
{
}

//-----------------------------------------------------------------------------------------------
// Given an existing OS Window, create a Rendering Context (RC) for OpenGL or DirectX to draw to it.
void Renderer::CreateRenderingContext()
{
	// Creates an OpenGL rendering context (RC) and binds it to the current window's device context (DC)
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	memset(&pixelFormatDescriptor, 0, sizeof(pixelFormatDescriptor));
	pixelFormatDescriptor.nSize = sizeof(pixelFormatDescriptor);
	pixelFormatDescriptor.nVersion = 1;
	pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits = 24;
	pixelFormatDescriptor.cDepthBits = 24;
	pixelFormatDescriptor.cAccumBits = 0;
	pixelFormatDescriptor.cStencilBits = 8;

	HWND windowHandle =::GetActiveWindow(); // #TempHack: Later, the window subsystem will hold this
	HDC displayDeviceContext = GetDC( windowHandle );

	// These two OpenGL-like functions (wglCreateContext and wglMakeCurrent) will remain here for now.
	int pixelFormatCode = ChoosePixelFormat(displayDeviceContext, &pixelFormatDescriptor);
	SetPixelFormat(displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor);
	g_openGLRenderingContext = wglCreateContext(displayDeviceContext);
	wglMakeCurrent(displayDeviceContext, g_openGLRenderingContext);

	// Configure some initial drawing state for OpenGL ( enable alpha blending )
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//-----------------------------------------------------------------------------------------------
void Renderer::ClearScreen(Rgba8 const& clearColor)
{
	// Clear all screen (back buffer) pixels to clearColor
	float floatR = static_cast<float>(clearColor.r) / 256.f;
	float floatG = static_cast<float>(clearColor.g) / 256.f;
	float floatB = static_cast<float>(clearColor.b) / 256.f;
	float floatA = static_cast<float>(clearColor.a) / 256.f;

	// Note: glClearColor takes colors as floats in [0,1], not bytes in [0,255]
	glClearColor(floatR, floatG, floatB, floatA);
	glClear(GL_COLOR_BUFFER_BIT); // ALWAYS clear the screen at the top of each frame's Render()!
}

//-----------------------------------------------------------------------------------------------
void Renderer::BeginCamera(Camera const& camera)
{
	// Establish a 2D (orthographic) drawing coordinate system: (0,0) bottom-left to (10,10) top-right
	glLoadIdentity();

	// arguments are: xLeft, xRight, yBottom, yTop, zNear, zFar
	glOrtho(camera.GetOrthoBottomLeft().x, camera.GetOrthoTopRight().x, camera.GetOrthoBottomLeft().y, camera.GetOrthoTopRight().y, 0.f, 1.f); 
}

//-----------------------------------------------------------------------------------------------
void Renderer::EndCamera(Camera const& camera)
{

}

//-----------------------------------------------------------------------------------------------
void Renderer::DrawVertexArray(int numVertexes, Vertex const* vertexes)
{
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < numVertexes; ++i) 
	{
		Vertex const& vert = vertexes[i];
		glColor4ub(vert.m_color.r, vert.m_color.g, vert.m_color.b, vert.m_color.a);
		glTexCoord2f(vert.m_uvTexCoords.x, vert.m_uvTexCoords.y);
		glVertex3f(vert.m_position.x, vert.m_position.y, vert.m_position.z);
	}

	glEnd(); // GL_TRIANGLES
}

