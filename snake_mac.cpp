#include "OpenGL.h"
#include "GLUT/glut.h"
#include "OpenGL/GL.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <list>
#include <ctime>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <string>

//cout << getcwd(NULL,0);

using namespace std;
const int gridHeight = 50;
const int gridWidth = 60;
const int INITIAL_SIZE = 5;
const double PGenerateFruit = 0.04;
const double PGoldenFruit = 0.1;
const double PCherryFruit = .5;

bool bw = false;
int headX = 25;
int headY = 25;
int grow = INITIAL_SIZE;

list<int> snakeX;
list<int> snakeY;

char board[gridWidth][gridHeight];
char direction = 'E';
bool gameOver = true;
bool paused = true;
int score = 0;
int aCount = 0;
int gCount = 0;
int cCount = 0;
vector<string> savedNames;

void DrawElement(double i, double j, char element) {
	double x = (i + .5) / gridWidth * 2.0 - 1.0;
	double y = (j + .5) / gridHeight * 2.0 - 1.0;
	double r = 1.0 / gridHeight;
	switch (element) {
	case 'S':
		glColor3f(0, bw*.5, !bw*.5);
		DrawCircle(x, y, r, 10);
		break;
	case 's':
		glColor3f(0, !bw*.5, bw*.5);
		DrawCircle(x, y, r, 10);
		break;
	case 'A':
		glColor3f(1.0, 0.0, 0);
		DrawCircle(x, y, r, 10);
		glColor3f(0, 1.0, 0);
		DrawCircle(x + r * .15, y + r * .9, r*.4, 10);
		break;
	case 'G':
		glColor3f(1.0, .75, 0);
		DrawCircle(x, y, r, 10);
		glColor3f(0, 1.0, 0);
		DrawCircle(x + r * .15, y + r * .9, r*.4, 10);
		break;
	case 'C':
		glColor3f(0.0, 0.0, 0.0);
		DrawCircle(x, y, r, 10);
		glColor3f(1.0, 0.0, 0);
		DrawCircle(x + r * .15, y + r * .9, r*.4, 10);
		break;
	case 'W':
		glColor3f(0.5, 0.5, 0.5);
		DrawCircle(x, y, r, 10);
		break;
	case 'R':
		glColor3f(.5, 1.0, 0.0);
		DrawRectangle(x - r, y - r, x + r, y + r, "img/4_of_clubs.png", true);
		break;
	case ' ':
		break;
	default:
		break;
	}
}

void update() {
	bw = !bw;
	//Generate Fruit...
	if (rand() / (double)RAND_MAX < PGenerateFruit) {
		int x = rand() % gridWidth;
		int y = rand() % gridHeight;
		if (board[x][y] != 'S' && board[x][y] != 's' && board[x][y] != 'G' && board[x][y] != 'C' && board[x][y] != 'A' && board[x][y] != 'R') {
			if (rand() / (double)RAND_MAX < PGoldenFruit)
				board[x][y] = 'G';
			else if (rand() / (double)RAND_MAX < PCherryFruit)
				board[x][y] = 'C';
			else
				board[x][y] = 'A';
		}
	}

	//Draw the Walls
	for (int i = 1; i <= 1; i++)
		for (int j = 16; j < 36; j++)
			board[15][j] = 'R';

	for (int i = 1; i <= 1; i++)
		for (int j = 16; j < 36; j++)
			board[45][j] = 'R';

	for (int i = 21; i <= 39; i++)
		board[i][43] = 'R';

	for (int i = 21; i <= 39; i++)
		board[i][9] = 'R';

	//Find out where the head will be...
	switch (direction) {
	case 'N': headY++; break;
	case 'E': headX++; break;
	case 'S': headY--; break;
	case 'W': headX--; break;
	}
	if (headX < 0) headX = gridWidth - 1;
	if (headY < 0) headY = gridHeight - 1;
	if (headX >= gridWidth)  headX = 0;
	if (headY >= gridHeight) headY = 0;

	//Move the snake's tail..
	if (grow == 0) {
		int tailX = snakeX.back();
		int tailY = snakeY.back();
		snakeX.pop_back();
		snakeY.pop_back();
		board[tailX][tailY] = '.';
	}
	else if (grow > 0) {
		grow--;
	}
	else {
		if (snakeX.size() >= 1 && snakeY.size() >= 1)
		{
			int tailX = snakeX.back();
			int tailY = snakeY.back();
			snakeX.pop_back();
			snakeY.pop_back();
			if (snakeX.size() == 0 && snakeY.size() == 0)
				gameOver = true;
			board[tailX][tailY] = '.';
			tailX = snakeX.back();
			tailY = snakeY.back();
			snakeX.pop_back();
			snakeY.pop_back();
			if (snakeX.size() == 0 && snakeY.size() == 0)
				gameOver = true;
			board[tailX][tailY] = '.';

			grow++;
		}
		else
			gameOver = true;
	}

	switch (board[headX][headY])
	{
	case 'A': grow += 4; score += 5; aCount += 1; break;
	case 'G': grow += 8; score += 10; gCount += 1; break;
	case 'C': grow -= 6; score -= 8; cCount += 1; break;
	case 'S': case 's': gameOver = true; break;
	case 'R': gameOver = true; break;
	}

	//Move the snake's head.
	board[headX][headY] = bw ? 'S' : 's';
	snakeX.push_front(headX);
	snakeY.push_front(headY);
}


void draw() {
	for (int i = 0; i < gridWidth; i++) {
		for (int j = 0; j < gridHeight; j++) {
			DrawElement(i, j, board[i][j]);
		}
	}
	usleep(100000);
	if (!gameOver && paused)
		update();
}

