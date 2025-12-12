package com.demo.lib3d;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.os.Environment;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLRender3D implements GLSurfaceView.Renderer  {
    private static final String TAG = "GLRender3D";

    private Context context;

    public GLRender3D(Context context) {
        this.context = context;
    }

    public void initResource(String resourceRoot) {
        Jni3dlibNativeLoader.Initialization(resourceRoot);
    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
//        Log.d(TAG, "onSurfaceCreated");
        Jni3dlibNativeLoader.DestroyInstance();

        Jni3dlibNativeLoader.OnSurfaceCreated();

        String resourceDir = this.context.getExternalFilesDir(Environment.DIRECTORY_DOWNLOADS).getAbsolutePath();
//        Log.d(TAG, "resourceDir:"+ resourceDir);
        // copy assets to sdcard
        AssetsUtils.copyAssetsDirToSDCard(context, "avm_resource", resourceDir);

        String resourceRoot = resourceDir + "/avm_resource/image/carmodel3d/junlian/";

//        resourceRoot += "E245";
//        resourceRoot += "E245_J1_LHD";
//        resourceRoot += "E245_J1_RHD";
//        resourceRoot += "E245_J1_PROTON";
        // resourceRoot += "E335";
        
        resourceRoot += "FX11_A2";

//        Log.d(TAG, "resourceRoot:"+ resourceRoot);
        long currentTimeMillis = System.currentTimeMillis();
        this.initResource(resourceRoot);
//        Log.d(TAG, "duration:" + (System.currentTimeMillis() - currentTimeMillis) + "(ms)");
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int i, int i1) {
//        Log.d(TAG, "onSurfaceChanged");
        String vendor = gl10.glGetString(GL10.GL_VENDOR);
//        Log.d(TAG, "GL_VENDOR:" + vendor);

        String version = gl10.glGetString(GL10.GL_VERSION);
//        Log.d(TAG, "GL_VERSION:" + version);

        String renderer = gl10.glGetString(GL10.GL_RENDERER);
//        Log.d(TAG, "GL_RENDERER:" + renderer);

        String shaderLanguageVersion = gl10.glGetString(GLES20.GL_SHADING_LANGUAGE_VERSION);
//        Log.d(TAG, "GL_SHADING_LANGUAGE_VERSION:" + shaderLanguageVersion);


        int[] maxTexSize = new int[1];
        gl10.glGetIntegerv(gl10.GL_MAX_TEXTURE_SIZE, maxTexSize, 0);
//        Log.d(TAG, "GL_MAX_TEXTURE_SIZE:" + maxTexSize[0]);

        int[] maxTexUnits = new int[1];
        gl10.glGetIntegerv(GLES30.GL_MAX_TEXTURE_IMAGE_UNITS, maxTexUnits, 0);
//        Log.d(TAG, "GL_MAX_TEXTURE_IMAGE_UNITS:" + maxTexUnits[0]);

        int[] maxArrayTexLayers = new int[1];
        gl10.glGetIntegerv(GLES30.GL_MAX_ARRAY_TEXTURE_LAYERS, maxArrayTexLayers, 0);
//        Log.d(TAG, "GL_MAX_ARRAY_TEXTURE_LAYERS:" + maxArrayTexLayers[0]);

        String extensions = gl10.glGetString(GL10.GL_EXTENSIONS);
//        Log.d(TAG, "GL_EXTENSIONS:" + extensions);
        Jni3dlibNativeLoader.OnSurfaceChanged(i, i1);
//        Log.d(TAG, "i="+i + " i1="+i1);
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
//        Log.d(TAG, "onDrawFrame");
        Jni3dlibNativeLoader.OnDrawFrame();
    }
}
