#include "UIDisplay.hpp"
#include "Model.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <android/log.h>
UIDisplay::UIDisplay()
    : mVBO(0)
    , mVAO(0)
    , mTextureId(0)
    , mX(0)
    , mY(0)
    , mWidth(0)
    , mHeight(0)
    , mOnClickCallback(nullptr) {
}

UIDisplay::~UIDisplay() {
    if (mVBO) {
        glDeleteBuffers(1, &mVBO);
    }
    if (mVAO) {
        glDeleteVertexArrays(1, &mVAO);
    }
}

bool UIDisplay::init() {
    const std::string shaderDir = "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new";
    const std::string vertexPath = shaderDir + "/simple.vs";
    const std::string fragmentPath = shaderDir + "/simple.fs";

//      screenWidth = 1280;
//      screenHeight = 2856;
   screenWidth = 1080;
   screenHeight = 2400;

    aspect = screenWidth / screenHeight;
    
    mShader.FromSourceFile(vertexPath.c_str(), fragmentPath.c_str());

    createBuffers();

    const char* texturePath = "tingche.png";
    const std::string textureDir = "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/image/confirm";
    
    mTextureId = BaseLib::Model::TextureFromFile(texturePath, textureDir);

    return mTextureId != 0;
}

void UIDisplay::render(float x, float y, float width, float height) {
    // 存储当前UI元素的位置和大小，用于点击检测
    mX = x;
    mY = y;
    mWidth = width;
    mHeight = height;

    if (mShader.ID == 0 || mVAO == 0 || mVBO == 0 || mTextureId == 0) {
        return;
    }

    float ndcX = (2.0f * x) - 1.0f;
    float ndcY = 1.0f - (2.0f * y);
    float ndcWidth = 2.0f * width;
    float ndcHeight = 2.0f * height * aspect;

    float vertices[] = {
        ndcX,              ndcY,               0.0f,   0.0f, 0.0f,
        ndcX + ndcWidth,   ndcY,               0.0f,   1.0f, 0.0f,
        ndcX,              ndcY - ndcHeight,   0.0f,   0.0f, 1.0f,
        ndcX + ndcWidth,   ndcY - ndcHeight,   0.0f,   1.0f, 1.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureId);

    mShader.UseProgram();
    mShader.SetInt("textureDiffuse1", 0);

    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}



void UIDisplay::setOnClickCallback(const OnClickCallback& callback) {
    mOnClickCallback = callback;
}

bool UIDisplay::checkClick(float touchX, float touchY) {
    bool clicked = touchX >= mX && touchX <= (mX + mWidth) &&
                  touchY >= mY && touchY <= (mY + mHeight);

    if (clicked && mOnClickCallback) {
        mOnClickCallback();
    }

    return clicked;
}

void UIDisplay::createBuffers() {
    float vertices[] = {
        -1.0f,  1.0f, 0.0f,   0.0f, 0.0f,
         1.0f,  1.0f, 0.0f,   1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,   0.0f, 1.0f,
         1.0f, -1.0f, 0.0f,   1.0f, 1.0f
    };

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}