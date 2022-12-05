// Algoritms.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <random>
#include <conio.h>
#include <stdio.h>
#include <time.h>
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

    void fullfill() {
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
        
        return field[x][y] != '0';
    }

    void newCh(int x, int y, char K)
    {
        this->field[x][y] = K;
    }

    char getCoordinate(int x, int y) {
        return this->field[x][y];
    }
};

class Unit {
protected:
    int coins;
    int x, y;
    Field field;
    int hp;

public:
    Unit(Field field, int x0, int y0) {
        this->field = field;
        this->x = x0;
        this->y = y0;
        this->coins = 0;
        this->hp = 3;
    }

    virtual bool move(char key) = 0;

    int check() {
        cout << endl << "HUD:";
        char key = this->field.getCoordinate(this->x, this->y);

        if (key == '_') {
            cout << endl << " COINS: " << this->coins << endl;
            cout << " HP: " << this->hp << endl;
            return 0;
        }
        else if (key == '*') {
            this->hp -= 1;
            if (this->hp == 0) {
                cout << "\n Game over!\n";
                return 1;
            }
            cout << endl << " COINS: " << this->coins << endl;
            cout << " HP: " << this->hp << endl;
            return 0;
        }
        else{
            this->coins += 1;
            cout << endl << " COINS: " << this->coins << endl;
            cout << " HP: " << this->hp << endl;
            return 0;
        }
    }
};

class Pawn : public Unit {
public:
    Pawn(Field field, int x, int y) : Unit(field, x, y) {
        this->field.newCh(this->x, this->y, 'P');
    }

    bool move(char key) override {
        bool act=true;
        switch (key) {
            case 'w':
                this->field.newCh(this->x, this->y, '_');
                this->x -= 1;
                if (field.isLegalMove(this->x, this->y)) {
                    if(this->check() == 1){ act=false; break;}
                    this->field.newCh(this->x, this->y, 'P');
                    this->field.output();
                }
                else {
                    cout << "\nno way\n";
                    this->x += 1;
                    this->field.newCh(this->x, this->y, 'P');
                    this->field.output();
                }
                cout << endl << this->x << ", " << this->y << endl;
            break;

            case 's':
                this->field.newCh(this->x, this->y, '_');
                this->x += 1;
                if (field.isLegalMove(this->x, this->y)) {
                    if(this->check() == 1){ act=false; break;}
                    this->field.newCh(this->x, this->y, 'P');
                    this->field.output();
                }
                else {
                    cout << "\nno way\n";
                    this->x -= 1;
                    this->field.newCh(this->x, this->y, 'P');
                    this->field.output();
                }
                cout << endl << this->x << ", " << this->y << endl;
            break;

            case 'a':
                this->field.newCh(this->x, this->y, '_');
                this->y -= 1;
                if (field.isLegalMove(this->x, this->y)) {
                    if(this->check() == 1){ act=false; break;}
                    this->field.newCh(this->x, this->y, 'P');
                    this->field.output();
                }
                else {
                    cout << "\nno way\n";
                    this->y += 1;
                    this->field.newCh(this->x, this->y, 'P');
                    this->field.output();
                }
                cout << endl << this->x << ", " << this->y << endl;
            break;

            case 'd':
                this->field.newCh(this->x, this->y, '_');
                this->y += 1;
                if (field.isLegalMove(this->x, this->y)) {
                    if(this->check() == 1){ act=false; break;}
                    this->field.newCh(this->x, this->y, 'P');
                    this->field.output();
                }
                else {
                    cout << "\nno way\n";
                    this->y -= 1;
                    this->field.newCh(this->x, this->y, 'P');
                    this->field.output();
                }
                cout << endl << this->x << ", " << this->y << endl;
            break;

            case '0':
                act=false; 
            break;
            
        }
        if(act == false) return false;
        return true;
    }
};

class Knight : public Unit {
public:
    Knight(Field field, int x, int y) : Unit(field, x, y) {
        this->field.newCh(this->x, this->y, 'P');
    }

