#include <iostream>

#include "include.hpp"


using namespace sel;
using namespace voro;

int main (int argc, char* argv[])
{
    
    std::cout << "works" << std::endl;
    State state{true};


    state["x=5"];
    voronoicell v;
    v.init(-1,1,-1,1,-1,1);
    return 0;
}
