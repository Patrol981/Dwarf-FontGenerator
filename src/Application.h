#pragma once

#include <memory>

#define GLFW_INCLUDE_NONE
#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

#include "vendor/imgui/imgui.h";
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Window.h"
#include "RenderSettings.h"
#include "FontManager.h"
#include "Shader.h"

class Application {
public:
  void Run();

private:
  void RenderUI();
  void RenderUIInit();
  void RenderLoop();

  void InitFont();
  void InitQuad();
  void InitFramebuffer();
  void InitImGui();

  void RenderAtlas(Shader& shader, float x, float y);

  void ApplyRenderData();
  void WriteToPng(int width, int height);

  const char* m_glslVersion = "#version 430";
  ImGuiIO m_io;

  unsigned int m_vao, m_vbo, m_fbo;
  unsigned int m_fboTexture;
  unsigned int m_renderBuffer;
  unsigned int m_quadVAO, m_quadVBO;

  Shader m_shader;
  Shader m_screenShader;

  float m_quadVertices[24] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };

  bool m_atlasRendered = false;
  std::vector<GLubyte> m_pixels = {};

  std::unique_ptr<Window> m_Window;
  std::unique_ptr<RenderSettings> m_RenderSettings;
  std::unique_ptr<FontManager> m_FontManager;
};