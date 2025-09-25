#include "delegate.h"

#include <iostream>
#include <vector>

int main()
{
    Delegate<int(int, int)> delegate;
    delegate.add([](int a, int b) { return a + b; });

    auto p = delegate(2, 3);
    std::cout << "Result: " << p.at(0) << std::endl;
    return 0;
}