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

            p.addpointForCell(x, y, z, l, cellID);
            facevertexIDs.push_back(l);
            currentVertexLabel++;
        }

        faces[faceID] = facevertexIDs;
    };


    friend std::ostream& operator << (std::ostream &f, const polywriter& p)
    {
        f << "POINTS" << std::endl;
        f << std::fixed;
        for(auto it =  p.p.points.begin();
                it != p.p.points.end();
                ++it)
        {
            f << it->l << ":\t" <<  std::setprecision(12) << it->x << " " << std::setprecision(12) << it-> y << " " << std::setprecision(12) << it->z<< std::endl;
        }

        f << "POLYS" <<  std::endl;
        for (
            auto it = p.faces.rbegin();
            it != p.faces.rend();
            ++ it)
        {
            f << it->first << ":\t";
            for (auto it2 = it->second.rbegin(); it2 != it->second.rend(); ++it2)
            {
                f << (*it2) << " ";
            }
            unsigned int faceID = it->first;
            unsigned int cellID = p.faceCellMap.at(faceID);
            f << "< c(0, 0, 0, " << cellID << ")" << std::endl;
        }

        f << "END";
        return f;
    };


    void savePointPatternForGnuplot( std::string filename)
    {
        std::cout << "writing PointPattern file" << std::endl;
        std::ofstream file;
        file.open(filename);
        file << p;
        file.close();
    }

    void removeduplicates (double epsilon, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
    {
        std::cout << "polywriter: remove duplicates" << std::endl;
        duplicationremover d(16,16,16);
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
};

#endif
