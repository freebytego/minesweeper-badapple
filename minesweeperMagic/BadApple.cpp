#include "BadApple.h"
#include "alphanum.hpp"

std::vector<std::string> BadApple::BadApple::framesPath{};
std::vector<cimg_library::CImg<float>> BadApple::BadApple::frames{};
std::string BadApple::BadApple::locationBase{};

void BadApple::BadApple::initialize(std::string _locationBase)
{
    locationBase = _locationBase;
    for (const auto& entry : std::filesystem::directory_iterator(BadApple::locationBase)) {
        framesPath.push_back(entry.path().string());
    }
    std::sort(framesPath.begin(), framesPath.end(), doj::alphanum_less<std::string>());
}

void BadApple::BadApple::prepareFrames(const fieldSettings_t& settings)
{
    for (const auto& frameName : framesPath) {
        cimg_library::CImg<float> frame{ frameName.c_str() };
        frame.resize(settings.width, settings.height, -100, -100, 5);
        frames.push_back(frame);
        std::cout << frameName << " resized to " << settings.width << 'x' << settings.height << '\n';
    }
}

Minesweeper::Game::Game()
{
    processHandle = GetModuleHandleW(0);
    pID = GetProcessId(GetCurrentProcess());
    minesweeper = OpenProcess(PROCESS_ALL_ACCESS, 0, pID);
    moduleBase = (uintptr_t)processHandle;
    drawAllBlocks = (_DrawAllBlocks)(moduleBase + DRAW_ALL_BLOCKS_OFFSET);
    revealAllBombs = (_RevealAllBombs)(moduleBase + REVEAL_ALL_MINES_OFFSET);
    finishGame = (_FinishGame)(moduleBase + FINISH_GAME_OFFSET);
    drawRemainingMines = (_DrawRemainingMines)(moduleBase + DRAW_REMAINING_MINES_OFFSET);
}

void Minesweeper::Game::setMinesRemaining(int mines)
{
    auto result = WriteProcessMemory(minesweeper, (LPVOID)(moduleBase + CURRENT_MINES_OFFSET), &mines, sizeof(mines), NULL);
    if (!result) {
        ShowLastError();
    }
}

void Minesweeper::Game::ShowLastError() {
    auto error = GetLastError();
    std::cout << "Error occurred: " << error << '\n';
}

void Minesweeper::Field::createEmptyField()
{
    for (int row = 0; row < FIELD_ROWS; row++) {
        for (int col = 0; col < FIELD_COLS; col++) {
            field[row][col] = 0x0F;
        }
    }
    for (int i = 0; i < (fieldSettings.width + 2); i++) {
        field[0][i] = 0x10;
        field[fieldSettings.height + 1][i] = 0x10;
    }
    for (int row = 1; row <= fieldSettings.height; row++) {
        field[row][0] = 0x10;
        field[row][fieldSettings.width + 1] = 0x10;
    }
}

void Minesweeper::Field::writeCurrentField()
{
    auto result = WriteProcessMemory(game->getMinesweeper(), (LPVOID)(game->getModuleBase() + FIELD_ARRAY_OFFSET), &field, sizeof(field), NULL);
    if (!result) {
        game->ShowLastError();
    }
}

int Minesweeper::Field::createFieldFromPng(cimg_library::CImg<float>& image)
{
    int mines = 0;
    createEmptyField();
    for (int y = 0; y < fieldSettings.height; ++y) {
        for (int x = 0; x < fieldSettings.width; ++x) {
            if (((int)image(x, y) > 1)) {
                field[y + 1][x + 1] = 0x0F;
            }
            else {
                field[y + 1][x + 1] = 0x8F;
                ++mines;
            }
        }
    }
    writeCurrentField();
    return mines;
}

void Minesweeper::Field::initializeFieldSettings() {
    ReadProcessMemory(game->getMinesweeper(), (LPCVOID)(game->getModuleBase() + SETTINGS_OFFSET), &fieldSettings, sizeof(fieldSettings), NULL);
}

const fieldSettings_t& Minesweeper::Field::getFieldSettings() {
    return fieldSettings;
}