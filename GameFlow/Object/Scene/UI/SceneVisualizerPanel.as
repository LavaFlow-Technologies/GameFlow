#include "Magma/UI/Object.as"

#include "ProjectTab.as"

shared class SceneVisualizer : ObjectPanel
{
    UI::Image Cursor;
    UI::Image GizmoTranslate;
    UI::Image GizmoRotate;
    UI::Image GizmoScale;

    SceneVisualizer()
    {
        Cursor = Editor.GetImage("Magma/assets/icons/Cursor.png");
        GizmoTranslate = Editor.GetImage("Magma/assets/icons/GizmoTranslate.png");
        GizmoRotate = Editor.GetImage("Magma/assets/icons/GizmoRotate.png");
        GizmoScale = Editor.GetImage("Magma/assets/icons/GizmoScale.png");

        Cursor.SetSize(35, 35);
        GizmoTranslate.SetSize(35, 35);
        GizmoRotate.SetSize(35, 35);
        GizmoScale.SetSize(35, 35);

        // SetContext();
    }

    void Update(float ts)
    {
        ProjectTab@ tab = Editor.GetTab("Project");
        if(tab.GetState() != ScreenState::Edit)
            return;

        // PhysicsWorld.OnUpdate(ts);
        SceneRendererEditor@ renderer = Editor.GetObjectRenderer("Scene");
        renderer.IsHovered(s_Hovered);
        renderer.Update(ts);
    }

    void Draw()
    {
        Widget.Begin("Scene Visualizer");

        Widget.End();
    }

    void SetImage()
    {
        
    }

    void ResetImage()
    {

    }

    void Add(Entity entity)
    {

    }
}