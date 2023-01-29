#ifndef UNIT_H
#define UNIT_H

#include "Field.h"

class Unit {
protected:
    int coins;
    int hp;
    int x, y;
    Field field;
    int** graph; 
    Unit* Enemy = nullptr; 

public:
    Unit(Field field, int x0, int y0) {
        this->field = field;
        this->x = x0;
        this->y = y0;
        this->coins = 0;
        this->hp = 3;
    }

    virtual void makeEnemy(Unit f) = 0;
    virtual bool move(char key) = 0;

    virtual void createGraph() = 0;
    virtual void printGraph() = 0;

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

class King : public Unit
{
public:
    King(Field field, int x, int y) : Unit(field, x, y) {
        this->field.newCh(this->x, this->y, 'K');
    }
    
    void makeEnemy(Unit Envy) override
    {
        Enemy = &(Envy);
    }

    char autoStep(int x1, int  y1)
    {
        // heuristic: f(x) = g(x) + h(x)
        // g(x) = |x_current - x|^2 + |y_current - y|
        // h(x) = |x1 - x_current|^2 + |y1 - y_current|^2

        int x_current = this->x;
        int y_current = this->y; 

        int euristic[8]=
{ 
abs(x1 - (x_current-1))*abs(x1 - (x_current-1)) + abs(y1 - (y_current-1))*abs(y1 - (y_current-1)), //y
abs(x1 - (x_current))*abs(x1 - (x_current)) + abs(y1 - (y_current-1))*abs(y1 - (y_current-1)), // u
abs(x1 - (x_current+1))*abs(x1 - (x_current+1)) + abs(y1 - (y_current-1))*abs(y1 - (y_current-1)), // i

abs(x1 - (x_current-1))*abs(x1 - (x_current-1)) + abs(y1 - (y_current))*abs(y1 - (y_current)), // h
abs(x1 - (x_current+1))*abs(x1 - (x_current+1)) + abs(y1 - (y_current))*abs(y1 - (y_current)), // k

abs(x1 - (x_current-1))*abs(x1 - (x_current-1)) + abs(y1 - (y_current+1))*abs(y1 - (y_current+1)), // b
abs(x1 - (x_current))*abs(x1 - (x_current)) + abs(y1 - (y_current+1))*abs(y1 - (y_current+1)), // n
abs(x1 - (x_current+1))*abs(x1 - (x_current+1)) + abs(y1 - (y_current+1))*abs(y1 - (y_current+1)) // m 
};

        int min = INT_MAX;
        // for (int i = 0; i < 8; i++)
        // {
        //     cout << " " << euristic[i];
        // }
        int counter = -1;
        
        for (int i: euristic)
        {
            if (i < min) 
            {
                counter++;
                min = i;
            }
            cout << endl << i;
        }

        cout << endl << "The min element is " << min << std::endl << counter;
        char step;
        //choose way
        switch (counter)
        {
            case 0:
            step = 'y';
            break;

            case 1:
            step = 'u';
            break;

            case 2:
            step = 'i';
            break;

            case 3:
            step = 'h';
            break;

            case 4:
            step = 'k';
            break;

            case 5:
            step = 'b';
            break;

            case 6:
            step = 'n';
            break;

            case 7:
            step = 'm';
            break;
        }
        cout << step;
        return step;
    }

    void createGraph() override
    {
        graph = new int*[field.Width()];
        for (int i = 0; i < field.Width(); i++) {
            graph[i] = new int [field.Height()];
        }

     //create int table
        for (int i = 0; i < field.Width(); i++)
        {
            for (int j = 0; j < field.Height(); j++)
            {
                if (this->field.getCoordinate(i, j) == '0')
                {
                    graph[i][j] = 99;
                }
                else if (this->field.getCoordinate(i, j) == '*')
                {
                    graph[i][j] = 5;
                }
                else
                {
                    graph[i][j] = 1;
                }   
            }    
        }
    }

    void printGraph() override
    {
        cout << endl;
        for (int i = 0; i < field.Width(); i++) {
            for (int j = 0; j < field.Height(); j++) {
                cout << setw(3) << graph[i][j] << " ";
            }
            cout << endl;
        }
    }

    bool move(char key) override {
        bool act=true;
        switch (key) {//around rus 'o'
            case 'u':
                this->field.newCh(this->x, this->y, '_');
                this->x -= 1;
                if (field.isLegalMove(this->x, this->y)) {
                    if(this->check() == 1){ act=false; break;}
                    this->field.newCh(this->x, this->y, 'K');
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

            case 'n':
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

            case 'h':
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

            case 'k':
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

            case 'y':
                this->field.newCh(this->x, this->y, '_');
                this->y -= 1;
                this->x -= 1;
                if (field.isLegalMove(this->x, this->y)) {
                    if(this->check() == 1){ act=false; break;}
                    this->field.newCh(this->x, this->y, 'K');
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

            case 'i':
                this->field.newCh(this->x, this->y, '_');
                this->y -= 1;
                this->x += 1;
                if (field.isLegalMove(this->x, this->y)) {
                    if(this->check() == 1){ act=false; break;}
                    this->field.newCh(this->x, this->y, 'K');
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

            case 'b':
                this->field.newCh(this->x, this->y, '_');
                this->y += 1;
                this->x -= 1;
                if (field.isLegalMove(this->x, this->y)) {
                    if(this->check() == 1){ act=false; break;}
                    this->field.newCh(this->x, this->y, 'K');
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

            case 'm':
                this->field.newCh(this->x, this->y, '_');
                this->y += 1;
                this->x += 1;
                if (field.isLegalMove(this->x, this->y)) {
                    if(this->check() == 1){ act=false; break;}
                    this->field.newCh(this->x, this->y, 'K');
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
            case '0':
                act=false; 
            break;
            
        }
        if(act == false) return false;
        return true;
    }   
};

#endif