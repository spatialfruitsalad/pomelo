#ifndef POSTPROCESSING_H_123456_GUARD
#define POSTPROCESSING_H_123456_GUARD

#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <map>
#include <algorithm>

inline void DoPostProcessing(std::string folder)
{
    std::string filepath = folder + "custom.dat";
    std::ifstream infile(filepath);
    
    std::string line;

    // faceshistogram
    std::map<unsigned long, long> histogramFaces;
    std::vector<double> vecVolume;

    while(std::getline(infile, line))
    {
        std::istringstream iss(line);
        unsigned long l = 0;
        unsigned long f = 0;
        double v = 0.0;
        
        if (!(iss >>l >> f >> v))
        {
            break;  //error
        }

        vecVolume.push_back(v);

        if (!histogramFaces.count(f))
        {
            histogramFaces.insert(std::pair<unsigned int,long >(f,0));
        }
        histogramFaces.find(f)->second +=1.0;
    };
    
    // calculate volume histogram
    double minVol = *std::min_element(vecVolume.begin(), vecVolume.end());
    double maxVol = *std::max_element(vecVolume.begin(), vecVolume.end());
    double count = vecVolume.size();


    double binCount = count/20;
    if (binCount >= 100)
        binCount = 100;

    double dv = (maxVol - minVol)/(binCount + 1);

    std::vector<long> histogramVolume (binCount+1, 0);
    for (double v : vecVolume)
    {
        unsigned int idx = static_cast<unsigned int>((v -minVol)/dv);
        histogramVolume[idx] += 1;
    }

    // save output 
    
    {
        std::ofstream outFaces (folder+"faces.stat");
        outFaces << "#NumberOfFaces Occurrence" << std::endl;

        for (auto it = histogramFaces.begin(); it != histogramFaces.end(); ++it)
        {
            outFaces << it->first << " " << it->second << std::endl;
        }
        outFaces.close();
    }

    {
        std::ofstream outVolumes(folder+"volumes.stat");
        outVolumes << "#volume  occurrence" << std::endl;
        for (unsigned int i = 0; i != histogramVolume.size(); ++i)
        {
            double v = minVol + i * dv;
            double c = histogramVolume[i];

            outVolumes << v << " " << c << std::endl;
        }
    }
}

#endif
