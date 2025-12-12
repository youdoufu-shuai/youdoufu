#include <jni.h>
#include <string>

#include <RenderContext3D.h>

#define NATIVE_WRAPPER_BASE(java_class, native_function) Java_##java_class##_##native_function

/* @brief specified java native load class, replace . with _ */
#define NATIVE_WRAPPER(native_function) NATIVE_WRAPPER_BASE(com_demo_lib3d_Jni3dlibNativeLoader, native_function)

bool funcLog(std::string message)
{
	LOGGER_I("funcLog: %s", message.c_str());

    return true;
}


/** @brief prototype of void Initialization(std::string) */
extern "C" JNIEXPORT void JNICALL NATIVE_WRAPPER(Initialization) (
        JNIEnv* env,
        jobject /* this */,
        jstring resourceRoot) {
    const char *cstr = env->GetStringUTFChars(resourceRoot, NULL);
    std::string str = std::string(cstr);
    env->ReleaseStringUTFChars(resourceRoot, cstr);

    /** @brief 用例 */
    auto lambdaLog = [](std::string info) -> bool {
		LOGGER_I("lambdaLog: %s", info.c_str());
		return true;
	};

    RenderContext3D::GetInstance()->Init(str);
//    RenderContext3D::GetInstance()->carModel3D->Initialization(str, funcLog);
}


extern "C" JNIEXPORT void JNICALL NATIVE_WRAPPER(OnSurfaceCreated)(JNIEnv *env, jobject instance)
{
    RenderContext3D::GetInstance()->OnSurfaceCreated();
}


extern "C" JNIEXPORT void JNICALL NATIVE_WRAPPER(OnSurfaceChanged)
        (JNIEnv *env, jobject instance, jint width, jint height)
{
    RenderContext3D::GetInstance()->OnSurfaceChanged(width, height);

}


extern "C" JNIEXPORT void JNICALL NATIVE_WRAPPER(OnDrawFrame)(JNIEnv *env, jobject instance)
{
    RenderContext3D::GetInstance()->OnDrawFrame();
}

extern "C" JNIEXPORT void JNICALL NATIVE_WRAPPER(DestroyInstance)(JNIEnv *env, jobject instance)
{
    RenderContext3D::GetInstance()->DestroyInstance();
}

extern "C" JNIEXPORT void JNICALL NATIVE_WRAPPER(OnTouchEvent)(JNIEnv *env, jobject instance, jfloat deltaX, jfloat deltaY)
{
    RenderContext3D::GetInstance()->OnTouchEvent(deltaX, deltaY);
}

extern "C" JNIEXPORT void JNICALL NATIVE_WRAPPER(OnScale)(JNIEnv *env, jobject instance, jfloat scaleFactor)
{
    RenderContext3D::GetInstance()->OnScale(scaleFactor);
}

extern "C" JNIEXPORT void JNICALL NATIVE_WRAPPER(OnParkingSlotTouch)(JNIEnv *env, jobject instance, jfloat x, jfloat y)
{
    RenderContext3D::GetInstance()->Touch(x, y);
}
