#include "Field.h"
#include "unit.h"

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
        cout << "Enter bukvy: ";
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
        cout << "Enter bukvy: ";
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
        cout << "Enter bukvy: ";
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