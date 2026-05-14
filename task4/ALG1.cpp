#include <iostream>
#include <chrono>
#include <cstdlib>
#include <vector>
#include <thread>
#include <iomanip>

int divide(std::vector<int>& arr, int begin, int end)
{
	int root = arr[end];
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
		int pivot = divide(arr, begin, end);
		Qsort(arr, begin, pivot - 1);
		Qsort(arr, pivot + 1, end);
	}
}

void Qsort_parallel(std::vector<int>& arr, int begin, int end, int num_threads)
{
	if (end > begin)
	{
		int pivot = divide(arr, begin, end);

		if (num_threads > 1)
		{
			int left_threads = num_threads / 2;
			int right_threads = num_threads - left_threads;

			std::thread left(Qsort_parallel, std::ref(arr), begin, pivot - 1, left_threads);
			std::thread right(Qsort_parallel, std::ref(arr), pivot + 1, end,  right_threads);
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

template<typename Func, typename... Args>
double bench_algorithm(Func&& f, Args&&... a)
{
	auto start = std::chrono::high_resolution_clock::now();
	f(std::forward<Args>(a)...);
	auto end = std::chrono::high_resolution_clock::now();
	double time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9;
	return time;
}

int main()
{
	int mthreads = 8;
	for (int i = 10; i < 1e+8; i*=10)
	{
		std::vector<double> p = {};
		for (int t = 1; t <= mthreads; t *= 2)
		{
			std::vector<int> arr = {};
			for (int j = 0; j < i; j++)
			{
				arr.push_back(rand());
			}
			p.push_back(bench_algorithm(Qsort_parallel, arr, 0, i - 1, t));
		}
		for (double time : p)
		{
			std::cout << std::fixed << std::setprecision(7) << time << '|' <<
				std::fixed << std::setprecision(7) << p[0] / time << ' ';
		}
		std::cout << '\n';
	}
}