#include "Renderer.h"

Renderer::Renderer(Updatable* parent) : Render(parent), m_disp(EGL_NO_CONTEXT), m_surface(EGL_NO_SURFACE), m_context(EGL_NO_CONTEXT),
										m_conf(0), m_nbConf(0), m_format(0), m_width(0), m_window(0)
{
}

Renderer::~Renderer()
{
	terminate();
}

void Renderer::terminate()
{
	if(m_disp == EGL_NO_DISPLAY)
		return;

	eglMakeCurrent(m_disp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(m_disp, m_context);
	eglDestroySurface(m_disp, m_surface);

	m_surface = EGL_NO_SURFACE;
	m_context = EGL_NO_CONTEXT;
	eglTerminate(m_disp);
	m_disp = EGL_NO_DISPLAY;
}

bool Renderer::initializeContext(ANativeWindow* window)
{
	m_start = false;
	m_window = window;
	terminate();

	//Initialize the egl context
	const EGLint attribs[] = 
	{
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_NONE
	};

	const EGLint eglAttribs[] =
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	if((m_disp = eglGetDisplay(EGL_DEFAULT_DISPLAY))==EGL_NO_DISPLAY)
	{
		LOG_ERROR("Can't get the default display. Error : %d", eglGetError());
		return false;
	}

	if(!eglInitialize(m_disp, NULL, NULL))
	{
		LOG_ERROR("Can't init the egl buffer. Error : %d", eglGetError());
		return false;
	}

	if(!eglChooseConfig(m_disp, attribs, &m_conf, 1, &m_nbConf))
	{
		LOG_ERROR("Can't configure the buffer. Error : %d", eglGetError());
		return false;
	}

	if(!eglGetConfigAttrib(m_disp, m_conf, EGL_NATIVE_VISUAL_ID, &m_format))
	{
		LOG_ERROR("Can't get an attributes. Error : %d", eglGetError());
		return false;
	}

	if(!(m_context = eglCreateContext(m_disp, m_conf, EGL_NO_CONTEXT, eglAttribs)))
	{
		LOG_ERROR("Can't create an EGL context. Error : %d", eglGetError());
		return false;
	}

	eglMakeCurrent(m_disp, EGL_NO_SURFACE, EGL_NO_SURFACE, m_context);
	Drawable::initShaders();
	initializeSurface(window);
	Drawable::initShaders();
	return true;
}

void Renderer::initializeSurface(ANativeWindow* window)
{
	if(window == NULL)
		return;
	deleteSurface();
	eglMakeCurrent(m_disp, EGL_NO_SURFACE, EGL_NO_SURFACE, m_context);
	m_window = window;
	m_start = false;

	if(!(m_surface = eglCreateWindowSurface(m_disp, m_conf, window, 0)))
	{
		LOG_ERROR("Can't create an EGL surface from this window. Error : %d", eglGetError());
		return;
	}

	if(!eglMakeCurrent(m_disp, m_surface, m_surface, m_context))
	{
		LOG_ERROR("ERROR : Can't make this context the current one. Error : %d", eglGetError());
		initializeSurface(window);
		return;
	}

	ANativeWindow_setBuffersGeometry(window, 0, 0, m_format);

	eglQuerySurface(m_disp, m_surface, EGL_WIDTH, &m_width);
	eglQuerySurface(m_disp, m_surface, EGL_HEIGHT, &m_height);

	//Initialize OpenGL
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, m_width, m_height);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

	m_start = true;
}

void Renderer::init()
{
}

void Renderer::display()
{
	eglSwapBuffers(m_disp, m_surface);
}

void Renderer::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::updateFocus(uint32_t pID)
{
	Updatable::updateFocus(pID, *this);
	if(focusIsCheck == false)
		onFocus(pID, *this);
	Updatable::focusIsCheck = false;
}

void Renderer::update(Render& render)
{
	Render::update(render);
	Render::updateGPU(render);
}

void Renderer::initDraw()
{
	if(!eglMakeCurrent(m_disp, m_surface, m_surface, m_context))
	{
		LOG_ERROR("Init Draw Can't make this context the current one. Error : %d", eglGetError());
		return;
	}
}

void Renderer::stopDraw()
{
	eglMakeCurrent(m_disp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

void Renderer::onDownTouchEvent(uint32_t i, float x, float y)
{
	touchCoord[i].type   = DOWN;
	touchCoord[i].startX = x;
	touchCoord[i].x      = x;
	touchCoord[i].startY = y;
	touchCoord[i].y      = y;

	updateFocus(i);
}

void Renderer::accelerometerEvent(float x, float y, float z)
{
}

void Renderer::onUpTouchEvent(uint32_t i, float x, float y)
{
	touchCoord[i].type = UP;
	touchCoord[i].x    = x;
	touchCoord[i].y    = y;
}

void Renderer::onMoveTouchEvent(uint32_t i, float x, float y)
{
	touchCoord[i].x    = x;
	touchCoord[i].y    = y;
	touchCoord[i].type = MOVE;
}

void Renderer::deleteSurface()
{
	eglMakeCurrent(m_disp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	if(m_surface != EGL_NO_SURFACE)
		eglDestroySurface(m_disp, m_surface);

	m_surface = EGL_NO_SURFACE;
}

bool Renderer::hasDisplay()
{
	return (m_disp != EGL_NO_DISPLAY);
}
