#include <iostream>
#include <fstream>
#include <map>
#include <limits>
#include "include.hpp"
#include "fileloader.hpp"
#include "pointpattern.hpp"
#include "duplicationremover.hpp"
#include "polywriter.hpp"
using namespace sel;
using namespace voro;


void  getFaceVerticesOfFace( std::vector<int>& f, unsigned int k, std::vector<unsigned int>& vertexList)
{
    vertexList.clear();

    // iterate through face-vertices vector bracketed
    // (number of vertices for face 1) vertex 1 ID face 1, vertex 2 ID face 1 , ... (number of vertices for face 2, ...
    unsigned long long index = 0;
    // we are at face k, so we have to iterate through the face vertices vector up to k
    for (unsigned long long cc = 0; cc <= k; cc++)
    {

        unsigned long long b = f[index];    // how many vertices does the current face (index) have?
        // iterate index "number of vertices of this face" forward
        for (unsigned long long bb = 1; bb <= b; bb++)  
        {
            index++;
            // if we have found the correct face, get all the vertices for this face and save them
            if (cc == k)
            {
                //std::cout << "\t" << f[index] << std::endl;
                int vertexindex = f[index];
                vertexList.push_back(vertexindex);
            }
        }
        index++;
    }
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
    const double xmin = state["xmin"];
    const double ymin = state["ymin"];
    const double zmin = state["zmin"];

    const double xmax = state["xmax"];
    const double ymax = state["ymax"];
    const double zmax = state["zmax"];
    std::cout << "remove duplicates" << std::endl;
    duplicationremover d(16,16,16); 
    d.setboundaries(xmin, xmax, ymin, ymax, zmin, zmax);
    d.addPoints(pp);
    d.removeduplicates(1e-6);
    d.getallPoints(pp);

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

    pre_container pcon(xmin, xmax, ymin, ymax, zmin, zmax, false, false, false);
   
    std::cout << "creating label id map " ;
    std::map < unsigned long long, unsigned long long > labelidmap;

    {
        unsigned long long id = 0; 
        for(    auto it = pp.points.begin();
                it != pp.points.end();
                ++it)
        {
            pcon.put(id, it->x, it->y, it->z);
            labelidmap[id] = it->l;
            ++id;
        }
    }
    unsigned long long numberofpoints = labelidmap.size();
    std::cout << "finished" << std::endl;

    int nx, ny, nz;
    pcon.guess_optimal(nx,ny,nz);
    container con(xmin, xmax, ymin, ymax, zmin, zmax, nx, ny, nz, false, false, false, 8);
    pcon.setup(con);
    std::cout << "setting up voro++ container with division: (" << nx << " " << ny << " " << nz << ") for N= " << numberofpoints << " particles " << std::numeric_limits<int>::max() << std::endl;

    //con.draw_cells_gnuplot("cells.gnu");


    // merge voronoi cells to set voronoi diagram and write poly file
    std::cout << "merge voronoi cells "; 
    
    pointpattern ppreduced;
    polywriter pw;
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

                std::vector<int> f; // list of face vertices (bracketed, as ID)
                c.face_vertices(f);

                std::vector<double> vertices;   // all vertices for this cell
                c.vertices(xc,yc,zc, vertices); 
                
                
                std::vector<int> w; // neighbors of faces 
                c.neighbors(w);
                // for this cell, loop over all faces and get the corresponding neighbors
                for (unsigned long long k = 0; k != w.size(); ++k)
                {
                    // compare if id for this cell and the face-neighbor is the same
                    int n = w[k];   // ID of neighbor cell
                    if (labelidmap[n] == l)
                    {
                        // discard this face, since they have the same id
                        //std::cout << "discarding face " << std::endl;
                    }
                    else
                    {
                        std::vector<unsigned int> facevertexlist;
                        getFaceVerticesOfFace(f, k, facevertexlist);
                        std::vector<double> positionlist;
                        for (
                                auto it = facevertexlist.begin(); 
                                it != facevertexlist.end();
                                ++it)
                        {
                            unsigned int vertexindex = (*it);
                            double x = vertices[vertexindex*3];
                            double y = vertices[vertexindex*3+1];
                            double z = vertices[vertexindex*3+2];
                            ppreduced.addpoint(x,y,z,l); 
                            positionlist.push_back(x);
                            positionlist.push_back(y);
                            positionlist.push_back(z);
                        }
                        pw.addface(positionlist, l);

                    }
                }

            } 
        }while (cla.inc());
    }
    std::cout << " finished with N= " << ppreduced.points.size() << std::endl; 
    {
    std::ofstream file;
    file.open("cell.poly");
    file << pw;
    file.close();
    }
    // remove duplicate points for voronoi cells
    //ppreduced.removeduplicates(1e-6);
    std::cout << "remove duplicates in voronoi vertices" << std::endl;
    duplicationremover d2(16,16,16);
    d2.setboundaries(xmin, xmax, ymin, ymax, zmin, zmax);
    d2.addPoints(ppreduced);
    d2.removeduplicates(1e-6);
    d2.getallPoints(ppreduced);
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
