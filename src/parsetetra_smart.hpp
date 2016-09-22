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
#ifndef PARSETETRA_SMART_H_GUARD_123456
#define PARSETETRA_SMART_H_GUARD_123456

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "pointpattern.hpp"
#include "duplicationremover.hpp"
#include "splitstring.hpp"
#include "triangle.hpp"
#include "tetrahedra.hpp"

class parsetetrasmart
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

    parsetetrasmart () : xmin(0),  ymin(0), zmin(0), xmax(0) ,ymax(0), zmax(0), xpbc(false), ypbc(false), zpbc(false)
    {};

    void parse(std::string const filename, pointpattern& pp, double shrink = 0.95, int depth = 3)
    {
        std::cout << "parse tetra file" << std::endl;
        std::ifstream infile;
        infile.open(filename);
        if (infile.fail())
        {
            throw std::string("cannot open tetra input file");
        }
        std::string line = "";
        unsigned long linesloaded = 0;
        std::vector<double> xvals;
        std::vector<double> yvals;
        std::vector<double> zvals;
        std::cout << "parse: using shrink of " << shrink << std::endl;

        unsigned long n = 0;

        std::vector<tetrahedra> tetraList;

        while(std::getline(infile, line))   // parse lines
        {
            if(line.find('#') != std::string::npos) continue;
            n++;
            if (n%1000==0) std::cout << "parsed " << n << " lines" << std::endl;

            std::istringstream iss(line);
            double x1, y1, z1;
            double x2, y2, z2;
            double x3, y3, z3;
            double x4, y4, z4;
            double bounds;
            if (!(iss >> x1 >> y1 >> z1 >> x2 >> y2 >> z2>> x3 >> y3 >> z3 >> x4 >> y4 >> z4 >> bounds))
            {
                std::cerr << "error parsing one line in XYZ file" << std::endl;
                std::cout << line << std::endl;
                break;
            }
            linesloaded++;
        //std::cout << "line loaded" << std::endl;
            
            point p1(x1,y1,z1,linesloaded);
            point p2(x2,y2,z2,linesloaded);
            point p3(x3,y3,z3,linesloaded);
            point p4(x4,y4,z4,linesloaded);

            tetrahedra t(p1, p2, p3, p4);

            tetraList.push_back(t);

            xvals.push_back(p1.x);
            yvals.push_back(p1.y);
            zvals.push_back(p1.z);
            xvals.push_back(p2.x);
            yvals.push_back(p2.y);
            zvals.push_back(p2.z);
            xvals.push_back(p3.x);
            yvals.push_back(p3.y);
            zvals.push_back(p3.z);
            xvals.push_back(p4.x);
            yvals.push_back(p4.y);
            zvals.push_back(p4.z);

            std::vector<point> p = {p1,p2,p3,p4};
            dumbShrink( p, shrink);

            //std::cout << "points loaded" << std::endl;
            triangle t1 (p[0], p[1], p[2]); 
            triangle t2 (p[0], p[1], p[3]); 
            triangle t3 (p[0], p[2], p[3]); 
            triangle t4 (p[1], p[2], p[3]); 
            //std::cout << "triangles created" << std::endl;

            std::vector<triangle> list = {t1, t2, t3, t4}; 

            //std::cout << "subdivision started" << std::endl;
            triangle::recusiveSubdivide(depth, list);
            //std::cout << "subdivision ended" << std::endl;
            
            // add the surface points to a pointpattern for this tetrahedra first to make some duplicationchecks
            pointpattern pptetra; 
            for(triangle t : list)
            {
                for(point x : t.p)
                {
                    pptetra.addpoint(x.l, x.x, x.y, x.z);
                }
            }
            
            pptetra.removeduplicates(1e-9); 
            for ( point x : pptetra.points)
            {
                pp.addpoint(x.l, x.x, x.y, x.z);
            }
        }

        std::cout << "parsed "  << linesloaded << " lines" << std::endl;

        std::cout << "created N = " << pp.points.size() << " points"  << std::endl;
        std::cout << "setting boundaries "<< std::endl;
        
        xmin = *std::min_element(xvals.begin(), xvals.end());
        ymin = *std::min_element(yvals.begin(), yvals.end());
        zmin = *std::min_element(zvals.begin(), zvals.end());
        xmax = *std::max_element(xvals.begin(), xvals.end());
        ymax = *std::max_element(yvals.begin(), yvals.end());
        zmax = *std::max_element(zvals.begin(), zvals.end());

    };
private:
    static void dumbShrink (std::vector<point>& p, double f  = 0.95)
    {
        //std::cout << "dumbshrink: using shrink of " << f << std::endl;
        if(p.size() == 0) return;
    
        // calculate center of mass
        point com (0,0,0,p[0].l);
        for(point q : p)
        {
            com = com + q;
        }
        
        com = com / static_cast<double>(p.size());
        //std::cout << "center of mass\n" << com.x << " " << com.y << " " << com.z << std::endl;
        
        // shift collection to com = 0
        // shrink all values
        // move them back to original com
        for(size_t i = 0; i != p.size(); ++i)
        {
            point q = p[i];
            //std::cout << "calc 1 " << q.x << " " << q.y << " " << q.z << std::endl;
            q = q + (-1.0)* com; 
            //std::cout << "calc 2 " << q.x << " " << q.y << " " << q.z << std::endl;
            q = q * f;
            //std::cout << "calc 3 " << q.x << " " << q.y << " " << q.z << std::endl;
            p[i] = q + com;
            //std::cout << "calc 4 " << p[i].x << " " << p[i].y << " " << p[i].z << std::endl;
        }
    }
};

#endif
