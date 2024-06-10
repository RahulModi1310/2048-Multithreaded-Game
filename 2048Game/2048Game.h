#pragma once
#include<iostream>
#include <fstream>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<vector>
#include<algorithm>
#include<ctime>
#include<cstdlib>

using namespace std;

const int SIZE = 4;
const char* HIGHSCORE_FILENAME = "highscore.txt";

vector<vector<int>> board(SIZE, vector<int>(SIZE, 0));
mutex mtx;
condition_variable cv;
bool gameOver = false;
int updated = 0;
unsigned int emptyTile = 0;
unsigned long score;
unsigned long highscore;

void printBoard()
{
    system("CLS");
    cout << "2048 Game" << endl;
    cout << "---------" << endl;
    cout << "Highscore: " << highscore << endl;
    cout << "-------------------------" << endl;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            cout << board[i][j] << "\t";
        }
        cout << endl;
    }
    cout << "---------" << endl;
    cout << "Score: " << score << endl;
    cout << "Use W/A/S/D keys to move tiles" << endl;
    cout << "Press any key to quit" << endl;
    cout.flush();
}

void addNewTile()
{
    int x, y;
    if (gameOver || emptyTile == 0) return;
    do
    {
        x = rand() % SIZE;
        y = rand() % SIZE;
    } while (board[x][y] != 0);
    board[x][y] = (rand() % 2) ? 2 : 4;
}

bool canMove()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] == 0)
                return true;
            if (j < SIZE - 1 && board[i][j] == board[i][j + 1])
                return true;
            if (i < SIZE - 1 && board[i][j] == board[i + 1][j])
                return true;
        }
    }
    return false;
}

void moveUp()
{
    emptyTile = 0;
    for (int j = 0; j < SIZE; j++)
    {
        vector<int> temp;
        for (int i = 0; i < SIZE; i++)
        {
            if (board[i][j] != 0)
                temp.push_back(board[i][j]);
        }
        for (int i = 0; i < int(temp.size()) - 1; i++)
        {
            if (temp[i] == temp[i + 1])
            {
                temp[i] *= 2;
                score += temp[i];
                temp.erase(temp.begin() + i + 1);
            }
        }
        for (int i = 0; i < SIZE; i++)
        {
            if (i < temp.size())
                board[i][j] = temp[i];
            else
                emptyTile++, board[i][j] = 0;
        }
    }
}

void moveDown()
{
    emptyTile = 0;
    for (int j = 0; j < SIZE; j++)
    {
        vector<int> temp;
        for (int i = SIZE - 1; i >= 0; i--)
        {
            if (board[i][j] != 0)
                temp.push_back(board[i][j]);
        }
        for (int i = 0; i < int(temp.size()) - 1; i++)
        {
            if (temp[i] == temp[i + 1])
            {
                temp[i] *= 2;
                score += temp[i];
                temp.erase(temp.begin() + i + 1);
            }
        }
        for (int i = 0; i < SIZE; i++)
        {
            if (i < temp.size())
                board[SIZE - 1 - i][j] = temp[i];
            else
                emptyTile++, board[SIZE - 1 - i][j] = 0;
        }
    }
}

void moveLeft()
{
    emptyTile = 0;
    for (int i = 0; i < SIZE; i++)
    {
        vector<int> temp;
        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] != 0)
                temp.push_back(board[i][j]);
        }
        for (int j = 0; j < int(temp.size()) - 1; j++)
        {
            if (temp[j] == temp[j + 1])
            {
                temp[j] *= 2;
                score += temp[j];
                temp.erase(temp.begin() + j + 1);
            }
        }
        for (int j = 0; j < SIZE; j++)
        {
            if (j < temp.size())
                board[i][j] = temp[j];
            else
                emptyTile++, board[i][j] = 0;
        }
    }
}

void moveRight()
{
    emptyTile = 0;
    for (int i = 0; i < SIZE; i++)
    {
        vector<int> temp;
        for (int j = SIZE - 1; j >= 0; j--)
        {
            if (board[i][j] != 0)
                temp.push_back(board[i][j]);
        }
        for (int j = 0; j < int(temp.size()) - 1; j++)
        {
            if (temp[j] == temp[j + 1])
            {
                temp[j] *= 2;
                score += temp[j];
                temp.erase(temp.begin() + j + 1);
            }
        }
        for (int j = 0; j < SIZE; j++)
        {
            if (j < temp.size())
                board[i][SIZE - 1 - j] = temp[j];
            else
                emptyTile++, board[i][SIZE - 1 - j] = 0;
        }
    }
}

void handleInput()
{
    char c;
    unique_lock<mutex> lock(mtx);
    while (true)
    {
        cv.wait(lock, []
            { return (gameOver || updated == 1); });
        if (gameOver) {
            cv.notify_all();
            break;
        }
        cin >> c;
        if (c == 'w' || c == 'W')
            moveUp();
        else if (c == 's' || c == 'S')
            moveDown();
        else if (c == 'a' || c == 'A')
            moveLeft();
        else if (c == 'd' || c == 'D')
            moveRight();
        else {
            gameOver = true;
            updated = 2;
            cv.notify_all();
            break;
        }
        updated = 2;
        cv.notify_all();
    }
}

void updateGame()
{
    unique_lock<mutex> lock(mtx);
    while (true)
    {
        cv.wait(lock, []
            { return (gameOver || updated == 2); });
        if (gameOver) {
            cv.notify_all();
            break;
        }
        addNewTile();
        updated = 0;
        cv.notify_all();
    }
    cv.notify_all();
}

void displayGame()
{
    unique_lock<mutex> lock(mtx);
    while (true)
    {
        cv.wait(lock, []
            { return (gameOver || updated == 0); });
        printBoard();
        if (gameOver || !canMove())
        {
            gameOver = true;
            cv.notify_all();
            break;
        }
        updated = 1;
        cv.notify_all();
    }
}

void getHighscore() {
    ifstream fileIn(HIGHSCORE_FILENAME);
    fileIn >> highscore;
    fileIn.close();
}

void updateHighscore() {
    if (score < highscore) return;
    highscore = score;
    ofstream fileOut(HIGHSCORE_FILENAME);
    fileOut << highscore;
    fileOut.close();
}

void printScore() {
    updateHighscore();
    printBoard();
    cout << "------------****---------" << endl;
    cout << "Oops! Game Over" << endl;
    cout << "Your Score: " << score << endl;
    cout << "------------****---------" << endl;
}

void init2048Game() {
    srand(time(0));
    emptyTile = SIZE * SIZE;
    updated = 0;
    getHighscore();
    addNewTile();
    addNewTile();
}