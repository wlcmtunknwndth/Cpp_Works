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
    K.scan(0, 0);
    return 0;
}
