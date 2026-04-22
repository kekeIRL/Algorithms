#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>

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
		int temp = arr[end];
		arr[end] = arr[pivot + 1]; arr[pivot + 1] = temp;
	}
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
void Quick_sort(std::vector<int>& arr)
{
	Qsort(arr, 0, arr.size() - 1);
}

double bench_algorithm_sorted(void (*func)(std::vector<int>&), int arr_size)
{
	std::vector<int> arr(arr_size);
	for (int i = 0; i < arr.size(); i++) arr[i] = i; //sorted array
	auto start = std::chrono::high_resolution_clock::now();
	func(arr);
	auto end = std::chrono::high_resolution_clock::now();
	double time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9;
	return time;
}
double bench_algorithm_inverse(void (*func)(std::vector<int>&), int arr_size)
{
	std::vector<int> arr(arr_size);
	for (int i = 0; i < arr.size(); i++) arr[i] = arr.size() - i; //inverted array
	auto start = std::chrono::high_resolution_clock::now();
	func(arr);
	auto end = std::chrono::high_resolution_clock::now();
	double time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9;
	return time;
}
double bench_algorithm_average(void (*func)(std::vector<int>&), int arr_size, int trials)
{
	std::vector<int> arr(arr_size);
	double times = 0;
	for (int j = 0; j < trials; j++)
	{
		for (int i = 0; i < arr.size(); i++) arr[i] = rand() % arr.size(); //random array
		auto start = std::chrono::high_resolution_clock::now();
		func(arr);
		auto end = std::chrono::high_resolution_clock::now();
		double time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9;
		times += time;
	}
	return times / trials;
}

int main()
{
	std::vector<void (*)(std::vector<int>&)> fs = {insertion_sort, radix_sort, Quick_sort};
	std::vector<std::string> fn = { "insertion_sort", "radix_sort", "Quick_sort" };
	for (int f = 0; f < 3; f++)
	{
		std::cout << fn[f] << '\n';
		for (int i = 1; i <= 4 + (f>0)*2; i++)
		{
			std::cout << "1e+" << i << '\t';
			std::cout << bench_algorithm_inverse(fs[f], pow(10, i))    << " | ";
			std::cout << bench_algorithm_sorted (fs[f], pow(10, i))    << " | ";
			std::cout << bench_algorithm_average(fs[f], pow(10, i), 5) << '\n';
		}
	}
}
