#include "Field.h"
#include "Unit.h"

int main()
{
    srand(time(0));
    Field f(10, 5);
    f.Empty();
    f.output();
    King K(f, 5, 2);
    f.output();
    K.createGraph();
    K.printGraph();
    // K.autoStep(0, 0);
    char tmp = K.autoStep(0, 0);
    cout << endl << "idk" << tmp << "idk";
    K.move(K.autoStep(0, 0));
    return 0;
}
