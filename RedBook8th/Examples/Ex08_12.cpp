/*
* Ex08_12.cpp
*
* Created on: Mar 17, 2014
* Author: Andrew Zhabura
*/

#include "Ex08_12.h"

#include "GL/LoadShaders.h"
#include "Auxiliary/vmath.h"
#include "Auxiliary/vermilion.h"

Ex08_12::Ex08_12()
	: OGLWindow("Example08_12", "Example 8.12 (M, A, N)")
	, recalc_mat(true)
{
}

Ex08_12::~Ex08_12()
{
	glUseProgram(0);
	glDeleteProgram(object_prog);
}

void Ex08_12::InitGL()
{
	if (! LoadGL() )
		return;

	ShaderInfo  object_shaders[] = {
		{ GL_VERTEX_SHADER, "Shaders/sh08_12.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/sh08_12.frag" },
		{ GL_NONE, NULL }
	};

	object_prog = LoadShaders( object_shaders );

	glGenBuffers(1, &quad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);

	static const GLfloat quad_data[] =
	{
		0.75f, -0.75f,
		-0.75f, -0.75f,
		-0.75f, 0.75f,
		0.75f, 0.75f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(8 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glLinkProgram(object_prog);
	
	object_mat_mv_loc = glGetUniformLocation(object_prog, "MVMatrix");
	object_mat_mvp_loc = glGetUniformLocation(object_prog, "MVPMatrix");
	GLuint NormalMatrix_loc = glGetUniformLocation(object_prog, "NormalMatrix");
	GLuint LightPosition_loc = glGetUniformLocation(object_prog, "LightPosition");

	GLuint BrickColor_loc = glGetUniformLocation(object_prog, "BrickColor");
	GLuint MortarColor_loc = glGetUniformLocation(object_prog, "MortarColor");

	GLuint BrickSize_loc = glGetUniformLocation(object_prog, "BrickSize");
	GLuint BrickPct_loc = glGetUniformLocation(object_prog, "BrickPct");
	MortarPct_loc = glGetUniformLocation(object_prog, "MortarPct");

	glUseProgram(object_prog);	
	
	GLfloat normal_matrix[3 * 3] = {
		0.5f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	glUniformMatrix3fv(NormalMatrix_loc, 1, GL_TRUE, normal_matrix);
	glUniform3fv(LightPosition_loc, 1, vmath::vec3(0.0f, 0.0f, -1.0f));

	glUniform3fv(BrickColor_loc, 1, vmath::vec3(1.0f, 0.35f, 0.15f));
	glUniform3fv(MortarColor_loc, 1, vmath::vec3(0.7f, 0.7f, 0.7f));

	glUniform2fv(BrickSize_loc, 1, vmath::vec2(0.5f, 0.35f));
	glUniform2fv(BrickPct_loc, 1, vmath::vec2(0.9f, 0.9f));
	glUniform2fv(MortarPct_loc, 1, vmath::vec2(0.1f, 0.1f));
}

void Ex08_12::Display()
{
	static const unsigned int start_time = GetTickCount() - 50000;
	float t = float((GetTickCount() - start_time)) / float(0x3FFF);
	static const vmath::vec3 X(1.0f, 0.0f, 0.0f);
	static const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
	static const vmath::vec3 Z(0.0f, 0.0f, 1.0f);

	vmath::mat4 tc_matrix(vmath::mat4::identity());

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);

	float aspect = float(getHeight()) / getWidth();

	glUseProgram(object_prog);

	if (recalc_mat)
	{
		tc_matrix = vmath::translate(vmath::vec3(0.0f, 0.0f, -1.5f)) *
			vmath::rotate(80.0f * 3.0f * t, Y) * vmath::rotate(50.0f * 3.0f * t, Z);

		glUniformMatrix4fv(object_mat_mv_loc, 1, GL_FALSE, tc_matrix);

		tc_matrix = vmath::perspective(35.0f, 1.0f / aspect, 0.1f, 100.0f) * tc_matrix;
		glUniformMatrix4fv(object_mat_mvp_loc, 1, GL_FALSE, tc_matrix);
	}
	recalc_mat = true;

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	Swap();
}

void Ex08_12::keyboard( unsigned char key, int x, int y )
{
	switch( key ) {
	case 'M': 
		for (int i = 0; i < c_repeat; ++i)
			Display();
		break;
	case 'A': 
		glUseProgram(object_prog);
		glUniform2fv(MortarPct_loc, 1, vmath::vec2(0.1f, 0.1f));
		glUseProgram(0);
		recalc_mat = false;
		Display();
		break;
	case 'N': 
		glUseProgram(object_prog);
		glUniform2fv(MortarPct_loc, 1, vmath::vec2(0.0f, 0.0f));
		glUseProgram(0);
		recalc_mat = false;
		Display();
		break;
	default:
		OGLWindow::keyboard(key, x, y);
		break;
	}
}
