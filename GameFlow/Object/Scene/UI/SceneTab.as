#include "Magma/UI/Object.as"

#include "Scene.as"

shared class SceneTab : ObjectTab
{
    Scene Context;

    SceneTab()
    {

    }

    SceneTab(string path)
    {

    }

    void OnSelect()
    {
        SceneVisualizerPanel@ panel = GetPanel("SceneVisualizer");
        EditorSceneRenderer@ renderer = Editor.GetObjectRenderer("Scene");
        renderer.SetContext(panel);
    }

    void OnUpdate(float ts)
    {
        
    }

    void Setup()
    {

    }
}