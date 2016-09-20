#ifndef  GLW_H_
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iterator>
#include <vector>

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

namespace glw
{
	class Model
	{
	public:
		Model();
		void SetIndices(const std::vector<GLuint>& indices);
		void SetVertices(const std::vector<GLfloat>& xs);
		void SetShader(const char *vertex, const char *fragment);
		void SetRGB(const std::vector<GLfloat>& xs);
		void DrawLines(float size);
		void DrawPoints(float size);
		void DrawTriangles(const std::vector<GLuint>& xs);
		void SetMVP(glm::mat4 mvp);
		void Render();

	private:
		GLint render_type_;
		GLint matrix_id_;
		GLuint program_;
		GLuint vbo_;
		std::vector<GLuint> indices_;
		uintmax_t vertex_count_;
		void set(const std::vector<GLfloat>& xs, int location, int bind_index, int dimension);
		GLuint addShader(const char* text, int type);
	};

	class Camera
	{
	public:
		Camera(float radians, int width, int height);
		glm::mat4 LookAt(glm::vec3 position, glm::vec3 angle);
		glm::mat4 projection_;
	};

	class Window
	{
	public:
		void Init(const char* title, int width, int height);
		bool Update();
		void BackgroundColor(float r, float g, float b, float a);
		Camera& Window::GetCamera(float radians);
		~Window();

	private:
		GLFWwindow *window_;
		int width_;
		int height_;
		GLclampf bg_r_;
		GLclampf bg_g_;
		GLclampf bg_b_;
		GLclampf bg_a_;
	};
}

#endif // ! GLW_H_