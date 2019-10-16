#include "stdafx.h"
#include "BinaryBufferBuilder.h"

using namespace VEngine::Renderer;

int main()
{
	int height = 1000;
	int width = 1000;
    auto toolkit = new VulkanToolkit(height, width, true, "Full screen pass example");
    toolkit->getMedia()->scanDirectory("../../media");
    toolkit->getMedia()->scanDirectory("../../shaders");
    
    auto vert = toolkit->getShaderFactory()->build(VEngineShaderModuleType::Vertex, "raytrace-fullscreenpass.vert.spv");
    auto frag = toolkit->getShaderFactory()->build(VEngineShaderModuleType::Fragment, "raytrace-trace.frag.spv");

    auto layout = toolkit->getDescriptorSetLayoutFactory()->build();
    layout->addField(VEngineDescriptorSetFieldType::FieldTypeStorageBuffer, VEngineDescriptorSetFieldStage::FieldStageFragment);
    layout->addField(VEngineDescriptorSetFieldType::FieldTypeUniformBuffer, VEngineDescriptorSetFieldStage::FieldStageFragment);

    auto stage = toolkit->getRenderStageFactory()->build(height, width, { vert, frag }, { layout });

	auto boxBuffer = toolkit->getBufferFactory()->build(VEngineBufferType::BufferTypeStorage, sizeof(float) * 65535);
	auto cameraBuffer = toolkit->getBufferFactory()->build(VEngineBufferType::BufferTypeUniform, sizeof(float) * 128);

    auto set = layout->generateDescriptorSet();
    set->bindBuffer(0, boxBuffer);
	set->bindBuffer(1, cameraBuffer);

    stage->setSets({ set });

    auto output = toolkit->getSwapChainOutputFactory()->build(stage);

    auto fullScreenQuad3dInfo = toolkit->getObject3dInfoFactory()->getFullScreenQuad();

	BinaryBufferBuilder camBufferBuilder = BinaryBufferBuilder();
	camBufferBuilder.emplaceFloat32(0.0f);
	camBufferBuilder.emplaceFloat32(0.0f);
	camBufferBuilder.emplaceFloat32(3.0f);
	camBufferBuilder.emplaceFloat32(0.5f);

	camBufferBuilder.emplaceFloat32(0.0f);
	camBufferBuilder.emplaceFloat32(0.0f);
	camBufferBuilder.emplaceFloat32(0.0f);
	camBufferBuilder.emplaceFloat32(1.0f);

	void* camBufPtr;
	cameraBuffer->map(0, camBufferBuilder.buffer.size(), &camBufPtr);
	memcpy(camBufPtr, camBufferBuilder.buffer.data(), camBufferBuilder.buffer.size());
	cameraBuffer->unmap();


	BinaryBufferBuilder boxBufferBuilder = BinaryBufferBuilder();

	boxBufferBuilder.emplaceFloat32(1.0f);
	boxBufferBuilder.emplaceFloat32(0.0f);
	boxBufferBuilder.emplaceFloat32(0.0f);
	boxBufferBuilder.emplaceFloat32(0.0f);

	boxBufferBuilder.emplaceFloat32(0.0f);
	boxBufferBuilder.emplaceFloat32(0.0f);
	boxBufferBuilder.emplaceFloat32(0.0f);
	boxBufferBuilder.emplaceFloat32(1.0f);

	boxBufferBuilder.emplaceFloat32(1.0f);
	boxBufferBuilder.emplaceFloat32(1.0f);
	boxBufferBuilder.emplaceFloat32(1.0f);
	boxBufferBuilder.emplaceFloat32(0.0f);

	boxBufferBuilder.emplaceFloat32(1.0f);
	boxBufferBuilder.emplaceFloat32(0.0f);
	boxBufferBuilder.emplaceFloat32(1.0f);
	boxBufferBuilder.emplaceFloat32(0.0f);

	void* boxBufPtr;
	boxBuffer->map(0, boxBufferBuilder.buffer.size(), &boxBufPtr);
	memcpy(boxBufPtr, boxBufferBuilder.buffer.data(), boxBufferBuilder.buffer.size());
	boxBuffer->unmap();

    while (!toolkit->shouldCloseWindow()) {

        output->beginDrawing();
        output->drawMesh(fullScreenQuad3dInfo, 1);
        output->endDrawing();
        output->submit({});

        toolkit->poolEvents();
    }

    return 0;
}

