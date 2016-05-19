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
#ifndef COLORTABLE_H_GUARD_123456
#define COLORTABLE_H_GUARD_123456

#include <vector>
#include <random>

///////////////////////////////////////////////////
/// random number Generator stuff. I use the c++11 implementation here
///////////////////////////////////////////////////

typedef struct {
    double r;       
    double g;       
    double b;       
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // percent
    double v;       // percent
} hsv;



class colorTable
{
public:


    static rgb hsv2rgb(hsv in)
    {
        double      hh, p, q, t, ff;
        long        i;
        rgb         out;

        if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
            out.r = in.v;
            out.g = in.v;
            out.b = in.v;
            return out;
        }
        hh = in.h;
        if(hh >= 360.0) hh = 0.0;
        hh /= 60.0;
        i = (long)hh;
        ff = hh - i;
        p = in.v * (1.0 - in.s);
        q = in.v * (1.0 - (in.s * ff));
        t = in.v * (1.0 - (in.s * (1.0 - ff)));

        switch(i) {
        case 0:
            out.r = in.v;
            out.g = t;
            out.b = p;
            break;
        case 1:
            out.r = q;
            out.g = in.v;
            out.b = p;
            break;
        case 2:
            out.r = p;
            out.g = in.v;
            out.b = t;
            break;

        case 3:
            out.r = p;
            out.g = q;
            out.b = in.v;
            break;
        case 4:
            out.r = t;
            out.g = p;
            out.b = in.v;
            break;
        case 5:
        default:
            out.r = in.v;
            out.g = p;
            out.b = q;
            break;
        }
        return out;     
    }

    static std::vector<rgb> getRandomColors ( unsigned int n)
    {
        n++;
        std::default_random_engine rng;
        std::uniform_real_distribution<double> dist(0,1);
        std::cout << "create " << n << " random colors" << std::endl;
        std::vector<rgb> ret(n);
        double s = 0.8;
        double v = 0.6;
        
        for(size_t i = 0; i != ret.size(); ++i)
        {
            double h = dist(rng) * 360; 
            hsv c;   

            c.h = h;
            c.s = s;
            c.v = v;

            ret.at(i) = hsv2rgb(c);
        }

        return ret;
    };
};









#endif

