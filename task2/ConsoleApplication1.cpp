#include <iostream>
#include <cmath>
#include <random>
double f1(double x) { return  3*sin(x) + 5; }
double f2(double x) { return -2*cos(x) + 3; }
double monte_carlo(int n)
{
	// variant 20
	std::default_random_engine gen;
	std::uniform_real_distribution<double> xdist(1.0, 2.0);
	std::uniform_real_distribution<double> ydist(1.0, 9.0);

	double area = (2 - 1) * (9 - 1);
	int est = 0;

	for (int i = 0; i < n; i++)
	{
		double x = xdist(gen);
		double y = ydist(gen);
		if (y < f1(x) and y > f2(x)) est++;
	}
	return area * est / n;
}

int main()
{
	for (int i = 1; i < 8; i++) std::cout << monte_carlo(pow(10, i)) << '\n';
}