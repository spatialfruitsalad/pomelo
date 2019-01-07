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
#ifndef WRITERFE_H_GUARD_123456
#define WRITERFE_H_GUARD_123456

#include <iomanip>
#include <vector>
#include <map>
#include <cmath>

#include "IWriter.hpp"
#include "colorTable.hpp"

struct edgeManager
{
    long currentEdgeID; // for new edges
    std::map<long, std::pair<unsigned long, unsigned long> > alledges;

    long getEdgeID(unsigned long v1, unsigned long v2)
    {
        for(auto kvp : alledges)
        {
            if (kvp.second.first == v1 && kvp.second.second == v2)
                return kvp.first;
            else if (kvp.second.first == v2 && kvp.second.second == v1)
                return -kvp.first;
        }
        // pair point not found, so add it now
        alledges[currentEdgeID] = std::pair<unsigned long, unsigned long> (v1, v2);
        return currentEdgeID++; // whopsie codie
    }
};

class writerfe : public IWriter
{
public:
    writerfe()
    {
    };
    writerfe(IWriter const& other)
    {
        faceCellMap = other.faceCellMap;
        p = other.p;
        faces = other.faces;
    }

    void print(std::ostream& f) const
    {
        f << "// cell.fe\tcreated by pomelo\n";
        f << "// Set-Voronoi tesselation\n";
        f << "vertices //\tcoordinates\n\n";
        for(auto pt : p.points)
        {
            f << pt.l << " " <<  std::setprecision(10) << pt.x << " " << std::setprecision(10) << pt.y << " " << std::setprecision(10) << pt.z<< "\n";
        }
        f << "\n";
        
        
        f << "edges // endpoints\n";

        edgeManager em;
        em.currentEdgeID = 1;
        std::map<unsigned long, std::vector<unsigned long> >  bodies;
        std::string faceString = "";
        // building edges and bodies data structures
        int removedFaces = 0;
        unsigned long count = 0;
        unsigned long all = 0;
        for (
            auto it = faces.begin();
            it != faces.end();
            ++ it)
        {
            count++;
            if (count%100 == 0)
            {
                all+= count;
                count = 0;
                std::cout << static_cast<double>(all)/static_cast<double>(faces.size()) * 100.0 << "% " << std::endl;
            }
            unsigned int faceID = it->first;
            unsigned int cellID = faceCellMap.at(faceID);
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
            




            if(testing.size() > 2)
            {
                unsigned long faceID = it->first - removedFaces;
                faceString += std::to_string(faceID) + " ";
                bodies[cellID].push_back(faceID);

                for (unsigned long kk = 0; kk < testing.size()-1; ++kk)
                {
                    unsigned long v1 = testing[kk];
                    unsigned long v2 = testing [kk+1];
                    long edgeID = em.getEdgeID(v1,v2);
                    
                    if (edgeID > 0)
                        f << edgeID << " " << v1  << " " <<  v2 << "\n";


                    faceString += std::to_string(edgeID) + " ";
                }
                // add last edge to face
                unsigned long v1 = testing[testing.size()-1];
                unsigned long v2 = testing[0];
                long edgeID = em.getEdgeID(v1,v2);

                if (edgeID > 0)
                    f << edgeID << " " << v1  << " " <<  v2 << "\n";
                faceString += std::to_string(edgeID) + " ";


                faceString += "\n";
            }
            else removedFaces++;
        }
        f << "\nfaces // oriented\n";
        f << faceString << "\n";

        f << "bodies\n";
        for (auto kvp : bodies)
        {
            f << kvp.first << " ";
            for (unsigned long fid : kvp.second)
            {
               f << fid << " ";
            }
            f << "\n";
        } 
        f << "\n";
    };


};

struct perc_dir
{
   perc_dir(const bool cperc, const bool cdir): perc(cperc), dir(cdir) {};
   bool perc, dir;
};

double xmax,xmin;
double ymax,ymin;
double zmax,zmin;

class writerfe_perc : public IWriter
{
public:
    writerfe_perc()
    {
    };
    writerfe_perc(IWriter const& other)
    {
        faceCellMap = other.faceCellMap;
        p = other.p;
        faces = other.faces;
    }

