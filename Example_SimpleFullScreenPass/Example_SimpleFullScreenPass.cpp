#include "stdafx.h"
#include "BinaryBufferBuilder.h"

#define GLFW_KEY_A   65
#define GLFW_KEY_B   66
#define GLFW_KEY_C   67
#define GLFW_KEY_D   68
#define GLFW_KEY_E   69
#define GLFW_KEY_F   70
#define GLFW_KEY_G   71
#define GLFW_KEY_H   72
#define GLFW_KEY_I   73
#define GLFW_KEY_J   74
#define GLFW_KEY_K   75
#define GLFW_KEY_L   76
#define GLFW_KEY_M   77
#define GLFW_KEY_N   78
#define GLFW_KEY_O   79
#define GLFW_KEY_P   80
#define GLFW_KEY_Q   81
#define GLFW_KEY_R   82
#define GLFW_KEY_S   83
#define GLFW_KEY_T   84
#define GLFW_KEY_U   85
#define GLFW_KEY_V   86
#define GLFW_KEY_W   87
#define GLFW_KEY_X   88
#define GLFW_KEY_Y   89
#define GLFW_KEY_Z   90

using namespace VEngine::Renderer;

int main()
{
	int height = 1000;
	int width = 1000;
    auto toolkit = new VulkanToolkit(height, width, true, "Full screen pass example");
    toolkit->getMedia()->scanDirectory("../../media");
    toolkit->getMedia()->scanDirectory("../../shaders");

	auto mouse = toolkit->getMouse();
	auto keyboard = toolkit->getKeyboard();
    
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

	mouse->setOnMouseDownHandler([](int key) {

		}
	);

	mouse->setOnMouseUpHandler([](int key) {

		}
	);

	keyboard->setOnCharHandler([](unsigned int c) {
		}
	);
	float roughness = 0.0;
	keyboard->setOnKeyPressHandler([&](int key) {
		if (key == GLFW_KEY_P) {
			roughness += 0.1;
			if (roughness > 1.0) roughness = 1.0;
		}
		if (key == GLFW_KEY_O) {
			roughness -= 0.1;
			if (roughness < 0.0) roughness = 0.0;
		}
	}
	);
	keyboard->setOnKeyRepeatHandler([](int key) {

		}
	);
	keyboard->setOnKeyReleaseHandler([](int key) {

		}
	);

	glm::vec3 campos = glm::vec3(0.0);

    while (!toolkit->shouldCloseWindow()) {

		auto cursor = mouse->getCursorPosition();
		glm::quat camrotx = glm::angleAxis((float)glm::radians(180.0 + 360.0 * (std::get<0>(cursor) / (double)width)), glm::vec3(0.0, 1.0, 0.0));
		glm::quat camroty = glm::angleAxis((float)glm::radians(-90.0 + 180.0 * (std::get<1>(cursor) / (double)height)), glm::vec3(1.0, 0.0, 0.0));
		auto camrot = camroty * camrotx;

		if (keyboard->isKeyDown(GLFW_KEY_W)) {
			campos += glm::vec3(0.0, 0.0, -1.0) * camrot;
		}
		if (keyboard->isKeyDown(GLFW_KEY_S)) {
			campos += glm::vec3(0.0, 0.0, 1.0) * camrot;
		}
		if (keyboard->isKeyDown(GLFW_KEY_A)) {
			campos += glm::vec3(-1.0, 0.0, 0.0) * camrot;
		}
		if (keyboard->isKeyDown(GLFW_KEY_D)) {
			campos += glm::vec3(1.0, 0.0, 0.0) * camrot;
		}

		BinaryBufferBuilder camBufferBuilder = BinaryBufferBuilder();
		camBufferBuilder.emplaceFloat32(campos.x);
		camBufferBuilder.emplaceFloat32(campos.y);
		camBufferBuilder.emplaceFloat32(campos.z);
		camBufferBuilder.emplaceFloat32(1.0 / fov);

		camBufferBuilder.emplaceFloat32(camrot.x);
		camBufferBuilder.emplaceFloat32(camrot.y);
		camBufferBuilder.emplaceFloat32(camrot.z);
		camBufferBuilder.emplaceFloat32(camrot.w);

		camBufferBuilder.emplaceInt32(16);
		camBufferBuilder.emplaceInt32(16);
		camBufferBuilder.emplaceInt32(16);
		camBufferBuilder.emplaceInt32(16);

		camBufferBuilder.emplaceFloat32((float)toolkit->getExecutionTime());
		camBufferBuilder.emplaceFloat32((float)toolkit->getExecutionTime());
		camBufferBuilder.emplaceFloat32((float)toolkit->getExecutionTime());
		camBufferBuilder.emplaceFloat32((float)toolkit->getExecutionTime());

		void* camBufPtr;
		cameraBuffer->map(0, camBufferBuilder.buffer.size(), &camBufPtr);
		memcpy(camBufPtr, camBufferBuilder.buffer.data(), camBufferBuilder.buffer.size());
		cameraBuffer->unmap();


		BinaryBufferBuilder boxBufferBuilder = BinaryBufferBuilder();


		for (int x = 0; x < 4; x++) {
			for (int y = 0; y < 4; y++) {
				glm::quat boxorient = glm::angleAxis(sin((float)toolkit->getExecutionTime() * (x * y * 0.1f + 0.2f)), glm::vec3(1.0, 0.0, 1.0));
				boxBufferBuilder.emplaceFloat32(1.0f + x * 2.0f);
				boxBufferBuilder.emplaceFloat32(sin(x + y * -1234.567f) - 1.0);
				boxBufferBuilder.emplaceFloat32(5.0f + y * 2.0f);
				boxBufferBuilder.emplaceFloat32(0.0f);

				boxBufferBuilder.emplaceFloat32(boxorient.x);
				boxBufferBuilder.emplaceFloat32(boxorient.y);
				boxBufferBuilder.emplaceFloat32(boxorient.z);
				boxBufferBuilder.emplaceFloat32(boxorient.w);

				boxBufferBuilder.emplaceFloat32(2.0f);
				boxBufferBuilder.emplaceFloat32(1.0f);
				boxBufferBuilder.emplaceFloat32(3.0f);
				boxBufferBuilder.emplaceFloat32(0.0f);

				boxBufferBuilder.emplaceFloat32(sin(x + y * -234.567f) * 0.5 + 0.5);
				boxBufferBuilder.emplaceFloat32(sin(x + y * -124.567f) * 0.5 + 0.5);
				boxBufferBuilder.emplaceFloat32(sin(x + y * -12.567f) * 0.5 + 0.5);
				boxBufferBuilder.emplaceFloat32(roughness);
			}
		}


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