    bool move(char key) override {
        bool act=true;
        //x-height, y-width
        switch (key) {
            case 'e':
                this->field.newCh(this->x, this->y, '_');
                this->x -= 2;
                this->y += 1;
                if (field.isLegalMove(this->x, this->y)) {
                    if(this->check() == 1){ act=false; break;}
                    this->field.newCh(this->x, this->y, 'K');
                    this->field.output();
                }
                else {
                    cout << "\nno way\n";
                    this->x += 2;
                    this->y -= 1;
                    this->field.newCh(this->x, this->y, 'K');
                    this->field.output();
                }
                cout << endl << this->x << ", " << this->y << endl;
            break;

            case 's':
                this->field.newCh(this->x, this->y, '_');
                this->x += 2;
                this->y -= 1;
                if (field.isLegalMove(this->x, this->y)) {
                    if(this->check() == 1){ act=false; break;}
                    this->field.newCh(this->x, this->y, 'K');
                    this->field.output();
                }
                else {
                    cout << "\nno way\n";
                    this->x -= 2;
                    this->y += 1;
                    this->field.newCh(this->x, this->y, 'K');
                    this->field.output();
                }
                cout << endl << this->x << ", " << this->y << endl;
            break;

            case 'q':
                this->field.newCh(this->x, this->y, '_');
                this->y -= 2;
                this->x -= 1;
                if (field.isLegalMove(this->x, this->y)) {
                    if(this->check() == 1){ act=false; break;}
                    this->field.newCh(this->x, this->y, 'K');
                    this->field.output();
                }
                else {
                    cout << "\nno way\n";
                    this->y += 2;
                    this->x += 1;
                    this->field.newCh(this->x, this->y, 'K');
                    this->field.output();
                }
                cout << endl << this->x << ", " << this->y << endl;
            break;

            case 'f':
                this->field.newCh(this->x, this->y, '_');
                this->y += 2;
                this->x += 1;
                if (field.isLegalMove(this->x, this->y)) {
                    if(this->check() == 1){ act=false; break;}
                    this->field.newCh(this->x, this->y, 'K');
                    this->field.output();
                }
                else {
                    cout << "\nno way\n";
                    this->y -= 2;
                    this->x -= 1;
                    this->field.newCh(this->x, this->y, 'K');
                    this->field.output();
                }
                cout << endl << this->x << ", " << this->y << endl;
            break;

            case 'w':
                this->field.newCh(this->x, this->y, '_');
                this->x -= 2;
                this->y -= 1;
                if (field.isLegalMove(this->x, this->y)) {
                    if(this->check() == 1){ act=false; break;}
                    this->field.newCh(this->x, this->y, 'K');
                    this->field.output();
                }
                else {
                    cout << "\nno way\n";
                    this->x += 2;
                    this->y += 1;
                    this->field.newCh(this->x, this->y, 'K');
                    this->field.output();
                }
                cout << endl << this->x << ", " << this->y << endl;
            break;

            case 'd':
                this->field.newCh(this->x, this->y, '_');
                this->x += 2;
                this->y += 1;
                if (field.isLegalMove(this->x, this->y)) {
                    if(this->check() == 1){ act=false; break;}
                    this->field.newCh(this->x, this->y, 'K');
                    this->field.output();
                }
                else {
                    cout << "\nno way\n";
                    this->x -= 2;
                    this->y -= 1;
                    this->field.newCh(this->x, this->y, 'K');
                    this->field.output();
                }
                cout << endl << this->x << ", " << this->y << endl;
            break;

            case 'a':
                this->field.newCh(this->x, this->y, '_');
                this->y -= 2;
                this->x += 1;
                if (field.isLegalMove(this->x, this->y)) {
                    if(this->check() == 1){ act=false; break;}
                    this->field.newCh(this->x, this->y, 'K');
                    this->field.output();
                }
                else {
                    cout << "\nno way\n";
                    this->y += 2;
                    this->x -= 1;
                    this->field.newCh(this->x, this->y, 'K');
                    this->field.output();
                }
                cout << endl << this->x << ", " << this->y << endl;
            break;

            case 'r':
                this->field.newCh(this->x, this->y, '_');
                this->y += 2;
                this->x -= 1;
                if (field.isLegalMove(this->x, this->y)) {
                    if(this->check() == 1){ act=false; break;}
                    this->field.newCh(this->x, this->y, 'K');
                    this->field.output();
                }
                else {
                    cout << "\nno way\n";
                    this->y -= 2;
                    this->x += 1;
                    this->field.newCh(this->x, this->y, 'K');
                    this->field.output();
                }
                cout << endl << this->x << ", " << this->y << endl;
            break;
            
            case '0':
                act=false; 
            break;
        }
        if(act == false) return false;
        return true;
    }
};

