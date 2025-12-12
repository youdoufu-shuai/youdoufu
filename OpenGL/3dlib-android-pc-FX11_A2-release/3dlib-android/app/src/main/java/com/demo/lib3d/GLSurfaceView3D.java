package com.demo.lib3d;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;

import androidx.annotation.NonNull;

public class GLSurfaceView3D extends GLSurfaceView implements ScaleGestureDetector.OnScaleGestureListener {

    GLRender3D glender3D;

    private GestureDetector mGestureDetector;
    private ScaleGestureDetector mScaleGestureDetector;

    public float lastX;
    public float lastY;
    public boolean isDragging = false;
    public boolean isScaling = false;
    public boolean hasScrolled = false;

    public GLSurfaceView3D(Context context) {
        this(context, null);
    }

    public GLSurfaceView3D(Context context, AttributeSet attrs) {
        this(context, attrs, null);
    }

    public GLSurfaceView3D(Context context, AttributeSet attrs, GLRender3D glender3D) {
        super(context, attrs);
        this.glender3D = glender3D;

        setEGLConfigChooser(8, 8, 8, 8, 16, 8);
        this.setEGLContextClientVersion(3);

        setRenderer(glender3D);
        setRenderMode(RENDERMODE_CONTINUOUSLY);

        mScaleGestureDetector = new ScaleGestureDetector(context, this);
        mGestureDetector = new GestureDetector(context, new GestureDetector.SimpleOnGestureListener() {
            @Override
            public boolean onSingleTapUp(MotionEvent e) {
                float x = e.getX();
                float y = e.getY();
                Jni3dlibNativeLoader.OnParkingSlotTouch(x, y);
                return true;
            }

            @Override
            public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
                hasScrolled = true;
                return false;
            }
        });
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        mGestureDetector.onTouchEvent(event);
        mScaleGestureDetector.onTouchEvent(event);

        float x = event.getX();
        float y = event.getY();

        int action = event.getActionMasked();
        switch (action) {
            case MotionEvent.ACTION_DOWN:
                lastX = x;
                lastY = y;
                isDragging = true;
                hasScrolled = false;
                break;

            case MotionEvent.ACTION_POINTER_DOWN:
                isDragging = false;
                break;

            case MotionEvent.ACTION_MOVE:
                if (isDragging && !isScaling && event.getPointerCount() == 1 && hasScrolled) {
                    float deltaX = x - lastX;
                    float deltaY = y - lastY;
                    Jni3dlibNativeLoader.OnTouchEvent(deltaX, deltaY);
                    lastX = x;
                    lastY = y;
                }
                break;

            case MotionEvent.ACTION_POINTER_UP:
                if (event.getPointerCount() <= 2) {
                    isDragging = false;
                }
                break;

            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
                isDragging = false;
                break;
        }

        return true;
    }

    @Override
    public boolean onScale(@NonNull ScaleGestureDetector scaleGestureDetector) {
        float scaleFactor = scaleGestureDetector.getScaleFactor();
        Jni3dlibNativeLoader.OnScale(scaleFactor);
        return true;
    }

    @Override
    public boolean onScaleBegin(@NonNull ScaleGestureDetector scaleGestureDetector) {
        isScaling = true;
        return true;
    }

    @Override
    public void onScaleEnd(@NonNull ScaleGestureDetector scaleGestureDetector) {
        isScaling = false;
        isDragging = false;
    }
}
