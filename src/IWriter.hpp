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
#ifndef IWRITER_H_123456
#define IWRITER_H_123456

#include <iomanip>
#include <vector>
#include <map>
#include <cmath>

#include "duplicationremover.hpp"


class IWriter
{
public:
    void addface( std::vector<double> positionlist, unsigned int cellID)
    {
        unsigned int faceID = currentFaceLabel;
        currentFaceLabel++;
        faceCellMap[faceID] = cellID;


        std::vector<unsigned int> facevertexIDs;
        for(unsigned int i = 0; i != positionlist.size(); ++(++(++i)) )
        {
            double x = positionlist[i];
            double y = positionlist[i+1];
            double z = positionlist[i+2];
            unsigned int l = currentVertexLabel;

            p.addpointForCell(x, y, z, l, faceID, cellID);
            facevertexIDs.push_back(l);
            currentVertexLabel++;
        }

        faces[faceID] = facevertexIDs;
    };

    

    friend std::ostream& operator << (std::ostream &f, const IWriter& p)
    {
        p.print(f);
        return f;
    };


    void savePointPatternForGnuplot( std::string filename)
    {
        std::cout << "writing PointPattern file" << std::endl;
        std::ofstream file;
        file.open(filename);
        file >> p;
        file.close();
    }

    void removeduplicates (double epsilon, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, unsigned int nx = 16, unsigned int ny = 16, unsigned int nz = 16 )
    {
        std::cout << "IWriter: remove duplicates" << std::endl;
        duplicationremover d(nx, ny, nz);
        d.setboundaries(xmin, xmax, ymin, ymax, zmin, zmax);
        std::cout << "\tadding points" << std::endl;
        d.addPoints(p, true);
        std::cout << "\tremoving duplicates" << std::endl;
        d.removeduplicates(epsilon);
        std::cout << "\tget back points" << std::endl;
        d.getallPoints(p);
        std::cout << "\tmatch back indices" << std::endl;
        rearrangeIndices(d.indexShift);

        //std::cout << "\torder indices" << std::endl;
        orderIndices();
    }

    void orderIndices ()
    {
        std::cout << "order indices" << std::endl;
        std::map<unsigned int, long> indexShift;
        unsigned int label = 1;
        for( auto it = p.points.begin(); it != p.points.end(); ++it)
        {
            indexShift[(*it).l] = label;
            (*it).l = label;
            label++;
        }
        rearrangeIndices(indexShift, false);
    }

    void rearrangeIndices(std::map<unsigned int, long>& indexShift, bool multipleTimes = true)
    {

        //std::cout << "### index map" << std::endl;
        unsigned int maxIndex = (*indexShift.rbegin()).first;
        for(unsigned int i = 1; i != maxIndex; ++i)
        {
            if(indexShift.find(i) == indexShift.end())
            {
                indexShift[i] = -1;
            }
            //std::cout << i << " " << indexShift[i] << std::endl;
        }

        std::cout << "\tPolywriter rearrange Indices"<< std::endl;
        unsigned int i = 0;
        double fivepercentSteps = 0.05*static_cast<double>(faces.size());
        double target = fivepercentSteps;
        for (auto it = faces.begin(); it != faces.end(); ++ it)
        {
            i++;
            if ( i >= target)
            {
                target += fivepercentSteps;
                std::cout << static_cast<int>(static_cast<double>(i)/static_cast<double>(faces.size())*100) << " \%\t"<< std::flush;
            } 
            unsigned int j = 0;
            for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            {
                j++;
                //std::cout << "\t\t\t" << j << " / " << it->second.size() << std::endl;
                unsigned int vertexIndex = (*it2);

                if( multipleTimes)
                {
                    while (indexShift[vertexIndex] != -1 )
                    {
                        //std::cout << "\t\t\t\t" << vertexIndex << std::endl;
                        vertexIndex = indexShift[vertexIndex];
                        if (indexShift.find(vertexIndex) == indexShift.end())
                        {
                            indexShift[vertexIndex] = -1;
                            break;
                        }
                    }
                    (*it2) = vertexIndex;
                }
                else
                {
                    (*it2) =indexShift[(*it2)];
                }
            }
        }
        std::cout << std::endl;
    }



    pointpattern p; // holds all the points
    std::map<unsigned int, unsigned int> faceCellMap;   // first is face id, second is cell id
    std::map<unsigned int, std::vector<unsigned int > > faces;
private:
    unsigned int currentVertexLabel = 1;
    unsigned int currentFaceLabel = 1;

protected:
// since you cant override operators, this is just another level of indirection
    virtual void print (std::ostream& out) const = 0;
};



#endif
