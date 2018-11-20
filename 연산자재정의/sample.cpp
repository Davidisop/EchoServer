#include "iostream"

class point
{
public:
	int x;
	int y;

public:
	point() {}
	point(int x_in, int y_in) :x(x_in), y(y_in) {}
	point(const point & david) : x(david.x), y(david.y) {}
	
	void show_position()
	{
		std::cout << '[' << x << ' ' << y << ']' << std::endl;
	}

	point operator* (int times_in)
	{
	
		this->x *= times_in;
		this->y *= times_in;
		return *(this);
	}

	point operator + (const point &yang)
	{
	
		this->x = this->x + yang.x;
		this->y = this->y + yang.y;
		return *(this);
	}

	point operator - (const point &yang)
	{
		this->x = this->x - yang.x;
		this->y = this->y - yang.y;
		return *(this);
	}
};

int main(void)
{
	point pos(1, 2);
	pos.show_position();

	point david;
	david.x = 10;
	david.y = 10;

	david.show_position();

	david = david + pos;
	david.show_position();

	david = david - pos;
	david.show_position();

	david = david * 10;
	david.show_position();


	return 0;
}