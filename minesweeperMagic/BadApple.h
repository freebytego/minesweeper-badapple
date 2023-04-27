#pragma once

#define cimg_use_png

#include "CImg.h"
#include <png.h>
#include <string>
#include <set>
#include <vector>
#include <filesystem>
#include <Windows.h>
#include <WinUser.h>
#include <iostream>
#include <algorithm>

#define SETTINGS_OFFSET 0x5330
#define FIELD_ARRAY_OFFSET 0x5340
#define CURRENT_MINES_OFFSET 0x5194
#define DRAW_ALL_BLOCKS_OFFSET 0x1c3e
#define REVEAL_ALL_MINES_OFFSET 0x2f80
#define FINISH_GAME_OFFSET 0x347c
#define DRAW_REMAINING_MINES_OFFSET 0x2801

#define FIELD_ROWS 27
#define FIELD_COLS 32

struct fieldSettings_t {
	int mines;
	int width;
	int height;
};

namespace BadApple 
{
	class BadApple
	{
	private:
		static std::string locationBase;
		static std::vector<std::string> framesPath;
	public:
		static std::vector<cimg_library::CImg<float>> frames;
		static void initialize(std::string _locationBase);
		static void prepareFrames(const fieldSettings_t& settings);
	};
}

namespace Minesweeper
{
	class Game
	{
	private:
		HMODULE processHandle{};
		DWORD pID{};
		HANDLE minesweeper{};
		uintptr_t moduleBase;

		typedef void(__stdcall* _DrawAllBlocks)();
		_DrawAllBlocks DrawAllBlocks;

		typedef void(__stdcall* _RevealAllBombs)(BYTE type);
		_RevealAllBombs RevealAllBombs;

		typedef void(__stdcall* _FinishGame)(bool isWin);
		_FinishGame FinishGame;

		typedef void(__stdcall* _DrawRemainingMines)();
		_DrawRemainingMines DrawRemainingMines;
	public:
		Game();
		void ShowLastError();
		_DrawAllBlocks drawAllBlocks{};
		_RevealAllBombs revealAllBombs{};
		_FinishGame finishGame{};
		_DrawRemainingMines drawRemainingMines{};
		void setMinesRemaining(int mines);

		HANDLE getMinesweeper() {
			return minesweeper;
		}
		uintptr_t getModuleBase() {
			return moduleBase;
		}
	};

	class Field
	{
	private:
		Game* game;
		byte field[FIELD_ROWS][FIELD_COLS]{};
		fieldSettings_t fieldSettings;
		void initializeFieldSettings();
	public:
		Field(Game* game) : game(game)
		{
			initializeFieldSettings();
		}
		const fieldSettings_t& getFieldSettings();
		void createEmptyField();
		void writeCurrentField();
		int createFieldFromPng(cimg_library::CImg<float>& image);
	};
}