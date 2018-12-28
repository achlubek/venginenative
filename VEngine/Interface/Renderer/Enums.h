#pragma once
namespace VEngine
{
    namespace Renderer
    {

        enum VEngineShaderModuleType {
            Vertex,
            Fragment,
            Compute
        };

        enum VEngineCullMode {
            CullModeNone,
            CullModeFront,
            CullModeBack
        };

        enum VEngineAttachmentBlending {
            None,
            Additive,
            Alpha
        };

        enum VEngineBufferType {
            BufferTypeUniform,
            BufferTypeStorage,
            BufferTypeTransferSource,
            BufferTypeTransferDestination
        };

        enum VEngineImageUsage {
            ColorAttachment = 1,
            Sampled = 2,
            Storage = 4,
            Depth = 8,
            TransferDestination = 16
        };

        enum VEngineImageAspect {
            ColorAspect,
            DepthAspect
        };

        enum VEngineImageLayout {
            Preinitialized,
            Undefined
        };

        enum VEngineDescriptorSetFieldType {
            FieldTypeSampler,
            FieldTypeUniformBuffer,
            FieldTypeStorageBuffer,
            FieldTypeStorageImage
        };

        enum VEngineDescriptorSetFieldStage {
            FieldStageAll,
            FieldStageAllGraphics,
            FieldStageCompute,
            FieldStageVertex,
            FieldStageFragment
        };

        enum VEngineImageFormat {
            R8inorm,
            RG8inorm,
            RGBA8inorm,

            R8unorm,
            RG8unorm,
            RGBA8unorm,

            R16i,
            RG16i,
            RGBA16i,

            R16u,
            RG16u,
            RGBA16u,

            R16f,
            RG16f,
            RGBA16f,

            R32i,
            RG32i,
            RGBA32i,

            R32u,
            RG32u,
            RGBA32u,

            R32f,
            RG32f,
            RGBA32f,

            Depth16u,
            Depth32f,

            RGBA8srgb,
            RGB5unormPack16,
            RGBA8snorm,
            ABGR8unormPack32,
            ABGR8snormPack32,
            ABGR8srgbPack32,
            ARGB10unormPack32,
            ABGR10unormPack32,
            RGBA16unorm,
            RGBA16snorm,
            BGR11ufloatPack32,
            BGR5unormPack16,
            BGRA8snorm,
            BGRA8unorm,
            A1RGB5unormPack16,
            RGBA4unormPack16,
            BGRA4unormPack16,
            RGB4A1unormPack16,
            BGR5A1unormPack16
        };

        typedef union VEngineClearColorValue {
            float       float32[4];
            int32_t     int32[4];
            uint32_t    uint32[4];
        } VEngineClearColorValue;
    }
}