
class CameraComponent
{
    Camera@ Cam;

    CameraComponent() { }
    CameraComponent(Camera@ camera)
        : Cam(camera) { }
};

class TagComponent
{
    string Tag;

    TagComponent()
        : Tag("Empty Tag") { }
    TagComponent(string tag)
        : Tag(tag) { }
};

class TransformComponent
{
    Vec3 Translation = { 0.0f, 0.0f, 0.0f };
    Vec3 Rotation	 = { 0.0f, 0.0f, 0.0f };
    Vec3 Scale		 = { 1.0f, 1.0f, 1.0f };

    TransformComponent() { }
    TransformComponent(Vec3 t, Vec3 r, Vec3 s)
        : Translation(t), Rotation(r), Scale(s) { }
    TransformComponent(const Transform& t)
        : Translation(t.Translation), Rotation(t.Rotation), Scale(t.Scale) { }

    operator Transform() const { return { Translation, Rotation, Scale }; }
};

class AudioComponent
{
    Ash::Asset AudioAsset;

    AudioComponent() { }
    AudioComponent(Asset asset)
        : AudioAsset(asset) { }
};

class MeshComponent
{
    Ash::Asset MeshSourceAsset;
    Ash::Asset MaterialAsset;

    MeshComponent() { }
    MeshComponent(Asset source, Asset mat)
        : MeshSourceAsset(source), MaterialAsset(mat) { }
};

class SkyboxComponent
{
    Ash::Asset CubemapAsset;

    SkyboxComponent() { }
    SkyboxComponent(Asset asset)
        : CubemapAsset(asset) { }
};

class ScriptComponent
{
    Ash::Asset ModuleAsset;
    ScriptObject Instance;

    ScriptComponent() { }
    ScriptComponent(Asset asset, ScriptObject obj)
        : ModuleAsset(asset), Instance(obj) { }
};

class RigidBodyComponent
{
    Physics::RigidBody Body;

    RigidBodyComponent() { }
    RigidBodyComponent(Ref<Physics::RigidBody> body)
        : Body(body) { }
};

class DirectionalLightComponent
{
    Vec3 Ambient;
    Vec3 Diffuse;
    Vec3 Specular;
    Vec3 Position;
    Vec3 Direction;

    DirectionalLightComponent() { }
    DirectionalLightComponent(const Vec3& a, const Vec3& d, const Vec3& s,
                              const Vec3& pos, const Vec3& dir)
        : Ambient(a), Diffuse(d), Specular(s), Position(pos), Direction(dir) { }
    DirectionalLightComponent(const DirectionalLightComponent& other) { }
};

class PointLightComponent
{
    Vec3 Ambient;
    Vec3 Diffuse;
    Vec3 Specular;
    Vec3 Position;
    float Constant;
    float Linear;
    float Quadratic;
    bool Bloom;

    PointLightComponent() { }
    PointLightComponent(const Vec3& a, const Vec3& d, const Vec3& s,
                        const Vec3& pos, float c, float l, float q, bool b)
        : Ambient(a), Diffuse(d), Specular(s), Position(pos),
        Constant(c), Linear(l), Quadratic(q), Bloom(b) { }
    PointLightComponent(const PointLightComponent& other) { }
};

class SpotlightComponent
{
    Vec3 Ambient;
    Vec3 Diffuse;
    Vec3 Specular;
    Vec3 Position;
    Vec3 Direction;
    float CutoffAngle;
    float OuterCutoffAngle;

    SpotlightComponent() { }
    SpotlightComponent(const Vec3& a, const Vec3& d, const Vec3& s,
                        const Vec3& pos, const Vec3& dir,
                        float inner, float outer)
        : Ambient(a), Diffuse(d), Specular(s), Position(pos), Direction(dir),
        CutoffAngle(inner), OuterCutoffAngle(outer) { }
    SpotlightComponent(const SpotlightComponent& other) { }
};

class ParticleEmitterComponent
{
    Vec3 Position;
    uint64_t MaxParticleCount;
    float ParticleLifetime; // In milliseconds
    float SpawnInterval; // In milliseconds
    float Offset;
    Asset MaterialAsset;

    ParticleEmitterComponent() { }
    ParticleEmitterComponent(const Vec3& pos, uint64_t max, float lifetime,
        float spawnInterval, float offset, const Asset& asset)
        : Position(pos), MaxParticleCount(max), ParticleLifetime(lifetime),
        SpawnInterval(spawnInterval), Offset(offset), MaterialAsset(asset) { }
};
