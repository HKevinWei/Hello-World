#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

///////////////////////////////////////////////////////////////////////////
// Manifest constants
///////////////////////////////////////////////////////////////////////////

const int MAXROWS = 20;             // max number of rows in the pit
const int MAXCOLS = 40;             // max number of columns in the pit
const int MAXSNAKES = 180;          // max number of snakes allowed

const int UP = 0;
const int DOWN = 1;
const int LEFT = 2;
const int RIGHT = 3;

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function declarations
///////////////////////////////////////////////////////////////////////////

int decodeDirection(char dir);
bool directionToDeltas(int dir, int& rowDelta, int& colDelta);
void clearScreen();

///////////////////////////////////////////////////////////////////////////
// Type definitions
///////////////////////////////////////////////////////////////////////////

class Pit;  // This is needed to let the compiler know that Pit is a
// type name, since it's mentioned in the Snake declaration.

class Snake
{
public:
	// Constructor
	Snake(Pit* pp, int r, int c);

	// Accessors
	int  row() const;
	int  col() const;

	// Mutators
	void move();

private:
	Pit* m_pit;
	int  m_row;
	int  m_col;
};

class Player
{
public:
	// Constructor
	Player(Pit *pp, int r, int c);

	// Accessors
	int  row() const;
	int  col() const;
	int  age() const;
	bool isDead() const;

	// Mutators
	void   stand();
	void   move(int dir);
	void   setDead();

private:
	Pit*  m_pit;
	int   m_row;
	int   m_col;
	int   m_age;
	bool  m_dead;
};

class Pit
{
public:
	// Constructor/destructor
	Pit(int nRows, int nCols);
	~Pit();

	// Accessors
	int     rows() const;
	int     cols() const;
	Player* player() const;
	int     snakeCount() const;
	int     numberOfSnakesAt(int r, int c) const;
	void    display(string msg) const;

	// Mutators
	bool   addSnake(int r, int c);
	bool   addPlayer(int r, int c);
	bool   destroyOneSnake(int r, int c);
	bool   moveSnakes();

private:
	int     m_rows;
	int     m_cols;
	Player* m_player;
	Snake*  m_snakes[MAXSNAKES];
	int     m_nSnakes;
};

class Game
{
public:
	// Constructor/destructor
	Game(int rows, int cols, int nSnakes);
	~Game();

	// Mutators
	void play();

private:
	Pit* m_pit;
};

///////////////////////////////////////////////////////////////////////////
//  Snake implementation
///////////////////////////////////////////////////////////////////////////

Snake::Snake(Pit* pp, int r, int c)
{
	if (pp == nullptr)
	{
		cout << "***** A snake must be in some Pit!" << endl;
		exit(1);
	}
	if (r < 1 || r > pp->rows() || c < 1 || c > pp->cols())
	{
		cout << "***** Snake created with invalid coordinates (" << r << ","
			<< c << ")!" << endl;
		exit(1);
	}
	m_pit = pp;
	m_row = r;
	m_col = c;
}

int Snake::row() const
{
	return m_row;
}

int Snake::col() const
{
	return m_col;
}

void Snake::move()
{
	// Attempt to move in a random direction; if we can't move, don't move
	switch (rand() % 4)
	{
	case UP:     if (m_row > 1)             m_row--; break;
	case DOWN:   if (m_row < m_pit->rows()) m_row++; break;
	case LEFT:   if (m_col > 1)             m_col--; break;
	case RIGHT:  if (m_col < m_pit->cols()) m_col++; break;
	}
}

///////////////////////////////////////////////////////////////////////////
//  Player implementations
///////////////////////////////////////////////////////////////////////////

Player::Player(Pit* pp, int r, int c)
{
	if (pp == nullptr)
	{
		cout << "***** The player must be in some Pit!" << endl;
		exit(1);
	}
	if (r < 1 || r > pp->rows() || c < 1 || c > pp->cols())
	{
		cout << "**** Player created with invalid coordinates (" << r
			<< "," << c << ")!" << endl;
		exit(1);
	}
	m_pit = pp;
	m_row = r;
	m_col = c;
	m_age = 0;
	m_dead = false;
}

int Player::row() const
{
	return m_row;
}

int Player::col() const
{
	return m_col;
}

int Player::age() const
{
	return m_age;
}

void Player::stand()
{
	m_age++;
}

