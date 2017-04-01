#include "stdafx.h"
#include "RagdollController.h"


RagdollController::RagdollController(Mesh3d* m)
{
    mesh = m;
    auto pose = new SkeletonPose();
    mesh->getLodLevel(0)->skeletonPose = pose;
    auto skel = mesh->getLodLevel(0)->skeleton;
    auto mrstickobj = mesh->getLodLevel(0)->info3d;
    for (int i = 0; i < skel->positions.size(); i++) {

        glm::vec3 sum = skel->positions[i];
        float sumr = skel->radiues[i];

        pose->pose.push_back(glm::translate(glm::mat4(1), sum));
    }

    int vertid = 0;
    for (int z = 0; z < mrstickobj->vbo.size(); z += 12) {
        // if (skel->weights.size() <= vertid) break;
        glm::vec3 vert = glm::vec3(mrstickobj->vbo[z], mrstickobj->vbo[z + 1], mrstickobj->vbo[z + 2]);
        int i = -1;
        float dst = 99999.0f;

        for (int g = 0; g < skel->positions.size(); g++) {
            glm::vec3 pos = skel->positions[g];
            if (glm::distance(pos, vert) < dst) {
                dst = glm::distance(pos, vert);
                i = g;
            }
        }
        int iparent = skel->parents[i];
        int ichild1 = -1;
        int ichild2 = -1;
        int ichild3 = -1;
        int ichild4 = -1;
        for (int g = 0; g < skel->positions.size(); g++) if (skel->parents[g] == i) {
            if (ichild1 == -1) ichild1 = g;
            else if (ichild2 == -1) ichild2 = g;
            else if (ichild3 == -1) ichild3 = g;
            else if (ichild4 == -1) ichild4 = g;
        }
        int tab[6] = { i, iparent , ichild1 , ichild2, ichild3, ichild4 };
        vector<SkeletonBoneWeight> ws = {};
        skel->weights.push_back({});
        for (int g = 0; g < 6; g++) {
            int bone = tab[g];
            if (bone < 0) continue;
            glm::vec3 pos = skel->positions[bone];

            float w = 1.0 / ((glm::distance(vert, pos) + 1.0) * 1.0f);

            SkeletonBoneWeight skwk = SkeletonBoneWeight(bone, 1.0);
            skel->weights[vertid].push_back(skwk);
        }

        vertid++;

    }


    bonesBodies = vector<PhysicalBody*>{};
    bonesConstrs = vector<PhysicalConstraint*>{};
    for (int i = 0; i < skel->positions.size(); i++) {
        auto p = skel->positions[i];
        PhysicalBody* pb = Game::instance->world->physics->createBody(glm::min(0.05f, 1.0f), new TransformationManager(p + glm::vec3(0.0, 6.0, 0.0)), new btSphereShape(skel->radiues[i]));
        bonesBodies.push_back(pb);
    }
    for (int i = 0; i < skel->positions.size(); i++) {
        auto p = skel->positions[i];
        int closestindex = skel->parents[i];
        if (closestindex < 0) {
            bonesBodies[i]->enable();
            continue;
        }
        auto closest = skel->positions[skel->parents[i]];
        auto midway = (closest + p) * 0.5f;
        auto vector1_to_midway = (midway - p);
        auto vector2_to_midway = (midway - closest);
        btTransform frameInA, frameInB;
        frameInA = btTransform::getIdentity();
        frameInB = btTransform::getIdentity();

        frameInA.setOrigin(bulletify3(vector1_to_midway));
        frameInB.setOrigin(bulletify3(vector2_to_midway));
        auto ct1 = new btGeneric6DofSpring2Constraint(*(bonesBodies[i]->getRigidBody()), *(bonesBodies[closestindex]->getRigidBody()), frameInA, frameInB);
        PhysicalConstraint* pc = new PhysicalConstraint(ct1, bonesBodies[i], bonesBodies[closestindex]);
        ct1->setBreakingImpulseThreshold(999990.0f);
        float moverange = 0.2;
        ct1->setAngularLowerLimit(btVector3(-moverange, -moverange, -moverange));
        ct1->setAngularUpperLimit(btVector3(moverange, moverange, moverange));
        ct1->setLinearLowerLimit(btVector3(0, 0, 0));
        ct1->setLinearUpperLimit(btVector3(0, 0, 0));
        /*
        ct1->setBounce(0, 0);
        ct1->setBounce(1, 0);
        ct1->setBounce(2, 0);
        ct1->setStiffness(0, 0);
        ct1->setStiffness(1, 0);
        ct1->setStiffness(2, 0);*/

        bonesConstrs.push_back(pc);
        bonesBodies[i]->enable();
        bonesBodies[closestindex]->enable();
        pc->enable();
    }
    for (int i = 0; i < skel->positions.size(); i++) {
        for (int g = 0; g < skel->positions.size(); g++) {
            bonesBodies[i]->getRigidBody()->setIgnoreCollisionCheck(bonesBodies[g]->getRigidBody(), true);
        }
    }
}


RagdollController::~RagdollController()
{
}

void RagdollController::update()
{
    mesh->getInstance(0)->transformation->setPosition(bonesBodies[0]->transformation->getPosition());
    auto relpos = bonesBodies[0]->transformation->getPosition();
    for (int i = 0; i < bonesBodies.size(); i++) {

        auto wt = bonesBodies[i]->getTransformationManager()->getWorldTransform();
        mesh->getLodLevel(0)->skeletonPose->pose[i] = glm::translate(glm::mat4(), bonesBodies[i]->getTransformationManager()->getPosition() - relpos)
            * glm::mat4_cast(bonesBodies[i]->getTransformationManager()->getOrientation())
            * glm::translate(glm::mat4(), -mesh->getLodLevel(0)->skeleton->positions[i]);

        mesh->getLodLevel(0)->skeleton->poseNeedsUpdate = true;
         
    }
}
