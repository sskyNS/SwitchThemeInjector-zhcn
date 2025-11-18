#pragma once
#include "SDL.hpp"
#include <cstdarg>
#include <cstddef>
#include <cstring>
#include <string>
#include <switch.h>

class SDLConsole
{
    public:
        // No copying.
        SDLConsole(const SDLConsole &) = delete;
        SDLConsole(SDLConsole &&) = delete;
        SDLConsole &operator=(const SDLConsole &) = delete;
        SDLConsole &operator=(SDLConsole &&) = delete;

        static bool Initialize(const char* title = "NXThemes Installer", int width = 1280, int height = 720)
        {
            if (!SDL::Initialize(title, width, height))
                return false;
            
            if (!SDL::Text::Initialize())
            {
                SDL::Exit();
                return false;
            }
            
            SDLConsole &Instance = SDLConsole::GetInstance();
            Instance.m_Initialized = true;
            Instance.m_MaxLineCount = 30; // 最大行数
            Instance.m_FontSize = 24;     // 字体大小
            Instance.m_ClearColor = {0x000000FF}; // 黑色背景
            return true;
        }

        static void Exit()
        {
            SDLConsole &Instance = SDLConsole::GetInstance();
            if (Instance.m_Initialized)
            {
                SDL::Text::Exit();
                SDL::Exit();
                Instance.m_Initialized = false;
            }
        }

        static void SetMaxLineCount(size_t MaxLineCount)
        {
            SDLConsole &Instance = SDLConsole::GetInstance();
            Instance.m_MaxLineCount = MaxLineCount;
        }

        static void SetFontSize(int FontSize)
        {
            SDLConsole &Instance = SDLConsole::GetInstance();
            Instance.m_FontSize = FontSize;
        }

        static void SetClearColor(SDL::Color Color)
        {
            SDLConsole &Instance = SDLConsole::GetInstance();
            Instance.m_ClearColor = Color;
        }

        // 支持中文输出的Printf函数
        static void Printf(const char *Format, ...)
        {
            SDLConsole &Instance = SDLConsole::GetInstance();
            if (!Instance.m_Initialized) return;

            char VaBuffer[0x1000];
            std::va_list VaList;
            va_start(VaList, Format);
            vsnprintf(VaBuffer, 0x1000, Format, VaList);
            va_end(VaList);

            // 计算新行数
            size_t NewLineCount = 0;
            char *NewLineSearch = VaBuffer;
            while ((NewLineSearch = std::strchr(NewLineSearch, '\n')) != NULL)
            {
                ++NewLineCount;
                ++NewLineSearch;
            }

            Instance.m_LineCount += NewLineCount;

            // 如果行数超过最大限制，删除最旧的行
            if (Instance.m_LineCount >= Instance.m_MaxLineCount)
            {
                size_t LinesToTrim = Instance.m_LineCount - Instance.m_MaxLineCount;

                for (size_t i = 0, CurrentLinePosition = 0; i < LinesToTrim; i++)
                {
                    CurrentLinePosition = Instance.m_ConsoleText.find_first_of('\n', CurrentLinePosition) + 1;
                    if (CurrentLinePosition != Instance.m_ConsoleText.npos)
                    {
                        Instance.m_ConsoleText.erase(0, CurrentLinePosition);
                    }
                    else
                    {
                        break;
                    }
                    ++CurrentLinePosition;
                    Instance.m_LineCount -= LinesToTrim;
                }
            }
            
            // 追加缓冲区到控制台输出
            Instance.m_ConsoleText += VaBuffer;
            Instance.Render();
        }

        static void Reset(void)
        {
            SDLConsole &Instance = SDLConsole::GetInstance();
            Instance.m_LineCount = 0;
            Instance.m_ConsoleText.clear();
        }

        static void RenderFrame()
        {
            SDLConsole &Instance = SDLConsole::GetInstance();
            Instance.Render();
        }

    private:
        // 禁止构造函数
        SDLConsole(void) = default;
        
        // 获取单例实例
        static SDLConsole &GetInstance(void)
        {
            static SDLConsole Instance;
            return Instance;
        }
        
        // 渲染函数
        static void Render(void)
        {
            SDLConsole &Instance = SDLConsole::GetInstance();
            if (!Instance.m_Initialized) return;

            // 开始帧并清除背景
            SDL::FrameBegin(Instance.m_ClearColor);
            
            // 渲染文本，支持中文
            // 使用较大的字体大小以确保中文清晰显示
            SDL::Text::Render(NULL, 20, 20, Instance.m_FontSize, 1240, {0xFFFFFFFF}, Instance.m_ConsoleText.c_str());
            
            // 结束帧并显示
            SDL::FrameEnd();
        }
        
        // 初始化状态
        bool m_Initialized = false;
        
        // 行数统计
        size_t m_LineCount = 0;
        size_t m_MaxLineCount = 0;
        
        // 字体大小
        int m_FontSize = 20;
        
        // 背景颜色
        SDL::Color m_ClearColor = {0x000000FF};
        
        // 控制台文本内容
        std::string m_ConsoleText;
};