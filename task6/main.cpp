#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include <random>

const int GRID_SIZE     = 30; // Размер поля NxN
const int CELL_SIZE     = 30; // Размер одной ячейки в пикселях
const int WINDOW_SIZE   = GRID_SIZE * CELL_SIZE;

const int MAX_RAND_COST = 10;
const int OBSTACLE_PROB = 20; // %
const int GRID[10][10] = {
	{0,0,0,0,0,0,0,1,1,0},
	{1,0,0,1,0,0,1,0,0,0},
	{0,0,0,0,1,0,0,0,1,0},
	{0,0,0,0,0,0,1,0,1,0},
	{1,0,0,0,0,0,1,0,0,1},
	{1,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,1,0,1,0,0},
	{2,0,0,0,1,0,1,1,0,0},
	{0,1,0,1,0,0,0,0,0,0},
	{0,0,1,0,1,0,1,0,0,3}
};

// Типы ячеек
enum CellType {
	EMPTY,
	OBSTACLE,
	START,
	END,
	PATH,
	VISITED
};
// Структура для представления ячейки
struct Cell {
	int x, y;
	int px, py; // C уакзателями возникали проблемы, поэтому так
	CellType type;
	int cost;
	int f, g, h; // Для алгоритма A*
	Cell(int x, int y) : x(x), y(y), px(-1), py(-1), type(EMPTY), f(0), g(0), h(0), cost(1) {}
	// Перезагрузка оператора == для сравнения ячеек
	bool operator==(const Cell& other) const {
		return x == other.x && y == other.y;
	}
};
struct Compare {
	bool operator()(const Cell& a, const Cell& b)
	{
		return a.f > b.f;
	}
};

// Функция для вычисления эвристики (манхэттенское расстояние)
int heuristic(const Cell& a, const Cell& b) {
	return abs(a.x - b.x) + abs(a.y - b.y);
}
int refresh_path(std::vector<std::vector<Cell>>& grid, Cell start, Cell end)
{
	int x = end.px, y = end.py;
	while (x >= 0 and y >= 0)
	{
		int px = grid[x][y].px;
		int py = grid[x][y].py;
		if (px == -1 || py == -1) break;
		grid[x][y].type = PATH;
		x = px;
		y = py;
	}
	return 0;
}
int a_star(std::vector<std::vector<Cell>> &grid, Cell& start, Cell& end)
{
	int dx[4] = { 1, -1, 0, 0 };
	int dy[4] = { 0, 0, 1, -1 };
	std::vector<Cell> closed_set;
	std::priority_queue<Cell, std::vector<Cell>, Compare> open_set;
	open_set.push(start);

	while (!open_set.empty())
	{
		const Cell* current = &open_set.top();
		open_set.pop();
		int x = current->x, y = current->y;

		if (*current == end) return refresh_path(grid, start, end);
		if (grid[x][y].type != START) grid[x][y].type = VISITED;

		for (int i = 0; i < 4; i++)
		{
			if (!(x + dx[i] < GRID_SIZE && y + dy[i] < GRID_SIZE &&
				x + dx[i] >= 0 && y + dy[i] >= 0))
				continue;
				
			if (grid[x + dx[i]][y + dy[i]].type != OBSTACLE && 
				!(grid[x + dx[i]][y + dy[i]].type == VISITED || grid[x + dx[i]][y + dy[i]].type == START))
			{
				
				int g = current->g + grid[x + dx[i]][y + dy[i]].cost;
				if (g < grid[x + dx[i]][y + dy[i]].g)
				{
					grid[x + dx[i]][y + dy[i]].g = g;
					grid[x + dx[i]][y + dy[i]].f = g + grid[x + dx[i]][y + dy[i]].h;
					grid[x + dx[i]][y + dy[i]].px = x;
					grid[x + dx[i]][y + dy[i]].py = y;
					open_set.push(grid[x + dx[i]][y + dy[i]]);
				}
			}
		}
	}
	return 1;
}

std::vector<std::vector<Cell>> random_grid()
{
	std::vector<std::vector<Cell>> grid(GRID_SIZE);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> perc(0, 100);
	std::uniform_int_distribution<> cost(1, MAX_RAND_COST);

	for (int x = 0; x < GRID_SIZE; x++)
	{
		for (int y = 0; y < GRID_SIZE; y++)
		{
			grid[x].push_back(Cell(x, y));
			grid[x][y].type = perc(gen) < OBSTACLE_PROB ? OBSTACLE : EMPTY;
			grid[x][y].h = heuristic(grid[x][y], Cell(GRID_SIZE - 1, GRID_SIZE - 1));
			grid[x][y].g = GRID_SIZE * GRID_SIZE * MAX_RAND_COST;
			grid[x][y].cost = cost(gen);
		}
	}
	grid[0][0].type = START;
	grid[0][0].g = 0;
	grid[0][0].cost = 0;
	grid[GRID_SIZE - 1][GRID_SIZE - 1].type = END;

	return grid;
}
std::vector<std::vector<Cell>> reconstruct_grid()
{
	std::vector<std::vector<Cell>> grid(10);
	int ex, ey;
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			if (GRID[x][y] == 3)
			{
				ex = x; ey = y;
				break;
			}
		}
	}
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			grid[x].push_back(Cell(x, y));
			grid[x][y].type = CellType(GRID[x][y]);
			grid[x][y].g = 100;
			grid[x][y].h = heuristic(grid[x][y], Cell(ex, ey));
		}
	}
	return grid;
}

int main()
{
	sf::Font font;
	sf::Text text;

	if (!font.loadFromFile("arial.ttf")) {
		return -1;
	}

	text.setFont(font);
	text.setFillColor(sf::Color::Black);
	text.setCharacterSize(CELL_SIZE / 2);
	std::vector<std::vector<Cell>> grid = random_grid();
	int sx = -1, sy = -1, ex = -1, ey = -1;
	for (auto& r: grid)
	{
		for (Cell& i: r)
		{
			if (i.type == END)
			{
				ex = i.x; ey = i.y;
			}
			if (i.type == START)
			{
				i.g = 0;
				sx = i.x; sy = i.y;
			}
		}
	}
	std::cout << (a_star(grid, grid[sx][sy], grid[ex][ey]) ? "no path((\n" : "found");
	sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "astar or smth");
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			else if (event.type == sf::Event::MouseButtonPressed) {
				grid = random_grid();
				std::cout << (a_star(grid, grid[sx][sy], grid[ex][ey]) ? "no path((\n" : "found\n");
			}
		}
		window.clear();
		for (int i = 0; i < GRID_SIZE; ++i) {
			for (int j = 0; j < GRID_SIZE; ++j) {
				sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
				cell.setPosition(i * CELL_SIZE, j * CELL_SIZE);
				switch (grid[i][j].type) {
				case EMPTY:    cell.setFillColor(sf::Color::White); break;
				case OBSTACLE: cell.setFillColor(sf::Color::Black); break;
				case START:    cell.setFillColor(sf::Color::Green); break;
				case END:      cell.setFillColor(sf::Color::Red);   break;
				case PATH:     cell.setFillColor(sf::Color::Blue);  break;
				case VISITED:  cell.setFillColor(sf::Color::Cyan);  break;
				}
				window.draw(cell);
				if (MAX_RAND_COST > 1)
				{
					text.setString(sf::String(std::to_string(grid[i][j].cost)));
					text.setPosition(i * CELL_SIZE, j * CELL_SIZE);
					window.draw(text);
				}
			}
		}
		window.display();
	}
}
