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
#include "pointpattern.hpp"
#include "splitstring.hpp"

class parsexyz
{
public:
    double xmin;
    double ymin;
    double zmin;
    double xmax;
    double ymax;
    double zmax;
    bool xpbc;
    bool ypbc;
    bool zpbc;
    bool percstruct;

    parsexyz () : xmin(0),  ymin(0), zmin(0), xmax(0) ,ymax(0), zmax(0), xpbc(false), ypbc(false), zpbc(false), percstruct(false)
    {};
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
        std::getline(infile, line); // ignore first line
        splitstring commentline;
        std::getline(infile, commentline); // parse comment line
       
        std::vector<std::string> parameters = commentline.split(','); 

        bool setsize = false;
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
                std::cout << "xyz parser boxsize: " << v << std::endl;
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
                std::cout << "xyz parser boxsize X: " << v << std::endl;
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
                std::cout << "xyz parser boxsize X: " << v << std::endl;
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

        }
        
        std::cout << "xyz boxsize: "<< xmax << " " << ymax << " " << zmax << std::endl;

        while(std::getline(infile, line))   // parse lines
        {
            line = line.substr(2, line.size()); // remove "P "
            std::istringstream iss(line);
            double x ,y, z;
            if (!(iss >> x >> y >> z))
            {
                std::cerr << "error parsing one line in XYZ file" << std::endl;
                break;
            }
            //std::cout << "parsed particle: " << x << " " << y << " " << z << std::endl;
            linesloaded++;
            pp.addpoint(linesloaded, x,y,z);
        }
        std::cout << "parsed "  << linesloaded << " lines" << std::endl;
    };
};

#endif
