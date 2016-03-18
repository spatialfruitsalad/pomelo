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

#include<fstream>
#include<iostream>

#include<sstream>
#include<string>
#include "fileloader.hpp"

#include "csplitstring.hpp"

void fileloader::read(std::string filename, std::vector<particleparameterset>& set)
{
    std::ifstream infile;
    infile.open(filename, std::ifstream::in);
    if (infile.fail())
    {
        std::cout << "Cannot load file " << filename << std::endl;
        return;
    }
#pragma GCC diagnostic ignored "-Wwrite-strings"
    cSplitString line("");
    unsigned int linesloaded = 0;
    std::getline(infile, line);
    while (std::getline(infile, line))
    {
        if(line.find("#")!=std::string::npos) continue; // ignore comment lines

        std::vector<std::string> thisparams = line.split(' ');

        particleparameterset p;
        for (auto it = thisparams.begin(); it != thisparams.end(); ++it)
        {
            double d = std::stof((*it));
            p.push_back(d);
        }
        linesloaded++;
        set.push_back(p);

    }
    std::cout << "Lines loaded: " << linesloaded << std::endl << std::endl;

    infile.close();
}
