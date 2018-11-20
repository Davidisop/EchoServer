#include <iostream>
#include <string>
using namespace std;

class Shape
{
public:
	void Draw() { cout << "½¦ÀÌÇÁ Draw" << endl; }
	Shape() {}
	~Shape() {}
};

class Triangle : public Shape
{
public:
	void Draw() { cout << "»ï°¢Çü Draw" << endl; }
	Triangle() {}
	~Triangle() {}
};

class Rectangle : public Shape
{
public:
	void Draw() { cout << "»ç°¢Çü Draw" << endl; }
	Rectangle() {}
	~Rectangle() {}
};



int main()
{

	Shape* a[2];
	a[0] = new Triangle;
	a[1] = new Rectangle;

	a[0]->Draw();
	a[1]->Draw();

	delete a[0];
	delete a[1];

	return 0;
}

