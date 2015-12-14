#include <iostream>

#include "include.hpp"


using namespace sel;

int main (int argc, char* argv[])
{
    
    std::cout << "works" << std::endl;
    State state{true};

    state["x=5"];
    return 0;
}
