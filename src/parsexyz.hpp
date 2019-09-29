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

        bool setsize = false;
        bool origin_center = false;
        xmin = 0;
        ymin = 0;
        zmin = 0;

        for (auto s:parameters)
        {
            if (s.find("boxsz") != std::string::npos)
            {
                splitstring split (s.c_str());
                std::vector<std::string> boxsplit= split.split('=');
                if (boxsplit.size() != 2)
                    throw std::string("cannot parse parameters from XYZ file");
                
                double v = std::stod(boxsplit[1]);
                std::cout << "polymer parser boxsize: " << v << std::endl;
                if (!setsize)
                {
                    xmax = v;
                    ymax = v;
                }
                zmax = v;
                            
            }
            else if (s.find("boxsy") != std::string::npos)
            {
                splitstring split (s.c_str());
                std::vector<std::string> boxsplit= split.split('=');
                if (boxsplit.size() != 2)
                    throw std::string("cannot parse parameters from XYZ file");
                
                double v = std::stod(boxsplit[1]);
                std::cout << "polymer parser boxsize X: " << v << std::endl;
                ymax = v;
                setsize = true;
            }
            else if (s.find("boxsx") != std::string::npos)
            {
                splitstring split (s.c_str());
                std::vector<std::string> boxsplit= split.split('=');
                if (boxsplit.size() != 2)
                    throw std::string("cannot parse parameters from XYZ file");
                
                double v = std::stod(boxsplit[1]);
                std::cout << "polymer parser boxsize X: " << v << std::endl;
                xmax = v;
                setsize = true;
            }
            else if (s.find("boundary_condition") != std::string::npos)
            {
                splitstring split (s.c_str());
                std::vector<std::string> pbcsplit= split.split('=');
                if (pbcsplit.size() != 2)
                    throw std::string("cannot parse parameters from XYZR file");

                if (pbcsplit[1].find("periodic_cuboidal") != std::string::npos)
                {
                    xpbc = ypbc = zpbc = true;
                    std::cout << "polymer parser boundaries: " << pbcsplit[1] << std::endl;
                }
                else if (pbcsplit[1].find("periodic_xy") != std::string::npos)
                {
                    xpbc = ypbc =  true;
                    std::cout << "polymer parser boundaries: " << pbcsplit[1] << std::endl;
                }
                else if (pbcsplit[1].find("periodic_xz") != std::string::npos)
                {
                    xpbc = zpbc =  true;
                    std::cout << "polymer parser boundaries: " << pbcsplit[1] << std::endl;
                }
                else if (pbcsplit[1].find("periodic_yz") != std::string::npos)
                {
                    ypbc = zpbc =  true;
                    std::cout << "polymer parser boundaries: " << pbcsplit[1] << std::endl;
                }
                else if (pbcsplit[1].find("periodic_x") != std::string::npos)
                {
                    xpbc =  true;
                    std::cout << "polymer parser boundaries: " << pbcsplit[1] << std::endl;
                }
                else if (pbcsplit[1].find("periodic_y") != std::string::npos)
                {
                    ypbc =  true;
                    std::cout << "polymer parser boundaries: " << pbcsplit[1] << std::endl;
                }
                else if (pbcsplit[1].find("periodic_z") != std::string::npos)
                {
                    zpbc =  true;
                    std::cout << "polymer parser boundaries: " << pbcsplit[1] << std::endl;
                }
            }
            else if (s.find("percolating_cluster") != std::string::npos)
            {
                percstruct = true;
            }
            else if (s.find("box_origin_center") != std::string::npos)
            {
                origin_center = true;
            }
            else if (s.find("poly_subset") != std::string::npos)
            {
                splitstring split (s.c_str());
                std::vector<std::string> stepsSplit = split.split('=');
                if (stepsSplit.size() != 2)
                throw std::string("cannot parse parameters from ELLIP file");

                splitstring split2 (stepsSplit[1].c_str());
                std::vector<std::string> stepsSplit2 = split2.split('-');
                if (stepsSplit2.size() != 2)
                throw std::string("cannot parse parameters from ELLIP file");

                cellmin=std::stoi(stepsSplit2[0]);
                cellmax=std::stoi(stepsSplit2[1]);

                if(cellmin > cellmax){
                    throw std::string("poly_output in wrong order");
                    cellmin=0;
                    cellmax=0;
                }
            }
        }
        
        std::cout << "xyz boxsize: "<< xmax << " " << ymax << " " << zmax << std::endl;

        if(origin_center){
            xmax *= 0.5;
            xmin = -xmax;

            ymax *= 0.5;
            ymin = -ymax;

            zmax *= 0.5;
            zmin = -zmax;
        }

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
