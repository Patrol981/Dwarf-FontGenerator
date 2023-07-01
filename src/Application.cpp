#include "Application.h"

bool wireframeMode = 0;
bool saveToPng = 0;
int saveWidth = 1024;
int saveHeight = 1024;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  switch (action) {
  case GLFW_PRESS:
    if (key == GLFW_KEY_GRAVE_ACCENT) {
      if (wireframeMode == 1) {
        wireframeMode = 0;
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }
      else {
        wireframeMode = 1;
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }
    }

    if (key == GLFW_KEY_ENTER) {
      saveToPng = 1;
    }

    if (key == GLFW_KEY_ESCAPE) {
      glfwSetWindowShouldClose(window, 1);
    }
    break;
  default:
    break;
  }
}

void Application::Run() {
  m_Window = std::make_unique<Window>();
  m_RenderSettings = std::make_unique<RenderSettings>();
  m_FontManager = std::make_unique<FontManager>();

  m_Window->CreateWindow();
  m_FontManager->Init();

  m_shader = Shader("font.vert", "font.frag");
  m_screenShader = Shader("framebuffer.vert", "framebuffer.frag");

  glfwSetKeyCallback(m_Window->m_GlfwWindow, key_callback);

  glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(saveWidth), 0.0f, static_cast<float>(saveHeight));
  m_shader.use();
  glUniformMatrix4fv(glGetUniformLocation(m_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

  m_screenShader.use();
  m_screenShader.setInt("screenTexture", 0);

  // init font
  this->InitFont();

  // init quad
  this->InitQuad();

  // init framebuffer
  this->InitFramebuffer();

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  this->InitImGui();

  this->RenderLoop();

  m_FontManager->Destroy();
  glfwDestroyWindow(m_Window->m_GlfwWindow);
  glfwTerminate();
}

void Application::InitFont() {
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Application::InitQuad() {
  glGenVertexArrays(1, &m_quadVAO);
  glGenBuffers(1, &m_quadVBO);
  glBindVertexArray(m_quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(m_quadVertices), &m_quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void Application::InitFramebuffer() {
  glGenFramebuffers(1, &m_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  glGenTextures(1, &m_fboTexture);
  glBindTexture(GL_TEXTURE_2D, m_fboTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, saveWidth, saveHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTexture, 0);

  // render buffer
  glGenRenderbuffers(1, &m_renderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, saveWidth, saveHeight);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderBuffer);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Application::InitImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(m_Window->m_GlfwWindow, true);
  ImGui_ImplOpenGL3_Init(m_glslVersion);
  m_io = ImGui::GetIO();
  m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
}

void Application::RenderLoop() {
  while (!glfwWindowShouldClose(m_Window->m_GlfwWindow)) {
    if (m_atlasRendered) {
      glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
      glEnable(GL_DEPTH_TEST);
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shader.use();
    RenderAtlas(m_shader, 0, m_RenderSettings->characterSize[0] * 9.75f);

    if (!m_atlasRendered) {
      RenderUIInit();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    if (m_atlasRendered) {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glDisable(GL_DEPTH_TEST);

      glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      m_screenShader.use();
      glBindVertexArray(m_quadVAO);
      glBindTexture(GL_TEXTURE_2D, m_fboTexture);
      glDrawArrays(GL_TRIANGLES, 0, 6);

      if (saveToPng) {
        WriteToPng(saveWidth, saveHeight);
        saveToPng = 0;
      }
    }

    glfwSwapBuffers(m_Window->m_GlfwWindow);
    glfwPollEvents();
  }
}

void Application::RenderUIInit() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  this->RenderUI();

  ImGui::Render();
}

void Application::RenderUI() {
  auto fonts = m_FontManager->GetAvailableFonts();
  static const char* currentItem = NULL;
  int test = 0;

  ImGui::Begin("Render Settings");

  ImGui::Text("Texture Data");
  ImGui::InputInt2("Size", m_RenderSettings->size);

  if (ImGui::BeginCombo("Font", fonts[m_RenderSettings->selectedOption].c_str())) {
    for (int i = 0; i < fonts.size(); ++i) {
      bool isSelected = (m_RenderSettings->selectedOption == i);
      if (ImGui::Selectable(fonts[i].c_str(), isSelected))
        m_RenderSettings->selectedOption = i;

      if (isSelected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }

  ImGui::SliderInt("Characters per row", &m_RenderSettings->charactersPerRow, 1, 42);

  if (ImGui::Button("Apply")) {
    this->ApplyRenderData();
  }

  ImGui::End();
}

void Application::RenderAtlas(Shader& shader, float x, float y) {
  shader.use();
  auto color = glm::vec3(1.0, 1.0f, 1.0f);
  glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(m_vao);

  float startingX = x;

  for (unsigned char c = 32; c < 128; c++) {
    Character character = m_FontManager->GetCharacters()[c];

    float xpos = x + character.bearing.x * 1.0f;
    float ypos = y - (character.size.y - character.bearing.y) * 1.0f;

    float w = character.size.x * 1.0f;
    float h = character.size.y * 1.0f;
    // update VBO for each character
    float vertices[6][4] = {
        { xpos,     ypos + h,   0.0f, 0.0f },
        { xpos,     ypos,       0.0f, 1.0f },
        { xpos + w, ypos,       1.0f, 1.0f },

        { xpos,     ypos + h,   0.0f, 0.0f },
        { xpos + w, ypos,       1.0f, 1.0f },
        { xpos + w, ypos + h,   1.0f, 0.0f }
    };


    // render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, character.textureID);
    // update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);

    x += m_RenderSettings->characterSize[1];
    if (x > m_RenderSettings->size[1]) {
      x = startingX;
      y -= m_RenderSettings->characterSize[1];
    }

    // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
    // x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Application::ApplyRenderData() {
  m_FontManager->LoadFont(m_FontManager->GetAvailableFonts()[m_RenderSettings->selectedOption], m_RenderSettings.get());
  m_atlasRendered = true;
}

void Application::WriteToPng(int width, int height) {
  std::cout << "Saving to png..." << std::endl;

  m_pixels.resize(4 * width * height);
  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &m_pixels[0]);

  std::vector<unsigned char> flippedPixels(4 * width * height);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int srcIndex = 4 * (y * width + x);
      int dstIndex = 4 * ((height - 1 - y) * width + x);
      flippedPixels[dstIndex] = m_pixels[srcIndex];
      flippedPixels[dstIndex + 1] = m_pixels[srcIndex + 1];
      flippedPixels[dstIndex + 2] = m_pixels[srcIndex + 2];
      flippedPixels[dstIndex + 3] = m_pixels[srcIndex + 3];
    }
  }

  if (stbi_write_png("atlas.png", width, height, 4, flippedPixels.data(), width * 4) == 0) {
    std::cerr << "Error writing PNG file: " << "atlas.png" << std::endl;
  }
}

int main() {
  auto app = std::make_unique<Application>();
  app->Run();
  return 0;
}