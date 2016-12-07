#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <thread>

#include "Periodic.hpp"

void f()
{
	std::cout << "f()\n";
}

void q()
{
	std::cout << "q()\n";
}

void w()
{
	std::cout << "w()\n";
}

void fancy()
{
	std::cout << "fancy()\n";
}

void spam()
{
	std::cout << "spam()\n";
}

int main()
{
	Periodic spammer;
	spammer.addFunction(spam, 3345000, true);

	{
		Periodic periodic;
		periodic.addFunction(f, 5000000, true);
		periodic.addFunction(q, 3000000, true);
		periodic.addFunction(w, 1000000, false);

		Periodic per;
		per.addFunction(fancy, 2000000, true);

		std::this_thread::sleep_for(std::chrono::milliseconds{ 15000 });
	}

	std::cin.get();
}