void saveGame() {
	string savedGame;
	string gameName;
	bool first = false;
	savedNames.clear();
	ofstream fileOutput("savedGames.txt", ofstream::app);

	ifstream fileNames("savedGames.txt");

	// i want the file to load names into the vector if they exist
	// my problem is if there aren't any names I want it to skip this and write the new name
	streampos current = fileNames.tellg();
	fileNames.seekg(0, fileNames.end);
	bool empty = !fileNames.tellg(); // true if empty file
	fileNames.seekg(current, fileNames.beg);

	while (fileNames >> gameName) {
		//if (fileNames.eof()) break;
		//fileNames >> gameName;
		savedNames.push_back(gameName);
	}

	// need something here to catch if the vector is empty
	cout << "Save Game (enter a name): ";
	cin >> savedGame;
	savedGame = savedGame + ".txt";
	if (empty) {
		first = true;
		savedNames.push_back(savedGame);
	}

	fileNames.close();

	int index = 0;
	int position = -1;
	bool found = false;

	while (index < savedNames.size() && !found) {
		if (savedNames[0] == savedGame) {
			if (empty) {
				position = 0;
			}
			else {
				position = -1;
			}
		}
		else if (savedNames[index] == savedGame) {
			found = true;
			position = -1;
			break;
		}
		else
			position = index;
		index++;
	}

	for (int i = 0; i < savedNames.size(); i++) {
		if (position == -1) {
			cout << "File name " << savedGame << " already exists. Try saving with a different name.\n";
			break;
		}
		else if (first) {
			fileOutput << savedNames[position] << endl;
			break;
		}
		else
		{
			savedNames.push_back(savedGame);
			fileOutput << savedGame << endl;
			break;
		}
	}

	fileOutput.close();

	if (!found) {
		ofstream fout(savedGame);

		for (int i = 0; i < gridWidth; i++) {
			for (int j = 0; j < gridHeight; j++) {
				fout << board[i][j];
			}
		}

		fout << endl;
		fout << snakeX.size() << endl;

		fout << direction << endl;
		fout << headX << endl;
		fout << headY << endl;

		for (int i : snakeX) {
			fout << i << " ";
		}
		fout << endl;

		for (int i : snakeY) {
			fout << i << " ";
		}
		fout << endl;

		fout << score << endl;
		fout << aCount << endl;
		fout << gCount << endl;
		fout << cCount << endl;

		fout.close();
	}
}

void loadGame() {
	string savedGame;
	string gameName;
	int snakeSize;

	score = 0;
	aCount = 0;
	gCount = 0;
	cCount = 0;

	ifstream fileNames("savedGames.txt");

	savedNames.clear();

	while (!fileNames.eof()) {
		fileNames >> gameName;
		if (fileNames.eof()) break;
		savedNames.push_back(gameName);
	}
	fileNames.close();

	cout << "Load Game (enter a name): ";
	cin >> savedGame;
	savedGame = savedGame + ".txt";

	int index = 0;
	int position = -1;
	bool found = false;

	while (index < savedNames.size() && !found) {
		if (savedNames[index] == savedGame) {
			found = true;
			position = index;
		}
		index++;
	}

	for (int i = 0; i < savedNames.size(); i++) {
		if (position == -1) {
			cout << "Name does not exist. Try loading with a different name." << endl;
			break;
		}
		else {
			ifstream fin(savedGame);

			snakeX.clear();
			snakeY.clear();

			for (int i = 0; i < gridWidth; i++) {
				for (int j = 0; j < gridHeight; j++) {
					fin >> board[i][j];
				}
			}

			fin >> snakeSize;
			fin >> direction;
			fin >> headX;
			fin >> headY;

			for (int i = 0; i < snakeSize; i++) {
				int x;
				fin >> x;
				snakeX.push_back(x);
			}

			for (int i = 0; i < snakeSize; i++) {
				int y;
				fin >> y;
				snakeY.push_back(y);
			}

			fin >> score;
			fin >> aCount;
			fin >> gCount;
			fin >> cCount;

			fin.close();
		}
	}
}

void keyboard(int key) {
	switch (key) {
	case 'w': if (direction == 'E' || direction == 'W') direction = 'N'; break;
	case 'a': if (direction == 'N' || direction == 'S')    direction = 'W'; break;
	case 's': if (direction == 'E' || direction == 'W')    direction = 'S'; break;
	case 'd': if (direction == 'N' || direction == 'S') direction = 'E'; break;
	case 'n': saveGame(); break;
	case 'm': loadGame(); break;
	case ';':
		cout << "Score is: " << score << endl;
		cout << "Apple count is: " << aCount << endl;
		cout << "Golden count is: " << gCount << endl;
		cout << "Cherry count is: " << cCount << endl;
		break;
	case 'p':
		paused = !paused;
		break;
	case '.':
		for (int i = 0; i < savedNames.size(); i++)
			cout << savedNames[i] << endl;
		break;
	case ' ':
		if (gameOver) {
			gameOver = false;
			snakeX.clear();
			snakeY.clear();
			headX = gridWidth / 2.0;
			headY = gridHeight / 2.0;
			score = 0;
			aCount = 0;
			gCount = 0;
			cCount = 0;
			for (int i = 0; i < gridWidth; i++) {
				for (int j = 0; j < gridHeight; j++) {
					board[i][j] = '.';
				}
			}
			grow = INITIAL_SIZE;
		}
		break;
	}
}

int main(int argc, char** argv) {
	RunOpenGL(argc, argv, draw, NULL, keyboard);
}
