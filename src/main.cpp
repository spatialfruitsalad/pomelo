#include <iostream>
#include <fstream>
#include <map>
#include <limits>
#include <sys/stat.h>
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
    State state {true};

    if(argc != 3 )
        {
        std::cerr << "Commandline parameters not correct .... aborting "  << std::endl;
        std::cerr << std::endl <<  "Use setvoronoi this way:\n\t ./setvoronoi [path-to-lua-file] [outputfolder]"  << std::endl;
        return -1;
    }

    const std::string filename = argv[1];
    std::string folder = argv[2];

    if(folder.empty())
    {
        throw std::string("outfilepath is empty");
    }
    
    char lastCharOfFolder = *folder.rbegin();
    if (lastCharOfFolder != '/')
    folder += '/';

    mkdir(folder.c_str(),0755);    

    state.Load(filename);

    std::string posfile = state["positionfile"];
    std::string readfile = state["readfile"];

    std::cout << "Working on " << posfile << " " << readfile << std::endl;

    // read particle parameters and positions
    std::vector<particleparameterset> setlist;
    fileloader loader;
    loader.read(posfile,setlist);

    // create a readstate that translates the particle parameters to surface shapes
    State readstate {true};
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
    const double epsilon = state["epsilon"];
    bool xpbc = state["xpbc"];
    bool ypbc = state["ypbc"];
    bool zpbc = state["zpbc"];

    std::cout << "remove duplicates" << std::endl;
    duplicationremover d(16,16,16);
    d.setboundaries(xmin, xmax, ymin, ymax, zmin, zmax);
    d.addPoints(pp);
    d.removeduplicates(epsilon);
    d.getallPoints(pp);

    // print out pointpattern to a file for debugging purpose
    {
        std::cout << "save point pattern file" << std::endl;
        std::ofstream file;
        file.open(folder + "pointpattern.xyz");
        file << pp;
        file.close();
    }


    // add particle shapes to voro++
    std::cout << "importing point pattern from file" << std::endl;

    pre_container pcon(xmin, xmax, ymin, ymax, zmin, zmax, xpbc, ypbc, zpbc);

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
    container con(xmin, xmax, ymin, ymax, zmin, zmax, nx, ny, nz, xpbc, ypbc, zpbc, 8);
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
    double tenpercentSteps = 0.1*static_cast<double>(numberofpoints);
    double target = tenpercentSteps;
    double xdist = xmax - xmin;
    double ydist = ymax - ymin;
    double zdist = zmax - zmin;

    std::vector<std::vector<double> > ref;
    unsigned long long refl = 0;

    if(cla.start())
    {
        std::cout << "started\n" << std::flush;
        do
        {
            voronoicell_neighbor c;
            status++;
            if(con.compute_cell(c,cla))
            {
                if ( status >= target)
                {
                    target += tenpercentSteps;
                    std::cout << static_cast<int>(static_cast<double>(status)/static_cast<double>(numberofpoints)*100) << " \%\t" << std::flush;
                } 
                //std::cout << "computed"  << std::endl;
                double xc = 0;
                double yc = 0;
                double zc = 0;
                // Get the position of the current particle under consideration
                cla.pos(xc,yc,zc);
	
                unsigned int id = cla.pid();
                unsigned long long l = labelidmap[id];
		
		
		if(refl != l){		
		    if(ref.size() < l+1) ref.resize(l+1);
		    if(ref[l].size() == 0){
                        ref[l].push_back(xc);
                        ref[l].push_back(yc);
                        ref[l].push_back(zc);
			ref[l].push_back(0);
                        ref[l].push_back(0);
                        ref[l].push_back(0);
		    }
		    refl = l;
		}
	
		double xabs = (xc-ref[l][0]);		
		double yabs = (yc-ref[l][1]);		
		double zabs = (zc-ref[l][2]);	

		double xabs2 = xabs*xabs;
		double yabs2 = yabs*yabs;
		double zabs2 = zabs*zabs;
		
		ref[l][3]=0;
		if(xabs2 > 4){
			if(xabs < 0) ref[l][3] = xdist;
			else ref[l][3] = -xdist;
		}
		ref[l][4]=0;
		if(yabs2 > 4){
			if(yabs < 0) ref[l][4] = ydist;
			else ref[l][4] = -ydist;
		}
		ref[l][5]=0;
		if(zabs2 > 4){
			if(zabs < 0) ref[l][5] = zdist;
			else ref[l][5] = -zdist;
		}
	
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
                        // std::cout << "discarding face " << l << " " << labelidmap[n] << std::endl;
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
                            	
			    if(xpbc) x += ref[l][3]; 
			    if(ypbc) y += ref[l][4]; 
			    if(zpbc) z += ref[l][5]; 

			    ppreduced.addpoint(x,y,z,l);
                            positionlist.push_back(x);
                            positionlist.push_back(y);
                            positionlist.push_back(z);
                        }
                        pw.addface(positionlist, l);

                    }
                }

            }
        }
        while (cla.inc());
    }
    std::cout << std::endl << " finished with N= " << ppreduced.points.size() << std::endl;


    pw.savePointPatternForGnuplot(folder + "reduced.xyz");
   
    // polywriter: remove duplicates
    pw.removeduplicates(epsilon, xmin, xmax, ymin, ymax, zmin, zmax);

    {
        std::cout << "writing poly file" << std::endl;
        std::ofstream file;
        file.open(folder + "cell.poly");
        file << pw;
        file.close();
    }

    return 0;
}
