#include "native_renderer.h"
#include <iostream>
#include <fstream>
#include <vector>

Renderer::Renderer(AAssetManager* asset_manager, std::string file_dir) {
  asset_manager_ = asset_manager;
  file_dir_ = file_dir;
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
      source += "\n" + line;
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
    LOGE("CompileShader: fail to compile");
    std::vector<char> msg(log_len + 1);
    glGetShaderInfoLog(shader_id, log_len, NULL, &msg[0]);
    LOGE("%s", &msg[0]);
    return 0;
  }
  return shader_id;
}

void Renderer::CreateProgram() {
  GLuint vertex_shader = CompileShader(VERTEX_SHADER, GL_VERTEX_SHADER);
  GLuint fragment_shader = CompileShader(FRAGMENT_SHADER, GL_FRAGMENT_SHADER);

  program_ = glCreateProgram();
  glAttachShader(program_, vertex_shader);
  glAttachShader(program_, fragment_shader);
  glLinkProgram (program_);
  glUseProgram(program_);

  a_position_ = glGetAttribLocation(program_, "a_Position");
  glEnableVertexAttribArray (a_position_);
  glVertexAttribPointer(a_position_, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                        0);
  a_color_ = glGetAttribLocation(program_, "a_Color");
  glEnableVertexAttribArray (a_color_);
  glVertexAttribPointer(a_color_, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                        (void*) (2 * sizeof(GLfloat)));
}

void Renderer::Initialize() {
  glGenBuffers(1, &vertex_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

  glGenBuffers(1, &index_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES,
               GL_STATIC_DRAW);

  CreateProgram();
  CheckGlError("Initialize");
}

void Renderer::Draw() {
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  CheckGlError("Draw");
}
