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
#ifndef WRITEROFF_H_GUARD_123456
#define WRITEROFF_H_GUARD_123456

#include <iomanip>
#include <vector>
#include <map>
#include <cmath>

#include "IWriter.hpp"
#include "colorTable.hpp"

class writeroff : public IWriter
{
public:
    writeroff()
    {
    };
    writeroff(IWriter const& other)
    {
        faceCellMap = other.faceCellMap;
        p = other.p;
        faces = other.faces;
    }

    void print(std::ostream& f) const
    {

        // create color table
        unsigned int maxCellID = 0;
        for(auto kvp : faceCellMap)
        {
            maxCellID = (maxCellID > kvp.second) ? maxCellID : kvp.second;
        }
        std::vector<rgb> colors = colorTable::getRandomColors(maxCellID);

        int removedFaces = 0;
        for (
            auto it = faces.begin();
            it != faces.end();
            ++ it)
        {
            std::vector<unsigned int> testing;
            for (auto it2 = it->second.rbegin(); it2 != it->second.rend(); ++it2)
            {
                bool doppelt = false;
                for(unsigned int kk = 0; kk < testing.size(); kk++ )
                {
                    if(testing[kk] == (*it2) )
                    {
                        doppelt = true;
                        break;
                    }
                }
                if(doppelt) continue;
                testing.push_back( (*it2) );
            }
            if(3 > testing.size())
            {
            removedFaces++;
            }
        }

        f << "OFF\n" << p.points.size() << " " << faces.size()-removedFaces << " 0\n";
        f << std::fixed;
    for(auto p : p.points )
    {
        f << std::setprecision(12) << p.x << " " << std::setprecision(12) << p.y << " " << std::setprecision(12) << p.z<< "\n";
    }

        for (
            auto it = faces.begin();
            it != faces.end();
            ++ it)
        {
            unsigned int faceID = it->first;
            unsigned int cellID = faceCellMap.at(faceID);
            double r = colors.at(cellID).r;
            double g = colors.at(cellID).g;
            double b = colors.at(cellID).b;
            std::vector<unsigned int> testing;
            for (auto it2 = it->second.rbegin(); it2 != it->second.rend(); ++it2)
            {
                bool doppelt = false;
                for(unsigned int kk = 0; kk < testing.size(); kk++ )
                {
                    if(testing[kk] == (*it2) )
                    {
                        doppelt = true;
                        break;
                    }
                }
                if(doppelt) continue;
                testing.push_back( (*it2) );
            }
            if(2 < testing.size())
            {
                f << testing.size() << " ";
                for(unsigned int kk = 0; kk < testing.size(); kk++ )
                {
                    f << testing[kk]-1 << " ";
                }
                f << r << " " << g << " " << b <<  " 1" << "" << std::endl;
            }
//            else std::cout << testing.size() << " " << cellID << std::endl;
        }

        f << "\n";
    };


};

#endif

