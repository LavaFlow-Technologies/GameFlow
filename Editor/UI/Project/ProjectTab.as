#include "Magma/UI/Tab.as"

class ProjectTab : Tab
{
    ProjectTab()
    {

    }

    void OnOpen()
    {
        Name = "Project";
    }

    void OnClose()
    {

    }

    void OnSelect()
    {

    }

    void OnDeselect()
    {

    }

    void OnUpdate(float ts)
    {

    }

    void OnRender()
    {
        Widget::Window()
        .Render(function()
        {
            
        });
    }
}