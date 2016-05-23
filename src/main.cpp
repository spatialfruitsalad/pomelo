/* 
Copyright 2016 Simon Weis and Philipp Schoenhoefer

This file is part of Pomelo.

Pomelo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pomelo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pomelo.  If not, see <http://www.gnu.org/licenses/>.

The development of Pomelo took place at the Friedrich-Alexander University of Erlangen and was funded by the German Research Foundation (DFG) Forschergruppe FOR1548 "Geometry and Physics of Spatial Random Systems" (GPSRS). 
*/


#include <iostream>
#include <fstream>
#include <map>
#include <limits>
#include <sys/stat.h>
#include "include.hpp"
#include "fileloader.hpp"
#include "parsexyz.hpp"
#include "parsexyzr.hpp"
#include "parsetetra.hpp"
#include "pointpattern.hpp"
#include "duplicationremover.hpp"
#include "writerpoly.hpp"
#include "writeroff.hpp"
#include "postprocessing.hpp"
#include "output.hpp"

std::string version = "0.1.1";
#ifdef USELUA 
using namespace sel;
#endif
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

enum eMode
{
    GENERIC,
    SPHERE,
    SPHEREPOLY,
    TETRA,
    ELLIP,
    SPHCYL
} thisMode;

int main (int argc, char* argv[])
{

    // command line argument parsing
    if(argc != 4 )
    {
        std::cerr << "Commandline parameters not correct .... aborting "  << std::endl;
        std::cerr << std::endl <<  "Use pomelo this way:\n\t./pomelo -TYPE [position-file] [outputfolder]"  << std::endl;
        std::cerr <<  "\twith -TYPE being -SPHERE, -SPHEREPOLY -TETRA, -ELLIP, -SPHCYL"  << std::endl;
        std::cerr << std::endl <<  "Or in a generic way:\n\t./pomelo -GENERIC [path-to-lua-file] [outputfolder]"  << std::endl;
        return -1;
    }

    std::cout << "\nP O M E L O\n\nVersion " << version << "\nCopyright (C) 2016\nSimon Weis and Philipp Schoenhoefer\n\n";
    std::cout << "pomelo home:\t\t http://www.theorie1.physik.uni-erlangen.de/" << std::endl << std::endl << std::endl;

/////////////////////
// Parse Command line arguments
/////////////////////
    const std::string mode = argv[1];
    const std::string filename = argv[2];
    std::string folder = argv[3];
    try
    {
    if (mode == "-SPHERE" || mode == "--SPHERE")
    {
        thisMode = SPHERE;
    }
    else if (mode == "-SPHEREPOLY" || mode == "--SPHEREPOLY")
    {
        thisMode = SPHEREPOLY;
    }
    else if (mode == "-TETRA")
    {
        thisMode = TETRA;
    }
    else if (mode == "-ELLIP")
    {
        thisMode = ELLIP;
    }
    else if (mode == "-SPHCYL")
    {
        thisMode = SPHCYL;
    }
    else if (mode == "-GENERIC" || mode == "--GENERIC")
    {
#ifdef USELUA
        thisMode = GENERIC;
#else
        throw std::string ("unknown mode " + mode + ".\n to enable GENERIC mode, you must build pomelo with 'make GENERIC'");

#endif
    }
    else
    {
        throw std::string ("unknown mode " + mode);
    }
    }
    catch (std::string const &  e)
    {
        std::cerr << "an error occurred: " << e << std::endl;
        return -1;
    }
/////////////////////
// check if folder is ok and create it
/////////////////////
    // command line sanity check
    if(folder.empty())
    {
        std::cerr << "outfilepath is not valid" << std::endl;
        return -1;
    }

    // sanitize folder path and create folder 
    char lastCharOfFolder = *folder.rbegin();
    if (lastCharOfFolder != '/')
        folder += '/';
    mkdir(folder.c_str(),0755);


/////////////////////
// Parameters that are needed
/////////////////////
    // pp contains the triangulation of the particle surfaces
    pointpattern pp;
    double epsilon = 1e-12;
    double xmin = 0;
    double ymin = 0;
    double zmin = 0;
    double xmax = 0;
    double ymax = 0;
    double zmax = 0;
    bool xpbc = false;
    bool ypbc = false;
    bool zpbc = false;
    output outMode;
    
/////////////////////
// Load Particles in GENERIC Mode
/////////////////////
#ifdef USELUA
    if (thisMode == GENERIC)
    {
        std::cout << "command line arguments parsed:\nLUA Parameter File: " << filename << "\noutfolder: " << folder << std::endl << std::endl;

        // lua state for the global parameter file
        State state {true};
        if(!state.Load(filename))
        {
            std::cerr << "error loading lua parameter file: " << filename << std::endl;
            return -1;
        }

        // parse output from lua file
        outMode.savesurface = state["savesurface"];
        outMode.savepoly = state["savepoly"];
        outMode.saveoff = state["saveoff"];
        outMode.savereduced = state["savereduced"];
        outMode.postprocessing = state["postprocessing"];
        // parse global parameters from lua file
        std::string posfile = state["positionfile"];
        std::string readfile = state["readfile"];

        std::cout << "Parsing Position File... \nWorking on " << posfile << " " << readfile << std::endl;

        // read particle parameters and positions
        std::vector<particleparameterset> setlist;
        fileloader loader;
        loader.read(posfile,setlist);

        std::cout << "Creating Surface Triangulation... " << std::flush;
        
        // scope for the readstate to ensure it won't lack out to anything else
        {
            // create a readstate that translates the particle parameters to surface shapes
            State readstate {true};
            readstate["pointpattern"].SetClass<pointpattern> ("addpoint", &pointpattern::addpoint );
            readstate.Load(readfile);


            for(auto it = setlist.begin(); it != setlist.end(); ++it )
            {
                // read one particle from the position file
                particleparameterset set = (*it);
                // put all parameters for this particle to the lua readstate
                for(unsigned int i = 0; i != set.parameter.size(); ++i)
                {
                    readstate["s"][i] = set.get(i);
                }

                // let the lua readstate calculate the surface triangulation for this particle
                readstate["docalculation"](pp);
            }
        }
        std::cout << "finished!" << std::endl;
        std::cout << "points created: " << pp.points.size() << std::endl << std::endl;

        // parse epsilon from the global lua parameter file
        epsilon = state["epsilon"];
        // parse boundaries from the global lua parameter file
        xmin = state["xmin"];
        ymin = state["ymin"];
        zmin = state["zmin"];
        xmax = state["xmax"];
        ymax = state["ymax"];
        zmax = state["zmax"];
        xpbc = false;
        ypbc = false;
        zpbc = false;

        std::string boundary = state["boundary"];
        if(boundary == "periodic")
        {
            std::cout << "boundary condition mode 'periodic' selected." ;
            xpbc = state["xpbc"];
            ypbc = state["ypbc"];
            zpbc = state["zpbc"];
            std::cout << "x: " << xpbc << "\ny: " << ypbc << "\nz: " << zpbc << std::endl;
        }
        else if (boundary == "none")
        {
            std::cout << "boundary condition mode 'none' selected." << std::endl;
        }
        else
        {
            std::cerr << "bondary condition mode " << boundary << " not known" << std::endl;
        }
        std::cout << std::endl;
    }
#endif
    if (thisMode == SPHERE)
    {
        parsexyz p;
        p.parse(filename, pp);

        xmin = p.xmin;
        ymin = p.ymin;
        zmin = p.zmin;
        xmax = p.xmax;
        ymax = p.ymax;
        zmax = p.zmax;
        xpbc = p.xpbc;
        ypbc = p.ypbc;
        zpbc = p.zpbc;
    }
    else if (thisMode == SPHEREPOLY)
    {
        parsexyzr p;
        p.parse(filename, pp);

        xmin = p.xmin;
        ymin = p.ymin;
        zmin = p.zmin;
        xmax = p.xmax;
        ymax = p.ymax;
        zmax = p.zmax;
        xpbc = p.xpbc;
        ypbc = p.ypbc;
        zpbc = p.zpbc;
    }
    else if (thisMode == TETRA)
    {
        parsetetra p;
        p.parse(filename, pp);
    
        std::cout << "epsilon " << epsilon << std::endl;
        xmin = p.xmin;
        ymin = p.ymin;
        zmin = p.zmin;
        xmax = p.xmax;
        ymax = p.ymax;
        zmax = p.zmax;
        xpbc = p.xpbc;
        ypbc = p.ypbc;
        zpbc = p.zpbc;
    }




    // clean degenerated vertices from particle surface triangulation pointpattern
    std::cout << "remove duplicates in surface triangulation" << std::endl;
    duplicationremover d(16,16,16);
    d.setboundaries(xmin, xmax, ymin, ymax, zmin, zmax);
    d.addPoints(pp);
    d.removeduplicates(epsilon);
    d.getallPoints(pp);
    std::cout << std::endl;

    // print out pointpattern file
    if (outMode.savesurface == true)
    {
        std::cout << "save point pattern file" << std::endl;
        std::ofstream file;
        file.open(folder + "surface_triangulation.xyz");
        file << pp;
        file.close();
    }
    std::cout << std::endl;


    // add particle surface triangulation to voro++ pre container for subcell division estimate
    std::cout << "importing surface triangulation to voro++" << std::endl;

    pre_container pcon(xmin, xmax, ymin, ymax, zmin, zmax, xpbc, ypbc, zpbc);
    
    // label ID map is used to  map surface point IDs to the respective particle label
    std::cout << "creating label id map " ;
    std::map < unsigned long long, unsigned long long > labelidmap;

    std::vector<std::vector<double> > ref;
    {
        unsigned long long id = 0;
        for(    auto it = pp.points.begin();
                it != pp.points.end();
                ++it)
        {
            pcon.put(id, it->x, it->y, it->z);
            unsigned int l = it->l;
            labelidmap[id] = l;
            ++id;
            if(ref.size() < l+1) ref.resize(l+1);
            if(ref[l].size() == 0)
            {
                ref[l].push_back(it->x);
                ref[l].push_back(it->y);
                ref[l].push_back(it->z);
                ref[l].push_back(0);
                ref[l].push_back(0);
                ref[l].push_back(0);
            }
        }
    }
    unsigned long long numberofpoints = labelidmap.size();
    std::cout << "finished" << std::endl;
    
    // setting up voro++ container
    int nx, ny, nz;
    pcon.guess_optimal(nx,ny,nz);
    container con(xmin, xmax, ymin, ymax, zmin, zmax, nx, ny, nz, xpbc, ypbc, zpbc, 8);
    pcon.setup(con);
    std::cout << "setting up voro++ container with division: (" << nx << " " << ny << " " << nz << ") for N= " << numberofpoints << " particles " << std::endl;
    std::cout << std::endl;

    // postprocessing for normal voronoi Output
    if(outMode.postprocessing == true)
    {

        std::cout << "Performing Postprocessing for normal (unmerged) Voronoi Cells" << std::endl;

        std::string customFileName = folder + "custom.dat";
        con.print_custom("%i %s %v", customFileName.c_str());
        DoPostProcessing(folder);
    }
    else
    {
        std::cout << "skipping postprocessing" << std::endl;
    }
    std::cout << std::endl;
    
    // merge voronoi cells to set voronoi diagram
    std::cout << "merge voronoi cells ";
    
    pointpattern ppreduced;
    writerpoly pw;
    // loop over all voronoi cells
    c_loop_all cla(con);
    // cell currently worked on
    unsigned long long status = 0;
    // counter for process output
    double tenpercentSteps = 0.1*static_cast<double>(numberofpoints);
    double target = tenpercentSteps;

    double xdist = xmax - xmin;
    double ydist = ymax - ymin;
    double zdist = zmax - zmin;

    //unsigned long long refl = 0;

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

                double xabs = (xc-ref[l][0]);
                double yabs = (yc-ref[l][1]);
                double zabs = (zc-ref[l][2]);

                double xabs2 = xabs*xabs;
                double yabs2 = yabs*yabs;
                double zabs2 = zabs*zabs;

                ref[l][3]=0;
                if(xabs2 > 4)
                {
                    if(xabs < 0) ref[l][3] = xdist;
                    else ref[l][3] = -xdist;
                }
                ref[l][4]=0;
                if(yabs2 > 4)
                {
                    if(yabs < 0) ref[l][4] = ydist;
                    else ref[l][4] = -ydist;
                }
                ref[l][5]=0;
                if(zabs2 > 4)
                {
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
                        // discard this neighbour/face, since they have the same id
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

                            ppreduced.addpoint(l,x,y,z);
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
    std::cout << std::endl;

    // save point pattern output
    if(outMode.savereduced == true)
    {
        pw.savePointPatternForGnuplot(folder + "reduced.xyz");
    }

    std::cout << std::endl;
    // remove duplicates and label back indices
    pw.removeduplicates(epsilon, xmin, xmax, ymin, ymax, zmin, zmax);

    std::cout << std::endl;
    // Write poly file for karambola
    if(outMode.savepoly == true)
    {
        std::cout << "writing poly file" << std::endl;
        std::ofstream file;
        file.open(folder + "cell.poly");
        file << pw;
        file.close();
    }
    if(outMode.saveoff == true)
    {
    std::cout << "writing off file" << std::endl;
    std::ofstream file;
    file.open(folder+"cell.off");
    writeroff wo(pw);
    file << wo;
    file.close();
    }

    std::cout << "\nworking for you has been nice. Thank you for using me & see you soon. :) "<< std::endl;

    return 0;
}
