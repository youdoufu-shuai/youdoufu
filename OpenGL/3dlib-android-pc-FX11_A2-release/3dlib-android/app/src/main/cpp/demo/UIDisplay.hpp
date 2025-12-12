#ifndef UIDISPLAY_HPP
#define UIDISPLAY_HPP

#include <GLES3/gl3.h>
#include <string>
#include <memory>
#include "Shader.hpp"
#include "Model.hpp"

class UIDisplay {
public:
    UIDisplay();
    ~UIDisplay();

    bool init();
    
    void render(float x, float y, float width, float height);

    using OnClickCallback = std::function<void()>;
    void setOnClickCallback(const OnClickCallback& callback);

    bool checkClick(float touchX, float touchY);

private:
    float screenWidth;
    float screenHeight;
    float aspect;

    float mX;
    float mY;
    float mWidth;
    float mHeight;

    OnClickCallback mOnClickCallback;

    void createBuffers();

    BaseLib::Shader mShader;
    GLuint mVBO;
    GLuint mVAO;
    GLuint mTextureId;
};

#endif