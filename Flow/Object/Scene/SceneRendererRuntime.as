#include "SceneRenderer.as"

#include "RendererAPI/Renderer.as"
#include "RendererAPI/Camera.as"

class SceneRendererRuntime : SceneRenderer
{
    CameraController Controller;

    UniformBuffer DirectionaLightBuffer;
    UniformBuffer PointLightBuffer;
    UniformBuffer SpotlightBuffer;

    SceneRendererRuntime()
    {
        FinalOutputBuffer = Framebuffer::Create();

        DirectionalLightBuffer = UniformBuffer::Create("DirectionalLight");
        PointLightBuffer = UniformBuffer::Create("PointLight");
        SpotlightBuffer = UniformBuffer::Create("Spotlight");
    }
}