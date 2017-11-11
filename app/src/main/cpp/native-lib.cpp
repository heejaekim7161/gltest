#include "test_com_gltest_Renderer.h"

#include <android/asset_manager_jni.h>
#include "native_renderer.h"

extern "C"

JNIEXPORT jlong JNICALL Java_test_com_gltest_Renderer_nativeInit
(JNIEnv* env, jobject obj, jobject jasset_manager, jstring jfile_dir) {
  const char* file_dir = env->GetStringUTFChars(jfile_dir, NULL);
  Renderer* renderer = new Renderer(
      AAssetManager_fromJava(env, jasset_manager), std::string(file_dir));
  env->ReleaseStringUTFChars(jfile_dir, file_dir);
  return (long)renderer;
}

JNIEXPORT void JNICALL Java_test_com_gltest_Renderer_nativeOnSurfaceCreated
(JNIEnv* env, jobject obj, jlong ptr) {
  if (ptr) {
    Renderer* renderer = (Renderer*) ptr;
    renderer->Initialize();
  }
}

JNIEXPORT void JNICALL Java_test_com_gltest_Renderer_nativeDraw
(JNIEnv* env, jobject obj, jlong ptr) {
  if (ptr) {
    Renderer* renderer = (Renderer*) ptr;
    renderer->Draw();
  }
}