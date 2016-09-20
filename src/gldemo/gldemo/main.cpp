// gldemo.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "glw.h"

static const char* vertex_shader = ""
"#version 450\n"
"layout (location = 0) in vec3 position;      \n"
"uniform mat4 mvp;                            \n"
"void main()                                  \n"
"{                                            \n"
"    gl_Position = mvp * vec4(position, 1.0); \n"
"}"
"";
static const char* fragment_shader = ""
"#version 450\n"
"out vec4 out_color;                       \n"
"void main()                               \n"
"{                                         \n"
"    out_color = vec4(1.0, 1.0, 1.0, 1.0); \n"
"}"
"";

static void set_dot(glw::Model& model)
{
	model.SetVertices({
		0, 0, 0,
		0.7f, 0.5f, 0
	});
	model.DrawPoints(100.0f);
}

static void set_line(glw::Model& model)
{
	model.SetVertices({
		 0.5f, 0, 0,
		 -0.5f, 0, 0,
		 0.5f, 0.5f, 0,
		 -0.5f, 0.5f, 0,
	});
	model.DrawLines(100.0f);
}

static void set_triangle(glw::Model& model)
{
	model.SetVertices({
		 0.5f, -0.5f, 0,
		    0,  0.5f, 0,
		-0.5f, -0.5f, 0,
	});
	model.DrawTriangles({
		0, 1, 2
	});
}

static void set_quadrangle(glw::Model& model)
{
	model.SetVertices({
		 0.5, -0.5, 0,
		 0.5,  0.5, 0,
		-0.5,  0.5, 0,
		-0.5, -0.5, 0,
	});
	model.DrawTriangles({
		0, 1, 2,
		0, 2, 3,
	});
}

static void set_cube(glw::Model& model, glw::Camera& camera)
{
	model.SetVertices({
		 1.0, -1.0, -1.0,
		 1.0, -1.0,  1.0,
		-1.0, -1.0,  1.0,
		-1.0, -1.0, -1.0,

		 1.0,  1.0, -1.0,
		 1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		-1.0,  1.0, -1.0
	});
	model.DrawTriangles({
		1, 3, 0,
		7, 5, 4,

		4, 1, 0,
		5, 2, 1,

		2, 7, 3,
		0, 7, 4,

		1, 2, 3,
		7, 6, 5,

		4, 5, 1,
		5, 6, 2,

		2, 6, 7,
		0, 3, 7
	});

	glm::mat4 mvp = camera.LookAt(glm::vec3(4, 2, 6), glm::vec3(0, 0, 0));
	model.SetMVP(mvp);
}

int main()
{
	glw::Window window;
	window.Init("OpenGL demo", 400, 300);
	window.BackgroundColor(0.3f, 0.6f, 0.3f, 1.0f);

	glw::Camera camera = window.GetCamera(45.0f);
	glw::Model model;
	model.SetShader(vertex_shader, fragment_shader);

	//set_dot(model);
	//set_line(model);
	//set_triangle(model);
	//set_quadrangle(model);
	set_cube(model, camera);

	do
	{
		model.Render();
	} while (window.Update());

    return 0;
}