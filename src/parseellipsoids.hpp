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
#ifndef PARSEELLIPSOID_H_GUARD_12345
#define PARSEELLIPSOID_H_GUARD_12345

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "pointpattern.hpp"
#include "splitstring.hpp"
#include "GenericMatrix.h"

class parseellipsoid
{
public:
    double xmin;
    double ymin;
    double zmin;
    double xmax;
    double ymax;
    double zmax;
    double shrink;
    unsigned int steps;
    bool xpbc;
    bool ypbc;
    bool zpbc;

    parseellipsoid () : xmin(0),  ymin(0), zmin(0), xmax(0) ,ymax(0), zmax(0), shrink ( 0.95), steps(10), xpbc(false), ypbc(false), zpbc(false)
    {};
    void parse(std::string const filename, pointpattern& pp)
    {
        std::cout << "parse ellip file" << std::endl;
        std::ifstream infile;
        infile.open(filename);
        if (infile.fail())
        {
            throw std::string("cannot open ellip input file");
        }
        std::string line = "";
        unsigned long linesloaded = 0;

        while(std::getline(infile, line))   // parse lines
        {
            if(line.find('#') != std::string::npos)
            {
                if (line.find("nx") != std::string::npos)
                {
                    splitstring split (line.c_str());
                    std::vector<std::string> boxsplit= split.split('=');
                    if (boxsplit.size() != 2)
                    {
                        throw std::string ("cannot parse nx parameter.");
                    }
                    double v = std::stod(boxsplit[1]);
                    xmax = static_cast<int>(v);
                }
                if (line.find("ny") != std::string::npos)
                {
                    splitstring split (line.c_str());
                    std::vector<std::string> boxsplit= split.split('=');
                    if (boxsplit.size() != 2)
                    {
                        throw std::string ("cannot parse nx parameter.");
                    }
                    double v = std::stod(boxsplit[1]);
                    ymax = static_cast<int>(v);
                }
                if (line.find("nz") != std::string::npos)
                {
                    splitstring split (line.c_str());
                    std::vector<std::string> boxsplit= split.split('=');
                    if (boxsplit.size() != 2)
                    {
                        throw std::string ("cannot parse nx parameter.");
                    }
                    double v = std::stod(boxsplit[1]);
                    zmax = static_cast<int>(v);
                }
                if (line.find("steps") != std::string::npos)
                {
                    splitstring split (line.c_str());
                    std::vector<std::string> boxsplit= split.split('=');
                    if (boxsplit.size() != 2)
                    {
                        throw std::string ("cannot parse nx parameter.");
                    }
                    double v = std::stod(boxsplit[1]);
                    steps = static_cast<int>(v);
                }
                continue;
            }

            std::istringstream iss(line);
            double cx, cy, cz;
            double x1, y1, z1;
            double x2, y2, z2;
            double x3, y3, z3;
            double a, b, c;
            unsigned long l;
            if (!(iss >> l >> cx >> cy >> cz >> a >> x1 >> y1 >> z1 >> b >> x2 >> y2 >> z2>> c >> x3 >> y3 >> z3))
            {
                std::cerr << "error parsing one line in ellip file" << std::endl;
                std::cout << line << std::endl;
                break;
            }
            linesloaded++;
            //std::cout << "line loaded" << std::endl;
            
            if (steps == 1)
            {

                pp.addpoint(l, cx, cy, cz);
                continue;
            }

            matrix rotate(x1, x2, x3, y1, y2, y3, z1, z2, z3);
            svec center (cx,cy,cz);


            double phi = -M_PI;   //-180 - 180
            double theta = 0; //0 - 180

            double x,y,z;

            double d_theta = M_PI/(2.*static_cast<double>(steps));
            unsigned int theta_steps = 2*steps;

            for(unsigned int i = 0; i < theta_steps; i++ )
            {
                theta = static_cast<double>(i)*d_theta;
                unsigned int phi_steps = 4*steps*fabs(sin(theta));
                double d_phi = 2.*M_PI/(static_cast<double>(phi_steps));
                for(unsigned int j = 0; j < phi_steps; j++)
                {
                    phi = j*d_phi;

                    x = a * sin(theta) * cos(phi);
                    y = b * sin(theta) * sin(phi);
                    z = c * cos(theta);

                    svec p(x,y,z);

                    x = b * c * sin(theta) * cos(phi);
                    y = a * c * sin(theta) * sin(phi);
                    z = a * b * cos(theta);

                    svec s(x,y,z);

                    double norm = s.abs();
                    s = ( s * (1.0/ norm) ) * shrink;
                    p =  p - s;
                    
                    p = rotate * p;
                    p = p + center;
                     
                    pp.addpoint(l, p.x(), p.y(), p.z());
                }
            }
        }
        std::cout << "parsed "  << linesloaded << " lines" << std::endl;

        std::cout << "created N = " << pp.points.size() << " points"  << std::endl;
        std::cout << "setting boundaries "<< std::endl;
        std::cout << "\t nx="<< nx << std::endl;
        std::cout << "\t nx="<< ny << std::endl;
        std::cout << "\t nx="<< nz << std::endl;
        
        xmin = 0;
        ymin = 0;
        zmin = 0;
    };
private:
    static void dumbShrink (std::vector<point>& p, double f  = 0.95)
    {
        if(p.size() == 0) return;
    
        // calculate center of mass
        point com (0,0,0,p[0].l);
        for(point q : p)
        {
            com = com + q;
        }

        com = com / static_cast<double>(p.size());
        
        // shift collection to com = 0
        // shrink all values
        // move them back to original com
        for(point q : p)
        {
            q = q + (-1.0)* com;
            q = q * f;
            q = q + com;
        }
    }
};

#endif
