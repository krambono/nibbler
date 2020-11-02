#include "Retro.hpp"

Retro::Retro(): _win(nullptr)
{}

Retro::~Retro()
{
	this->close();
}

void Retro::init(GameData &gData)
{
	setlocale(LC_ALL, "");
	if (!initscr())
		throw std::runtime_error("Failed to initialize ncurses");
	start_color();
	noecho();
	raw();
	curs_set(0);
	refresh();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);
	init_pair(5, COLOR_WHITE, COLOR_BLACK);
	_win = newwin(gData.area.height() + 2, gData.area.width() * 2 + 2, 1, 1);
	if (!_win)
		throw std::runtime_error("Failed to create window");
	keypad(_win, true);
}

void Retro::close()
{
	if (_win)
	{
		delwin(_win);
		_win = nullptr;
	}
	endwin();
}

Input Retro::getInput()
{
	const std::unordered_map<int, Input> map = {
		{ 27, Input::EXIT },                                // EXIT
		{ KEY_UP, Input::UP },       { 'w', Input::UP },    // UP
		{ KEY_RIGHT, Input::RIGHT }, { 'd', Input::RIGHT }, // RIGHT
		{ KEY_DOWN, Input::DOWN },   { 's', Input::DOWN },  // DOWN
		{ KEY_LEFT, Input::LEFT },   { 'a', Input::LEFT },  // LEFT
	};

	wtimeout(_win, 0);
	int c = wgetch(_win);

	for (const auto &[key, input]: map)
		if (c == key)
			return input;

	return Input::NONE;
}

void Retro::render(GameData &gData)
{
	wclear(_win);
	wattron(_win, COLOR_PAIR(5));
	box(_win, 0, 0);

	for (int y = 0; y < gData.area.height(); y++)
	{
		for (int x = 0; x < gData.area.width(); x++)
		{
			Position    pos = { x, y };
			std::string color;

			if (gData.area.isWall(pos))
				wattron(_win, COLOR_PAIR(1));
			else if (gData.area.isFood(pos))
				wattron(_win, COLOR_PAIR(2));
			else if (gData.area.isSnake(pos))
				gData.snake.isHead(pos) ? wattron(_win, COLOR_PAIR(4)) : wattron(_win, COLOR_PAIR(3));

			if (!gData.area.isFree(pos))
				mvwprintw(_win, y + 1, x * 2 + 1, "◼");
		}
	}
	wrefresh(_win);
}
