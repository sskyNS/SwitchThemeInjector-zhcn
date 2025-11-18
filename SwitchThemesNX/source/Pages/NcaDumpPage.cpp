#include "NcaDumpPage.hpp"
#include "../ViewFunctions.hpp"
#include "../fs.hpp"
#include "../SwitchTools/hactool.hpp"
#include <filesystem>
#include "../Platform/Platform.hpp"
#include "../SwitchThemesCommon/NXTheme.hpp"

using namespace std;

NcaDumpPage::NcaDumpPage()
{
	Name = "提取主菜单";
}

void NcaDumpPage::Render(int X, int Y)
{
	Utils::ImGuiSetupPage(this, X, Y);
	ImGui::PushFont(font30);

	ImGui::TextWrapped("要安装.nxtheme文件，您需要先提取主菜单。\n"
		"每次固件更改时都需要这样做，无论是升级还是降级。\n"
		"当提取的版本与您的固件不匹配时，系统会提示您执行此操作。\n\n"
		"通常您不需要手动提取，但如果遇到问题，可以在此处尝试。");

	if (ImGui::Button("提取主菜单"))
	{
		PushFunction([]() {
			if ((gamepad.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] && gamepad.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER]))
			{
				DialogBlocking("输入了超级秘密组合，仅会转储主菜单NCA（不会被提取）");
				DisplayLoading("正在提取NCA...");
				if (fs::theme::DumpHomeMenuNca())
					Dialog("主菜单NCA已提取，现在使用注入器完成设置。\n如果您不是故意这样做，请忽略此消息。");
				return;
			}
			if (!YesNoPage::Ask(
			"要安装自定义主题，您需要先提取主菜单，此过程可能需要几分钟，不要让主机进入睡眠模式，也不要按主页按钮。\n"
			"您要继续吗？")) return;
			fs::RemoveSystemDataDir();
			try
			{				
				hactool::ExtractHomeMenu();
				hactool::ExtractPlayerSelectMenu();
				hactool::ExtractUserPage();
				Dialog("完成，主菜单已提取，现在您可以安装nxtheme文件了！");
			}
			catch (std::runtime_error &err)
			{
				DialogBlocking("提取主菜单时出错：" + string(err.what()));
			}
		});
	}
	PAGE_RESET_FOCUS;
	
	ImGui::PopFont();
	Utils::ImGuiCloseWin();
}

void NcaDumpPage::Update()
{	
	if (Utils::PageLeaveFocusInput()){
		Parent->PageLeaveFocus(this);
	}
}

void NcaDumpPage::CheckHomeMenuVer()
{
	if (!filesystem::exists(SD_PREFIX "/themes/systemData/ResidentMenu.szs"))
	{
		DialogBlocking("要安装自定义主题，您需要先提取主菜单，此过程可能需要几分钟，不要让主机进入睡眠模式，也不要按主页按钮。\n按A选项开始");
		goto DUMP_HOMEMENU;
	}
	
	if (filesystem::exists(SD_PREFIX "/themes/systemData/ver.cfg"))
	{
		FILE *ver = fopen(SD_PREFIX "/themes/systemData/ver.cfg", "r");
		if (ver)
		{
			char str[50] = {0};
			fgets(str,49,ver);
			fclose(ver);
			string version(str);
			if (version != SystemVer) goto ASK_DUMP;
			else return;
		}
		else goto ASK_DUMP;
	}
	else if (HOSVer.major >= 7) goto ASK_DUMP;
	else WriteHomeNcaVersion();
	return;
	
ASK_DUMP:
	if (!YesNoPage::Ask("当前固件版本与提取的主菜单版本不同，您要再次提取主菜单吗？\n如果提取的主菜单与安装的不匹配，主题将会崩溃。"))
	{
		DialogBlocking("您将不再看到此消息，如果发生崩溃，您可以从主菜单中的`提取主菜单`选项手动提取主菜单");
		WriteHomeNcaVersion();
		return;
	}
	
DUMP_HOMEMENU:
	fs::RemoveSystemDataDir();
	try
	{
		hactool::ExtractHomeMenu();
	}
	catch (std::runtime_error &err)
	{
		DialogBlocking("提取主菜单时出错：" + string(err.what()));
	}
}

void NcaDumpPage::WriteHomeNcaVersion()
{
	FILE* ver = fopen(fs::path::NcaVersionCfg.c_str(), "w");
	if (!ver)
		return;
	fprintf(ver, "%s", SystemVer.c_str());
	fclose(ver);
}



