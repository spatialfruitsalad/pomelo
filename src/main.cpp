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
    readstate.Load(readfile);
    //readstate["pointpattern"].SetClass<pointpattern> ("addpoint", &pointpattern::addpoint );
    readstate["particleparameterset"].SetClass<particleparameterset> ("get", &particleparameterset::get );

    pointpattern pp;

    for(auto it = setlist.begin(); it != setlist.end(); ++it )
    {
        particleparameterset set = (*it);
        readstate["docalculation"](set); 

        //double x = readstate["result"][1][1][1];
        //std::cout << x << std::endl;
    }
    return 0;
}
