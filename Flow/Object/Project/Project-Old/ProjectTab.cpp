#include "ProjectTab.h"

#include <thread>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <Magma/Core/YAMLSerializer.h>

#include <Lava/Core/App.h>

#include "Editor/EditorApp.h"
#include "Editor/Tab.h"
#include "Editor/AssetImporter.h"
#include "Editor/SceneLoader.h"
#include "Editor/ScriptManager.h"

#include "Scene/SceneHierarchyPanel.h"
#include "Scene/SceneVisualizerPanel.h"
#include "Scene/ComponentEditorPanel.h"

#include "ContentBrowserPanel.h"
#include "AssetEditorPanel.h"
#include "ScriptEditorPanel.h"
#include "ConsolePanel.h"

using namespace Lava;

namespace Magma {

struct {
	struct {
		bool newScene = false;
		bool openScene = false;
		bool saveScene = false;
		bool saveAsScene = false;
	} file;
	struct {
		bool addEntity = false;
	} edit;
} static menu;

static Ref<std::thread> s_AppThread;
static std::mutex s_Mutex;
static std::condition_variable s_Condition;
static bool s_Updated = false;

static TimeStep s_TimeStep;
static ScreenState s_State;

ProjectTab::ProjectTab()
	: Tab(TabType::Project)
{
	AddPanel<ContentBrowserPanel>()->SetTab(this);
	AddPanel<ConsolePanel>()->SetTab(this);
	AddPanel<ScriptEditorPanel>()->SetTab(this);
	AddPanel<AssetEditorPanel>()->SetTab(this);
	GetPanel("Console")->As<ConsolePanel>()->Open = true;
	GetPanel("AssetEditor")->As<AssetEditorPanel>()->Open = true;
	GetPanel("ContentBrowser")->As<ContentBrowserPanel>()->Open = true;

	m_Name = Editor::GetProject().Name;
	Setup();
}

ProjectTab::~ProjectTab() {
	OnStop();

	auto path = Editor::GetProject().Path;
	Application::PushDir(path);
	ImGui::SaveIniSettingsToDisk("Editor/layout.ini");
	Application::PopDir();
}

void ProjectTab::Setup() {
	Application::PushDir();
	m_PlayButton.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/PlayButton.png"));
	m_PauseButton.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/PauseButton.png"));
	m_ResumeButton.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/ResumeButton.png"));
	m_StopButton.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/StopButton.png"));
	Application::PopDir();

	auto path = Editor::GetProject().Path;

	auto runPath = (fs::path(path) / "Editor" / "Run.yaml").string();
	if(FileUtils::FileExists(runPath)) {
		YAML::Node file;
		try {
			file = YAML::LoadFile(runPath);
		}
		catch(YAML::ParserException e) {
			VOLCANICORE_ASSERT_ARGS(false, "Could not load file %s: %s",
									path.c_str(), e.what());
		}
	
		for(auto sceneIterNode : file["Scenes"]) {
			auto node = sceneIterNode["Scene"];
			m_Configs.SceneConfigs.Emplace(
				node["Name"].as<std::string>(),
				node["Screen"].as<std::string>(),
				node["UI"].as<std::string>()
			);
		}
	
		for(auto uiIterNode : file["UIPages"]) {
			auto node = uiIterNode["UI"];
			m_Configs.UIConfigs.Emplace(
				node["Name"].as<std::string>(),
				node["Screen"].as<std::string>(),
				node["UI"].as<std::string>()
			);
		}
	}

	auto layoutPath = (fs::path(path) / "Editor" / "layout.ini").string();
	if(FileUtils::FileExists(layoutPath)) {
		Application::PushDir(path);
		ImGui::LoadIniSettingsFromDisk("Editor/layout.ini");
		Application::PopDir();
	}
}

void ProjectTab::Update(TimeStep ts) {
	if(m_Debugging) {
		std::lock_guard<std::mutex> lock(s_Mutex);
		s_TimeStep = ts;
		s_State = m_ScreenState;
		s_Updated = true;
		s_Condition.notify_one();
	}
	else if(App::Get()->Running)
		App::Get()->OnUpdate(ts);

	for(auto panel : m_Panels)
		panel->Update(ts);
}

void ProjectTab::Render() {
	ImGui::BeginMainMenuBar();
	{
		if(ImGui::BeginMenu("File")) {
			if(ImGui::MenuItem("New", "Ctrl+N"))
				menu.file.newScene = true;
			if(ImGui::MenuItem("Open", "Ctrl+O"))
				menu.file.openScene = true;
			if(ImGui::MenuItem("Save", "Ctrl+S"))
				menu.file.saveScene = true;
			if(ImGui::MenuItem("Save As", "Ctrl+Shift+S"))
				menu.file.saveAsScene = true;

			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("Edit")) {

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

	auto flags = ImGuiWindowFlags_NoDecoration
			//    | ImGuiWindowFlags_NoMove
			//    | ImGuiWindowFlags_NoDocking
			;
	ImGui::Begin("##Project", nullptr, flags);
	{
		ImGui::SeparatorText("Project Settings");
		auto& proj = Editor::GetProject();
		ImGui::Text(proj.Name.c_str());

		ImGui::InputText("Start Screen", &proj.StartScreen);
	}
	ImGui::End();

	RenderEssentialPanels();
}

void ProjectTab::RenderEssentialPanels() {
	if(GetPanel("ScriptEditor")->Open)
		GetPanel("ScriptEditor")->Draw();

	GetPanel("ContentBrowser")->Draw();
	GetPanel("Console")->Draw();
	GetPanel("AssetEditor")->Draw();
}

void ProjectTab::RenderButtons() {
	auto type = Editor::GetCurrentTab()->Type;
	if(type != TabType::Scene && type != TabType::UI)
		return;

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));

	float x = Application::GetWindow()->GetWidth() - 90.0f;
	float y = 23.0f;
	float size = 17.5f;

	UI::Button* button = &m_PlayButton;
	if(m_ScreenState == ScreenState::Play)
		button = &m_PauseButton;
	else if(m_ScreenState == ScreenState::Pause)
		button = &m_ResumeButton;

	button->x = x;
	button->y = y;
	button->SetSize(size, size);
	button->Render();

	ImGui::SameLine();
	m_StopButton.x = x + size + 7.0f;
	m_StopButton.y = y;
	m_StopButton.SetSize(size, size);
	m_StopButton.Render();

	if(button->GetState().Clicked)
		if(button == &m_PlayButton)
			OnPlay();
		else if(button == &m_ResumeButton)
			OnResume();
		else
			OnPause();

	if(m_StopButton.GetState().Clicked)
		OnStop();

	ImGui::PopStyleColor(3);
}

void ProjectTab::OnPlay(bool debug) {
	Ref<Tab> current = Editor::GetCurrentTab();
	
	if(current->Type == TabType::Scene) {
		auto tab = current->As<SceneTab>();
		auto* scene = tab->GetScene();
		auto [found, idx] = m_Configs.SceneConfigs.Find(
			[&](auto& element) -> bool
			{
				return element.Name == scene->Name;
			});

		if(!found) {
			VOLCANICORE_LOG_WARNING("Cound not find screen for scene '%s'",
				scene->Name.c_str());
			return;
		}

		auto screen = m_Configs.SceneConfigs[idx].Screen;
		m_ScreenState = ScreenState::Play;
		tab->SaveScene();

		App::Get()->PrepareScreen();
		tab->Test(App::Get()->GetScene());

		m_Debugging = debug;
		if(m_Debugging) {
			ScriptManager::StartDebug();

			s_Updated = false;
			s_AppThread = CreateRef<std::thread>(
				[tab, scene, screen, this]()
				{
					App::Get()->Running = true;
					App::Get()->OnLoad();
					App::Get()->LoadScene(scene);
					App::Get()->ScreenSet(screen);

					while(true) {
						std::unique_lock<std::mutex> lock(s_Mutex);
						s_Condition.wait(lock, []() { return s_Updated; });
						s_Updated = false;

						if(s_State == ScreenState::Play)
							App::Get()->OnUpdate(s_TimeStep);
						else if(s_State == ScreenState::Pause)
							continue;
						else if(s_State == ScreenState::Edit)
							break;
					}

					App::Get()->OnClose();
					App::Get()->Running = false;

					asThreadCleanup();
				});

			s_AppThread->detach();
		}
		else {
			App::Get()->Running = true;
			App::Get()->OnLoad();
			App::Get()->LoadScene(scene);
			App::Get()->ScreenSet(screen);
		}
	}
}

void ProjectTab::OnPause() {
	m_ScreenState = ScreenState::Pause;
	if(m_Debugging) {
		std::lock_guard<std::mutex> lock(s_Mutex);
		s_State = m_ScreenState;
		s_Updated = true;
		s_Condition.notify_one();
	}
	else
		App::Get()->Running = false;
}

void ProjectTab::OnResume() {
	m_ScreenState = ScreenState::Play;
	if(m_Debugging) {
		std::lock_guard<std::mutex> lock(s_Mutex);
		s_State = m_ScreenState;
		s_Updated = true;
		s_Condition.notify_one();
	}
	else
		App::Get()->Running = true;
}

void ProjectTab::OnStop() {
	if(m_ScreenState == ScreenState::Edit)
		return;

	m_ScreenState = ScreenState::Edit;
	if(m_Debugging) {
		{
			std::lock_guard<std::mutex> lock(s_Mutex);
			s_State = m_ScreenState;
			s_Updated = true;
			s_Condition.notify_one();
		}

		s_AppThread.reset();
		ScriptManager::EndDebug();
		m_Debugging = false;
	}
	else {
		App::Get()->OnClose();
		App::Get()->Running = false;
	}

	Ref<Tab> current = Editor::GetCurrentTab();
	if(current->Type == TabType::Scene) {
		auto tab = current->As<SceneTab>();
		tab->Reset();
	}
}

}