void Player::move(int dir)
{
	m_age++;
	int maxCanMove = 0;  // maximum distance player can move in direction dir
	switch (dir)
	{
	case UP:     maxCanMove = m_row - 1;             break;
	case DOWN:   maxCanMove = m_pit->rows() - m_row; break;
	case LEFT:   maxCanMove = m_col - 1;             break;
	case RIGHT:  maxCanMove = m_pit->cols() - m_col; break;
	}
	if (maxCanMove == 0)  // against wall
		return;
	int rowDelta;
	int colDelta;
	if (!directionToDeltas(dir, rowDelta, colDelta))
		return;

	// No adjacent snake in direction of movement

	if (m_pit->numberOfSnakesAt(m_row + rowDelta, m_col + colDelta) == 0)
	{
		m_row += rowDelta;
		m_col += colDelta;
		return;
	}

	// Adjacent snake in direction of movement, so jump

	if (maxCanMove >= 2)  // need 
	{
		m_pit->destroyOneSnake(m_row + rowDelta, m_col + colDelta);
		m_row += 2 * rowDelta;
		m_col += 2 * colDelta;
		if (m_pit->numberOfSnakesAt(m_row, m_col) > 0)  // landed on a snake!
			setDead();
	}
}

bool Player::isDead() const
{
	return m_dead;
}

void Player::setDead()
{
	m_dead = true;
}

///////////////////////////////////////////////////////////////////////////
//  Pit implementations
///////////////////////////////////////////////////////////////////////////

Pit::Pit(int nRows, int nCols)
{
	if (nRows <= 0 || nCols <= 0 || nRows > MAXROWS || nCols > MAXCOLS)
	{
		cout << "***** Pit created with invalid size " << nRows << " by "
			<< nCols << "!" << endl;
		exit(1);
	}
	m_rows = nRows;
	m_cols = nCols;
	m_player = nullptr;
	m_nSnakes = 0;
}

Pit::~Pit()
{
	for (int k = 0; k < m_nSnakes; k++)
		delete m_snakes[k];
	delete m_player;
}

int Pit::rows() const
{
	return m_rows;
}

int Pit::cols() const
{
	return m_cols;
}

Player* Pit::player() const
{
	return m_player;
}

int Pit::snakeCount() const
{
	return m_nSnakes;
}

int Pit::numberOfSnakesAt(int r, int c) const
{
	int count = 0;
	for (int k = 0; k < m_nSnakes; k++)
	{
		const Snake* sp = m_snakes[k];
		if (sp->row() == r  &&  sp->col() == c)
			count++;
	}
	return count;
}

void Pit::display(string msg) const
{
	// Position (row,col) in the pit coordinate system is represented in
	// the array element grid[row-1][col-1]
	char grid[MAXROWS][MAXCOLS];
	int r, c;

	// Fill the grid with dots
	for (r = 0; r < rows(); r++)
		for (c = 0; c < cols(); c++)
			grid[r][c] = '.';

	// Indicate each snake's position
	for (int k = 0; k < m_nSnakes; k++)
	{
		const Snake* sp = m_snakes[k];
		char& gridChar = grid[sp->row() - 1][sp->col() - 1];
		switch (gridChar)
		{
		case '.':  gridChar = 'S'; break;
		case 'S':  gridChar = '2'; break;
		case '9':  break;
		default:   gridChar++; break;  // '2' through '8'
		}
	}

	// Indicate player's position
	if (m_player != nullptr)
	{
		char& gridChar = grid[m_player->row() - 1][m_player->col() - 1];
		if (m_player->isDead())
			gridChar = '*';
		else
			gridChar = '@';
	}

	// Draw the grid
	clearScreen();
	for (r = 0; r < rows(); r++)
	{
		for (c = 0; c < cols(); c++)
			cout << grid[r][c];
		cout << endl;
	}
	cout << endl;

	// Write message, snake, and player info
	cout << endl;
	if (msg != "")
		cout << msg << endl;
	cout << "There are " << snakeCount() << " snakes remaining." << endl;
	if (m_player == nullptr)
		cout << "There is no player." << endl;
	else
	{
		if (m_player->age() > 0)
			cout << "The player has lasted " << m_player->age() << " steps." << endl;
		if (m_player->isDead())
			cout << "The player is dead." << endl;
	}
}

bool Pit::addSnake(int r, int c)
{
	// Dynamically allocate a new Snake and add it to the pit
	if (m_nSnakes == MAXSNAKES)
		return false;
	m_snakes[m_nSnakes] = new Snake(this, r, c);
	m_nSnakes++;
	return true;
}

bool Pit::addPlayer(int r, int c)
{
	// Don't add a player if one already exists
	if (m_player != nullptr)
		return false;

	// Dynamically allocate a new Player and add it to the pit
	m_player = new Player(this, r, c);
	return true;
}

bool Pit::destroyOneSnake(int r, int c)
{
	for (int k = 0; k < m_nSnakes; k++)
	{
		if (m_snakes[k]->row() == r  &&  m_snakes[k]->col() == c)
		{
			delete m_snakes[k];
			m_snakes[k] = m_snakes[m_nSnakes - 1];
			m_nSnakes--;
			return true;
		}
	}
	return false;
}

