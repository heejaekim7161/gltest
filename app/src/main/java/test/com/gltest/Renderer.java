package test.com.gltest;

import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class Renderer implements GLSurfaceView.Renderer {

    private long mNativePtr;

    public Renderer(Context context) {
        mNativePtr = nativeInit(context.getAssets(), context.getFilesDir().toString());
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        GLES20.glClearColor(0, 0, 0, 0);
        nativeOnSurfaceCreated(mNativePtr);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        GLES20.glViewport(0, 0, width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        nativeDraw(mNativePtr);
    }

    static {
        System.loadLibrary("native-lib");
    }

    private native long nativeInit(AssetManager assetManager, String fileDir);

    private native void nativeOnSurfaceCreated(long mNativePtr);

    private native void nativeDraw(long mNativePtr);
}
