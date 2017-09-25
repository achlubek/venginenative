#include "stdafx.h"
#include "AnimationPlayer.h"


AnimationPlayer::AnimationPlayer(TransformationManager* t, float * buffer, int ioffset, int iframecount) {
    trans = t;
    floats = buffer;
    framecount = iframecount;
    offset = ioffset;
}


AnimationPlayer::~AnimationPlayer()
{
}


void AnimationPlayer::nextFrame()
{
    delay++;
    if (delay > 6) {
        delay = 0;
        int O = 0;
        glm::mat4 mat = glm::mat4(1);
        mat[0][0] = floats[offset + frame * 16 + O++];
        mat[0][1] = floats[offset + frame * 16 + O++];
        mat[0][2] = floats[offset + frame * 16 + O++];
        mat[0][3] = floats[offset + frame * 16 + O++];
        mat[1][0] = floats[offset + frame * 16 + O++];
        mat[1][1] = floats[offset + frame * 16 + O++];
        mat[1][2] = floats[offset + frame * 16 + O++];
        mat[1][3] = floats[offset + frame * 16 + O++];
        mat[2][0] = floats[offset + frame * 16 + O++];
        mat[2][1] = floats[offset + frame * 16 + O++];
        mat[2][2] = floats[offset + frame * 16 + O++];
        mat[2][3] = floats[offset + frame * 16 + O++];
        mat[3][0] = floats[offset + frame * 16 + O++];
        mat[3][1] = floats[offset + frame * 16 + O++];
        mat[3][2] = floats[offset + frame * 16 + O++];
        mat[3][3] = floats[offset + frame * 16 + O++];

        glm::vec4 multres = glm::vec4(0.0, 0.0, 0.0, 1.0) * mat;
        glm::vec3 pos = glm::vec3(multres);

        trans->setPosition(pos * 3.059f);
        trans->setOrientation(glm::quat(glm::mat3(mat)));
        frame++;
        if (frame >= framecount) frame = 0;
    }
}
