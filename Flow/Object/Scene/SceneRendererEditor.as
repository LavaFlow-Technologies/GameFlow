#include "SceneRenderer.as"

#include "RendererAPI/Renderer.as"
#include "RendererAPI/Camera.as"

class SceneRendererEditor : SceneRenderer
{
    CameraController Controller;
    RenderPass GridPass;

    SceneRendererEditor()
    {
        auto camera = StereographicCamera::Create(75.0);
        camera.SetPosition(Vec3{ 0.0, 1.0, 15.0 });
        camera.Resize(1920, 1080);
        camera.SetProjection(0.001, 10000.0);
        Controller.SetCamera(camera);

        FinalOutputBuffer = Framebuffer::Create();

    }

    void SubmitCamera(Entity entity)
    {
        auto camera = entity.Get("CameraComponent").Cam;
        if(!camera)
            return;

        HasCamera = true;
        AddBillboard(camera.GetPosition(), 1);

        if(Selected != entity)
            return;

        Mat4 inverse = Math::Inverse(camera.GetViewProjection());

        Vec4 p0 = inverse * Vec4(-1, -1, -1, 1); // Front bottom left
        Vec4 p1 = inverse * Vec4( 1, -1, -1, 1); // Front bottom right
        Vec4 p2 = inverse * Vec4( 1,  1, -1, 1); // Front top right
        Vec4 p3 = inverse * Vec4(-1,  1, -1, 1); // Front top left
        Vec4 p4 = inverse * Vec4(-1, -1,  1, 1); // Back bottom left
        Vec4 p5 = inverse * Vec4( 1, -1,  1, 1); // Back bottom right
        Vec4 p6 = inverse * Vec4( 1,  1,  1, 1); // Back top right
        Vec4 p7 = inverse * Vec4(-1,  1,  1, 1); // Back top left

        Point points[] =
        {
            { p0 / p0.w, Vec3(1.0) },
            { p1 / p1.w, Vec3(1.0) },
            { p2 / p2.w, Vec3(1.0) },
            { p3 / p3.w, Vec3(1.0) },
            { p4 / p4.w, Vec3(1.0) },
            { p5 / p5.w, Vec3(1.0) },
            { p6 / p6.w, Vec3(1.0) },
            { p7 / p7.w, Vec3(1.0) },
            { camera.GetPosition(), Vec3(1.0) }
        };

        uint32 indexCount = 24;
        uint32 indices[indexCount] =
        {
            8, 7, // Left Top
            8, 6, // Right Top
            8, 4, // Left Bottom
            8, 5, // Right Bottom

            3, 2, // Front Top
            0, 1, // Front Bottom
            3, 0, // Front Left
            2, 1, // Front Right

            7, 6, // Back Top
            4, 5, // Back Bottom
            7, 4, // Back Left
            6, 5, // Back Right
        };

        Editor.RenderLines(points, indices);
    }

    void SubmitSkybox(const Entity& entity)
    {
        
    }
}