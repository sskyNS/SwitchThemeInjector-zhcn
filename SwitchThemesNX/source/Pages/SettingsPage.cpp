#include "SettingsPage.hpp"
#include "../ViewFunctions.hpp"
#include "../Platform/Platform.hpp"

using namespace std;

namespace Settings {
	bool UseIcons = true;
	bool UseCommon = true;
	SwitchThemesCommon::LayoutCompatibilityOption HomeMenuCompat = SwitchThemesCommon::LayoutCompatibilityOption::Default;
};

SettingsPage::SettingsPage() 
{
	Name = "设置";
}

void SettingsPage::Render(int X, int Y)
{
	Utils::ImGuiSetupWin(Name.c_str(), X, Y, DefaultWinFlags);
	ImGui::SetWindowSize(ImVec2(SCR_W - (float)X - 30, SCR_H - (float)Y - 70));
	ImGui::PushFont(font25);

	ImGui::PushFont(font30);
	ImGui::TextUnformatted("NX主题设置");
	ImGui::PopFont();
	ImGui::TextWrapped("这些设置仅适用于安装nx主题，不会保存，每次启动应用时都需要重新设置。此设置允许您安装为比当前运行版本更旧的固件版本制作的主题。");
	ImGui::Checkbox("启用自定义图标", &Settings::UseIcons);
	PAGE_RESET_FOCUS;
	ImGui::Checkbox("启用额外布局 (如 common.szs)", &Settings::UseCommon);

	ImGui::NewLine();
	ImGui::Text("主菜单兼容性选项。");
	ImGui::TextWrapped("更改此选项可帮助解决最新固件上旧主题的安装问题。");
	ImGui::RadioButton("自动决定 (默认)", (int*)&Settings::HomeMenuCompat, (int)SwitchThemesCommon::LayoutCompatibilityOption::Default);
	ImGui::RadioButton("强制使用原始主菜单小程序图标 (固件 <= 10.0)", (int*)&Settings::HomeMenuCompat, (int)SwitchThemesCommon::LayoutCompatibilityOption::Firmware10);
	ImGui::RadioButton("强制使用带NS在线图标的主菜单布局 (固件 11.0)", (int*)&Settings::HomeMenuCompat, (int)SwitchThemesCommon::LayoutCompatibilityOption::Firmware11);
	ImGui::RadioButton("不应用兼容性修复", (int*)&Settings::HomeMenuCompat, (int)SwitchThemesCommon::LayoutCompatibilityOption::DisableFixes);

	ImGui::NewLine();

	ImGui::PopFont();
	Utils::ImGuiCloseWin();
}

void SettingsPage::Update()
{	
	if (Utils::PageLeaveFocusInput(true))
	{
		Parent->PageLeaveFocus(this);
		return;
	}
}









