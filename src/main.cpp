#include <iostream>
#include <fstream>
#include <map>
#include "include.hpp"
#include "fileloader.hpp"
#include "pointpattern.hpp"
using namespace sel;
using namespace voro;


bool compare(double d, double e, double epsilon)
{
    return (abs(d-e) < epsilon);
}

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
        for(unsigned int i = 0; i != set.parameter.size(); ++i)
        {
            readstate["s"][i] = set.get(i);
        }
        readstate["docalculation"](pp); 

        //double x = readstate["result"][1][1][1];
        //std::cout << x << std::endl;
    }

    const double epsilon = state["epsilon"];
    
    //checking for degenerated points
    /*
    for (   auto it = pp.points.begin();
                it != pp.points.end();
                ++it)
    {
        for (   auto it2 = pp.points.begin();
                it2 != pp.points.end();
                ++it2)
        {

        }
    }*/
    
    // print out pointpattern to a file for debugging purpose
    {
    std::cout << "save point pattern file" << std::endl;
    std::ofstream file;
    file.open("pointpattern.xyz");
    file << pp;
    file.close();
    }


    // add particle shapes to voro++
    std::cout << "importing point pattern from file" << std::endl;
    const double xmin = state["xmin"];
    const double ymin = state["ymin"];
    const double zmin = state["zmin"];

    const double xmax = state["xmax"];
    const double ymax = state["ymax"];
    const double zmax = state["zmax"];

    container con(xmin, xmax, ymin, ymax, zmin, zmax, 8,8,8, false, false, false, 16);
    //con.import("pointpattern.xyz");
   
    std::cout << "creating label id map" << std::endl;
    unsigned int id = 0; 
    std::map < int, int> labelidmap;
    for(    auto it = pp.points.begin();
            it != pp.points.end();
            ++it)
    {
        con.put(id, it->x, it->y, it->z);
        labelidmap[id] = it->l;
        ++id;
    }
    con.draw_cells_gnuplot("cells.gnu");


    std::cout << "map back ids to labels" << std::endl;
    //loop over all voronoi cells
    c_loop_all cla(con);
    std::vector<pointpattern> vclist;
    if(cla.start()) 
    {
        std::cout << "started"  << std::endl;
        do
        {
            voronoicell_neighbor c;
            if(con.compute_cell(c,cla)) 
            {
                //std::cout << "computed"  << std::endl;
                double x = 0;
                double y = 0; 
                double z = 0; 
                // Get the position of the current particle under consideration
                cla.pos(x,y,z);
                unsigned int id = cla.pid();
                unsigned int l = labelidmap[id];
                
                // get the position of the vertices of the cell                
                std::vector <double> v;
                c.vertices(x,y,z, v);
                
                std::vector <int> w;
                c.neighbors(w);
                    
                std::cout << "size "  << w.size() << std::endl;
                for (auto it = w.begin(); it != w.end(); ++ it)
                {
                    std::cout << (*it) << std::endl;
                }
                std::cout << std::endl;

                //std::cout << x << " " << y << " " << z << std::endl;
                pointpattern p;
                for (unsigned int i = 0; i != v.size(); ++(++(++i)))
                {
                    p.addpoint(v[i], v[i+1], v[i+2], l);
                    //std::cout << v[i] << " " << v[i+1] << " " << v[i+2] << std::endl;
                }
                vclist.push_back(p);
            } 
        }while (cla.inc());
    }
    
    // merge voronoi cells to set voronoi diagram
    // TODO replace O(N^2) loop by smarter algo
    std::cout << "merge voronoi cells" << std::endl; 
    pointpattern ppreduced;
    for(unsigned int i = 0; i != vclist.size() -1;++i)
    {
        std::cout << i << "/" << vclist.size() << std::endl;
        std::vector<point>& p1 = vclist[i].points;
        for (unsigned int j = i+1; j!= vclist.size();++j)
        {
            std::vector<point>& p2 = vclist[j].points;

            for (auto it1 = p1.begin(); it1 != p1.end(); ++it1)
            {
                for (auto it2 = p2.begin(); it2 != p2.end(); ++it2)
                {
                    if ((*it1).l == (*it2).l)
                    {
                        continue;
                    }
                    double x1 = (*it1).x;
                    double x2 = (*it2).x;
                    double y1 = (*it1).y;
                    double y2 = (*it2).y;
                    double z1 = (*it1).z;
                    double z2 = (*it2).z;
                    bool bx = compare(x1,x2,epsilon); 
                    bool by = compare(y1,y2,epsilon); 
                    bool bz = compare(z1,z2,epsilon); 
                    if (bx && by && bz)
                    {
                        ppreduced.addpoint(x1,y1,z1, (*it1).l);
                    }
                }
            }
        }
    }
    
    // print out reduced pointpattern to a file for debugging purpose
    {
    std::cout << "save reduced voronoi diagram" << std::endl;
    std::ofstream file;
    file.open("reduced.xyz");
    file << ppreduced;
    file.close();
    }

    
    return 0;
}
