#include "nibbler.hpp"

void loop(Game &game)
{
	std::string line;

	std::cout << game.area << std::endl;
	while (std::getline(std::cin, line) && line != "q")
	{
		game.snake.grow(game.area);
		if (line == "w")
			game.snake.moveTop(game.area, game.foods);
		else if (line == "a")
			game.snake.moveLeft(game.area, game.foods);
		else if (line == "s")
			game.snake.moveBottom(game.area, game.foods);
		else if (line == "d")
			game.snake.moveRight(game.area, game.foods);
		else
			game.snake.moveForward(game.area, game.foods);
		std::cout << game.area << std::endl;
	}
}

void startGame(const Options &options)
{
	Game game = {.area = {options.areaSize}};

	game.snake.setId();
	game.snake.setSnakeOnArea(game.area);

	game.foods.addRandomFood(game.area);

	game.walls.addRandomWall(game.area);
	game.walls.addRandomWall(game.area);
	game.walls.addRandomWall(game.area);

	loop(game);
	// libA();
}

int parsingErrorHandler(const Exception::ParsingOptions &e)
{
	int exitStatus = e.getExitStatus();
	if (exitStatus == EXIT_FAILURE)
		std::cerr << "\033[0;31mError: \033[0m" << e.what() << std::endl;
	else
		std::cout << e.what() << std::endl;
	return exitStatus;
}

int main(int argc, const char *argv[])
{
	Options options;

	srand(time(NULL));
	try
	{
		options = parseCommandLine(argc, argv);
	}
	catch (const Exception::ParsingOptions &e)
	{
		return parsingErrorHandler(e);
	}
	catch (const std::exception &e)
	{
		std::cerr << "\033[0;31mError: \033[0m" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	try
	{
		startGame(options);
	}
	catch (const std::exception &e)
	{
		std::cerr << "\033[0;31mError: \033[0m" << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
