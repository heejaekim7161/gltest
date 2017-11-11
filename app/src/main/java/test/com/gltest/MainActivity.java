package test.com.gltest;

import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

public class MainActivity extends AppCompatActivity {

    private GLSurfaceView mSurfaceView;
    private Renderer mRenderer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mRenderer = new Renderer(this);
        mSurfaceView = (GLSurfaceView) findViewById(R.id.surfaceview);
        mSurfaceView.setPreserveEGLContextOnPause(true);
        mSurfaceView.setEGLContextClientVersion(2);
        mSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0); // Alpha used for plane blending.
        mSurfaceView.setRenderer(mRenderer);
        mSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mSurfaceView != null)
            mSurfaceView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mSurfaceView != null)
            mSurfaceView.onResume();
    }
}
