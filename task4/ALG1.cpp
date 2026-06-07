#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <random>
#include <thread>

void insertion_sort(std::vector<int>& arr)
{
	for (int i = 1; i < arr.size(); i++)
	{
		int j = i;
		while (j > 0 and arr[j] < arr[j - 1])
		{
			int t = arr[j];
			arr[j] = arr[j - 1];
			arr[j - 1] = t;
			j--;
		}
	}
}

void radix_sort(std::vector<int>& arr)
{
	int digit_index = 1;
	int max_ = *(arr.begin());
	for (int i : arr)
	{
		max_ = max_ >= i ? max_ : i;
	}

	while (max_ / digit_index != 0)
	{
		std::vector<std::vector<int>> buckets(10);
		for (int i : arr)
		{
			buckets[(i / digit_index) % 10].push_back(i);
		}
		std::vector<int> partial_sort = {};
		for (std::vector<int> i : buckets) partial_sort.insert(partial_sort.end(), i.begin(), i.end());
		arr = partial_sort;
		digit_index *= 10;
	}
}

int divide(std::vector<int>& arr, int begin, int end)
{
	int root = end - begin < 10 ? arr[end] : arr[(end + begin) / 2];
	int pivot = begin - 1;
	for (int i = begin; i < end; i++)
	{
		if (arr[i] < root)
		{
			pivot++;
			int temp = arr[i];
			arr[i] = arr[pivot]; arr[pivot] = temp;
		}
	}
	int temp = arr[end];
	arr[end] = arr[pivot + 1]; arr[pivot + 1] = temp;
	return pivot + 1;
}
void Qsort(std::vector<int>& arr, int begin, int end)
{
	if (end > begin)
	{
		int pivot = divide(std::ref(arr), begin, end);
		Qsort(arr, begin, pivot - 1);
		Qsort(arr, pivot + 1, end);
	}
}
void Quick_sort(std::vector<int>& arr)
{
	Qsort(std::ref(arr), 0, arr.size() - 1);
}

void Qparallel(std::vector<int>& arr, int begin, int end, int num_threads)
{
	if (end > begin)
	{
		int pivot = divide(arr, begin, end);

		if (num_threads > 1 && end - begin >= 32)
		{
			int left_threads = num_threads / 2;
			int right_threads = num_threads - left_threads;

			std::thread left(Qparallel, std::ref(arr), begin, pivot - 1, left_threads);
			std::thread right(Qparallel, std::ref(arr), pivot + 1, end, right_threads);
			left.join();
			right.join();
		}
		else
		{
			Qsort(arr, begin, pivot - 1);
			Qsort(arr, pivot + 1, end);
		}
	}
}
void Quick_parallel(std::vector<int>& arr, int threads)
{
	Qparallel(std::ref(arr), 0, arr.size() - 1, threads);
}

template <typename Func, typename... Args>
double bench_algorithm(Func&& func, Args&&... args)
{
	auto start = std::chrono::high_resolution_clock::now();
	func(std::forward<Args>(args)...);
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9;
}

int main()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(1, 1000000);

	sf::VertexArray  quick(sf::LineStrip);
	sf::VertexArray  parallel2(sf::LineStrip);
	sf::VertexArray  parallel4(sf::LineStrip);
	sf::VertexArray  parallel8(sf::LineStrip);

	sf::VertexArray xAxis(sf::Lines, 2);
	xAxis[0].position = sf::Vector2f(50, 550); // Начало оси X
	xAxis[0].color = sf::Color::White; // Цвет оси
	xAxis[1].position = sf::Vector2f(750, 550); // Конец оси X
	xAxis[1].color = sf::Color::White;

	sf::VertexArray yAxis(sf::Lines, 2);
	yAxis[0].position = sf::Vector2f(50, 50); // Начало оси Y
	yAxis[0].color = sf::Color::White; // Цвет оси
	yAxis[1].position = sf::Vector2f(50, 550); // Конец оси Y
	yAxis[1].color = sf::Color::White;

	size_t s = 10;
	for (int i = 1; i < 23; i++)
	{
		std::vector<int> arr1(s);
		std::vector<int> arr2(s);
		std::vector<int> arr4(s);
		std::vector<int> arr8(s);

		for (size_t j = 0; j < s; j++)
		{
			int t = dist(gen);
			arr1[j] = t; arr2[j] = t;
			arr4[j] = t; arr8[j] = t;
		}
		int x = 51 + 30 * i;
		float ans1 = bench_algorithm(Quick_sort, std::ref(arr1));
		float ans2 = bench_algorithm(Quick_parallel, std::ref(arr2), 2);
		float ans4 = bench_algorithm(Quick_parallel, std::ref(arr4), 4);
		float ans8 = bench_algorithm(Quick_parallel, std::ref(arr8), 8);

		quick.append(sf::Vertex(sf::Vector2f(x, 550 - 50 * ans1), sf::Color::Cyan));
		parallel2.append(sf::Vertex(sf::Vector2f(x, 550 - 50 * ans2), sf::Color::Red));
		parallel4.append(sf::Vertex(sf::Vector2f(x, 550 - 50 * ans4), sf::Color::Blue));
		parallel8.append(sf::Vertex(sf::Vector2f(x, 550 - 50 * ans8), sf::Color::Green));
		
		printf("%i: %f %f %f %f | n/a %f %f %f\n", i, ans1, ans2, ans4, ans8, ans1 / ans2, ans1 / ans4, ans1 / ans8);
		s <<= 1;
	}

	{
		sf::RenderWindow window(sf::VideoMode(800, 600), "plot");
		sf::Font font;
		if (!font.loadFromFile("arial.ttf")) {
			return -1;
		}

		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window.close();
				}
			}
			window.draw(xAxis);
			window.draw(yAxis);
			
			window.draw(quick);
			window.draw(parallel2);
			window.draw(parallel4);
			window.draw(parallel8);
			window.display();
		}

		return 0;
	}
}
