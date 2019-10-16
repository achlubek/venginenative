#include "stdafx.h"
#include "BinaryBufferBuilder.h"

using namespace VEngine::Renderer;

int main()
{
	int height = 1000;
	int width = 1000;
    auto toolkit = new VulkanToolkit(height, width, false, "Full screen pass example");
    toolkit->getMedia()->scanDirectory("../../media");
    toolkit->getMedia()->scanDirectory("../../shaders");

	auto mouse = toolkit->getMouse();
    
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

	float fov = 0.5;

	mouse->setOnMouseScrollHandler([&fov](double x, double y) {
			fov += y * 0.01;
		}
	);

    while (!toolkit->shouldCloseWindow()) {

		auto cursor = mouse->getCursorPosition();
		glm::quat camrotx = glm::angleAxis((float)glm::radians(180.0 + 360.0 * (std::get<0>(cursor) / (double)width)), glm::vec3(0.0, 1.0, 0.0));
		glm::quat camroty = glm::angleAxis((float)glm::radians(-90.0 + 180.0 * (std::get<1>(cursor) / (double)height)), glm::vec3(1.0, 0.0, 0.0));
		auto camrot = camroty * camrotx;

		BinaryBufferBuilder camBufferBuilder = BinaryBufferBuilder();
		camBufferBuilder.emplaceFloat32(0.0f);
		camBufferBuilder.emplaceFloat32(0.0f);
		camBufferBuilder.emplaceFloat32(3.0f);
		camBufferBuilder.emplaceFloat32(1.0 / fov);

		camBufferBuilder.emplaceFloat32(camrot.x);
		camBufferBuilder.emplaceFloat32(camrot.y);
		camBufferBuilder.emplaceFloat32(camrot.z);
		camBufferBuilder.emplaceFloat32(camrot.w);

		camBufferBuilder.emplaceInt32(1);

		void* camBufPtr;
		cameraBuffer->map(0, camBufferBuilder.buffer.size(), &camBufPtr);
		memcpy(camBufPtr, camBufferBuilder.buffer.data(), camBufferBuilder.buffer.size());
		cameraBuffer->unmap();


		BinaryBufferBuilder boxBufferBuilder = BinaryBufferBuilder();

		glm::quat boxorient = glm::angleAxis(sin((float)toolkit->getExecutionTime()), glm::vec3(1.0, 0.0, 1.0));

		boxBufferBuilder.emplaceFloat32(1.0f);
		boxBufferBuilder.emplaceFloat32(0.0f);
		boxBufferBuilder.emplaceFloat32(0.0f);
		boxBufferBuilder.emplaceFloat32(0.0f);

		boxBufferBuilder.emplaceFloat32(boxorient.x);
		boxBufferBuilder.emplaceFloat32(boxorient.y);
		boxBufferBuilder.emplaceFloat32(boxorient.z);
		boxBufferBuilder.emplaceFloat32(boxorient.w);

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

        output->beginDrawing();
        output->drawMesh(fullScreenQuad3dInfo, 1);
        output->endDrawing();
        output->submit({});

        toolkit->poolEvents();
    }

    return 0;
}

