/* 
Copyright 2018 Simon Weis and Philipp Schoenhoefer

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
#include "cmdlparser.hpp"
#include "fileloader.hpp"
#include "parsexyz.hpp"
#include "parsexyzr.hpp"
#include "parsetetra.hpp"
#include "parsetetra_blunt.hpp"
#include "parsesphcyl.hpp"
#include "parseellipsoids.hpp"
#include "pointpattern.hpp"
#include "duplicationremover.hpp"
#include "writerfe.hpp"
#include "writerpoly.hpp"
#include "writeroff.hpp"
#include "postprocessing.hpp"
#include "output.hpp"

std::string version = "0.1.4";
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


int main (int argc, char* argv[])
{
    

    std::cout << "\nP O M E L O\n\nVersion " << version << "\nCopyright (C) 2016\nSimon Weis and Philipp Schoenhoefer\n\n";
    std::cout << "pomelo home:\t\t http://www.theorie1.physik.uni-erlangen.de/" << std::endl << std::endl << std::endl;


/////////////////////
// Parse Command line arguments
/////////////////////
    cmdlParser cp;
    try
    {
        cp.parseArguments(argc, argv); 
        cp.sanityCheckParameters();
    }
    catch(std::string& e)
    {
        cp.printHelp();
        std::cerr << e << std::endl;
        return -1;
    }


    std::string folder = cp.outfolder;
    
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
    std::cout << "creating Folder for output at " << folder << std::endl;
    mkdir(folder.c_str(),0755);


/////////////////////
// Parameters that are needed
/////////////////////
    // pp contains the triangulation of the particle surfaces
    pointpattern pp;
    double epsilon = 1e-10;
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
    if (cp.thisMode == GENERIC)
    {
        std::cout << "command line arguments parsed:\nLUA Parameter File: " << cp.filename << "\noutfolder: " << folder << std::endl << std::endl;

        // lua state for the global parameter file
        State state {true};
        if(!state.Load(cp.filename))
        {
            std::cerr << "error loading lua parameter file: " << cp.filename << std::endl;
            return -1;
        }

        // parse output from lua file
        outMode.savesurface = state["savesurface"];
        outMode.savepoly = state["savepoly"];
        outMode.saveoff = state["saveoff"];
        outMode.savereduced = state["savereduced"];
        outMode.savefe = state["savefe"];
        outMode.postprocessing = state["postprocessing"];
        // parse global parameters from lua file
        std::string posfile = state["positionfile"];
        std::string readfile = state["readfile"];

        std::cout << "Parsing Position File... \nWorking on " << posfile << " " << readfile << std::endl;

        {
            std::ifstream in(readfile);
            if (!in.good())
            {
                std::cerr << "Error in GENERIC mode: readfile " << readfile << " not found!\n\nAborting." << std::endl;
                return 5;
            }
        }

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
            std::cout << "\nx: " << xpbc << "\ny: " << ypbc << "\nz: " << zpbc << std::endl;
        }
        else if (boundary == "none")
        {
            std::cout << "boundary condition mode 'none' selected." << std::endl;
            std::string ignoreboundarycells = state["ignoreboundarycells"];
        }
        else if (boundary == "ignore")
        {
           cp.ignoreBoundaryCellsswitch = true;
        }
        else
        {
            std::cerr << "bondary condition mode " << boundary << " not known" << std::endl;
        }
        std::cout << std::endl;
    }
#endif

    if(cp.polyswitch)
    {
        outMode.savesurface = false;
        outMode.savepoly = true;
        outMode.saveoff = false;
        outMode.savereduced = false;
        outMode.savefe = false;
        outMode.postprocessing = false;
    }
    
    if (cp.thisMode == SPHERE)
    {
        parsexyz p;
        p.parse(cp.filename, pp);

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
    else if (cp.thisMode == SPHEREPOLY)
    {
        parsexyzr p;
        std::cout << "loading file: " << cp.filename << std::endl;
        p.parse(cp.filename, pp);

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
    else if (cp.thisMode == ELLIP)
    {
        parseellipsoid p;
        p.parse(cp.filename, pp);
        outMode.postprocessing = true; 
    
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
    else if (cp.thisMode == TETRA)
    {
        parsetetra p;
        double shrink = cp.shrink;
        int iterations = cp.iterations;
        p.parse(cp.filename, pp, shrink, iterations);
        outMode.postprocessing = false; 
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
    else if (cp.thisMode == TETRABLUNT)
    {
        parsetetrablunt p;
        double shrink = cp.shrink;
        int iterations = cp.iterations;
        std::cout << "number of iterations: " << iterations << std::endl;
        p.parse(cp.filename, pp, shrink, iterations);
        outMode.postprocessing = true; 
        outMode.saveoff = false; 
        outMode.savesurface = true;
        outMode.savereduced = true;
        epsilon = 1e-11;
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
    else if (cp.thisMode == SPHCYL)
    {

        parsesphcyl p;
        p.parse(cp.filename, pp);

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


    std::cout << "boundarybox\n";
    std::cout << "nx: [" << xmin << ", " << xmax << "]\n";
    std::cout << "ny: [" << ymin << ", " << ymax << "]\n";
    std::cout << "nz: [" << zmin << ", " << zmax << "]\n";
     
    // clean degenerated vertices from particle surface triangulation pointpattern
    {
        std::cout << "remove duplicates in surface triangulation" << std::endl;
        duplicationremover d(40,40,40);
        d.setboundaries(xmin, xmax, ymin, ymax, zmin, zmax);
        d.addPoints(pp);
        d.removeduplicates(epsilon);
        d.getallPoints(pp);
        std::cout << std::endl;
    }
    // print out pointpattern file
    if (outMode.savesurface == true)
    {
        std::cout << "save surface triangulation file" << std::endl;
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
    // volumemap is a map from particlelabel to voronoi cell volume
    std::vector <double> volumeMap;
    std::vector<std::vector<double> > neighbourcell;
    unsigned long long maxParticleLabel = 0;
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
            if (l > maxParticleLabel) maxParticleLabel =l;
            
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
    
    if (outMode.postprocessing == true)
    {

        volumeMap.resize(maxParticleLabel+1, 0);
        neighbourcell.resize(maxParticleLabel+1);
    }

    std::cout << "finished" << std::endl;
    
    std::cout << "clear Surface Triangulation ... ";

    pp.clear();

    std::cout << "done" << std::endl;



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
        if( cp.ignoreBoundaryCellsswitch == false) 
        {
            std::cout << "Performing Postprocessing for normal (unmerged) Voronoi Cells" << std::endl;

            std::string customFileName = folder + "custom.dat";
            con.print_custom("%i %s %v", customFileName.c_str());
            DoPostProcessing(folder);
        }
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
    double tenpercentSteps = 0.01*static_cast<double>(numberofpoints);
    double target = tenpercentSteps;

    double xdist = xmax - xmin;
    double ydist = ymax - ymin;
    double zdist = zmax - zmin;

    //unsigned long long refl = 0;
    
    // create ignore map 
    std::map<unsigned long, bool> ignoreMap;
    for (unsigned int ii = 0; ii != maxParticleLabel; ++ii)
    {
        ignoreMap[ii] = false;
    }

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
                    std::cout << static_cast<int>(static_cast<double>(status)/static_cast<double>(numberofpoints)*100) << " \% " << std::flush;
                }
                //std::cout << "computed"  << std::endl;
                double xc = 0;
                double yc = 0;
                double zc = 0;
                // Get the position of the current particle under consideration
                cla.pos(xc,yc,zc);

                unsigned int id = cla.pid();
                unsigned long long l = labelidmap[id];
                if (outMode.postprocessing == true)
                {
                    if (cp.ignoreBoundaryCellsswitch)
                    {
                        // check neighbor cells
                        std::vector<int> w; // neighbors of faces
                        c.neighbors(w);
                        // for this cell, loop over all faces and get the corresponding neighbors
                        for (unsigned long long k = 0; k != w.size(); ++k)
                        {
                            // compare if id for this cell and the face-neighbor is the same
                            int n = w[k];   // ID of neighbor cell
                            if (labelidmap[n] == 0)
                            {
                                ignoreMap[l] = true;
                                break;
                            }
                        }
                    } 
                    if (ignoreMap[l] == false)
                    {
                        volumeMap[l] += c.volume();  
                    }
                    else
                    {
                        volumeMap[l] = 0;
                    }
                }
                double xabs = (xc-ref[l][0]);
                double yabs = (yc-ref[l][1]);
                double zabs = (zc-ref[l][2]);

                double xabs2 = xabs*xabs;
                double yabs2 = yabs*yabs;
                double zabs2 = zabs*zabs;

                double xabs_alt, yabs_alt, zabs_alt;
                double xabs2_alt, yabs2_alt, zabs2_alt;

                ref[l][3]=0;
                if(xpbc)
                {
                    if(xabs < 0) xabs_alt = xabs + xdist;
                    else xabs_alt = xabs - xdist;
                    
                    xabs2_alt = xabs_alt*xabs_alt;

                    if(xabs2 < xabs2_alt) ref[l][3] = 0;
                    else
                    { 
                        if(xabs < 0) ref[l][3] = xdist;
                        else ref[l][3] = -xdist;
                    }
                }
                ref[l][4]=0;
                if(ypbc)
                {
                    if(yabs < 0) yabs_alt = yabs + ydist;
                    else yabs_alt = yabs - ydist;
                    
                    yabs2_alt = yabs_alt*yabs_alt;

                    if(yabs2 < yabs2_alt) ref[l][4] = 0;
                    else
                    { 
                        if(yabs < 0) ref[l][4] = ydist;
                        else ref[l][4] = -ydist;
                    }
                }
                ref[l][5]=0;
                if(zpbc)
                {
                    if(zabs < 0) zabs_alt = zabs + zdist;
                    else zabs_alt = zabs - zdist;
                    
                    zabs2_alt = zabs_alt*zabs_alt;

                    if(zabs2 < zabs2_alt) ref[l][5] = 0;
                    else
                    { 
                        if(zabs < 0) ref[l][5] = zdist;
                        else ref[l][5] = -zdist;
                    }
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
                        //std::cout << "discarding face " << l << " " << labelidmap[n] << std::endl;

                    }
                    else
                    {
                        if (outMode.postprocessing == true && l != 0 && labelidmap[n] != 0)
                        {
                            bool NotAlreadyNeighbour = true;
                            for( unsigned long long kk = 0; kk != neighbourcell[l].size(); ++kk){
                                if(labelidmap[n] == neighbourcell[l][kk]){
                                    NotAlreadyNeighbour = false;
                                    break;
                                }
                            }

                            if(NotAlreadyNeighbour){
                                neighbourcell[l].push_back(labelidmap[n]);
                                neighbourcell[labelidmap[n]].push_back(l);
                            }
                        }

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

    con.clear();

    // write parameter file
    {
        std::ofstream out(folder + "parameter.dat");
        out << "Input File Name: " << cp.filename + "\n";
        out << "Mode: " << cp.thisMode << "\n";
        out << "Output Folder: " << cp.outfolder << "\n";
        out << "Shrink (cmdl parser): " << cp.shrink << "\n";       
    }

    if(outMode.postprocessing == true)
    {
        std::cout << "save set voronoi cell volumes" << std::endl;
        // save set voronoi volumes
        std::ofstream out(folder+"setVoronoiVolumes.dat");
        out << "#1_particle label #2_set voronoi cell volume\n";
        for (unsigned long long i = 0; i != volumeMap.size(); ++i)
        {
            out << i << " " << std::setprecision(12) << volumeMap[i] << "\n";
        }
        out.close();

        std::cout << "save number of neighbours" << std::endl;
        std::ofstream nout(folder+"setVoronoiNeighbours.dat");
        nout << "#1_particle label #2_number of neighbours\n";
        for (unsigned long long i = 0; i != neighbourcell.size(); ++i)
        {
            nout << i << " " << neighbourcell[i].size() << " ( ";
            for (unsigned long long ii = 0; ii != neighbourcell[i].size(); ++ii)
                nout << neighbourcell[i][ii] << " ";
            nout << ")\n";
        }
        nout.close();
    } 

    if (ppreduced.points.size() == 0)
    {
        std::cout << "\nall Voronoi Vertices have been removed. Check for periodic boundary conditions. skipping further calculation." << std::endl;
        std::cout << "\nworking for you has been nice. Thank you for using me & see you soon. :) "<< std::endl;
        return 0;
    }
    // save point pattern output
    if(outMode.savereduced == true)
    {
        pw.savePointPatternForGnuplot(folder + "reduced.xyz");
    }

    std::cout << std::endl;
    // remove duplicates and label back indices
    pw.removeduplicates(epsilon, xmin, xmax, ymin, ymax, zmin, zmax, nx, ny, nz);

    std::cout << std::endl;
    // Write poly file for karambola
    if(outMode.savepoly == true)
    {
        std::cout << "writing poly file: " << folder + "cell.poly" << std::endl;
        std::ofstream file;

        file.open(folder + "cell.poly");
        if (!file.good())
        {
            std::cerr << "error: cannot open poly file for write" << std::endl;
            throw std::string("error: cannot open poly file for write");
        }
        file << pw;
        file.close();
    }
    if(outMode.saveoff == true)
    {
        std::cout << "writing off file" << std::endl;
        std::ofstream file;
        file.open(folder+"cell.off");
        if (!file.good())
        {
            std::cerr << "error: cannot open off file for write" << std::endl;
            throw std::string("error: cannot open off file for write");
        }
        writeroff wo(pw);
        file << wo;
        file.close();
    }
    if (outMode.savefe == true)
    {
        std::cout << "writing fe file" << std::endl;
        std::ofstream file;
        file.open(folder+"cell.fe");
        if (!file.good())
        {
            std::cerr << "error: cannot open fe file for write!" << std::endl;
            throw std::string("error: cannot open fe file for write!");
        }
        writerfe wf(pw);
        file << wf;
        file.close();
    }

    std::cout << "\nworking for you has been nice. Thank you for using me & see you soon. :) "<< std::endl;

    return 0;
}
