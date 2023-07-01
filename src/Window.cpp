#include "Window.h"

void Window::CreateWindow() {
  if (glfwInit() == -1) {
    printf("failed to init glfw \n");
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  m_GlfwWindow = glfwCreateWindow(1024, 1024, "Dwarf Font Generator", NULL, NULL);
  int width = 0;
  int height = 0;

  auto monitor = glfwGetPrimaryMonitor();
  auto mode = glfwGetVideoMode(monitor);

  if (m_GlfwWindow == NULL) {
    printf("Failed to initialize GLFW window \n");
    glfwTerminate();
  }

  glfwGetWindowSize(m_GlfwWindow, &width, &height);
  glfwSetWindowPos(m_GlfwWindow, (mode->width / 2) - (width / 2), (mode->height / 2) - (height / 2));
  // glfwMaximizeWindow(m_GlfwWindow);
  glfwMakeContextCurrent(m_GlfwWindow);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
  }
}