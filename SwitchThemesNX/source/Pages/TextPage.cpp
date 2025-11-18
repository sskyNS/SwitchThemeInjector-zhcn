#include "TextPage.hpp"
#include "../Version.hpp"
#include "../ViewFunctions.hpp"
#include "../UI/imgui/imgui_internal.h"

using namespace std;

TextPage::TextPage(const std::string& title, const std::string& text) :
	Text(text)
{
	Name = title;
	c_str = Text.c_str();
}

TextPage::TextPage(const char* title, const char* text) 
{
	Name = title;
	c_str = text;
}

void TextPage::Render(int X, int Y)
{
	Utils::ImGuiSetupPage(this, X, Y);
	ImGui::PushFont(font25);
	ImGui::TextWrapped(c_str);
	ImGui::PopFont();
	Utils::ImGuiSetWindowScrollable();
	Utils::ImGuiCloseWin();
}

void TextPage::Update()
{
	Parent->PageLeaveFocus(this);
}

CreditsPage::CreditsPage() :
	creditsText("NX主题安装器由 exelix 开发 - " + Version::Name + " - 核心版本 " + SwitchThemesCommon::CoreVer +
		'\n' + Version::Commit +
		"\n源代码: github.com/exelix11/SwitchThemeInjector"+
		"\n捐赠: ko-fi.com/exelix11\n\n")
{
	Name = "致谢";
}

extern void ShowFirstTimeHelp(bool WelcomeScr); //from main.cpp
void CreditsPage::Render(int X, int Y)
{
	Utils::ImGuiSetupPage(this, X, Y);
	ImGui::SetCursorPosY(20);
	ImGui::PushFont(font30);
	ImGui::TextWrapped(creditsText.c_str());
	ImGui::PopFont();

	ImGui::PushFont(font25);
	ImGui::TextWrapped(
		"感谢以下贡献者:\n"
		"Syroot 提供 BinaryData 库\n"
		"AboodXD 提供 Bntx 工具链和 sarc 库\n"
		"shchmue 提供 Lockpick\n"
		"SciresM 提供 hactool\n"
		"Atmosphere 和 libnx 的所有成员\n"
		"github 上 switch-stuff 的字体转换器\n"
		"Fincs 提供 hybrid_app 模板\n"
		"DearImgui github 仓库的所有贡献者"
	);

	// 添加汉化作者信息，使用青色字体
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 255)); // 青色
	ImGui::TextWrapped("汉化作者：B站 三上烤鸭");
	ImGui::PopStyleColor();

	if (ImGui::Button("显示首次启动信息"))
		PushFunction([]() {ShowFirstTimeHelp(false); });
	PAGE_RESET_FOCUS;

	ImGui::PopFont();
	Utils::ImGuiSetWindowScrollable();
	Utils::ImGuiCloseWin();
}

void CreditsPage::Update()
{
	if (Utils::PageLeaveFocusInput())
		Parent->PageLeaveFocus(this);
}

