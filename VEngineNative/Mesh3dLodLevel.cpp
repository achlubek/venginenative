#include "stdafx.h"
#include "Game.h"
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
	modelInfosBuffer = new VulkanGenericBuffer(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 64 * kb1);
	materialBuffer = new VulkanGenericBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, kb1);
	samplerIndices = {};
	modes = {};
	targets = {};
	sources = {};
	modifiers = {};
	uvScales = {};
	nodesDatas = {};
	nodesColors = {};
	modes = {};
	textureBinds = {};
	wrapModes = {};
	id = Game::instance->getNextId();
	Game::instance->registerId(id, this);
	descriptorSet = Game::instance->renderer->setManager.generateMesh3dDescriptorSet();
	int i = 0;
	descriptorSet.bindUniformBuffer(i++, Game::instance->renderer->uboHighFrequencyBuffer);
	descriptorSet.bindUniformBuffer(i++, Game::instance->renderer->uboLowFrequencyBuffer);

	descriptorSet.bindStorageBuffer(i++, *modelInfosBuffer);
	descriptorSet.bindUniformBuffer(i++, *materialBuffer);

	descriptorSet.bindImageViewSampler(i++, *Game::instance->dummyTexture);
	descriptorSet.bindImageViewSampler(i++, *Game::instance->dummyTexture);

	descriptorSet.update();
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
	/*
    if (material->diffuseColorTex != nullptr) material->diffuseColorTex->use(10);
    if (material->normalTex != nullptr) material->normalTex->use(11);
    if (material->bumpTex != nullptr) material->bumpTex->use(12);
    if (material->roughnessTex != nullptr) material->roughnessTex->use(13);
    if (material->metalnessTex != nullptr) material->metalnessTex->use(14);
	*/
    // address 0  here
    /*
    shader->setUniform("Roughness", material->roughness);//1
    shader->setUniform("Metalness", material->metalness);//2
    shader->setUniform("DiffuseColor", material->diffuseColor);//2 + 4 rgba = 6

    shader->setUniform("useDiffuseColorTexInt", material->diffuseColorTex != nullptr ? 1 : 0);
    shader->setUniform("useNormalTexInt", material->normalTex != nullptr ? 1 : 0);
    shader->setUniform("useBumpTexInt", material->bumpTex != nullptr ? 1 : 0);
    shader->setUniform("useRoughnessTexInt", material->roughnessTex != nullptr ? 1 : 0);
    shader->setUniform("useMetalnessTexInt", material->metalnessTex != nullptr ? 1 : 0);

    shader->setUniform("diffuseColorTexScale", material->diffuseColorTexScale);
    shader->setUniform("normalTexScale", material->normalTexScale);
    shader->setUniform("bumpTexScale", material->bumpTexScale);
    shader->setUniform("roughnessTexScale", material->roughnessTexScale);
    shader->setUniform("metalnessTexScale", material->metalnessTexScale);

    shader->setUniform("MeshID", mesh->id);
    shader->setUniform("LodLevelID", id);*/
	  
    stage->drawMesh(info3d, descriptorSet, instancesFiltered);

}

