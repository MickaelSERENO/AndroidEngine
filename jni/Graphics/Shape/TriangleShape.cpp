#include "Shape/TriangleShape.h"

TriangleShape::TriangleShape(const glm::vec3* vertexCoord, int nbVertex, const Color* colors, bool uniColor, GLuint mode) : Drawable(Drawable::shaders.get("colorshader")), m_nbVertex(nbVertex), m_useUniColor(uniColor), m_mode(mode)
{
	m_uniColor = (float*)malloc(4*sizeof(float));
	if(colors != NULL)
	{
		m_uniColor[0] = colors[0].r;	
		m_uniColor[1] = colors[0].g;	
		m_uniColor[2] = colors[0].b;	
		m_uniColor[3] = colors[0].a;	
	}
	else
		for(int i=0; i < 4; i++)
			m_uniColor[i] = 0.0f;

	float* c=NULL;
	if(!uniColor)
	{
		c = (float*)malloc(4*m_nbVertex*sizeof(float));
		if(colors != NULL)
		{
			for(int i=0; i < m_nbVertex; i++)
			{
				c[4*i+0] = colors[i].r;	
				c[4*i+1] = colors[i].g;	
				c[4*i+2] = colors[i].b;	
				c[4*i+3] = colors[i].a;	
			}
		}

		else
			for(int i=0; i < 4*m_nbVertex; i++)
				c[i] = 0.0f;
	}

	float* v=(float*)malloc(3*m_nbVertex*sizeof(float));
	if(vertexCoord == NULL)
		for(int i=0; i < 3*m_nbVertex; i++)
			v[i] = 0.0f;

	else
		for(int i=0; i < m_nbVertex; i++)
			for(int j=0; j < 3; j++)
				v[3*i+j] = vertexCoord[i][j];

	initVbos(v, c);
	free(v);
	if(c != NULL)
		free(c);
}

TriangleShape::~TriangleShape()
{
	Drawable::~Drawable();
	free(m_uniColor);
}

void TriangleShape::onDraw(Renderer* renderer)
{
	glm::mat4 mvp = renderer->getCamera()->getMatrix() * getMatrix();
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	{
		GLuint vPosition = glGetAttribLocation(m_shader->getProgramID(), "vPosition");
		GLuint vColor    = glGetAttribLocation(m_shader->getProgramID(), "vColor");

		GLuint uMvp      = glGetUniformLocation(m_shader->getProgramID(), "uMVP");
		GLuint uUseColor = glGetUniformLocation(m_shader->getProgramID(), "uUseColor");

		glEnableVertexAttribArray(vPosition);
		glEnableVertexAttribArray(vColor);

		glVertexAttribPointer(vPosition, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(0));
		glVertexAttribPointer(vColor, 4, GL_FLOAT, false, 0, BUFFER_OFFSET(m_nbVertex * 3*sizeof(float)));
		if(m_uniColor)
		{
			GLuint uColor    = glGetUniformLocation(m_shader->getProgramID(), "uColor");
			glUniform4fv(uColor, 1, m_uniColor);
		}
		glUniformMatrix4fv(uMvp, 1, false, glm::value_ptr(mvp));
		glUniform1i(uUseColor, m_useUniColor);
		glDrawArrays(m_mode, 0, m_nbVertex);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TriangleShape::setVertexCoord(const glm::vec3* vertexCoord)
{
	float* v=(float*)malloc(3*m_nbVertex*sizeof(float));
	if(vertexCoord == NULL)
		for(int i=0; i < 3*m_nbVertex; i++)
			v[i] = 0.0f;

	else
		for(int i=0; i < m_nbVertex; i++)
			for(int j=0; j < 3; i++)
				v[3*i+j] = vertexCoord[i][j];
	setArrayVertex(v);
	free(v);
}

void TriangleShape::setColors(const Color* colors)
{
	float* c = (float*)malloc(4*m_nbVertex*sizeof(float));
	for(int i=0; i < m_nbVertex; i++)
	{
		c[4*i+0] = colors[i].r;	
		c[4*i+1] = colors[i].g;	
		c[4*i+2] = colors[i].b;	
		c[4*i+3] = colors[i].a;	
	}
	setArrayColor(c);
	free(c);
}

void TriangleShape::setUniColor(const Color& color)
{
	m_uniColor[0] = color.r;	
	m_uniColor[1] = color.g;	
	m_uniColor[2] = color.b;	
	m_uniColor[3] = color.a;	
}

void TriangleShape::useUniColor(bool uniColor)
{
	m_useUniColor = uniColor;
}

bool TriangleShape::isUsingUniColor()
{
	return m_uniColor;
}

Color TriangleShape::getUniColor()
{
	Color c;

	c.r = m_uniColor[0];
	c.g = m_uniColor[1];
	c.b = m_uniColor[2];
	c.a = m_uniColor[3];

	return c;
}

int TriangleShape::getNbVertex()
{
	return m_nbVertex;
}

Color TriangleShape::getColor(int vertex)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	//m_nbVertex*3 are the offset of colors on the buffer
	float *colors = (float*) glMapBufferOES(GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES) + (m_nbVertex*3);

	Color c(colors[4*vertex], colors[4*vertex + 1], colors[4*vertex + 2], colors[4*vertex + 3]);
	glUnmapBufferOES(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return c;
}

glm::vec3 TriangleShape::getPosition(int vertex)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	float* position = (float*) glMapBufferOES(GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES);

	glm::vec3 v(position[3*vertex], position[3*vertex + 1], position[3*vertex + 2]);
	glUnmapBufferOES(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return v;
}

void TriangleShape::initVbos(const float* vertexCoord, const float* colors)
{
	int size = m_nbVertex * sizeof(float) * (3+4);
	deleteVbos();
	glGenBuffers(1, &m_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	{
		glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	setArrayVertex(vertexCoord);
	if(!m_useUniColor)
		setArrayColor(colors);
}

void TriangleShape::setArrayVertex(const float* vertexCoords)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, 3*m_nbVertex*sizeof(float), vertexCoords);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TriangleShape::setArrayColor(const float* colors)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	{
		glBufferSubData(GL_ARRAY_BUFFER, 3*m_nbVertex*sizeof(float), 4*m_nbVertex*sizeof(float), colors);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
