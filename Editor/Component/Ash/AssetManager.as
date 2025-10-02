
class EditorAssetManager : AssetManager
{
    EditorAssetManager()
    {
        Editor.FileWatcher.Track("Asset");
    }
}