class Bishop : public Unit {
public:
    Bishop(Field field, int x, int y) : Unit(field, x, y) {
        this->field.newCh(this->x, this->y, 'B');
    }
    //x-height, y-width
    bool move(char key) override {
        bool act=true;
        int tmp;
            switch (key) {
                case 'q':
                    this->field.newCh(this->x, this->y, '_');
                    cout << "\n enter num of steps: ";
                    cin >> tmp;
                    this->x -= tmp;
                    this->y -= tmp;
                    if (field.isLegalMove(this->x, this->y)) {
                        if( this->check() == 1) {act=false; break;}
                        this->field.newCh(this->x, this->y, 'B');
                        this->field.output();
                    }
                    else {
                        cout << "\nno way\n";
                        this->x += tmp;
                        this->y += tmp;
                        this->field.newCh(this->x, this->y, 'B');
                        this->field.output();
                    }
                    cout << endl << this->x << ", " << this->y << endl;
                break;

                case 'w':
                    this->field.newCh(this->x, this->y, '_');
                    cout << "\n enter num of steps: ";
                    cin >> tmp;
                    this->x -= tmp;
                    this->y += tmp;
                    if (field.isLegalMove(this->x, this->y)) {
                        if( this->check() == 1) {act=false; break;}
                        this->field.newCh(this->x, this->y, 'B');
                        this->field.output();
                    }
                    else {
                        cout << "\nno way\n";
                        this->x += tmp;
                        this->y -= tmp;
                        this->field.newCh(this->x, this->y, 'B');
                        this->field.output();
                    }
                    cout << endl << this->x << ", " << this->y << endl;
                break;

                case 's':
                    this->field.newCh(this->x, this->y, '_');
                    cout << "\n enter num of steps: ";
                    cin >> tmp;
                    this->x += tmp;
                    this->y += tmp;
                    if (field.isLegalMove(this->x, this->y)) {
                        if( this->check() == 1) {act=false; break;}
                        this->field.newCh(this->x, this->y, 'B');
                        this->field.output();
                    }
                    else {
                        cout << "\nno way\n";
                        this->x -= tmp;
                        this->y -= tmp;
                        this->field.newCh(this->x, this->y, 'B');
                        this->field.output();
                    }
                    cout << endl << this->x << ", " << this->y << endl;
                break;

                case 'a':
                    this->field.newCh(this->x, this->y, '_');
                    cout << "\n enter num of steps: ";
                    cin >> tmp;
                    this->x += tmp;
                    this->y -= tmp;
                    if (field.isLegalMove(this->x, this->y)) {
                        if( this->check() == 1) {act=false; break;}
                        this->field.newCh(this->x, this->y, 'B');
                        this->field.output();
                    }
                    else {
                        cout << "\nno way\n";
                        this->x -= tmp;
                        this->y += tmp;
                        this->field.newCh(this->x, this->y, 'B');
                        this->field.output();
                    }
                    cout << endl << this->x << ", " << this->y << endl;
                break;

                case '0':
                    act=false; 
                break;
            }
        if(act == false) return false;
        return true;
    }
};

int play(Field field);

int main()
{
    srand(time(0));
    Field f(10, 5);
    f.fullfill();
    play(f);
}

int play(Field field)
{
    cout << "1. Play.\n2. Exit.\n";
    char menu=_getch();
    switch (menu)
    {
    case '1':
    
    break;
    
    case '2':
        menu=0;
    break;
    
    default:
        cout << "\nNo option\n";
        play(field);
    break;
    }
    if(menu == 0) return 0;

    cout << "Viberi figuru(p - pawn, k - knight, b-bishop): ";
    char figura=_getch();
    // cin >> figura;
    // char sw;
    switch (figura)
    {
    case 'p':
    {
        Pawn p(field, 5, 2);
        field.output();
        while (p.move(/*sw*/_getch()) != false) {
            //cin >> sw;
            cout << "Enter bukvy: ";
            //p.move(/*sw*/_getch());
        }
    }
    break;

    case 'b':
    {
        Bishop b(field, 5, 2);
        field.output();
        while (b.move(/*sw*/_getch()) != false) {
            //cin >> sw;
            cout << "Enter bukvy: ";
            //k.move(/*sw*/_getch());
        }
    }
    break;

    case 'k':
    {
        Knight k(field, 5, 2);
        field.output();
        while (k.move(/*sw*/_getch()) != false) {
            //cin >> sw;
            cout << "Enter bukvy: ";
            //k.move(/*sw*/_getch());
        }
    }
    break;

    case '2':
    break;
    
    default:
        cout << "\n!!! ERROR !!!\n";
        play(field);
    break;
    }
    return 0;
}