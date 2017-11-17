#include "native_renderer.h"
#include <iostream>
#include <fstream>

Renderer::Renderer(AAssetManager* asset_manager, std::string file_dir) {
  asset_manager_ = asset_manager;
  file_dir_ = file_dir;
  camera_ = new Camera(glm::tvec3<float, glm::highp>(0.0f, 0.0f, 4.0f));
}

Renderer::~Renderer() {
}

void Renderer::CheckGlError(char *func) {
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    LOGE("GlError: %s:: %d", func, error);
  }
}

void Renderer::ExtractShaderSource(std::string name, std::string& source) {
  std::string path = file_dir_ + "/" + name;
  FILE* file = fopen(path.c_str(), "rb");
  if (file)
    remove(path.c_str());

  AAsset* asset = AAssetManager_open(asset_manager_, name.c_str(),
                                     AASSET_MODE_STREAMING);
  if (asset) {
    file = fopen(path.c_str(), "w");
    int read = 0;
    char buffer[BUFSIZ];

    while ((read = AAsset_read(asset, buffer, BUFSIZ)) > 0) {
      fwrite(buffer, read, 1, file);
    }
    fclose(file);
    AAsset_close(asset);
  } else
    LOGE("ExtractAssetFile: asset is null");

  std::ifstream stream(path.c_str(), std::ios::in);
  if (stream.is_open()) {
    std::string line = "";
    while (getline(stream, line)) {
      source += line + "\n";
    }
    stream.close();
  } else
    LOGE("CompileShader: stream is null");
}

GLuint Renderer::CompileShader(std::string shader_file, GLenum type) {
  std::string source;
  ExtractShaderSource(shader_file, source);

  GLuint shader_id = glCreateShader(type);
  const char* shader_source = source.c_str();
  glShaderSource(shader_id, 1, &shader_source, NULL);
  glCompileShader(shader_id);

  GLint result = GL_FALSE;
  int log_len;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
  glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_len);
  if (result == 0) {
    LOGE("CompileShader: fail to compile: %s", shader_file.c_str());
    std::vector<char> msg(log_len + 1);
    glGetShaderInfoLog(shader_id, log_len, NULL, &msg[0]);
    LOGE("%s", &msg[0]);
    return 0;
  }
  return shader_id;
}

void Renderer::CreateProgram() {
  GLuint cube_vertex = CompileShader("cube_vertex.shader", GL_VERTEX_SHADER);
  GLuint cube_fragment = CompileShader("cube_fragment.shader", GL_FRAGMENT_SHADER);

  cube_program_ = glCreateProgram();
  glAttachShader(cube_program_, cube_vertex);
  glAttachShader(cube_program_, cube_fragment);
  glLinkProgram (cube_program_);
  glUseProgram(cube_program_);

  u_cube_model_ = glGetUniformLocation(cube_program_, "model");
  u_cube_view_ = glGetUniformLocation(cube_program_, "view");
  u_cube_projection_ = glGetUniformLocation(cube_program_, "projection");
  u_cube_object_color_ = glGetUniformLocation(cube_program_, "objectColor");
  u_cube_light_color_ = glGetUniformLocation(cube_program_, "lightColor");
  u_cube_light_pos_ = glGetUniformLocation(cube_program_, "lightPos");
  u_cube_view_pos_ = glGetUniformLocation(cube_program_, "viewPos");

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        (void*) 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        (void*) (3 * sizeof(GLfloat)));

  GLuint light_vertex = CompileShader("light_vertex.shader", GL_VERTEX_SHADER);
  GLuint light_fragment = CompileShader("light_fragment.shader",
                                        GL_FRAGMENT_SHADER);
  light_program_ = glCreateProgram();
  glAttachShader(light_program_, light_vertex);
  glAttachShader(light_program_, light_fragment);
  glLinkProgram (light_program_);
  glUseProgram(light_program_);

  u_light_model_ = glGetUniformLocation(light_program_, "model");
  u_light_view_ = glGetUniformLocation(light_program_, "view");
  u_light_projection_ = glGetUniformLocation(light_program_, "projection");

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        (void*) 0);
}

void Renderer::Initialize() {
  glEnable (GL_DEPTH_TEST);
  glGenBuffers(1, &vertex_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
  CreateProgram();
  CheckGlError("Initialize");

  LOGD("glsl version = %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void Renderer::OnSurfaceChanged(int width, int height) {
  width_ = width;
  height_ = height;
}

void Renderer::Draw() {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram (cube_program_);
  glUniform3f(u_cube_object_color_, 1.0f, 0.5f, 0.31f);
  glUniform3f(u_cube_light_color_, 1.0f, 1.0f, 1.0f);
  glUniform3fv(u_cube_light_pos_, 1, glm::value_ptr(light_pos_));
  glUniform3fv(u_cube_view_pos_, 1, glm::value_ptr(camera_->Position));

  glm::mat4 projection = glm::perspective(glm::radians(camera_->Zoom),
                                          (float) width_ / (float) height_,
                                          0.1f, 100.0f);
  glm::mat4 view = camera_->GetViewMatrix();
  glUniformMatrix4fv(u_cube_projection_, 1, GL_FALSE,
                     glm::value_ptr(projection));
  glUniformMatrix4fv(u_cube_view_, 1, GL_FALSE, glm::value_ptr(view));
  glm::mat4 model = glm::mat4();
  model = glm::translate(model, glm::vec3(0.0f, -0.5f, -4.0f));
  model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::scale(model, glm::vec3(1.5f));
  glUniformMatrix4fv(u_cube_model_, 1, GL_FALSE, glm::value_ptr(model));

  glDrawArrays(GL_TRIANGLES, 0, 36);
  CheckGlError("Draw: cube");

  glUseProgram (light_program_);
  glUniformMatrix4fv(u_light_projection_, 1, GL_FALSE,
                     glm::value_ptr(projection));
  glUniformMatrix4fv(u_light_view_, 1, GL_FALSE, glm::value_ptr(view));
  model = glm::mat4();
  model = glm::translate(model, light_pos_);
  model = glm::scale(model, glm::vec3(0.1f));
  glUniformMatrix4fv(u_light_model_, 1, GL_FALSE, glm::value_ptr(model));

  glDrawArrays(GL_TRIANGLES, 0, 36);
  CheckGlError("Draw: light");
}
