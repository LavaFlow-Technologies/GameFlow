#include "ECS/Entity.as"

abstract class SceneRenderer : ObjectRenderer
{
    void SubmitCamera(Entity) { }
    void SubmitSkybox(Entity) { }
    void SubmitLight(Entity) { }
    void SubmitParticles(Entity) { }
    void SubmitMesh(Entity) { }
}