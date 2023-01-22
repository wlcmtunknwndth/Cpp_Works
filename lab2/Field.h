#ifndef FIELD_H
#define FIELD_H

#include <iostream>
#include <random>
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <iomanip>
#include <algorithm> 
#include <climits>
using namespace std;
class Field {
    int W, H;
    char** field;

public:
    Field(){}

    Field(int W, int H) {
        this->W = W;
        this->H = H;
        field = new char* [W];
        for (int i = 0; i < W; i++) {
            field[i] = new char [H];
        }
    }

    void Rand() {
        for (int i = 0; i < W; i++) {
            for (int j = 0; j < H; j++) {
                int rng = rand() % 100;
                if (rng < 60) {
                    field[i][j] = '_';
                }
                else if (rng < 70) {
                    field[i][j] = '$';
                }
                else if (rng < 85) {
                    field[i][j] = '0';
                }
                else {
                    field[i][j] = '*';
                }
            }
        }
    }

    void Empty() {
        for (int i = 0; i < W; i++) {
            for (int j = 0; j < H; j++) {
                field[i][j] = '_';
            }
        }
    }

    void output() {
        cout << endl;
        for (int i = 0; i < W; i++) {
            for (int j = 0; j < H; j++) {
                cout << field[i][j] << " ";
            }
            cout << endl;
        }
    }

    bool isLegalMove(int x, int y) {
        if (x < 0 || x >= W || y < 0 || y >= H) {
            cout << endl << "\nOut of polygon\n" << endl;
            return false;
        }
        
        return true;
    }

    void newCh(int x, int y, char U)
    {
        this->field[x][y] = U;
    }

    char getCoordinate(int x, int y) {
        return this->field[x][y];
    }

    int Width(){ return this->W; }
    int Height(){ return this->H; }
};
#endif