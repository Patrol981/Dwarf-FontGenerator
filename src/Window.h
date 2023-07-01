#pragma once
#include <iostream>
#include "GLAD//glad.h"
#include "GLFW/glfw3.h"

class Window {
public:
	void CreateWindow();
	GLFWwindow* m_GlfwWindow;
};