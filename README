# pomelo

calculate generic set voronoi diagrams with c++11

## GENERAL 

This is Pomelo, a tool that allows you to calculate set voronoi tessellations. 
If you want to learn about set voronoi diagrams, please refer to http://www.tandfonline.com/doi/abs/10.1080/14786435.2013.834389#.Vulxk9qdJLU  (DOI:10.1080/14786435.2013.834389) where the algorithm is described.

Pomelo is designed to be as generic as possible. This is acquired by being independent of input format for the particles on which you want to calculate a set voronoi diagram of.
You provide a so called position file (e.g. the output of your simulation or image analysis) and a read file. This read file is a lua script, which allows you to spawn points according to the values in the position file.
More information is provided in the "usage" chapter.

It has been written by Simon Weis and Philipp Schoenhoefer at the Institute For Theoretical Physics 1 of the Friedrich-Alexander University Erlangen.
The development of this software was funded by the German Research Foundation (DFG) Forschergruppe FOR1548 "Geometry and Physics of Spatial Random Systems" (GPSRS).

Pomelo is free software. (released under GPL3, see License for details). While Pomelo is licensed under GPL3, external libraries (in the folder lib) may be licensed differently.

If you use Pomelo, we would be happy if you let us know. You can contact us at simon.weis@fau.de or philipp.schoenhoefer@fau.de

## System Requirements
 
- g++ 4.9.2 or clang++ 3.5.0-10
- lib-lua 5.2 (or higher) & respective devel package


## Compilation
To compile your version of Pomelo, just call make. This will invoke the Makefile, which builds Pomelo.
The output is stored in the folder bin.

If you want to use clang++ instead of g++ just change the respective commands in the Makefile.

## Usage 

### Pomelo workflow 
Pomelo is designed to be as generic as possible and can process any type of particles.

A set voronoi diagram will be calculated by spawning points on the surface of the particles (surface triangulation), by calculating the ordinary voronoi diagram of all the surface points and by merging the voronoi cells for one particle together.

Pomelo allows you to use your own position file (e.g. the output of a simulation or image processing) and allows you to script the surface triangulation the way you want it by passing a simple to write lua file.
 

### Calling Pomelo 
To invoke Pomelo, switch to the bin directory. There you can call Pomelo by typing 
./pomelo [file to lua path] [output folder]

The passed lua file is a parameter file, which contains all the information needed by pomelo. The output folder is the folder which will contain the calculated output. 

Pomelo comes equipped with a set of test cases (see folder test), ranging from simple tests for debugging purpose to complex geometry calculations. Feel free to use them as a reference an as a starting point for your own calculations.

### The Lua parameter File 
The lua parameter file requires some input from the user (you).

 - positionfile: (string) path to the file with all positions
 - readfile: (string) path to a lua file which tells pomelo how to use the position file and how to spawn surface triangulation
  x/y/z max/min: (numeric) size of the surrounding box
 - epsilon : (numeric) this is a threshold to remove duplicate points. If they are closer than epsilon, they will get removed. This value is also used for merging voronoi cells.
 - boundary: (string) boundary mode. "none" means just a box, "periodic" means periodic boundaries. for "periodic", additional input is required.
 - postprocessing: (bool) states if number of voronoi neighbors and voronoi volume for the point voronoi pattern (unmerged) will be calculated
 - savepoly: (bool) whether a poly file of the merged voronoi cells will be written
 - savereduced: (bool) whether a gnuplot readably file (splot u 2:3:4) of the merged voronoi cells will be written
 - savesurface: (bool) whether a gnuplot readable file (splot u 2:3:4) of the surface triangulation will be written.

### the read.lua file 
This file is intended to hold the description of how to triangulate the particles surface. 
For each line in the position file the values of this line will be stored in the lua variable s (accessed by s[0], s[1], ...) and can then be used for further calculation.
By calling p:addpoint(label, x, y, z) with the respective values, you can spawn one point of the surface triangulation. 

Please see the test/2015-12-22_spheres_center/read.lua for a simple example on how to spawn points exactly at the position given in the position file.
For spawning points on a sphere, refer to test/2015-12-22_onesphere/read.lua for an example on how to spawn points on a spheres surface.
Calling pomelo on this examples creates some output which can then be visualized calling gnuplot.

This is thus the tool to make surface triangulations for your own particles. If you have any implementation of surface triangulation for your own particles, feel free to add a pull request so we can add them to the list.


## License
Pomelo is licensed under GPL3. See COPYING for further details.
