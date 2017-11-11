#ifndef GLTEST_NATIVE_RENDERER_H
#define GLTEST_NATIVE_RENDERER_H

#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#include <string>

#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "log.h"

const GLfloat VERTICES[] = { -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,  // Top-left
    0.5f, 0.5f, 0.0f, 1.0f, 0.0f,  // Top-right
    0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  // Bottom-right
    -0.5f, -0.5f, 1.0f, 1.0f, 1.0f  // Bottom-left
    };

const GLuint INDICES[] = { 0, 1, 2, 2, 3, 0 };

const std::string VERTEX_SHADER = "vertex.shader";
const std::string FRAGMENT_SHADER = "fragment.shader";

class Renderer {
 public:
  Renderer(AAssetManager* asset_manager, std::string file_dir);
  ~Renderer();

  void Initialize();
  void Draw();
 private:
  void CheckGlError(char* func);
  void ExtractShaderSource(std::string name, std::string& source);
  GLuint CompileShader(std::string shader_file, GLenum type);
  void CreateProgram();

  GLuint vertex_buffer_, index_buffer_;
  GLuint program_;
  GLint a_position_, a_color_;

  AAssetManager* asset_manager_;
  std::string file_dir_;
};

#endif //GLTEST_NATIVE_RENDERER_H
