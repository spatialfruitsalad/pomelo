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

    
    pointpattern ppnew;
    
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
   
    std::cout << "creating label id map " ;
    std::map < unsigned long long, unsigned long long > labelidmap;
    {
    unsigned long long id = 0; 
    for(    auto it = pp.points.begin();
            it != pp.points.end();
            ++it)
    {
        con.put(id, it->x, it->y, it->z);
        labelidmap[id] = it->l;
        ++id;
    }
    }
    unsigned long long numberofpoints = labelidmap.size();
    std::cout << "finished" << std::endl;
    // will crash for reasons if there are too many voronoi cells
    //con.draw_cells_gnuplot("cells.gnu");


    // merge voronoi cells to set voronoi diagram
    std::cout << "merge voronoi cells "; 
    
    pointpattern ppreduced;
    //loop over all voronoi cells
    c_loop_all cla(con);
    unsigned long long status = 0;
    if(cla.start()) 
    {
        std::cout << "started" << std::flush;
        do
        {
            voronoicell_neighbor c;
            status++;
            if(con.compute_cell(c,cla)) 
            {
                std::cout << status << "/" << numberofpoints << "\n";
                //std::cout << "computed"  << std::endl;
                double xc = 0;
                double yc = 0; 
                double zc = 0; 
                // Get the position of the current particle under consideration
                cla.pos(xc,yc,zc);
                unsigned int id = cla.pid();
                unsigned long long l = labelidmap[id];

                std::vector<int> f; // vertices of faces (bracketed, as ID)
                c.face_vertices(f);

                std::vector<double> vertices;   // all vertices for this face
                c.vertices(xc,yc,zc, vertices); 
                
                
                std::vector<int> w; // neighbours of faces 
                c.neighbors(w);
                for (unsigned long long k = 0; k != w.size(); ++k)
                {
                    //std::cout << (*it) << std::endl;
                    int n = w[k];   // ID of neighbor cell
                    if (labelidmap[n] == l)
                    {
                        // discard this face
                        //std::cout << "discarding face " << std::endl;
                    }
                    else
                    {
                        unsigned long long index = 0;
                        for (unsigned long long cc = 0; cc <= k; cc++)
                        {
                            unsigned long long b = f[index];
                            for (unsigned long long bb = 1; bb <= b; bb++)
                            {
                                index++;
                                if (cc == k)
                                {
                                    //std::cout << "\t" << f[index] << std::endl;
                                    int vertexindex = f[index];
                                    double x = vertices[vertexindex*3];
                                    double y = vertices[vertexindex*3+1];
                                    double z = vertices[vertexindex*3+2];
                                    ppreduced.addpoint(x,y,z,l);
                                }
                            }
                            index++;
                        }
                    }
                }

            } 
        }while (cla.inc());
    }
    std::cout << " finished" << std::endl; 
    
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
