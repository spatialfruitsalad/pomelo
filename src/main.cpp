#include <iostream>

#include "include.hpp"
#include "fileloader.hpp"
#include "pointpattern.hpp"
using namespace sel;
using namespace voro;

int main (int argc, char* argv[])
{
    
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

    std::vector<particleparameterset> setlist;
    fileloader loader;
    loader.read(posfile,setlist);
   

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
        pp.print();
    }
    return 0;
}
