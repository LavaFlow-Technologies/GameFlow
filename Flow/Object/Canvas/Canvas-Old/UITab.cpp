#include "UITab.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/FileUtils.h>

#include "UIHierarchyPanel.h"
#include "UIVisualizerPanel.h"
#include "UIElementEditorPanel.h"

#include "Editor/EditorApp.h"

namespace Magma {

struct {
	struct {
		bool newUI = false;
		bool openUI = false;
		bool saveUI = false;
		bool saveAs = false;
	} file;
	struct {
		bool addWindow = false;
		bool addButton = false;
		bool addImage = false;
	} edit;
} static menu;

static Theme s_Theme;

Theme& UITab::GetTheme() { return s_Theme; }

UITab::UITab()
	: Tab(TabType::UI)
{
	Setup();
}

UITab::UITab(const std::string& path)
	: Tab(TabType::UI)
{
	Setup();
	Load(path);
}

UITab::~UITab() {
	if(m_UIPath == "")
		m_UIPath = "Magma/assets/ui/" + m_Root.Name + ".magma.ui.json";
	UILoader::EditorSave(m_Root, m_UIPath);
}

void UITab::Update(TimeStep ts) {
	for(auto& panel : m_Panels)
		if(panel->Open)
			panel->Update(ts);
}

void UITab::Render() {
	ImGui::BeginMainMenuBar();
	{
		if(ImGui::BeginMenu("File")) {
			if(ImGui::MenuItem("New", "Ctrl+N"))
				menu.file.newUI = true;
			if(ImGui::MenuItem("Open", "Ctrl+O"))
				menu.file.openUI = true;
			if(ImGui::MenuItem("Save", "Ctrl+S"))
				menu.file.saveUI = true;
			if(ImGui::MenuItem("Save As", "Ctrl+Shift+S"))
				menu.file.saveAs = true;

			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("Edit")) {
			if(ImGui::MenuItem("Add Window", "Ctrl+N"))
				menu.edit.addWindow = true;
			if(ImGui::MenuItem("Add Button", "Ctrl+N"))
				menu.edit.addButton = true;

			ImGui::EndMenu();
		}
		for(auto panel : m_Panels) {
			if(panel->Open)
				continue;

			if(ImGui::BeginMenu("View")) {
				if(ImGui::MenuItem(panel->Name.c_str()))
					panel->Open = true;
				ImGui::EndMenu();
			}
		}
	}
	ImGui::EndMainMenuBar();

	if(menu.file.newUI)
		NewUI();
	if(menu.file.openUI)
		OpenUI();
	if(menu.file.saveUI)
		SaveUI();
	if(menu.file.saveAs)
		SaveAs();

	if(menu.edit.addWindow)
		AddWindow();
	if(menu.edit.addButton)
		AddButton();

	for(auto& panel : m_Panels)
		if(panel->Open)
			panel->Draw();
}

void UITab::Setup() {
	AddPanel<UIHierarchyPanel>(&m_Root)->SetTab(this);
	AddPanel<UIVisualizerPanel>(&m_Root)->SetTab(this);
	AddPanel<UIElementEditorPanel>(&m_Root)->SetTab(this);

	GetPanel("UIHierarchy")->Open = true;
	GetPanel("UIVisualizer")->Open = true;
	GetPanel("UIElementEditor")->Open = true;
}

void UITab::Load(const std::string& path) {
	namespace fs = std::filesystem;

	m_UIPath = path;
	m_Root.Clear();

	UILoader::EditorLoad(m_Root, path, s_Theme);
	m_Name = "UI: " + m_Root.Name;
	GetPanel("UIVisualizer")->As<UIVisualizerPanel>()->SetContext(&m_Root);
}

void UITab::Save(const std::string& path) {
	UILoader::EditorSave(m_Root, path);
	m_Name = "UI: " + m_Root.Name;
	m_UIPath = path;
}

void UITab::NewUI() {
	m_Root.Clear();
	menu.file.newUI = false;
	m_Name = "UI";
}

void UITab::OpenUI() {
	namespace fs = std::filesystem;
	menu.file.openUI = true;

	auto path = Editor::GetProject().Path;
	IGFD::FileDialogConfig config;
	config.path = (fs::path(path) / "Visual" / "UI").string();
	auto instance = ImGuiFileDialog::Instance();
	instance->OpenDialog("ChooseFile", "Choose File", ".magma.ui.json", config);

	if(instance->Display("ChooseFile")) {
		if(instance->IsOk()) {
			std::string path = instance->GetFilePathName();
			Load(path);
		}

		instance->Close();
		menu.file.openUI = false;
	}
}

void UITab::SaveUI() {
	UILoader::EditorSave(m_Root, m_UIPath);
}

void UITab::SaveAs() {
	IGFD::FileDialogConfig config;
	config.path = ".";
	auto instance = ImGuiFileDialog::Instance();
	instance->OpenDialog("ChooseFile", "Choose File", ".magma.ui.json", config);

	if(instance->Display("ChooseFile")) {
		if(instance->IsOk()) {
			std::string path = instance->GetFilePathName();
			Save(path);
		}

		instance->Close();
		menu.file.saveUI = false;
	}
}

void UITab::AddWindow() {
	menu.edit.addWindow = false;

}

void UITab::AddButton() {
	menu.edit.addButton = false;

}

void UITab::AddImage() {
	menu.edit.addImage = false;

}

}