void Mesh3dLodLevel::updateBuffer(const Mesh3d* mesh, const vector<Mesh3dInstance*> &instances)
{
    // next frame it runs current buffer to 1, next buffer will be 

    // buffers into nextbuffer, draws current
    vec3 cameraPos = Game::instance->world->mainDisplayCamera->transformation->getPosition();
    vector<Mesh3dInstance*> filtered;
	newids.clear();
    bool changed = false;
    for (int i = 0; i < instances.size(); i++) {
        float dst = distance(cameraPos, instances[i]->transformation->getPosition());
        if (instances[i]->visible && dst >= distanceStart && dst < distanceEnd && checkIntersection(instances[i])) {
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

    if (changed) {


        if (fint > 0) {
            void* modelbufferpt = nullptr;
            modelInfosBuffer->map(0, 4 * 16 * fint, &modelbufferpt);
            int a = 0;
            for (unsigned int i = 0; i < fint; i++) {
                TransformationManager *mgr = filtered[i]->transformation;
                quat q = glm::inverse(mgr->getOrientation());
                vec3 p = mgr->getPosition();
                vec3 s = mgr->getSize();
                ((float*)modelbufferpt)[a++] = q.x;
                ((float*)modelbufferpt)[a++] = q.y;
                ((float*)modelbufferpt)[a++] = q.z;
                ((float*)modelbufferpt)[a++] = q.w;

                ((float*)modelbufferpt)[a++] = p.x;
                ((float*)modelbufferpt)[a++] = p.y;
                ((float*)modelbufferpt)[a++] = p.z;
                ((float*)modelbufferpt)[a++] = 1.0f;

                ((float*)modelbufferpt)[a++] = s.x;
                ((float*)modelbufferpt)[a++] = s.y;
                ((float*)modelbufferpt)[a++] = s.z;
                ((float*)modelbufferpt)[a++] = 1.0f;

                ((int*)modelbufferpt)[a++] = filtered[i]->id;
                ((int*)modelbufferpt)[a++] = filtered[i]->id;
                ((int*)modelbufferpt)[a++] = filtered[i]->id;
                ((int*)modelbufferpt)[a++] = filtered[i]->id;
            }

            modelInfosBuffer->unmap();
        }
    }

    if (materialBufferNeedsUpdate) {


		if (material->diffuseColorTex != nullptr) {
			descriptorSet.bindImageViewSampler(4, *material->diffuseColorTex);
		}

		if (material->normalTex != nullptr) {
			descriptorSet.bindImageViewSampler(5, *material->normalTex);
		}

		if (material->diffuseColorTex != nullptr || material->normalTex != nullptr) {
			descriptorSet.update();
		}

        materialBufferNeedsUpdate = false;
        vector<float> floats2;
        floats2.reserve(32);
        floats2.push_back(material->roughness);
        floats2.push_back(material->metalness);
        floats2.push_back(0.0f);
        floats2.push_back(0.0f);

        floats2.push_back(material->diffuseColor.x);
        floats2.push_back(material->diffuseColor.y);
        floats2.push_back(material->diffuseColor.z);
        floats2.push_back(0.0f);

        int one = 1;
        int zero = 0;

        floats2.push_back(vcasti((material->diffuseColorTex != nullptr ? one : zero)));
        floats2.push_back(vcasti((material->normalTex != nullptr ? one : zero)));
        floats2.push_back(vcasti((material->bumpTex != nullptr ? one : zero)));
        floats2.push_back(vcasti((material->roughnessTex != nullptr ? one : zero)));

        floats2.push_back(vcasti((material->metalnessTex != nullptr ? one : zero)));
        floats2.push_back(0.0f);
        floats2.push_back(0.0f);
        floats2.push_back(0.0f); // amd safety 

        floats2.push_back(material->diffuseColorTexScale.x);
        floats2.push_back(material->diffuseColorTexScale.y);
        floats2.push_back(material->normalTexScale.x);
        floats2.push_back(material->normalTexScale.y);

        floats2.push_back(material->bumpTexScale.x);
        floats2.push_back(material->bumpTexScale.y);
        floats2.push_back(material->roughnessTexScale.x);
        floats2.push_back(material->roughnessTexScale.y);

        floats2.push_back(material->metalnessTexScale.x);
        floats2.push_back(material->metalnessTexScale.y);
        floats2.push_back(0.0f);
        floats2.push_back(0.0f);

        floats2.push_back(vcasti(mesh->id));
        floats2.push_back(vcasti(id));
        floats2.push_back(0.0f);
        floats2.push_back(0.0f);

		void* materialpointer = nullptr;
		materialBuffer->map(0, 4 * floats2.size(), &materialpointer);
		memcpy(materialpointer, floats2.data(), 4 * floats2.size());
		materialBuffer->unmap();
    }
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

bool Mesh3dLodLevel::checkIntersection(Mesh3dInstance * instance)
{
	//return true;//debug
    if (ignoreFrustumCulling) return true;
    auto size = instance->transformation->getSize();
    float radius = info3d->radius * glm::max(glm::max(size.x, size.y), size.z);

    vec3 center = instance->transformation->getPosition();// +0.5f * (info3d->aabbmax + info3d->aabbmin);
    vec3 camerapos = Game::instance->world->mainDisplayCamera->transformation->getPosition();

    float dst = distance(camerapos, center);

    if (dst < radius) {
        //  printf("SKIP");
        return true;
    }

    // i cannot find it on net so i craft my own cone/sphere test

    vec3 rdirC = Game::instance->world->mainDisplayCamera->cone->reconstructDirection(glm::vec2(0.5));
    vec3 helper = camerapos + rdirC * dst;
    vec3 helpdir = normalize(helper - center);
    vec3 newpos = center + helpdir * radius;

    vec4 prraw = projectvdao2(Game::instance->viewProjMatrix, newpos);
    vec2 pr = (vec2(prraw.x, prraw.y) / prraw.w) * 0.5f + 0.5f;
    bool sphereintersect = true;
    if (pr.x < 0.0 || pr.x > 1.0 || pr.y < 0.0 || pr.y > 1.0) sphereintersect = false;
    if (prraw.z < 0.0) sphereintersect = false;
    return sphereintersect;// || decidingdot > maxdt;
}
