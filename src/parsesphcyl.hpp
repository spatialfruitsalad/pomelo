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
#ifndef PARSESPHCYL_H_GUARD_123456
#define PARSESPHCYL_H_GUARD_123456

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>
#include "IParser.hpp"
#include "pointpattern.hpp"
#include "splitstring.hpp"
#include "GenericMatrix.h"

class parsesphcyl : public IParser
{
public:
    double shrink;
    int stepsTheta;
    int stepsPhi;
    int stepsZ;

    parsesphcyl () : shrink(0.95), stepsTheta(10), stepsPhi(10),  stepsZ(10) {};

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
        parseSpecialParameters(parameters);

        while(std::getline(infile, line))   // parse lines
        {
            if (line[0] == '#') continue;
            std::istringstream iss(line);
            double x ,y, z, ax, ay, az, r, l;
            if (!(iss >> x >> y >> z >> ax >> ay >> az >> r >> l))
            {
                throw std::logic_error("error parsing one line in spherocylinder file:\n" + line);
            }

            if(  std::fabs(1.0- (ax*ax + ay*ay + az*az) ) > 0.01  )
            {
                throw std::logic_error("error parsing one line in spherocylinder file:\n" + line + "\n orientation vector is not of unit length\n");
            }

            linesloaded++;

            double a11 = 1;
            double a22 = 1;
            double a33 = 1;
            double a12 = 0;
            double a13 = 0;
            double a21 = 0;
            double a23 = 0;
            double a31 = 0;
            double a32 = 0;
            if(az*az != 1)
            {
                double sqrtz = 1.0/sqrt(1.0-az*az);;
                a11 = ax*az*sqrtz;
                a21 = ay*az*sqrtz;
                a31 = -1.0/sqrtz;
                a12 = -ay*sqrtz;
                a22 = ax*sqrtz;
                a32 = 0;
                a13 = ax;
                a23 = ay;
                a33 = az;
            }

            matrix rotate(a11, a21, a31, a12, a22, a32, a13, a23, a33);
 
            double invStepsPhi = 1.0/stepsPhi;
            double invStepsTheta = 1.0/stepsTheta;

            // spawn cylinder part first
            for (int i = 0; i != stepsPhi; ++i)
            for (int j = 0; j != stepsZ; ++j)
            {
                double deltaPhi = 2*M_PI*invStepsPhi;
                double deltaZ = l/stepsZ;

                double phi = 0 + i * deltaPhi;

                double xp = std::cos(phi) * r * shrink;
                double yp = std::sin(phi) * r * shrink;
                double zp = -l/2.0 + j * deltaZ;
                
                svec p(xp, yp, zp);
                p = rotate*p;
                pp.addpoint(linesloaded, p.x() + x, p.y() + y, p.z() + z);
            }
            
            std::cout << "cylinder points: " << pp.points.size() << std::endl;

            // TODO rotate point to correct orientation
           
            for(int i = 0; i <= stepsTheta; ++i)
            for(int j = 0; j != stepsPhi; ++j)
            {
                double theta = std::acos( static_cast<double>(i) * (2.0*invStepsTheta) - 1.0);
                double phi   = static_cast<double>(j)* M_PI*2.0*invStepsPhi;                

                double xp =  cos(phi) * sin(theta)*(r*shrink);
                double yp =  sin(phi) * sin(theta)*(r*shrink);
                double zp =  cos(theta)*(r * shrink);
                if (zp < 0 )
                { 
                    zp -= l/2.0; 
                }
                else if (zp > 0) 
                {
                    zp += l/2.0; 
                }
                else 
                {
                    std::cout << "no point added" << std::endl;
                    continue;
                }
                
                svec p(xp, yp, zp);
                p = rotate*p;
                pp.addpoint(linesloaded, p.x() + x, p.y() + y, p.z() + z);
            }

        }
        std::cout << "parsed "  << linesloaded << " lines" << std::endl;
    };
private:
    void parseSpecialParameters(std::vector<std::string> const& parameters)
    {
        for (auto const s : parameters)
        {
            if (s.find("shrink") != std::string::npos)
            {
                splitstring split (s.c_str());
                std::vector<std::string> shrinksplit = split.split('=');
                if (shrinksplit.size() != 2)
                    throw std::string("cannot parse parameters from XYZ file");
                shrink = std::stod(shrinksplit[1]);
            }
            else if (s.find("stepstheta") != std::string::npos)
            {
                splitstring split (s.c_str());
                std::vector<std::string> stepsThetaSplit = split.split('=');
                if (stepsThetaSplit.size() != 2)
                    throw std::string("cannot parse parameters from XYZ file");
                stepsTheta = std::stoi(stepsThetaSplit[1]);
            }
            else if (s.find("stepsphi") != std::string::npos)
            {
                splitstring split (s.c_str());
                std::vector<std::string> stepsPhiSplit = split.split('=');
                if (stepsPhiSplit.size() != 2)
                    throw std::string("cannot parse parameters from XYZ file");
                stepsPhi = std::stoi(stepsPhiSplit[1]);
            }
            else if (s.find("stepsz") != std::string::npos)
            {
                splitstring split (s.c_str());
                std::vector<std::string> stepsZSplit = split.split('=');
                if (stepsZSplit.size() != 2)
                    throw std::string("cannot parse parameters from XYZ file");
                stepsZ = std::stoi(stepsZSplit[1]);
            }
        }
    }
};

#endif
