package com.demo.lib3d;

/**
 * 3dlib native loader
 */
public class Jni3dlibNativeLoader {

    static {
        String NATIVE_LIBRARY_NAME = "lib3d";
        System.loadLibrary(NATIVE_LIBRARY_NAME);
    }

    public static native void Initialization(String resourceRoot);

    public static native void OnSurfaceCreated();

    public static native void OnSurfaceChanged(int width, int height);

    public static native void OnDrawFrame();

    public static native void DestroyInstance();

    public static native void OnTouchEvent(float deltaX, float deltaY);

    public static native void OnScale(float scaleFactor);

    public static native void OnParkingSlotTouch(float x, float y);
}
