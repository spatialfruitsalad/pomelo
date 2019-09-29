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
#ifndef PARSEXYZ_H_GUARD_123456
#define PARSEXYZ_H_GUARD_123456

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "IParser.hpp"
#include "pointpattern.hpp"
#include "splitstring.hpp"

class parsexyz : public IParser
{
public:

    parsexyz () =default;


    void parse(std::string const filename, pointpattern& pp)
    {
        std::ifstream infile;
        infile.open(filename);
        if (infile.fail())
        {
            throw std::string("cannot open xyz input file");
        }
        std::string line = "";
        unsigned long linesloaded = 0;
        splitstring commentline;
        std::getline(infile, commentline); // parse comment line
       
        std::vector<std::string> parameters = commentline.split(','); 
        
        parseBasicParameters(parameters);

        while(std::getline(infile, line))   // parse lines
        {
            if(line.find('#') != std::string::npos) continue;
            std::istringstream iss(line);
            double x ,y, z;
            if (!(iss >> x >> y >> z))
            {
                throw std::logic_error("error parsing one line in XYZ file:\n" + line);
            }
            linesloaded++;
            pp.addpoint(linesloaded, x,y,z);
        }
        std::cout << "parsed "  << linesloaded << " lines" << std::endl;
    };
};

#endif
