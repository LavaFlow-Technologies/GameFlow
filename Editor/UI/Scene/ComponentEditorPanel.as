#include "Magma/UI/Panel.as"
// #include "Magma/ECS/Entity.as"

// #include "ContentBrowserPanel.as"

shared class ComponentEditor : Panel
{
    // Entity Context;

    ComponentEditor() { }

    // void SetContext(Entity entity)
    // {
    //     Context = entity;
    //     ContentBrowser@ panel = Editor.GetTab("Project").GetPanel("ContentBrowser");
    //     panel.CancelSelect();
    // }

    void OnOpen()
    {
        
    }

    void OnClose()
    {

    }

    void OnUpdate(float ts)
    {

    }

    void OnRender()
    {
        // Widget.Begin();

        // if(Context.Has("CameraComponent"))
        //     Edit(Context.Get("CameraComponent"));
        // if(Context.Has("TagComponent"))
        //     Edit(Context.Get("TagComponent"));
        // if(Context.Has("TransformComponent"))
        //     Edit(Context.Get("TransformComponent"));
        // if(Context.Has("AudioComponent"))
        //     Edit(Context.Get("AudioComponent"));
        // if(Context.Has("MeshComponent"))
        //     Edit(Context.Get("MeshComponent"));
        // if(Context.Has("SkyboxComponent"))
        //     Edit(Context.Get("SkyboxComponent"));
        // if(Context.Has("ScriptComponent"))
        //     Edit(Context.Get("ScriptComponent"));
        // if(Context.Has("RigidBodyComponent"))
        //     Edit(Context.Get("RigidBodyComponent"));
        // if(Context.Has("DirectionalLightComponent"))
        //     Edit(Context.Get("DirectionalLightComponent"));
        // if(Context.Has("PointLightComponent"))
        //     Edit(Context.Get("PointLightComponent"));
        // if(Context.Has("SpotlightComponent"))
        //     Edit(Context.Get("SpotlightComponent"));
        // if(Context.Has("ParticleEmitterComponent"))
        //     Edit(Context.Get("ParticleEmitterComponent"));

        // Widget.End();
    }

    // void Edit(CameraComponent@ component)
    // {
    //     Widget.SeparatorText("CameraComponent");

    //     if(not component.Cam)
    //     {
    //         if(Widget.Button("Create StereographicCamera"))
    //             component.Cam = StereographicCamera::Create();
    //         if(Widget.Button("Create Orthographic"))
    //             component.Cam = Orthographic::Create();

            
    //     }
    // }

    // void Edit(TagComponent@ component)
    // {
    //     Widget.SeparatorText("TagComponent");
    //     Widget.InputText(component);
    // }

    // void Edit(TransformComponent@ component)
    // {
    //     Widget.SeparatorText("TransformComponent");

    //     Widget.DrawFloat3("Translation", component.Translation);
    //     Widget.DrawFloat3("Rotation", component.Rotation);
    //     Widget.DrawFloat3("Scale", component.Scale);
    // }

    // void Edit(AudioComponent@ component)
    // {
    //     Widget.SeparatorText("AudioComponent");

    //     ContentBrowser@ panel = Editor.GetTab("Project").GetPanel("ContentBrowser");
    //     if(Widget.Button(component.AudioAsset ? "Change Asset" : "Set Asset"))
    //         panel.Select(AssetType::Audio);
    //     if(panel.HasSelection())
    //         component.AudioAsset = panel.GetSelected();

    //     Widget.DrawFloat("Volume", component.Volume);
    //     Widget.DrawFloat("Pitch", component.Pitch);
    //     Widget.Checkbox("Looping", component.Looping);
    //     Widget.Checkbox("Play On Start", component.PlayOnStart);
    // }

    // void Edit(MeshComponent@ component)
    // {
    //     Widget.SeparatorText("MeshComponent");

    //     ContentBrowser@ panel = Editor.GetTab("Project").GetPanel("ContentBrowser");
    //     if(Widget.Button(component.MeshAsset ? "Change Asset" : "Set Asset"))
    //         panel.Select(AssetType::Mesh);
    //     if(panel.HasSelection())
    //         component.MeshAsset = panel.GetSelected();

    //     EditorAssetManager@ assets = Editor.GetAssetManager();
    //     if(assets.IsNativeAsset(component.MeshSourceAsset))
    //     {
    //         Widget.Text("Material: {0}", component.MaterialAsset.ID);
    //         if(Widget.Button(component.MaterialAsset ? "Change Asset" : "Set Asset"))
    //         {
    //             panel.CancelSelect();
    //             panel.Select(AssetType::Mesh, 1);
    //         }
    //         if(panel.HasSelection(1))
    //             component.MaterialAsset = panel.GetSelected();
    //     }
    // }

    // void Edit(SkyboxComponent@ component)
    // {
    //     Widget.SeparatorText("SkyboxComponent");

    //     ContentBrowser@ panel = Editor.GetTab("Project").GetPanel("ContentBrowser");
    //     if(Widget.Button(component.CubemapAsset ? "Change Asset" : "Set Asset"))
    //         panel.Select(AssetType::Mesh);
    //     if(panel.HasSelection())
    //         component.CubemapAsset = panel.GetSelected();
    // }

    // void Edit(ScriptComponent@ component)
    // {
    //     Widget.SeparatorText("ScriptComponent");

    //     ContentBrowser@ panel = Editor.GetTab("Project").GetPanel("ContentBrowser");
    //     if(Widget.Button(component.ModuleAsset ? "Change Asset" : "Set Asset"))
    //         panel.Select(AssetType::Mesh);
    //     if(panel.HasSelection())
    //         component.ModuleAsset = panel.GetSelected();

    //     if(not component.ModuleAsset)
    //         return;

    //     Widget.Indent(22.0);
    //     Widget.Text("Class: {0}", component.Instance.GetClass());
    //     Widget.SeparatorText("Fields");

    //     for(uint32 i = 0; i < component.Instance.GetPropertyCount(); i++)
    //     {
    //         auto field = component.Instance.GetProperty(i);
    //         bool hasTag = ScriptManager::GetMetadata(field, "EditorField");
    //         if(!editorField)
    //             continue;

    //         if(field.Type == "string")
    //         {

    //         }
    //         else if(field.Type == "Asset")
    //         {

    //         }
    //         else if(field.Type == "Vec2")
    //         {

    //         }
    //         else if(field.Type == "Vec3")
    //         {

    //         }
    //         else if(field.Type == "Vec4")
    //         {

    //         }
    //         else if(field.Type == "bool")
    //         {

    //         }
    //         else if(field.Type == "uint8")
    //         {

    //         }
    //     }
    // }

    // void Edit(RigidBodyComponent@ component)
    // {
    //     Widget.SeparatorText("RigidBodyComponent");
    // }

    // void Edit(DirectionalLightComponent@ component)
    // {
    //     Widget.SeparatorText("DirectionalLightComponent");

    //     Widget.DragVec3("Position", component.Position);
    //     Widget.DragVec3("Direction", component.Direction);

    //     Widget.ColorEdit("Ambient", component.Ambient);
    //     Widget.ColorEdit("Diffuse", component.Diffuse);
    //     Widget.ColorEdit("Specular", component.Specular);
    // }

    // void Edit(PointLightComponent@ component)
    // {
    //     Widget.SeparatorText("PointLightComponent");

    //     Widget.DragVec3("Position", component.Position);

    //     Widget.ColorEdit("Ambient", component.Ambient);
    //     Widget.ColorEdit("Diffuse", component.Diffuse);
    //     Widget.ColorEdit("Specular", component.Specular);

    //     Widget.DragFloat("Constant", component.Constant);
    //     Widget.DragFloat("Linear", component.Linear);
    //     Widget.DragFloat("Quadratic", component.Quadratic);
    //     Widget.DragFloat("Bloom", component.Bloom);
    // }

    // void Edit(SpotlightComponent@ component)
    // {
    //     Widget.SeparatorText("SpotlightComponent");

    //     Widget.DragVec3("Position", component.Position);
    //     Widget.DragVec3("Direction", component.Direction);

    //     Widget.ColorEdit("Ambient", component.Ambient);
    //     Widget.ColorEdit("Diffuse", component.Diffuse);
    //     Widget.ColorEdit("Specular", component.Specular);

    //     Widget.DragFloat("Cutoff Angle", component.CutoffAngle);
    //     Widget.DragFloat("Outer Cutoff Angle", component.OuterCutoffAngle);
    // }

    // void Edit(ParticleEmitterComponent@ component)
    // {
    //     Widget.SeparatorText("ParticleEmitterComponent");

    //     Widget.DragVec3("Position", component.Position);
    //     Widget.DragInt("Max Particle Count", component.MaxParticleCount, 5, 1000);
    //     Widget.DragFloat("Particle Lifetime (ms)", component.ParticleLifetime);
    // }
}