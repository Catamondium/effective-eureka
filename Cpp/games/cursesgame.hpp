#pragma once
#include <curses.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include "game.hpp"
using Mouse = MEVENT;

struct Color
{
    short id = 0, fg = 0, bg = 0;
    Color() = default;
    Color(short id, short fg, short bg) : id(id), fg(fg), bg(bg){};
};

constexpr std::chrono::milliseconds frameRate(double fps)
{
    return std::chrono::milliseconds((int)(1000 / fps));
}

void termkill()
{
    endwin();
}

void render(int yoff, int xoff, std::string img)
// renders char 'images', with each row newline delimited
{
    int col = 0, line = 0;
    for (auto ch : img)
    {
        if (ch == '\n')
        {
            col = 0;
            ++line;
        }
        else
        {
            mvaddch(line + yoff, col + xoff, ch);
            ++col;
        }
    }
}

template <size_t N = 0>
struct CursesGame : public Game
// 'Realtime' NCurses game
{

    void setFrameRate(double fps)
    {
        frameperiod = frameRate(fps);
    }

    void run() final
    {
        std::ofstream log{log_target};
        old_stdout = std::cout.rdbuf();
        std::cout.rdbuf(log.rdbuf());
        init_pallete();
        init();
        while (true)
        {
            getmaxyx(stdscr, height, width);
            erase();
            loop();
            input();

            if (stoploop)
                break;

            std::this_thread::sleep_for(frameperiod);
        }
        std::cout.rdbuf(old_stdout);
    }

    virtual void input() override
    {
        int ch = getch();
        if (ch == 27)
        { // ESC
            noLoop();
            return;
        }

        if (ch == KEY_MOUSE)
        {
            Mouse rawmouse;
            if (getmouse(&rawmouse) == OK)
                mouseEvent(ch, rawmouse);
        }
        else
        {
            keyPressed(ch);
        }
    }

    CursesGame()
    {
        initscr();
        start_color();

        atexit(termkill);

        cbreak();
        noecho();
        nonl();
        intrflush(stdscr, TRUE);
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        curs_set(0);
        mousemask(ALL_MOUSE_EVENTS, NULL);
        getmaxyx(stdscr, height, width);
    }

    ~CursesGame()
    {
        endwin();
    }

    virtual void keyPressed(int){};
    virtual void mouseEvent(int, Mouse){};
    virtual void init_pallete()
    {
        for (auto col : pallete)
        {
            init_pair(col.id, col.fg, col.bg);
        }
    };

private:
    std::streambuf *old_stdout;

protected:
    int height;
    int width;
    std::string log_target = "game.log";
    std::array<Color, N> pallete;
    std::chrono::milliseconds frameperiod = frameRate(60);
};
