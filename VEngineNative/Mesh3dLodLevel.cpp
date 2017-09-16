#include "stdafx.h"
#include "Application.h"
#include "Mesh3dLodLevel.h"

using namespace glm;

Mesh3dLodLevel::Mesh3dLodLevel(Object3dInfo *info, Material *imaterial, float distancestart, float distanceend)
{
    info3d = info;
    material = imaterial;
    distanceStart = distancestart;
    distanceEnd = distanceend;
    initialize();
}

Mesh3dLodLevel::Mesh3dLodLevel(Object3dInfo *info, Material *imaterial)
{
    info3d = info;
    material = imaterial;
    distanceStart = 0;
    distanceEnd = 99999.0;
    initialize();
}

Mesh3dLodLevel::Mesh3dLodLevel()
{
    info3d = nullptr;
    material = nullptr;
    distanceStart = 0;
    distanceEnd = 99999.0;
    initialize();
}

void Mesh3dLodLevel::initialize()
{
    int kb1 = 1024;
    int mb1 = kb1 * 1024;
    modelInfosBuffer = new VulkanGenericBuffer(Application::instance->vulkan, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 64 * kb1);

    id = Application::instance->getNextId();
    Application::instance->registerId(id, this);
    int i = 0;

    descriptorSet = Application::instance->meshModelsDataLayout->generateDescriptorSet();
    descriptorSet->bindStorageBuffer(0, modelInfosBuffer);
    descriptorSet->update();
}

Mesh3dLodLevel::~Mesh3dLodLevel()
{
}
#define vcasti(a) (*((float*)&a))
void Mesh3dLodLevel::draw(VulkanRenderStage* stage, const Mesh3d* mesh)
{
    if (instancesFiltered == 0) return;

    if (!mesh->visible) return;
    if (!visible) return;
    stage->drawMesh(info3d, { stage->meshSharedSet, descriptorSet, material->descriptorSet }, instancesFiltered);

}

void Mesh3dLodLevel::updateBuffer(const Mesh3d* mesh, glm::mat4 transform, const vector<Mesh3dInstance*> &instances)
{
    // next frame it runs current buffer to 1, next buffer will be 

    // buffers into nextbuffer, draws current
    vec3 cameraPos = Application::instance->mainDisplayCamera->transformation->getPosition();
    vector<Mesh3dInstance*> filtered;
    newids.clear();
    bool changed = false;
    for (int i = 0; i < instances.size(); i++) {
        float dst = distance(cameraPos, instances[i]->transformation->getPosition());
        if (instances[i]->visible && dst >= distanceStart && dst < distanceEnd && checkIntersection(transform, instances[i])) {
            filtered.push_back(instances[i]);
            newids.push_back(instances[i]->id);
            if (instances[i]->transformation->needsUpdate) changed = true;
        }
    }
    if (newids.size() != lastIdMap.size()) {
        changed = true;
    }
    else {
        for (int i = 0; i < newids.size(); i++) {
            if (newids[i] != lastIdMap[i]) {
                changed = true;
                break;
            }
        }
    }
    lastIdMap.clear();
    lastIdMap = newids;

    /*layout rotation f4 translation f3+1 scale f3+1 =>> 12 floats*/
    // Urgently transfrom it into permament mapped buffer 

    int fint = filtered.size();
    instancesFiltered = fint;

    changed = true;
    if (changed) {

        if (fint > 0) {
            VulkanBinaryBufferBuilder bb = VulkanBinaryBufferBuilder();
            for (unsigned int i = 0; i < fint; i++) {
                TransformationManager *mgr = filtered[i]->transformation;
                glm::mat4 trans = transform * mgr->getWorldTransform();

                bb.emplaceGeneric((unsigned char*)&trans, sizeof(glm::mat4));
                bb.emplaceInt32(filtered[i]->id);
                bb.emplaceInt32(filtered[i]->id);
                bb.emplaceInt32(filtered[i]->id);
                bb.emplaceInt32(filtered[i]->id);
            }
            void* modelbufferpt = nullptr;
            modelInfosBuffer->map(0, bb.buffer.size(), &modelbufferpt);

            memcpy(modelbufferpt, bb.getPointer(), bb.buffer.size());

            modelInfosBuffer->unmap();
        }
    }

    material->updateBuffer();
}

float rsi2(vec3 ro, vec3 rd, vec3 sp, float sr)
{
    vec3 oc = ro - sp;
    float b = 2.0 * dot(rd, oc);
    float c = dot(oc, oc) - sr*sr;
    float disc = sqrt(b * b - 4.0 * c);
    vec2 ex = vec2(-b - disc, -b + disc) / 2.0f;
    return ex.x > 0.0 ? ex.x : ex.y;
}

vec4 projectvdao2(mat4 mat, vec3 pos) {
    return (mat * vec4(pos, 1.0));
}
#undef max

bool Mesh3dLodLevel::checkIntersection(glm::mat4 transform, Mesh3dInstance * instance)
{
    return true;//debug
    if (ignoreFrustumCulling) return true;
    auto size = instance->transformation->getSize();
    float radius = info3d->radius * glm::max(glm::max(size.x, size.y), size.z);

    vec3 center = instance->transformation->getPosition();// +0.5f * (info3d->aabbmax + info3d->aabbmin);
    vec3 camerapos = Application::instance->mainDisplayCamera->transformation->getPosition();

    float dst = distance(camerapos, center);

    if (dst < radius) {
        //  printf("SKIP");
        return true;
    }

    // i cannot find it on net so i craft my own cone/sphere test

    vec3 rdirC = Application::instance->mainDisplayCamera->cone->reconstructDirection(glm::vec2(0.5));
    vec3 helper = camerapos + rdirC * dst;
    vec3 helpdir = normalize(helper - center);
    vec3 newpos = center + helpdir * radius;

    vec4 prraw = projectvdao2(Application::instance->viewProjMatrix, newpos);
    vec2 pr = (vec2(prraw.x, prraw.y) / prraw.w) * 0.5f + 0.5f;
    bool sphereintersect = true;
    if (pr.x < 0.0 || pr.x > 1.0 || pr.y < 0.0 || pr.y > 1.0) sphereintersect = false;
    if (prraw.z < 0.0) sphereintersect = false;
    return sphereintersect;// || decidingdot > maxdt;
}