    void print(std::ostream& f) const
    {
        f << "// cell.fe\tcreated by pomelo\n";
        f << "// Set-Voronoi tesselation\n";
        
        f << "TORUS\nperiods\n1 0 0\n0 1 0\n0 0 1\n\n";

        f << "vertices //\tcoordinates\n";

        std::vector<unsigned int> point_label; 
        std::vector<struct perc_dir> perc_x; 
        std::vector<struct perc_dir> perc_y; 
        std::vector<struct perc_dir> perc_z; 
        perc_dir init_dir(0,0);
        pointpattern perc_pattern;

        long perc_count = 1;

        for(auto pt : p.points)
        {
            point_label.push_back(0);
            perc_x.push_back(init_dir);
            perc_y.push_back(init_dir);
            perc_z.push_back(init_dir);

            if(pt.x < xmin )
            {
                pt.x += (xmax - xmin);
            
                perc_x[pt.l-1].perc=1;
                perc_x[pt.l-1].dir=1;
            }
            else
            {
                if(pt.x > xmax )
                {
                    pt.x -= (xmax - xmin);
                    perc_x[pt.l-1].perc=1;
                }
            }

            if(pt.y < ymin )
            {
                pt.y += (ymax - ymin);
            
                perc_y[pt.l-1].perc=1;
                perc_y[pt.l-1].dir=1;
            }
            else
            {
                if(pt.y > ymax )
                {
                    pt.y -= (ymax - ymin);
                    perc_y[pt.l-1].perc=1;
                }
            }

            if(pt.z < zmin )
            {
                pt.z += (zmax - zmin);

                perc_z[pt.l-1].perc=1;
                perc_z[pt.l-1].dir=1;
            }
            else
            {
                if(pt.z > zmax )
                {
                    pt.z -= (zmax - zmin);
                    perc_z[pt.l-1].perc=1;
                }
            }

            double nodouble_found = true;
            for(auto pr : perc_pattern.points)
            {
                point eps = pt-pr;
                if(eps.length() < 1e-8)
                {
                    point_label[pt.l-1] = pr.l;
                    nodouble_found = false;
                    break;   
                }    
            }
            if(nodouble_found){
                perc_pattern.addpoint(perc_count,pt.x,pt.y,pt.z);
                point_label[pt.l-1] = perc_count;
                perc_count++;
            }
        }

        for(auto pt : perc_pattern.points)
        {
            f << pt.l << " " <<  std::setprecision(10) << pt.x << " " << std::setprecision(10) << pt.y << " " << std::setprecision(10) << pt.z<< "\n";
        }
        f << "\n";
        
        
        f << "edges // endpoints\n";

        edgeManager em;
        em.currentEdgeID = 1;
        std::string facetString = "";
        // building edges and bodies data structures
        int removedFaces = 0;
        unsigned long count = 0;
        unsigned long all = 0;
        unsigned long face_count = 1;
        for (
            auto it = faces.begin();
            it != faces.end();
            ++ it)
        {
            count++;
            if (count%100 == 0)
            {
                all+= count;
                count = 0;
                std::cout << static_cast<double>(all)/static_cast<double>(faces.size()) * 100.0 << "% " << std::endl;
            }
            unsigned int faceID = it->first;
            unsigned int cellID = faceCellMap.at(faceID);
            

            if (cellID == 1)
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
                
                if(testing.size() > 2)
                {
                    facetString += std::to_string(face_count) + " ";

                    face_count++;
                    for (unsigned long kk = 0; kk < testing.size()-1; ++kk)
                    {
                        unsigned long vv1 = testing[kk]-1;
                        unsigned long vv2 = testing[kk+1]-1;
                        unsigned long v1 = point_label[vv1];
                        unsigned long v2 = point_label[vv2];
                        long edgeID = em.getEdgeID(v1,v2);
                        
                        if (edgeID > 0)
                        {
                            f << edgeID << " " << v1  << " " <<  v2;

                            if(perc_x[vv1].perc)
                            {
                                if(perc_x[vv2].perc)
                                {
                                    f << " * ";
                                }
                                else
                                {
                                    if(perc_x[vv1].dir)
                                    {
                                        f << " + ";
                                    }
                                    else
                                    {
                                        f << " - ";
                                    }
                                }
                            }
                            else
                            {
                                if(perc_x[vv2].perc)
                                {
                                    if(perc_x[vv2].dir)
                                    {
                                        f << " - ";
                                    }
                                    else
                                    {
                                        f << " + ";
                                    }
                                }
                                else
                                {
                                    f << " * ";
                                }
                            }

                            if(perc_y[vv1].perc)
                            {
                                if(perc_y[vv2].perc)
                                {
                                    f << " * ";
                                }
                                else
                                {
                                    if(perc_y[vv1].dir)
                                    {
                                        f << " + ";
                                    }
                                    else
                                    {
                                        f << " - ";
                                    }
                                }
                            }
                            else
                            {
                                if(perc_y[vv2].perc)
                                {
                                    if(perc_y[vv2].dir)
                                    {
                                        f << " - ";
                                    }
                                    else
                                    {
                                        f << " + ";
                                    }
                                }
                                else
                                {
                                    f << " * ";
                                }
                            }

                            if(perc_z[vv1].perc)
                            {
                                if(perc_z[vv2].perc)
                                {
                                    f << " * ";
                                }
                                else
                                {
                                    if(perc_z[vv1].dir)
                                    {
                                        f << " + ";
                                    }
                                    else
                                    {
                                        f << " - ";
                                    }
                                }
                            }
                            else
                            {
                                if(perc_z[vv2].perc)
                                {
                                    if(perc_z[vv2].dir)
                                    {
                                        f << " - ";
                                    }
                                    else
                                    {
                                        f << " + ";
                                    }
                                }
                                else
                                {
                                    f << " * ";
                                }
                            }

                            f << "\n";
                        }


                        facetString += std::to_string(edgeID) + " ";
                    }
                    // add last edge to face
                    unsigned long vv1 = testing[testing.size()-1]-1;
                    unsigned long vv2 = testing[0]-1;
                    unsigned long v1 = point_label[vv1];
                    unsigned long v2 = point_label[vv2];
                    long edgeID = em.getEdgeID(v1,v2);

                    if (edgeID > 0)
                    {
                        f << edgeID << " " << v1  << " " <<  v2;

                        if(perc_x[vv1].perc)
                        {
                            if(perc_x[vv2].perc)
                            {
                                f << " * ";
                            }
                            else
                            {
                                if(perc_x[vv1].dir)
                                {
                                    f << " + ";
                                }
                                else
                                {
                                    f << " - ";
                                }
                            }
                        }
                        else
                        {
                            if(perc_x[vv2].perc)
                            {
                                if(perc_x[vv2].dir)
                                {
                                    f << " - ";
                                }
                                else
                                {
                                    f << " + ";
                                }
                            }
                            else
                            {
                                f << " * ";
                            }
                        }

                        if(perc_y[vv1].perc)
                        {
                            if(perc_y[vv2].perc)
                            {
                                f << " * ";
                            }
                            else
                            {
                                if(perc_y[vv1].dir)
                                {
                                    f << " + ";
                                }
                                else
                                {
                                    f << " - ";
                                }
                            }
                        }
                        else
                        {
                            if(perc_y[vv2].perc)
                            {
                                if(perc_y[vv2].dir)
                                {
                                    f << " - ";
                                }
                                else
                                {
                                    f << " + ";
                                }
                            }
                            else
                            {
                                f << " * ";
                            }
                        }

                        if(perc_z[vv1].perc)
                        {
                            if(perc_z[vv2].perc)
                            {
                                f << " * ";
                            }
                            else
                            {
                                if(perc_z[vv1].dir)
                                {
                                    f << " + ";
                                }
                                else
                                {
                                    f << " - ";
                                }
                            }
                        }
                        else
                        {
                            if(perc_z[vv2].perc)
                            {
                                if(perc_z[vv2].dir)
                                {
                                    f << " - ";
                                }
                                else
                                {
                                    f << " + ";
                                }
                            }
                            else
                            {
                                f << " * ";
                            }
                        }

                        f << "\n";
                    }


                    facetString += std::to_string(edgeID) + " ";


                    facetString += "\n";
                }
                else removedFaces++;
            }
        }
        f << "\nfacets // oriented\n";
        f << facetString << "\n";
    };


};

#endif

