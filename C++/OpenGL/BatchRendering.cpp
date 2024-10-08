#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<iostream>
#include<string>
#include<vector>

const char* VERTEX_SHADER_SOURCE = "#version 330 core\n\
layout (location = 0) in vec3 aPos;\n\
layout (location = 1) in vec3 aColor;\n\
layout (location = 2) in float aId;\n\
uniform mat4 u_transform;\n\
\n\
out vec3 ourColor;\n\
void main() {\n\
	vec4 position = vec4(aPos, 1.0);\n\
	vec3 color = aColor;\n\
\n\
	if(aId == 1){\n\
		position = u_transform * position;\n\
		color = vec3(1.0, 0.0, 0.0);\n\
	}\n\
\n\
    gl_Position = position;\n\
    ourColor = color;\n\
}";

const char* FRAGMENT_SHADER_SOURCE = "#version 330 core\n\
in vec3 ourColor;\n\
out vec4 FragColor;\n\
void main() {\n\
    FragColor = vec4(ourColor, 1.0f);\n\
}";

struct BatchData {
	glm::vec3 position;
	glm::vec3 color;
	float id;
};

std::vector<BatchData> batchData = {
	// Positions                    Colors                       IDs
	{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.0f},
	{glm::vec3(1.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f},
	{glm::vec3(1.5f,  0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.0f},
	{glm::vec3(0.5f,  0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f},

	{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f},
	{glm::vec3(-1.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1.0f},
	{glm::vec3(-1.5f,  0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f},
	{glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f}
};

std::vector<unsigned int> i = {
	0, 1,  2,  2,  3, 0,
	4, 5,  6,  6,  7, 4,
	8, 9, 10, 10, 11, 8,
	12, 13, 14, 14, 15, 12
};

int main() {
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window
	GLFWwindow* window = glfwCreateWindow(800, 600, "Instance Rendering", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Create vertex and fragment shaders
	unsigned int vertexShader, fragmentShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader, 1, &VERTEX_SHADER_SOURCE, NULL);
	glShaderSource(fragmentShader, 1, &FRAGMENT_SHADER_SOURCE, NULL);
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	// Check for shader compilation errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Create shader program
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Check for program linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glm::mat4 modelMatrices[2];
	modelMatrices[0] = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
	modelMatrices[1] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind VAO
	glBindVertexArray(VAO);

	/////////////////////////////////////////////////////
	// Bind VBO, copy vertices, set attribute pointers //
	/////////////////////////////////////////////////////
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BatchData) * 12, nullptr, GL_DYNAMIC_DRAW); // Allocate memory

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BatchData), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BatchData), (void*)(sizeof(glm::vec3)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(BatchData), (void*)(2 * sizeof(glm::vec3)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/////////////////////////////////////////////////////
	// Bind EBO, copy indices						   //
	/////////////////////////////////////////////////////
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * i.size(), (unsigned int*)&i[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	float xPos = 0.0f;
	float velocity = 0.0002f;
	bool isBouncingBack = false;

	// Main rendering loop
	while (!glfwWindowShouldClose(window)) {
		// Clear screen
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);

		int u_transform = glGetUniformLocation(shaderProgram, "u_transform");
		glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(xPos, 0.0f, 0.0f))));

		if (isBouncingBack) {
			xPos -= velocity;
		}
		else
		{
			xPos += velocity;
		}

		if (xPos > 1) {
			isBouncingBack = true;
		}

		if (xPos < 0) {
			isBouncingBack = false;
		}

		// Inital data.
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(BatchData) * 4, sizeof(BatchData)* batchData.size(), &batchData[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		/* 
		* Splitted draw calls like this allows to draw more mesh without needing to resize/realloc VBO
		* Just by using a fixed size CPU side buffer, send subData and draw, add new mesh at some position in that buffer, 
		* resend subData and redraw, all in one frame.
		* 
		* With that, the fixed VBO size is a "MAX_VERTEX_COUNT", which in turn is the size of each frame.
		*/

		BatchData data[4];

		data[0] = { glm::vec3(0.0f, -0.3f, 0.0f), glm::vec3(0.0f, 1.0f, 3.0f), 0.0f };
		data[1] = { glm::vec3(-0.9f, -0.3f, 0.0f), glm::vec3(0.0f, 1.0f, 3.0f), 0.0f };
		data[2] = { glm::vec3(-0.9f, 0.3f, 0.0f), glm::vec3(0.0f, 1.0f, 3.0f), 0.0f };
		data[3] = { glm::vec3(0.0f, 0.3f, 0.0f), glm::vec3(0.0f, 1.0f, 3.0f), 0.0f };

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(BatchData) * 4, &data[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, i.size(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		data[0] = { glm::vec3(0.0f, -0.2f, 0.0f), glm::vec3(0.3f, 1.0f, 0.0f), 0.0f };
		data[1] = { glm::vec3(-0.5f, -0.2f, 0.0f), glm::vec3(0.3f, 1.0f, 0.0f), 0.0f };
		data[2] = { glm::vec3(-0.5f,  0.2f, 0.0f), glm::vec3(0.3f, 1.0f, 0.0f), 0.0f };
		data[3] = { glm::vec3(0.0f,  0.2f, 0.0f), glm::vec3(0.3f, 1.0f, 0.0f), 0.0f };

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(BatchData) * 4, &data[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, i.size(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		// Swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();
	return 0;
}