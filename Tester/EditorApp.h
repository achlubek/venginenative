#pragma once
#include "AbstractApp.h" 

#include "Car.h"
#include "SimpleWalker.h"
class Chat;
class Sound3d;

#define EDITOR_MODE_IDLE 0
#define EDITOR_MODE_MOVE_CAMERA 1
#define EDITOR_MODE_PICKING 2
#define EDITOR_MODE_EDITING 3
#define EDITOR_MODE_WRITING_TEXT 4

class AnimationPlayer {
public:
    TransformationManager* trans;
    float * floats = nullptr;
    int framecount = 0;
    int frame = 0;
    int delay = 0;
    AnimationPlayer(TransformationManager* t, string file) {
        trans = t;
        unsigned char* bytes;
        int bytescount = Media::readBinary(file, &bytes);
        floats = (float*)bytes;
        framecount = bytescount / sizeof(float) / 16;

    }

    void nextFrame() {
        delay++;
        if (delay > 8) {
            delay = 0;
            int O = 0;
            glm::mat4 mat = glm::mat4(1);
            mat[0][0] = floats[frame * 16 + O++];
            mat[0][1] = floats[frame * 16 + O++];
            mat[0][2] = floats[frame * 16 + O++];
            mat[0][3] = floats[frame * 16 + O++];

            mat[1][0] = floats[frame * 16 + O++];
            mat[1][1] = floats[frame * 16 + O++];
            mat[1][2] = floats[frame * 16 + O++];
            mat[1][3] = floats[frame * 16 + O++];

            mat[2][0] = floats[frame * 16 + O++];
            mat[2][1] = floats[frame * 16 + O++];
            mat[2][2] = floats[frame * 16 + O++];
            mat[2][3] = floats[frame * 16 + O++];

            mat[3][0] = floats[frame * 16 + O++];
            mat[3][1] = floats[frame * 16 + O++];
            mat[3][2] = floats[frame * 16 + O++];
            mat[3][3] = floats[frame * 16 + O++];

            glm::vec4 multres = glm::vec4(0.0, 0.0, 0.0, 1.0) * mat;
            glm::vec3 pos = glm::vec3(multres);

            trans->setPosition(pos * 2.059f);
            trans->setOrientation(glm::quat(glm::mat3(mat)));
            frame++;
            if (frame >= framecount) frame = 0;
        }
    }

};

class EditorApp : public AbstractApp
{
public:
    EditorApp();
    ~EditorApp();
     
    virtual void initialize() override;
    virtual void onRenderFrame(float elapsed) override;
    virtual void onWindowResize(int width, int height) override;
    virtual void onKeyPress(int key) override;
    virtual void onKeyRelease(int key) override;
    virtual void onKeyRepeat(int key) override;
    virtual void onMouseDown(int button) override;
    virtual void onMouseUp(int button) override;
    virtual void onChar(unsigned int c) override;
    virtual void onBind() override;

    void switchMode(int mode);

    Physics* physics;

    Chat* chat;
    void onChatSendText(std::string s);
    SimpleWalker* walker;

    Mesh3d * debug_marker;
    std::vector<AnimationPlayer*> players;

    int currentMode = EDITOR_MODE_IDLE;
    int lastMode = EDITOR_MODE_IDLE;

    float yaw = 0.0f, pitch = 0.0f;
    double lastcx = 0.0f, lastcy = 0.0f;
    Camera* cam;
    Sound3d* testsound, *testsound2, *wind, *ocean1, *ocean2, *ocean3, *ocean4;

    float FOV = 10.0f;
    glm::quat backquat = glm::quat();
    glm::vec3 backvectorlast = glm::vec3();
    glm::vec3 lastpos = glm::vec3();
    bool cameraFollowCar = false;
    vector<Car *> car;
};

