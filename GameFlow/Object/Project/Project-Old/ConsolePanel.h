#pragma once

#include <Magma/UI/Image.h>

#include "Editor/Panel.h"
#include "Editor/AssetManager.h"

namespace Magma {

enum class ConsoleLevel { Info, Warn, Error };
enum class ConsoleType { Log, Debug };

class ConsolePanel : public Panel {
public:
	ConsolePanel();
	~ConsolePanel() = default;

	void Update(VolcaniCore::TimeStep ts) override;
	void Draw() override;

	void Log(ConsoleLevel level, const std::string& message);

private:
	VolcaniCore::List<std::pair<ConsoleLevel, std::string>> m_Log;
	UI::Image m_Info, m_Warn, m_Error;
};

}