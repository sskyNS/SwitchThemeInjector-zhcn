#include "Platform.hpp"
#include <iostream>
#include "../UI/UIManagement.hpp"
#include "../UI/UI.hpp"
#include "../UI/SDLConsole.hpp"
#include <cstring>

#ifdef __SWITCH__
#include <switch.h>
#include <unistd.h>
//#define __NXLINK_ENABLE__

#ifdef __NXLINK_ENABLE__
static int nxlink_sock = -1;
#endif

GLFWgamepadstate gamepad;
GLFWgamepadstate OldGamepad;

bool NAV_UP;
bool NAV_DOWN;
bool NAV_LEFT;
bool NAV_RIGHT;

bool UseLowMemory = false;

void PlatformInit() 
{
	AppletType at = appletGetAppletType();
	if (at != AppletType_Application && at != AppletType_SystemApplication)
	{
		// 使用SDL输出中文错误信息
		if (SDLConsole::Initialize("NX主题安装器 - 内存错误", 1280, 720))
		{
			// 输出中文错误信息
			SDLConsole::Printf("\n\n\n\n");
			SDLConsole::Printf("    ======================================\n");
			SDLConsole::Printf("    NX主题安装器 - 内存错误\n");
			SDLConsole::Printf("    ======================================\n");
			SDLConsole::Printf("\n");
			SDLConsole::Printf("    此自制程序必须在APP模式下运行。\n");
			SDLConsole::Printf("    当前模式: %s\n", at == AppletType_LibraryApplet ? "相册模式" : "其他模式");
			SDLConsole::Printf("\n");
			SDLConsole::Printf("    原因: 相册模式无法提供足够的内存\n");
			SDLConsole::Printf("    来加载字体和资源。\n");
			SDLConsole::Printf("\n");
			SDLConsole::Printf("    解决方案:\n");
			SDLConsole::Printf("    1. 直接从自制程序启动器启动\n");
			SDLConsole::Printf("    2. 避免从相册或其他小程序启动\n");
			SDLConsole::Printf("    3. 确保在CFW中使用APP模式\n");
			SDLConsole::Printf("\n");
			SDLConsole::Printf("    按任意键退出...\n");
			SDLConsole::Printf("    ======================================\n");
			
			// 等待用户按键
			PadState pad;
			padInitializeDefault(&pad);
			while (appletMainLoop())
			{
				padUpdate(&pad);
				u64 kDown = padGetButtonsDown(&pad);
				if (kDown)
					break;
				
				// 渲染当前帧
				SDLConsole::RenderFrame();
			}
			
			SDLConsole::Exit();
		}
		else
		{
			// 如果SDL初始化失败，回退到原始的控制台输出
			consoleInit(NULL);
			consoleClear();
			
			printf("\n\n\n\n");
			printf("    ======================================\n");
			printf("    NX Theme Installer - Memory Error\n");
			printf("    ======================================\n");
			printf("\n");
			printf("    This homebrew must be run in APP mode.\n");
			printf("    Current mode: %s\n", at == AppletType_LibraryApplet ? "Library Applet" : "Other mode");
			printf("\n");
			printf("    Reason: Applet mode doesn't provide enough\n");
			printf("    memory to load fonts and resources.\n");
			printf("\n");
			printf("    Solution:\n");
			printf("    1. Launch directly from homebrew launcher\n");
			printf("    2. Avoid launching from album or applets\n");
			printf("    3. Ensure using APP mode in CFW\n");
			printf("\n");
			printf("    Press any button to exit...\n");
			printf("    ======================================\n");
			
			consoleUpdate(NULL);
			
			PadState pad;
			padInitializeDefault(&pad);
			while (appletMainLoop())
			{
				padUpdate(&pad);
				u64 kDown = padGetButtonsDown(&pad);
				if (kDown)
					break;
			}
			
			consoleExit(NULL);
		}
		
		exit(1); // 强制退出程序
	}

	romfsInit();
	socketInitializeDefault();

	hidInitializeTouchScreen();

#ifdef __NXLINK_ENABLE__
	nxlink_sock = nxlinkStdio();
#endif
}

void PlatformExit() 
{
#ifdef __NXLINK_ENABLE__
	if (nxlink_sock != -1)
		close(nxlink_sock);
#endif
	socketExit();
	romfsExit();
}

void PlatformAfterInit() 
{

}

void PlatformGetInputs()
{
	memcpy(&OldGamepad, &gamepad, sizeof(gamepad));
	if (!glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepad))
		std::cout << "Error reading from gamepad";
}

void PlatformImguiBinds() 
{
	ImGuiIO& io = ImGui::GetIO();
	HidTouchScreenState state = { 0 };
	if (hidGetTouchScreenStates(&state, 1) && state.count)
	{
		auto x = state.touches[0].x / GFX::WRatio;
		auto y = state.touches[0].y / GFX::HRatio;

		io.MousePos = ImVec2(x, y);
		io.MouseDown[0] = true;
	}
	else io.MouseDown[0] = false;
}

void PlatformSleep(float time)
{
	usleep(time * 1000);
}

char InputBuffer[32];
const char* PlatformTextInput(const char* current)
{
	std::memset(InputBuffer, 0, sizeof(InputBuffer));

	SwkbdConfig kbd;

	Result rc = swkbdCreate(&kbd, 0);
	if (R_FAILED(rc))
		throw std::runtime_error("swkbdCreate failed : " + std::to_string(rc));

	swkbdConfigMakePresetDefault(&kbd);
	swkbdConfigSetTextDrawType(&kbd, SwkbdTextDrawType_Line);
	
	if (current)
		swkbdConfigSetInitialText(&kbd, current);

	swkbdConfigSetStringLenMax(&kbd, sizeof(InputBuffer) - 1);
	if (R_FAILED(swkbdShow(&kbd, InputBuffer, sizeof(InputBuffer))))
	{
		std::strncpy(InputBuffer, current, sizeof(InputBuffer));
		InputBuffer[sizeof(InputBuffer) - 1] = 0;
	}

	swkbdClose(&kbd);
	return InputBuffer;
}

void PlatformReboot() 
{
	spsmInitialize();
	spsmShutdown(true);
}
#endif