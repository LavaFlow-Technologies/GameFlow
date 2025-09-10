#include "Magma/UI/Panel.as"

// #include "Ash/Asset.as"

class ContentBrowserPanel : Panel
{
    // UI::Image @FileIcon;
    // UI::Image @FolderIcon;
    // bool Selecting = false;
    // uint32 SelectionID = 0;
    // AssetType SelectionType = AssetType::None;
    // Asset Selection;
    // Vec2 Position;

    ContentBrowserPanel()
    {
        // @FileIcon = Editor.GetImage("Asset/Icons/FileIcon.png");
        // @FolderIcon = Editor.GetImage("Asset/Icons/FolderIcon.png");
    }

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
        // Widget.Begin("Content Browser");

        // Widget.Child("File Hierarchy", 300, 0)
        //     .With(ChildFlagNamedMenuBar)
        //     .With(ChildFlagBorder);
        // {
        //     // Widget.BeginTreeNode();
        //     // TraverseFolder(Editor.GetProject().Path);
        //     // Widget.EndTreeNode();
        // }
        // Widget.EndChild();

        // Widget.SameLine();

        // Widget.Child("Asset View")
        //     .With(ChildFlagNamedMenuBar)
        //     .With(ChildFlagBorder);
        // {
        //     if(Widget.Clicked(MouseButton::Left))
        //     {
        //     }

        //     // auto@ assets = Ash::AssetManager.AsEditorAssetManager();

        //     RenderAssetTable();
        // }
        // Widget.EndChild();

        // if(Selecting)
        //     DrawAssetSelectWindow();

        // Widget.End();
    }

    // void TraverseFolder(path entry)
    // {
    //     List<path> folders;
    //     List<path> files;

    //     if(entry.IsDirectory())
    //     {
    //         bool open =
    //             Widget.Node(entry)
    //                 .With(TreeNodeFlagSpan | TreeNodeFlagBullet);
    //         if(!open)
    //             return;

    //         FileUtils::ForEach(entry)
    //         .Do(
    //             function(str)
    //             {
    //                 if(str.IsDirectory())
    //                     folders.Add(str);
    //                 else
    //                     files.Add(str);
    //             });
    //     }
    //     else
    //     {

    //     }

    //     Widget.Hierarchy(TraverseFolder, folders, files);

    //     if(entry.IsDirectory())
    //         Widget.EndNode();
    // }

    // void RenderAssetTable()
    // {
    //     float padding = 24.0f;
    //     float thumbnailSize = 100.0f;
    //     float cellSize = thumbnailSize + padding;

    //     float panelWidth = Widget.GetSpaceX();
    //     int32_t columnCount = (int32_t)(panelWidth / cellSize);
    //     columnCount = columnCount ? columnCount : 1;

    //     Widget.Table("AssetsTable", columnCount);



    //     Widget.EndTable();
    // }

    // void DrawAssetSelectWindow()
    // {
    //     Widget.Begin();

    //     if(Selection.ID)
    //     {
    //         Widget.Image(5.0f, 0.0f, 120, 120)
    //         .SetContent(@FileIcon)
    //         // .SetContent(Thumbnails[Selection.ID])
    //         .Render();

    //         auto@ assetManager = AssetManager.AsEditorAssetManager();
    //         string name = assetManager.GetAssetName(Selection);
    //         filepath path = assetManager.GetAssetPath(Selection);
    //         if(name != "")
    //             Widget.Text(name);
    //         else if(path != "")
    //             Widget.Text(path.name());
    //         else
    //             Widget.Text("{0}", Selection.ID);
    //     }
    //     else
    //     {
    //         Widget.Text("No Asset Selected");
    //         // TODO(Art): Little indent with barred circle or something
    //     }

    //     bool closeWindow = false;
    //     closeWindow |= Widget.Button("Confirm");
    //     Widget.SameLine();
    //     closeWindow |= Widget.Button("Cancel");

    //     if(closeWindow) {
    //         SelectionType = AssetType::None;
    //         Selecting = false;
    //     }

    //     Widget.End();
    // }

    // void Select(AssetType type, uint32 id)
    // {
    //     Selecting = true;
    //     SelectionType = type;
    //     SelectionID = id;
    //     Position = Widget.GetCursorPos();

    //     if(Position.y + 170.0f > Widget.DisplaySize.y)
    //         Position.y -= 171.0f;
    //     if(Position.x + 130.0f > Widget.DisplaySize.x)
    //         Position.x -= 131.0f;
    // }

    // void CancelSelect()
    // {
    //     Selecting = false;
    //     SelectionType = AssetType::None;
    //     SelectionID = 0;
    //     Selection = Asset();
    // }

    // bool IsSelecting()
    // {
    //     return Selecting;
    // }

    // bool HasSelection(uint32 id)
    // {
    //     return !Selecting && Selection.ID && SelectionID == id;
    // }

    // Asset GetSelection()
    // {
    //     Asset asset = Selection;
    //     Selection = Asset();
    //     SelectionID = 0;
    //     SelectionType = AssetType::None;
    //     return asset;
    // }
}
