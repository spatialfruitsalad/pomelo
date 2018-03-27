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
#ifndef PARSEXYZR_H_GUARD_123456
#define PARSEXYZR_H_GUARD_123456

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>
#include "pointpattern.hpp"
#include "splitstring.hpp"

class parsexyzr
{
public:
    double xmin;
    double ymin;
    double zmin;
    double xmax;
    double ymax;
    double zmax;
    double shrink;
    int stepsTheta;
    int stepsPhi;
    bool xpbc;
    bool ypbc;
    bool zpbc;

    parsexyzr () : xmin(0),  ymin(0), zmin(0), xmax(0) ,ymax(0), zmax(0), shrink(0.95), stepsTheta(10), stepsPhi(10),  xpbc(false), ypbc(false), zpbc(false)
    {};
    void parse(std::string const filename, pointpattern& pp)
    {
        std::ifstream infile;
        infile.open(filename);
        if (infile.fail())
        {
            throw std::string("cannot open xyzr input file");
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
                    std::cout << "xyzr parser boundaries: " << pbcsplit[1] << std::endl;
                }
            }
            else if (s.find("shrink") != std::string::npos)
            {
                splitstring split (s.c_str());
                std::vector<std::string> shrinksplit = split.split('=');
                if (shrinksplit.size() != 2)
                    throw std::string("cannot parse parameters from XYZ file");
                shrink = std::stod(shrinksplit[1]);
                std::cout << "loaded a shrink value of " << shrink << std::endl;
            }
            else if (s.find("stepstheta") != std::string::npos)
            {
                splitstring split (s.c_str());
                std::vector<std::string> stepsThetaSplit = split.split('=');
                if (stepsThetaSplit.size() != 2)
                    throw std::string("cannot parse parameters from XYZ file");
                stepsTheta = std::stoi(stepsThetaSplit[1]);
                std::cout << "stepsTheta: " << stepsTheta << std::endl;
            }
            else if (s.find("stepsphi") != std::string::npos)
            {
                splitstring split (s.c_str());
                std::vector<std::string> stepsPhiSplit = split.split('=');
                if (stepsPhiSplit.size() != 2)
                    throw std::string("cannot parse parameters from XYZ file");
                stepsPhi = std::stoi(stepsPhiSplit[1]);
                std::cout << "stepsPhi: " << stepsPhi << std::endl;
            }

        }
        

        while(std::getline(infile, line))   // parse lines
        {
            line = line.substr(2, line.size()); // remove "P "
            std::istringstream iss(line);
            double x ,y, z, r;
            if (!(iss >> x >> y >> z >> r))
            {
                std::cerr << "error parsing one line in XYZR file" << std::endl;
                break;
            }
            
            if (r < shrink) std::cerr << "WARNING: Shrink (s=" << shrink << ") larger than particle (i=" << linesloaded << "9 radius (r= " << r << "). The result will be a negative radius" << std::endl;
            linesloaded++;
            for(int i = 0; i != stepsTheta; ++i)
            for(int j = 0; j <= stepsPhi; ++j)
            {
                double theta = static_cast<double>(i) * (1.0/stepsTheta) * std::acos(-1)*2.0;
                double phi =   std::acos( static_cast<double>(j) * (2.0/stepsPhi) - 1.0);
                //std::cout << phi << " " << theta << std::endl;
                double xp = x + cos(theta) * sin(phi)*(r-shrink);
                double yp = y + sin(theta) * sin(phi)*(r-shrink);
                double zp = z + cos(phi)*(r - shrink);
                pp.addpoint(linesloaded, xp, yp, zp);
            }

        }
        std::cout << "parsed "  << linesloaded << " lines" << std::endl;
    };
};

#endif
