#include <iostream>
#include <stack>

using namespace std;

#include "olcConsoleGameEngine.h"	// graphics engine, please enable UNICODE in project properties (Genereal tab)

class Labyrinth : public olcConsoleGameEngine
{
public:
	Labyrinth()
	{
		m_sAppName = L"MAZE";
	}

private:
	int  l_Width;
	int  l_Height;
	int *l_labyrinth;


	// Some bit fields for convenience
	enum
	{
		CELL_PATH_N = 0x01,
		CELL_PATH_E = 0x02,
		CELL_PATH_S = 0x04,
		CELL_PATH_W = 0x08,
		CELL_VISITED = 0x10,
	};


	// Algorithm variables
	int  m_nVisitedCells;
	stack<pair<int, int>> m_stack;	// (x, y) coordinate pairs
	int  m_nPathWidth;


protected:
	// Called by graphics engine
	virtual bool OnUserCreate()
	{
		// Maze parameters
		l_Width = 40;
		l_Height = 25;
		l_labyrinth = new int[l_Width * l_Height];
		memset(l_labyrinth, 0x00, l_Width * l_Height * sizeof(int));
		m_nPathWidth = 3;

		// Choose a starting cell
		int x = rand() % l_Width;
		int y = rand() % l_Height;
		m_stack.push(make_pair(x, y));
		l_labyrinth[y * l_Width + x] = CELL_VISITED;
		m_nVisitedCells = 1;

		return true;
	}

	// Called by graphics engine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		// Slow down for animation
		//this_thread::sleep_for(1ms);

		// Little lambda function to calculate index in a readable way
		auto offset = [&](int x, int y)
		{
			return (m_stack.top().second + y) * l_Width + (m_stack.top().first + x);
		};

		// Do labyrinth Algorithm
		if (m_nVisitedCells < l_Width * l_Height)
		{
			// Create a set of unvisted neighbours
			vector<int> neighbours;

			// North neighbour
			if (m_stack.top().second > 0 && (l_labyrinth[offset(0, -1)] & CELL_VISITED) == 0)
				neighbours.push_back(0);
			// East neighbour
			if (m_stack.top().first < l_Width - 1 && (l_labyrinth[offset(1, 0)] & CELL_VISITED) == 0)
				neighbours.push_back(1);
			// South neighbour
			if (m_stack.top().second < l_Height - 1 && (l_labyrinth[offset(0, 1)] & CELL_VISITED) == 0)
				neighbours.push_back(2);
			// West neighbour
			if (m_stack.top().first > 0 && (l_labyrinth[offset(-1, 0)] & CELL_VISITED) == 0)
				neighbours.push_back(3);

			// Are there any neighbours available?
			if (!neighbours.empty())
			{
				// Choose one available neighbour at random
				int next_cell_dir = neighbours[rand() % neighbours.size()];

				// Create a path between the neighbour and the current cell
				switch (next_cell_dir)
				{
				case 0: // North
					l_labyrinth[offset(0, -1)] |= CELL_VISITED | CELL_PATH_S;
					l_labyrinth[offset(0, 0)] |= CELL_PATH_N;
					m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second - 1)));
					break;

				case 1: // East
					l_labyrinth[offset(+1, 0)] |= CELL_VISITED | CELL_PATH_W;
					l_labyrinth[offset(0, 0)] |= CELL_PATH_E;
					m_stack.push(make_pair((m_stack.top().first + 1), (m_stack.top().second + 0)));
					break;

				case 2: // South
					l_labyrinth[offset(0, +1)] |= CELL_VISITED | CELL_PATH_N;
					l_labyrinth[offset(0, 0)] |= CELL_PATH_S;
					m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second + 1)));
					break;

				case 3: // West
					l_labyrinth[offset(-1, 0)] |= CELL_VISITED | CELL_PATH_E;
					l_labyrinth[offset(0, 0)] |= CELL_PATH_W;
					m_stack.push(make_pair((m_stack.top().first - 1), (m_stack.top().second + 0)));
					break;

				}

				m_nVisitedCells++;
			}
			else
			{
				// No available neighbours so backtrack!
				m_stack.pop();
			}
		}


		// === DRAWING STUFF ===

		// Clear Screen by drawing 'spaces' everywhere
		Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');

		// Draw Maze
		for (int x = 0; x < l_Width; x++)
		{
			for (int y = 0; y < l_Height; y++)
			{
				// Each cell is inflated by m_nPathWidth, so fill it in
				for (int py = 0; py < m_nPathWidth; py++)
					for (int px = 0; px < m_nPathWidth; px++)
					{
						if (l_labyrinth[y * l_Width + x] & CELL_VISITED)
							Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_WHITE); // Draw Cell
						else
							Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, PIXEL_SOLID, FG_BLUE); // Draw Cell
					}

				// Draw passageways between cells
				for (int p = 0; p < m_nPathWidth; p++)
				{
					if (l_labyrinth[y * l_Width + x] & CELL_PATH_S)
						Draw(x * (m_nPathWidth + 1) + p, y * (m_nPathWidth + 1) + m_nPathWidth); // Draw South Passage

					if (l_labyrinth[y * l_Width + x] & CELL_PATH_E)
						Draw(x * (m_nPathWidth + 1) + m_nPathWidth, y * (m_nPathWidth + 1) + p); // Draw East Passage
				}
			}
		}

		// Draw Unit - the top of the stack
		for (int py = 0; py < m_nPathWidth; py++)
			for (int px = 0; px < m_nPathWidth; px++)
				Draw(m_stack.top().first * (m_nPathWidth + 1) + px, m_stack.top().second * (m_nPathWidth + 1) + py, 0x2588, FG_GREEN); // Draw Cell


		return true;
	}
};


int hello(int &choice)
{
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "Labyrinth simulation" << std::endl;
	std::cout << "------------------------------------------------";
	std::cout << std::endl << "Please choose your way of running program:          ||| Type 0 or exit to exit" << std::endl;
	std::cout << "1. Run - Standard settigs" << std::endl;
	//std::cout << "2. Set up Your screen resolution" << std::endl;
	std::cout << "0. Exit" << std::endl << std::endl;
	std::cin >> choice;	std::cout << std::endl;
	return choice;
}


void iniciate(int height, int width, int resolution)
{
	Labyrinth game;
	if (game.ConstructConsole(height, width, resolution, resolution))		// if program exit with 0x0 try this setting  - fixed resolution (160, 120, 8, 8) an
		game.Start();
	else
		wcout << L"Could not construct console" << endl;
}


int setScreen(int &l, int &h, int &r)
{
	cout << "Set up width of the screen" << endl;
	cin >> l;
	cout << "Set up height of the screen" << endl;
	cin >> h;
	cout << "Pixel size (8-recommended)" << endl;
	cin >> r;
	return l, h, r;
}


int main()
{
	// Seed random number generator
	srand(clock());

	int c;
	hello(c);

	int resolution = 8;					//recommended from 2 to 8 - pixel size
	int height = 100;
	int width = 160;

	if (c == 1)
	{
		iniciate(width, height, resolution);
	}
	if (c = 2)
	{
		setScreen(width, height, resolution);
		iniciate(width, height, resolution);
	}
	else	cout << "goodbye";







	// Seed random number generator
	//srand(clock());

	// Use olcConsoleGameEngine derived app
	//Labyrinth game;
	//game.ConstructConsole(height, width, resolution, resolution);
	//game.Start();

	return 0;
}