// dllmain.cpp : Определяет точку входа для приложения DLL.
#include <Windows.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <WinUser.h>
#include <iostream>
#include <filesystem>
#include <set>
#include <chrono>
#include <thread>
#include "BadApple.h"

DWORD WINAPI MainThread(LPVOID params)
{
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
 
    BadApple::BadApple::initialize(".\\bad_apple");
    Minesweeper::Game game{};
    Minesweeper::Field field{ &game };

    field.createEmptyField();
    field.writeCurrentField();

    auto fieldSettings = field.getFieldSettings();
    BadApple::BadApple::prepareFrames(fieldSettings);

    auto frameDuration = duration_cast<std::chrono::system_clock::duration>(std::chrono::duration<double>{ 1.0 / 30 });
    auto frameStart = std::chrono::system_clock::now();
    auto frameEnd = frameStart + frameDuration;
    int frameCount = 0;
    auto prevSeconds = std::chrono::time_point_cast<std::chrono::seconds>(frameStart);

    while (true) {
        for (auto& frame : BadApple::BadApple::frames) {
            int mines = field.createFieldFromPng(frame);
            game.drawRemainingMines();
            game.revealAllBombs(0xA);
            game.setMinesRemaining(mines);
            
            auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
            ++frameCount;
            if (seconds > prevSeconds)
            {
                frameCount = 0;
                prevSeconds = seconds;
            }

            std::this_thread::sleep_until(frameEnd);
            frameStart = frameEnd;
            frameEnd = frameStart + frameDuration;
        }
    }

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        CreateThread(0, 0, MainThread, hModule, 0, 0);
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

