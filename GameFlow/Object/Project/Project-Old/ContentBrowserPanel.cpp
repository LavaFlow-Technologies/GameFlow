#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/List.h>

#include <Magma/UI/UIRenderer.h>

#include "Editor/EditorApp.h"
#include "Editor/AssetImporter.h"

#include "Project/AssetEditorPanel.h"

namespace Magma {

struct {
	struct {
		bool material = false;
		bool script = false;
	} add;
} static options;

static Ref<UI::Image> s_FileIcon;
static Ref<UI::Image> s_FolderIcon;

static bool s_Selecting = false;
static uint32_t s_SelectionID = 0;
static AssetType s_SelectType = AssetType::None;
static Asset s_Selection = { };
static ImVec2 s_Position;

ContentBrowserPanel::ContentBrowserPanel()
	: Panel("ContentBrowser")
{
	Application::PushDir();
	s_FileIcon =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/FileIcon.png"));
	s_FolderIcon =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/FolderIcon.png"));
	Application::PopDir();
}

ContentBrowserPanel::~ContentBrowserPanel() {

}

void ContentBrowserPanel::Update(VolcaniCore::TimeStep ts) {

}

static ImRect Traverse(fs::path folder);
static void DrawAssetSelectWindow();

void ContentBrowserPanel::Draw() {
	ImGui::Begin("Content Browser");
	{
		auto windowFlags = ImGuiWindowFlags_MenuBar;
		auto childFlags = ImGuiChildFlags_Border;

		ImVec2 size = { 300, ImGui::GetContentRegionAvail().y };
		ImGui::BeginChild("File-Hierarchy-View", size, childFlags, windowFlags);
		{
			ImVec2 windowStart = ImGui::GetCursorPos();
			ImGuiStyle& style = ImGui::GetStyle();
			const ImGuiWindow* window = ImGui::GetCurrentWindow();
			ImRect rect = window->MenuBarRect();

			ImGui::PushClipRect(rect.Min, rect.Max, false);
			{
				float x = style.FramePadding.x
						+ ImGui::GetCurrentContext()->FontSize
						+ style.ItemInnerSpacing.x;
				float y = style.FramePadding.y;
				ImGui::SetCursorPos({ x, y });
				ImGui::Text("File Hierarchy");
			}
			ImGui::PopClipRect();
			ImGui::SetCursorPos(windowStart);

			Traverse(Editor::GetProject().Path);
		}
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Assets-View", { }, childFlags, windowFlags);
		{
			ImVec2 windowStart = ImGui::GetCursorPos();
			ImGuiStyle& style = ImGui::GetStyle();
			const ImGuiWindow* window = ImGui::GetCurrentWindow();
			ImRect rect = window->MenuBarRect();

			ImGui::PushClipRect(rect.Min, rect.Max, false);
			{
				float x = style.FramePadding.x
						+ ImGui::GetCurrentContext()->FontSize
						+ style.ItemInnerSpacing.x;
				float y = style.FramePadding.y;
				ImGui::SetCursorPos({ x, y });
				ImGui::Text("Assets");
			}
			ImGui::PopClipRect();

			if(ImGui::IsMouseDown(1) && ImGui::IsWindowHovered())
				ImGui::OpenPopup("Options");

			if(ImGui::BeginPopup("Options")) {
				if(ImGui::MenuItem("New Material"))
					options.add.material = true;
				if(ImGui::MenuItem("New Script"))
					options.add.script = true;

				ImGui::EndPopup();
			}

			auto* assets = AssetManager::Get()->As<EditorAssetManager>();

			if(options.add.material)
				ImGui::OpenPopup("New Material");
			if(options.add.script)
				ImGui::OpenPopup("New Script");

			if(ImGui::BeginPopupModal("New Material")) {
				static std::string name;
				ImGui::InputTextWithHint("##", "Enter material name", &name);

				uint32_t close = 0;
				close = (uint32_t)ImGui::Button("Create");
				ImGui::SameLine();
				if(!close)
					close = 2 * (uint32_t)ImGui::Button("Cancel");

				if(close) {
					if(close == 1) {
						if(name == ""
						|| name.find_first_not_of(' ') == std::string::npos)
							close = 0;
						else if(auto asset = assets->GetNamedAsset(name); asset)
							close = 0;
						else {
							Asset newAsset = assets->Add(AssetType::Material);
							assets->NameAsset(newAsset, name);
							auto panel = m_Tab->GetPanel("AssetEditor");
							panel->As<AssetEditorPanel>()->Select(newAsset);
							name = "";
						}
					}

					if(close) {
						options.add.material = false;
						ImGui::CloseCurrentPopup();
					}
				}

				ImGui::EndPopup();
			}

			if(ImGui::BeginPopupModal("New Script")) {
				static std::string name;
				ImGui::InputTextWithHint("##", "Enter file name", &name);

				static uint32_t currentItem = 0;
				static List<std::string> types =
					{ "IEntityController", "IUIController" };
				if(ImGui::BeginCombo("##Combo", types[currentItem].c_str()))
				{
					for(uint32_t i = 0; i < types.Count(); i++) {
						auto item = types[i];
						bool isSelected = currentItem == i;
						if(ImGui::Selectable(item.c_str(), isSelected))
							currentItem = i;
						if(isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				uint32_t close = 0;
				close = (uint32_t)ImGui::Button("Create");
				ImGui::SameLine();
				if(!close)
					close = 2 * (uint32_t)ImGui::Button("Cancel");

				if(close) {
					if(close == 1) {
						
					}

					if(close) {
						options.add.script = false;
						ImGui::CloseCurrentPopup();
					}
				}

				ImGui::EndPopup();
			}

			RenderAssetTable();
		}
		ImGui::EndChild();

		if(s_Selecting)
			DrawAssetSelectWindow();
	}
	ImGui::End();
}

void ContentBrowserPanel::RenderAssetTable() {
	static float padding = 24.0f;
	static float thumbnailSize = 100.0f;
	static float cellSize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int32_t columnCount = (int32_t)(panelWidth / cellSize);
	columnCount = columnCount ? columnCount : 1;

	auto* assetManager = AssetManager::Get()->As<EditorAssetManager>();

	if(ImGui::BeginTable("AssetsTable", columnCount))
	{
		static Asset s_Asset;

		for(auto& [asset, _] : assetManager->GetRegistry()) {
			if(!asset.Primary || assetManager->IsNativeAsset(asset))
				continue;

			if(s_Selecting && s_SelectType != AssetType::None
			&& asset.Type != s_SelectType)
				continue;

			ImGui::TableNextColumn();

			std::string path = assetManager->GetPath(asset.ID);
			if(path != "")
				path = fs::path(path).filename().string();

			UI::Button button;
			button.Width = thumbnailSize;
			button.Height = thumbnailSize;
			button.Display = s_FileIcon;
			button.UsePosition = false;

			if(UI::UIRenderer::DrawButton(button).Clicked) {
				s_Asset = asset;
				if(!s_Selecting) {
					auto panel =
						m_Tab->GetPanel("AssetEditor")->As<AssetEditorPanel>();
					panel->Select(asset);
				}
				else
					s_Selection = s_Asset;
			}
			if(ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
				s_Asset = asset;
				ImGui::OpenPopup("Asset Options");
			}

			if(asset == s_Asset && ImGui::BeginPopup("Asset Options")) {
				if(ImGui::MenuItem("Delete"))
					;

				ImGui::EndPopup();
			}

			if(!s_Selecting)
				if(ImGui::BeginDragDropSource()) {
					ImGui::SetDragDropPayload("ASSET", &s_Asset, sizeof(Asset));

					UI::Image image;
					image.Content = s_FileIcon->Content;
					image.Width = thumbnailSize;
					image.Height = thumbnailSize;
					image.UsePosition = false;

					UI::UIRenderer::DrawImage(image);

					ImGui::EndDragDropSource();
				}

			auto name = assetManager->GetAssetName(asset);
			if(name != "")
				ImGui::Text(name.c_str());
			else if(path != "")
				ImGui::TextWrapped(path.c_str());
			else
				ImGui::Text("%llu", (uint64_t)asset.ID);
		}

		ImGui::EndTable();
	}
}

void ContentBrowserPanel::Select(AssetType type, uint32_t id) {
	s_Selecting = true;
	s_SelectType = type;
	s_SelectionID = id;
	s_Position = ImGui::GetCursorScreenPos();

	if(s_Position.y + 170.0f > ImGui::GetIO().DisplaySize.y)
		s_Position.y -= 171.0f;
	if(s_Position.x + 130.0f > ImGui::GetIO().DisplaySize.x)
		s_Position.x -= 131.0f;
}

void ContentBrowserPanel::CancelSelect() {
	s_Selecting = false;
	s_SelectionID = 0;
	s_SelectType = AssetType::None;
	s_Selection = { };
}

bool ContentBrowserPanel::IsSelecting() {
	return s_Selecting;
}

bool ContentBrowserPanel::HasSelection(uint32_t id) {
	return !s_Selecting && s_Selection.ID && s_SelectionID == id;
}

Asset ContentBrowserPanel::GetSelected() {
	Asset asset = s_Selection;
	s_Selection = { };
	s_SelectionID = 0;
	s_SelectType = AssetType::None;
	return asset;
}

void DrawAssetSelectWindow() {
	auto flags = ImGuiWindowFlags_NoTitleBar
			   | ImGuiWindowFlags_NoResize
			   | ImGuiWindowFlags_NoMove
			   | ImGuiWindowFlags_NoScrollbar
			   | ImGuiWindowFlags_NoScrollWithMouse
			   | ImGuiWindowFlags_NoSavedSettings;

	ImGui::SetNextWindowPos(s_Position);
	ImGui::SetNextWindowSizeConstraints({ 130.0f, 170.0f }, { 130.0f, 170.0f });
	ImGui::Begin("Select Asset", nullptr, flags);
	{
		if(s_Selection.ID) {
			UI::Image image;
			image.x = 5.0f;
			image.Width = 120;
			image.Height = 120;
			image.Content = s_FileIcon->Content;
			image.Render();

			auto assetManager = AssetManager::Get()->As<EditorAssetManager>();
			auto name = assetManager->GetAssetName(s_Selection);
			auto path = assetManager->GetPath(s_Selection.ID);
			if(name != "")
				ImGui::Text(name.c_str());
			else if(path != "")
				ImGui::Text(fs::path(path).filename().string().c_str());
			else
				ImGui::Text("%llu", (uint64_t)s_Selection.ID);
		}
		else {
			ImGui::Text("No Asset Selected");
		}

		bool closeWindow = false;
		closeWindow |= ImGui::Button("Confirm");
		ImGui::SameLine();
		closeWindow |= ImGui::Button("Cancel");

		if(closeWindow) {
			s_SelectType = AssetType::None;
			s_Selecting = false;
		}
	}
	ImGui::End();
}

ImRect Traverse(fs::path path) {
	List<fs::path> folders;
	List<fs::path> files;

	ImRect nodeRect;
	if(fs::is_directory(path)) {
		auto flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		auto tag = path.stem().string();
		bool open = ImGui::TreeNodeEx(tag.c_str(), flags, tag.c_str());
		nodeRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

		if(!open)
			return nodeRect;

		for(auto p : FileUtils::GetFiles(path.string())) {
			fs::path path(p);
			if(fs::is_directory(path))
				folders.Add(path);
			else
				files.Add(path);
		}
	}
	else {
		static std::string selected = "";
		static std::string type = "";

		auto flags = ImGuiTreeNodeFlags_SpanAvailWidth
					| ImGuiTreeNodeFlags_Bullet
					| ImGuiTreeNodeFlags_DefaultOpen;
		bool open = ImGui::TreeNodeEx(path.filename().string().c_str(), flags);

		nodeRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
		if(ImGui::IsMouseClicked(1) && ImGui::IsItemHovered()) {
			if(path.extension().string() == ".scene")
				type = "Scene";
			else if(path.extension().string() == ".json")
				type = "UI";

			if(type != "") {
				ImGui::OpenPopup("NewTab");
				selected = path.string();
			}
		}

		if(ImGui::BeginPopup("NewTab")) {
			if(ImGui::Button("Open file as new tab")) {
				if(type == "Scene")
					Editor::NewTab(CreateRef<SceneTab>(selected));
				if(type == "UI")
					Editor::NewTab(CreateRef<UITab>(selected));

				selected = "";
				type = "";
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if(open)
			ImGui::TreePop();
	}

	ImColor TreeLineColor = ImGui::GetColorU32(ImGuiCol_Text);
	float SmallOffsetX = -8.5f;
	ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();
	verticalLineStart.x += SmallOffsetX; // Line up with the arrow
	ImVec2 verticalLineEnd = verticalLineStart;

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	for(auto entries : { folders, files })
		for(auto entry : entries) {
			ImRect childRect = Traverse(entry);
			float horizontalLineSize = 8.0f; // Arbitrary
			float midpoint = (childRect.Min.y + childRect.Max.y) / 2.0f;
			auto p0 = ImVec2(verticalLineStart.x, midpoint);
			auto p1 = ImVec2(verticalLineStart.x + horizontalLineSize, midpoint);
			drawList->AddLine(p0, p1, TreeLineColor);
			verticalLineEnd.y = midpoint;
		}

	drawList->AddLine(verticalLineStart, verticalLineEnd, TreeLineColor);

	if(fs::is_directory(path))
		ImGui::TreePop();

	return nodeRect;
}

}