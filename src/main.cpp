#include <iostream>
#include <fstream>
#include "include.hpp"
#include "fileloader.hpp"
#include "pointpattern.hpp"
using namespace sel;
using namespace voro;

int main (int argc, char* argv[])
{
    
    // lua state for the global parameter file    
    State state{true};

    if(argc != 2 )
    {
        std::cerr << "No File given .... aborting "  << std::endl;
        return -1;
    }

    std::string filename = argv[1];
   
    state.Load(filename);

    std::string posfile = state["positionfile"];
    std::string readfile = state["readfile"];


    std::cout << "Working on " << posfile << " " << readfile << std::endl;
    
    // read particle parameters and positions
    std::vector<particleparameterset> setlist;
    fileloader loader;
    loader.read(posfile,setlist);
   
    // create a readstate that translates the particle parameters to surface shapes
    State readstate{true};
    readstate["pointpattern"].SetClass<pointpattern> ("addpoint", &pointpattern::addpoint );
    readstate.Load(readfile);
    pointpattern pp;

    for(auto it = setlist.begin(); it != setlist.end(); ++it )
    {
        particleparameterset set = (*it);
        readstate["s"][0] = set.get(0);
        readstate["s"][1] = set.get(1);
        readstate["s"][2] = set.get(2);
        readstate["s"][3] = set.get(3);
        readstate["s"][4] = set.get(4);
        readstate["docalculation"](pp); 

        //double x = readstate["result"][1][1][1];
        //std::cout << x << std::endl;
    }

    // print out pointpattern to a file for debugging purpose
    {
    std::ofstream file;
    file.open("pointpattern.xyz");
    file << pp;
    file.close();
    }


    // add particle shapes to voro++
    // TODO dont load from a file but add within code
    std::cout << "importing point pattern from file" << std::endl;
    const double xmin = state["xmin"];
    const double ymin = state["ymin"];
    const double zmin = state["zmin"];

    const double xmax = state["xmax"];
    const double ymax = state["ymax"];
    const double zmax = state["zmax"];

    container con(xmin, xmax, ymin, ymax, zmin, zmax, 8,8,8, false, false, false, 16);
    con.import("pointpattern.xyz");

    con.draw_cells_gnuplot("cells.gnu");
    return 0;
}
