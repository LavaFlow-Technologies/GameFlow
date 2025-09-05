#include "Magma/Object/UI.as"
#include "Magma/ECS/Entity.as"

shared class SceneHierarchy : ObjectPanel
{
    Entity Selected;

    SceneHierarchy()
    {

    }

    void Update(float ts)
    {

    }

    void Draw()
    {
        Widget.Begin();

        if(Widget.Clicked())
        {
            Selected = Entity();
            ComponentEditor@ editor = Editor.GetTab("Scene").GetPanel("ComponentEditor");
            SceneVisualizer@ visualizer = Editor.GetTab("Scene").GetPanel("SceneVisualizer");
            editor.Select(Selected);
            visualizer.Select(Selected);
        }

        if(Widget.Clicked(1))
            Widget.OpenPopup("Options");

        if(Widget.Popup("Options"))
        {

            Widget.EndPopup();
        }

        Widget.End();
    }
}