#ifndef POLYWRITER_H_123456
#define POLYWRITER_H_123456

#include <iomanip>
#include <vector>
#include <map>
#include <cmath>

#include "duplicationremover.hpp"

class polywriter
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
            p.addpoint(x,y,z,l);
            facevertexIDs.push_back(l);
            currentVertexLabel++;
        } 
        faces[faceID] = facevertexIDs; 
    };

    
    friend std::ostream& operator << (std::ostream &f, const polywriter& p)
    {
        f << "POINTS" << std::endl;

        for(auto it =  p.p.points.begin(); 
                it != p.p.points.end(); 
                ++it)
        {
            f << it->l << ":\t" << it->x << " " << it-> y << " " << it->z<< std::endl;
        }

        f << "POLYS" <<  std::endl;
        for (
                auto it = p.faces.begin(); 
                it != p.faces.end(); 
                ++ it)
        {
            f << it->first << ":\t";
            for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            {
                f << (*it2) << " ";
            }
            unsigned int faceID = it->first;
            unsigned int cellID = p.faceCellMap.at(faceID);
            f << "< c (0, 0, 0, " << cellID << ")" << std::endl;
        }

        return f;
    };

    void removeduplicates (double epsilon, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
    {
        duplicationremover d(16,16,16);
        d.setboundaries(xmin, xmax, ymin, ymax, zmin, zmax);
        d.addPoints(p);
        d.removeduplicates(epsilon);
        d.getallPoints(p);
        d.applyIndexShifts(faces);
    }


    pointpattern p; // holds all the points
    std::map<unsigned int, unsigned int> faceCellMap;   // first is face id, second is cell id
    std::map<unsigned int, std::vector<unsigned int > > faces;
private:

    unsigned int currentVertexLabel = 1;
    unsigned int currentFaceLabel = 1;
};

#endif
