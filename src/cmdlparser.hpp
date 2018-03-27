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

#pragma once
#include <string>
#include "splitstring.hpp"

enum eMode
{
    GENERIC,
    SPHERE,
    SPHEREPOLY,
    TETRA,
    TETRABLUNT,
    ELLIP,
    SPHCYL
};

class cmdlParser
{
public:

    void printHelp()
    {
        std::cerr << "Commandline parameters not correct .... aborting "  << std::endl;
        std::cerr << std::endl <<  "Use pomelo this way:\n\t./pomelo -mode [MODE] -i [position-file] -o [outputfolder] (-POLY)"  << std::endl;
        std::cerr <<  "\twith [MODE] being SPHERE, SPHEREPOLY TETRA, TETRABLUNT, ELLIP, SPHCYL"  << std::endl;
        std::cerr <<  "\tPOLY is optional and gives you only cell.poly"  << std::endl;
        std::cerr << std::endl <<  "Or in a generic way:\n\t./pomelo -mode GENERIC -i [path-to-lua-file] -o [outputfolder]"  << std::endl;
    }


    void parseArguments(int argc, char* argv[])
    {
        modeset=false;
        fileset=false;
        outset=false;
        polyswitch = false;
        ignoreBoundaryCellsswitch = false;
        shrink=1;
        shrinkset=false;
        iterations = 4;
        itset=false;
        // loop over all arguments
        for (int i = 1; i != argc; ++i)
        {
            parseMode(argc, argv, i);
            parseFileName(argc, argv, i);
            parseOut(argc, argv, i);
            parsePoly(argv, i);
            parseShrink(argc, argv, i);
            parseIterations(argc, argv, i);
        }
    }

    // mode in which pomelo operates. 
    eMode thisMode;
    bool modeset;

    // the filname of the input file (or config.lua in GENERIC mode)
    std::string filename;
    bool fileset;

    // the output folder path ( folder will be created by pomelo)
    std::string outfolder;
    bool outset;
    
    // if this is true, all other output is ignored. only poly file will be written
    bool polyswitch;

    // only important for volume calculation. Will ignore all cells that touch the cell which belongs to particle ID 0 
    // -- very hand for calculating packing fraction distributions in systems with non periodic boundaries. currently set only in GENERIC mode by boundary = "ignore" in the config. lua file
    bool ignoreBoundaryCellsswitch;

    // shrink value, only used for tetrahedra
    double shrink;
    bool shrinkset;

    // iterations value, only used for tetrahedra
    int iterations;
    bool itset;


    void sanityCheckParameters()
    {
        if (thisMode == GENERIC)
        {
            if (polyswitch == true)
            std::cerr << "WARNING: Parameter clash. -poly option in generic mode will be ignored. Please use the lua config file" << std::endl;
        }
        if(shrinkset || itset)
        {
            if (thisMode != TETRA && thisMode != TETRABLUNT)
                std::cerr << "WARNING: Parameter clash. shrink and iteration values are only valid for tetrahedra modes (TETRA, TETRABLUNT)" << std::endl;
        }
        if (!outset)
            throw std::string ("ERROR: No output folder specified!");
        if (!fileset)
            throw std::string ("ERROR: No FileName given");
        if (!modeset)
            throw std::string ("ERROR: No Mode selected");
    }

private:
    void parsePoly(char* argv[], int i)
    {
        std::string a = argv[i]; 
        if (a.find("-poly") != std::string::npos || a.find("--poly") != std::string::npos || a.find("-POLY") != std::string::npos || a.find("--POLY") != std::string::npos) polyswitch = true;
    }

    void parseShrink(int argc, char* argv[], int& i)
    {
        std::string a = argv[i]; 
        if (a.find("-shrink") != std::string::npos || a.find("--shrink") != std::string::npos)
        {
            if (shrinkset) std::cerr << "WARNING: shrink has aready been set. Overwriting old shrink" << std::endl;
            shrinkset = true;
            if (i == argc -1) throw std::string("cannot parse shrink");
            shrink = std::stod(argv[i+1]);
            ++i; 
            std::cout << "shrink set to: " << shrink << std::endl;
        }
    }

    void parseIterations(int argc, char* argv[], int& i)
    {
        std::string a = argv[i]; 
        if (a.find("-iterations") != std::string::npos || a.find("--iterations") != std::string::npos)
        {
            if (itset) std::cerr << "WARNING: iterations has aready been set. Overwriting old value" << std::endl;
            itset = true;
            if (i == argc -1) throw std::string("cannot parse iterations");
            iterations = std::stoi(argv[i+1]);
            ++i; 
            std::cout << "iterations set to: " << iterations << std::endl;
        }
    }


    void parseOut(int argc, char* argv[], int& i)
    {
        std::string a = argv[i]; 
        if (a.find("-o") != std::string::npos || a.find("--o") != std::string::npos)
        {
            if (outset) 
            {
                std::cerr << "WARNING: outfolder has aready been set. Overwriting old outfolder" << std::endl;
            }
            outset = true;
            if (i == argc -1) throw std::string("cannot parse folder");
            outfolder = argv[i+1];
            ++i; 
        }
    }

    void parseFileName(int argc, char* argv[], int& i)
    {
        std::string a = argv[i]; 
        if ((a.find("-i") != std::string::npos || a.find("--i") != std::string::npos) && a.find("-iter") == std::string::npos )
        {
            if (fileset) std::cerr << "WARNING: FileName has aready been set. Overwriting old filename" << std::endl;
            fileset = true;
            if (i == argc -1) throw std::string("cannot parse filename");
            filename = argv[i+1];
            ++i; 
        }
    }

    void parseMode(int argc, char* argv[], int& i)
    {
        std::string a = argv[i]; 
        if (a.find("-mode") != std::string::npos || a.find("--mode") != std::string::npos)
        {
            if (modeset) std::cerr << "WARNING: Mode has already been set. Overwriting old mode." << std::endl;
            modeset = true;
            if (i == argc -1) throw std::string("cannot parse mode");
            std::string mode = argv[i+1];
            if (mode == "SPHERE" || mode == "sphere")
            {
                thisMode = SPHERE;
            }
            else if (mode == "SPHEREPOLY" || mode == "spherepoly")
            {
                thisMode = SPHEREPOLY;
            }
            else if (mode == "TETRA" || mode =="tetra")
            {
                thisMode = TETRA;
            }
            else if (mode == "TETRABLUNT" || mode =="tetrablunt")
            {
                thisMode = TETRABLUNT;
            }
            else if (mode == "ELLIP" || mode == "ellip")
            {
                thisMode = ELLIP;
            }
            else if (mode == "SPHCYL" || mode == "sphcyl")
            {
                thisMode = SPHCYL;
            }
            else if (mode == "GENERIC" || mode == "generic")
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
    }
};
