#include "stdafx.h"
#include "glw.h"

namespace glw
{
	// --------------------------------------------------------------
	static void APIENTRY opengl_debug_callback(
		GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length,
		const GLchar *message, const void *param)
	{
		fprintf(stderr, "[%d, %d, %d, %d] %s\n",
			source,
			type,
			severity,
			id,
			message);
	}

	static void glfw_error_callback(int error, const char *message)
	{
		fprintf(stderr, "GLFW: %s\n", message);
	}

	static std::string loadPathOrDie(const char *path)
	{
		std::ifstream ifs(path, std::ios::binary);
		if (ifs.fail())
		{
			throw "fail";
		}

		std::stringstream string_stream;
		string_stream << ifs.rdbuf();
		ifs.close();
		return string_stream.str();
	}

	static GLuint loadShader(const char *path, int type)
	{
		std::string text = loadPathOrDie(path);
		GLuint shader = glCreateShader(type);
		const char* buffer = text.c_str();
		glShaderSource(shader, 1, &buffer, NULL);
		glCompileShader(shader);
		return shader;
	}

	// --------------------------------------------------------------
	void Window::Init(const char* title, int width, int height)
	{
		if (glfwInit() != GL_TRUE) {
			throw std::runtime_error("Failed to initialize GLFW");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		window_ = glfwCreateWindow(width, height, title, NULL, NULL);

		if (!window_) {
			throw std::runtime_error("Failed to create window");
		}

		glfwMakeContextCurrent(window_);
		glewExperimental = GL_TRUE;
		GLenum ret_code = glewInit();
		if (ret_code != GLEW_OK) {
			throw std::runtime_error(reinterpret_cast<const char*>(glewGetErrorString(ret_code)));
		}
		glfwSetErrorCallback(glfw_error_callback);
		glDebugMessageCallback(opengl_debug_callback, NULL);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);

		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		width_ = width;
		height_ = height;
	}

	bool Window::Update()
	{
		glfwSwapBuffers(window_);
		glfwPollEvents();

		glClearColor(bg_r_, bg_g_, bg_b_, bg_a_);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return !glfwWindowShouldClose(window_);
	}

	void Window::BackgroundColor(float r, float g, float b, float a)
	{
		bg_r_ = r;
		bg_g_ = g;
		bg_b_ = b;
		bg_a_ = a;
	}

	Camera& Window::GetCamera(float radians)
	{
		return Camera(radians, width_, height_);
	}

	Window::~Window()
	{
		glfwTerminate();
	}

	// --------------------------------------------------------------
	Model::Model()
	{
		glCreateVertexArrays(1, &vbo_);
		render_type_ = GL_POINTS;
	}

	GLuint Model::addShader(const char* text, int type)
	{
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &text, NULL);
		glCompileShader(shader);

		GLint result;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{
			int buffer_size;
			int written_size;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &buffer_size);
			char *buffer = new char[buffer_size];
			glGetShaderInfoLog(shader, buffer_size, &written_size, buffer);
			std::string message(buffer, written_size);
			throw std::runtime_error(message);
		}

		return shader;
	}
	
	void Model::SetShader(const char *vertex_text, const char *fragment_text)
	{
		GLuint vertex = addShader(vertex_text, GL_VERTEX_SHADER);
		GLuint fragment = addShader(fragment_text, GL_FRAGMENT_SHADER);

		program_ = glCreateProgram();
		glAttachShader(program_, vertex);
		glAttachShader(program_, fragment);
		glLinkProgram(program_);

		glDeleteShader(vertex);
		glDeleteShader(fragment);

		glUseProgram(program_);

		matrix_id_ = glGetUniformLocation(program_, "mvp");
		glm::mat4 mvp(1);
		glUniformMatrix4fv(matrix_id_, 1, GL_FALSE, &mvp[0][0]);
	}

	void Model::SetIndices(const std::vector<GLuint>& indices)
	{
		std::copy(indices.begin(), indices.end(), std::back_inserter(this->indices_));
	}

	void Model::SetVertices(const std::vector<GLfloat>& xs)
	{
		const int dimension = 3;
		set(xs, 0, 0, dimension);
		vertex_count_ = xs.size() / dimension;
	}

	void Model::SetRGB(const std::vector<GLfloat>& xs)
	{
		const int dimension = 3;
		set(xs, 1, 1, dimension);
	}

	void Model::DrawLines(float size)
	{
		render_type_ = GL_LINES;
		glLineWidth(size);
	}

	void Model::DrawPoints(float size)
	{
		render_type_ = GL_POINTS;
		glPointSize(size);
	}

	void Model::DrawTriangles(const std::vector<GLuint>& xs)
	{
		render_type_ = GL_TRIANGLES;
		indices_.clear();
		std::copy(xs.begin(), xs.end(), std::back_inserter(indices_));
	}

	void Model::SetMVP(glm::mat4 mvp)
	{
		glUniformMatrix4fv(matrix_id_, 1, GL_FALSE, &mvp[0][0]);
	}

	void Model::Render()
	{
		glBindVertexArray(vbo_);
		switch (render_type_)
		{
		case GL_POINTS:
			glDrawArrays(render_type_, 0, vertex_count_);
			break;

		case GL_LINES:
			glDrawArrays(render_type_, 0, vertex_count_);
			break;

		case GL_TRIANGLES:
			glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, &indices_[0]);
			break;

		default:
			throw std::runtime_error("invalid render type");
		}
	}

	void Model::set(const std::vector<GLfloat>& xs, int location, int bind_index, int dimension)
	{
		if (xs.empty())
		{
			return;
		}

		const int dimension_size = sizeof(GLfloat) * dimension;
		const int buffer_size = sizeof(GLfloat) * xs.size();
		GLuint id;

		glCreateBuffers(1, &id);
		glNamedBufferData(id, buffer_size, &xs[0], GL_STATIC_DRAW);
		glEnableVertexArrayAttrib(vbo_, location);
		glVertexArrayAttribFormat(vbo_, location, dimension, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vbo_, location, bind_index);
		glVertexArrayVertexBuffer(vbo_, bind_index, id, 0, dimension_size);
	}

	// --------------------------------------------------------------
	Camera::Camera(float radians, int width, int height)
	{
		projection_ = glm::perspective(glm::radians(radians), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
	}

	glm::mat4 Camera::LookAt(glm::vec3 position, glm::vec3 angle)
	{
		glm::mat4 view = glm::lookAt(
			position,          // ワールド空間のカメラ座標
			angle,             // カメラの向き
			glm::vec3(0, 1, 0) // 頭が上方向
		);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 mvp = projection_ * view * model;
		return mvp;
	}
}