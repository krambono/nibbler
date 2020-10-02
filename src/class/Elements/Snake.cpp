#include "Snake.hpp"

Snake::Snake() : _direction(Direction::Right) {}

Snake::Snake(const Snake &snake)
{
	*this = snake;
}

Snake::~Snake() {}

Snake &Snake::operator=(const Snake &snake)
{
	this->_direction = snake._direction;
	return *this;
}

void Snake::move(Area &area, const Position &newHead, Foods &foods, Cron &cron)
{
	if (area.isDanger(newHead))
		throw Exception::GameOver();

	Position tail = this->_positions.back();
	area[tail].reset();
	this->_positions.pop_back();

	if (area.isFood(newHead))
	{
		foods.removeFood(newHead, area);
		CronData data;
		data.eventFunction = Snake::growFromCron;
		data.args = std::make_shared<GrowArgs>(GrowArgs({this, &area, this->_positions.back()}));
		cron.addEvent(data, 1);
		data.eventFunction = Foods::addRandomFoodFromCron;
		data.args = std::make_shared<AddRandomFoodArgs>(AddRandomFoodArgs({&foods, &area}));
		cron.addEvent(data, 0);
	}
	this->_positions.push_front(newHead);
	area[newHead].id = this->_id;
	area[newHead].type = ElementType::SnakeT;
}

void Snake::moveTop(Area &area, Foods &foods, Cron &cron, bool forward)
{
	if (!forward && (this->_direction == Direction::Top || this->_direction == Direction::Bottom))
	{
		if (this->_direction == Direction::Top)	  // to remove
			this->moveForward(area, foods, cron); // to remove
		return;
	}

	Position newHead = this->_positions.front();
	newHead.y--;
	this->move(area, newHead, foods, cron);
	this->_direction = Direction::Top;
}

void Snake::moveRight(Area &area, Foods &foods, Cron &cron, bool forward)
{
	if (!forward && (this->_direction == Direction::Left || this->_direction == Direction::Right))
	{
		if (this->_direction == Direction::Right) // to remove
			this->moveForward(area, foods, cron); // to remove
		return;
	}

	Position newHead = this->_positions.front();
	newHead.x++;
	this->move(area, newHead, foods, cron);
	this->_direction = Direction::Right;
}

void Snake::moveBottom(Area &area, Foods &foods, Cron &cron, bool forward)
{
	if (!forward && (this->_direction == Direction::Top || this->_direction == Direction::Bottom))
	{
		if (this->_direction == Direction::Bottom) // to remove
			this->moveForward(area, foods, cron);  // to remove
		return;
	}

	Position newHead = this->_positions.front();
	newHead.y++;
	this->move(area, newHead, foods, cron);
	this->_direction = Direction::Bottom;
}

void Snake::moveLeft(Area &area, Foods &foods, Cron &cron, bool forward)
{
	if (!forward && (this->_direction == Direction::Left || this->_direction == Direction::Right))
	{
		if (this->_direction == Direction::Left)  // to remove
			this->moveForward(area, foods, cron); // to remove
		return;
	}

	Position newHead = this->_positions.front();
	newHead.x--;
	this->move(area, newHead, foods, cron);
	this->_direction = Direction::Left;
}

void Snake::moveForward(Area &area, Foods &foods, Cron &cron)
{
	std::unordered_map<Direction, void (Snake::*)(Area &, Foods &, Cron &, bool)> fMap =
		{{Direction::Top, &Snake::moveTop},
		 {Direction::Right, &Snake::moveRight},
		 {Direction::Bottom, &Snake::moveBottom},
		 {Direction::Left, &Snake::moveLeft}};
	(this->*fMap[this->_direction])(area, foods, cron, true);
}

void Snake::grow(Area &area, Position oldTail)
{
	area[oldTail].id = this->_id;
	area[oldTail].type = ElementType::SnakeT;
	this->_positions.push_back(oldTail);
}

void Snake::growFromCron(const std::shared_ptr<void> &args)
{
	GrowArgs &data = *(std::static_pointer_cast<GrowArgs>(args));
	(data.snake->Snake::grow)(*data.area, data.oldTail);
}

void Snake::setSnakeOnArea(Area &area)
{
	Size areaSize = area.getSize();
	int y = areaSize.height / 2;
	int x = areaSize.width / 2 - 2;

	for (int i = 3; i >= 0; i--)
	{
		Position pos = {x + i, y};
		Cell &cell = area[pos];
		cell.id = this->_id;
		cell.type = ElementType::SnakeT;
		this->_positions.push_back(pos);
	}
}

bool Snake::isHead(const Position &pos) const
{
	return this->_positions.front() == pos;
}

bool Snake::isTail(const Position &pos) const
{
	return this->_positions.back() == pos;
}
