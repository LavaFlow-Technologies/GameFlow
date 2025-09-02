#include "Magma/ECS/ECS.as"

#include "SceneRenderer.as"

class Scene : ObjectECS
{
    string Name;

    Scene(string name)
    {
        Name = name;

        EntityWorld.RegisterType("CameraComponent");
        EntityWorld.RegisterType("TagComponent");
        EntityWorld.RegisterType("TransformComponent");
        EntityWorld.RegisterType("AudioComponent");
        EntityWorld.RegisterType("MeshComponent");
        EntityWorld.RegisterType("SkyboxComponent");
        EntityWorld.RegisterType("ScriptComponent");
        EntityWorld.RegisterType("RigidBodyComponent");
        EntityWorld.RegisterType("DirectionalLightComponent");
        EntityWorld.RegisterType("PointLightComponent");
        EntityWorld.RegisterType("SpotlightComponent");
        EntityWorld.RegisterType("ParticleEmitterComponent");

        EntityWorld.AddSystem("PhysicsSystem")
            .On(Stage::PreUpdate)
            .On(Stage::OnUpdate)
            .On(Stage::PostUpdate);
        EntityWorld.AddSystem("ScriptSystem")
            .On(Stage::PreUpdate);
    }

    void OnUpdate(float ts)
    {
        EntityWorld.OnUpdate(ts);
    }

    void OnRender(ObjectRenderer@ objectRenderer)
    {
        SceneRenderer@ renderer = cast<SceneRenderer>(objectRenderer);

        EntityWorld.ForEach("CameraComponent")
            .Do(function(entity) { renderer.SubmitCamera(entity); });
        EntityWorld.ForEach("SkyboxComponent")
            .Do(function(entity) { renderer.SubmitSkybox(entity); });
        EntityWorld
            .ForEach("DirectionalLightComponent").Or()
            .ForEach("PointLightComponent").Or()
            .ForEach("SpotLightComponent")
            .Do(function(entity) { renderer.SubmitLight(entity); });
        EntityWorld.ForEach("ParticleEmitterComponent")
            .Do(function(entity) { renderer.SubmitParticles(entity); });
        EntityWorld
            .ForEach("MeshComponent").And()
            .ForEach("TransformComponent")
            .Do(function(entity) { renderer.SubmitMesh(entity); });
    }
}