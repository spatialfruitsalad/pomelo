#include <iostream>

#include "include.hpp"
#include "fileloader.hpp"

using namespace sel;
using namespace voro;

int main (int argc, char* argv[])
{
    
    std::cout << "works" << std::endl;
    State state{true};


    state["x=5"];
    voronoicell v;
    v.init(-1,1,-1,1,-1,1);
    v.draw_gnuplot(0,0,0,"single_cell.gnu");

    std::vector<particleparameterset> setlist;
    fileloader loader;
    std::string filename = argv[1];
    loader.read(filename,setlist);
    return 0;
}
