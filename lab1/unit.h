#ifndef UNIT_H
#define UNIT_H

#include "Field.h"
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

#endif