bool Pit::moveSnakes()
{
	for (int k = 0; k < m_nSnakes; k++)
	{
		Snake* sp = m_snakes[k];
		sp->move();
		if (sp->row() == m_player->row() && sp->col() == m_player->col())
			m_player->setDead();
	}

	// return true if the player is still alive, false otherwise
	return !m_player->isDead();
}

///////////////////////////////////////////////////////////////////////////
//  Game implementations
///////////////////////////////////////////////////////////////////////////

Game::Game(int rows, int cols, int nSnakes)
{
	if (nSnakes > MAXSNAKES)
	{
		cout << "***** Trying to create Game with " << nSnakes
			<< " snakes; only " << MAXSNAKES << " are allowed!" << endl;
		exit(1);
	}

	// Create pit
	m_pit = new Pit(rows, cols);

	// Add player
	int rPlayer = 1 + rand() % rows;
	int cPlayer = 1 + rand() % cols;
	m_pit->addPlayer(rPlayer, cPlayer);

	// Populate with snakes
	while (nSnakes > 0)
	{
		int r = 1 + rand() % rows;
		int c = 1 + rand() % cols;
		// Don't put a snake where the player is
		if (r == rPlayer  &&  c == cPlayer)
			continue;
		m_pit->addSnake(r, c);
		nSnakes--;
	}
}

Game::~Game()
{
	delete m_pit;
}

void Game::play()
{
	Player* p = m_pit->player();
	if (p == nullptr)
	{
		m_pit->display("");
		return;
	}
	string msg = "";
	do
	{
		m_pit->display(msg);
		msg = "";
		cout << endl;
		cout << "Move (u/d/l/r//q): ";
		string action;
		getline(cin, action);
		if (action.size() == 0)
			p->stand();
		else
		{
			switch (action[0])
			{
			default:   // if bad move, nobody moves
				cout << '\a' << endl;  // beep
				continue;
			case 'q':
				return;
			case 'u':
			case 'd':
			case 'l':
			case 'r':
				p->move(decodeDirection(action[0]));
				break;
			}
		}
		m_pit->moveSnakes();
	} while (!m_pit->player()->isDead() && m_pit->snakeCount() > 0);
	m_pit->display(msg);
}

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function implementation
///////////////////////////////////////////////////////////////////////////

int decodeDirection(char dir)
{
	switch (dir)
	{
	case 'u':  return UP;
	case 'd':  return DOWN;
	case 'l':  return LEFT;
	case 'r':  return RIGHT;
	}
	return -1;  // bad argument passed in!
}

bool directionToDeltas(int dir, int& rowDelta, int& colDelta)
{
	switch (dir)
	{
	case UP:     rowDelta = -1; colDelta = 0; break;
	case DOWN:   rowDelta = 1; colDelta = 0; break;
	case LEFT:   rowDelta = 0; colDelta = -1; break;
	case RIGHT:  rowDelta = 0; colDelta = 1; break;
	default:     return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////
//  main()
///////////////////////////////////////////////////////////////////////////

int main()
{
	// Initialize the random number generator.  (You don't need to
	// understand how this works.)
	srand(static_cast<unsigned int>(time(0)));

	// Create a game
	// Use this instead to create a mini-game:   Game g(3, 3, 2);
	Game g(9, 10, 40);

	// Play the game
	g.play();
}

///////////////////////////////////////////////////////////////////////////
//  clearScreen implementations
///////////////////////////////////////////////////////////////////////////

// DO NOT MODIFY OR REMOVE ANY CODE BETWEEN HERE AND THE END OF THE FILE!!!
// THE CODE IS SUITABLE FOR VISUAL C++, XCODE, AND g++ UNDER LINUX.

// Note to Xcode users:  clearScreen() will just write a newline instead
// of clearing the window if you launch your program from within Xcode.
// That's acceptable.

#ifdef _MSC_VER  //  Microsoft Visual C++

#include <windows.h>

void clearScreen()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	COORD upperLeft = { 0, 0 };
	DWORD dwCharsWritten;
	FillConsoleOutputCharacter(hConsole, TCHAR(' '), dwConSize, upperLeft,
		&dwCharsWritten);
	SetConsoleCursorPosition(hConsole, upperLeft);
}

#else  // not Microsoft Visual C++, so assume UNIX interface

#include <cstring>

void clearScreen()  // will just write a newline in an Xcode output window
{
	static const char* term = getenv("TERM");
	if (term == nullptr || strcmp(term, "dumb") == 0)
		cout << endl;
	else
	{
		static const char* ESC_SEQ = "\x1B[";  // ANSI Terminal esc seq:  ESC [
		cout << ESC_SEQ << "2J" << ESC_SEQ << "H" << flush;
	}
}

#endif
