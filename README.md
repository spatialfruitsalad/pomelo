# pomelo
[![Build Status](https://travis-ci.com/spatialfruitsalad/pomelo.svg?branch=master)](https://travis-ci.com/spatialfruitsalad/pomelo)

calculate generic set voronoi diagrams with c++11
![3d rendering of a Voronoi diagram of spheres](https://cloud.githubusercontent.com/assets/17979038/25128693/dea86b48-243a-11e7-8a95-6b9cc504605d.png)
## GENERAL 

This is pomelo, a tool that allows you to calculate set voronoi tessellations. 
If you want to learn about set voronoi diagrams, refer to http://www.tandfonline.com/doi/abs/10.1080/14786435.2013.834389#.Vulxk9qdJLU  (DOI:10.1080/14786435.2013.834389) where the algorithm is described.
If you use this program or results of this program for scientific publications, we would acknowledge the citation of the following paper:
- https://epjwoc.epj.org/articles/epjconf/abs/2017/09/epjconf162065/epjconf162065.html (DOI: 10.1051/epjconf/201714006007, arxiv: https://arxiv.org/abs/1703.03643 )

Pomelo is designed to be as generic as possible. This is acquired by being independent of input format for the particles on which you want to calculate a set voronoi diagram of.
You provide a so called position file (e.g. the output of your simulation or image analysis) and a read file. The read file is a lua script, which allows you to spawn points according to the values in the position file.
More information is provided in the "usage" chapter.

Pomelo has been written by Simon Weis and Philipp Schoenhoefer at the Institute For Theoretical Physics 1 of the Friedrich-Alexander University Erlangen.
The development of this software was funded by the German Research Foundation (DFG) Forschergruppe FOR1548 "Geometry and Physics of Spatial Random Systems" (GPSRS).

Pomelo is free software. (released under GPL3, see License for details). While Pomelo is licensed under GPL3, external libraries (in the folder lib) may be licensed differently.

External libraries are
 - Voro++ for calculating point Voronoi cells(http://math.lbl.gov/voro++/)
 - Selene for parsing Lua scripts (https://github.com/jeremyong/Selene)

If you use Pomelo, we would be happy if you let us know. You can contact us at simon.weis@fau.de or philipp.schoenhoefer@fau.de


![2d visualisation of a Set Voronoi diagram of two pear shaped particles](https://cloud.githubusercontent.com/assets/17979038/25128704/e8dfaed2-243a-11e7-98b6-df2d2b926107.png)



## System Requirements
 
- g++ 4.9.2+, clang++ 3.5.0-10+
- lib-lua 5.2+ & respective devel package (if you want to use generic mode)


## Compilation
### Linux
There are two version of Pomelo, the normal one and the generic one. The normal one can process simple tasks, while the generic one is more complex and can process any type of particles.

To compile the normal version of Pomelo, just call `make`. This will invoke the Makefile, which builds Pomelo.
The output is stored in the folder `bin`. Temporary object files can be found in the folder `obj`.

```
make
```
If you want to use the generic version of Pomelo, call
```
make GENERIC
```

### Windows (cygwin)
Pomelo can also be run on Windows, using cygwin. Therefore you need to download cygwin (64 bit version is recommended, as the 32 bit version is limited to 4GB of memory) from the [cygwin website](http://www.cygwin.com/install.html).

You need to install cygwin and add some packages during the installation: `git`, `make`, `clang`, `lua`, `lua-devel`. We also recommend you install the text editor of your choice.
After that, start cygwin, clone this repository with
```
git clone git@github.com:spatialfruitsalad/pomelo.git
```
and call 
```
make
```

As the filenames and paths of the lua libraries are different for different linux distributions and of course for cygwin, you will need to make some small changes to the `Makefile` if you want to use GENERIC mode:
For the targets `obj/main.o` and `LINK` you need to change `-I/usr/include/lua5.2` to `-I/usr/include/lua` and `-llua5.2` to `llua`. That should do the trick.

## Usage 

### Run the unit tests
to compile and run the tests, just call 
```
make test
```


### Running Pomelo on a system of spheres
To use pomelo to calculate the Voronoi tesselation of spheres, you can use the mode `SPHERE`. Switch to the bin directory and type 

```
./pomelo -mode SPHERE -i ../test/2016-05-13_xyz/hs-16384_0.50.xyz -o [output folder]
```

The options have the following meaning:
-mode selects the mode. There are the following modes available: SPHERE, SPHEREPOLY, POLYMER, TETRA, TETRABLUNT, ELLIP, SPHCYL and GENERIC. While the last one has to be compiled with make GENERIC and thus requires selene, the other modes work fine. 
-i specifies the input file. In the case above, the SPHERE Mode expects a xyz file, that lists the particle's (spheres) center coordinates.
-o specifies the outpput folder. This folder will be created by pomelo and output will be written to it.


The example file is a small part of a system of a hard spheres simulation. Use any other xyz file and adapt the comment line as shown in the test case.
The first lines of an xyz file should look similar to this example:
```
boundary_condition = periodic_cuboidal, infile = hs-16384_0.50-coord.dat, num_sph = 18, boxsz = 51.583743, repr = 0 
# x y z
16.5855262696469 14.8968505096529 24.7938151355899
23.2206174557173 45.9892223675941 4.14961702400604
...
```

The first line are the number of particles.
The second line is a comment line, which contains information needed for running pomelo correctly. The boundary conditions can be: 
`periodic_cuboidal`: periodic in all three directions
`periodic_xy`,`periodic_xz`,`periodic_yz`: periodic in two directions and hard wall in the other direction
`periodic_x`,`periodic_y`,`periodic_z`: periodc in one direction and hard walls in the other two directions
`none`: system inside a hard box

The parameters `boxsx`,`boxsy`,`boxsz` define the size of the box in x,y and z direction that is used by pomelo. By only stating `boxsz` the other values are set to the same value as well. By adding the parameter `box_origin_center` the box dimensions go from -boxs(*xyz)/2 to boxs(*xyz)/2. Otherwise the box dimensions go from 0 to boxs(*xyz). 

The parameter 'poly_subset' defines a subset and creates polyfiles for every particle individually in a specific range. The range is indicated by the labels cellmin and cellmax and separated by "-": poly_subset=cellmin-cellmax.

The parameter `percolating_cluster` changes output files for a more convenient handling of cells with percolating features (for now it is designed for generating surface structures). The other parameters are not important for pomelo.


### Pomelo workflow (generic) 
Pomelo is designed to be as generic as possible and can process any type of particles.

A set voronoi diagram will be calculated by spawning points on the surface of the particles (surface triangulation), by calculating the ordinary voronoi diagram of all the surface points and by merging the voronoi cells for one particle together.

Pomelo allows you to use your own position file (e.g. the output of a simulation or image processing) and allows you to script the surface triangulation the way you want it by passing a simple to write lua file.
 

### Calling Pomelo (generic)
To invoke Pomelo, switch to the bin directory. For the generic mode to work, you will have to build the generic binary of pomelo. There you can call Pomelo by typing 

```
./pomelo -mode GENERIC -i [file to lua path] -o [output folder]
```

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
For each line in the position file the values of this line will be stored in the lua variable `s` (accessed by `s[0]`, `s[1]`, ...) and can then be used for further calculation.
By calling `p:addpoint(label, x, y, z)` with the respective values, you can spawn one point of the surface triangulation. 

Please see the `test/2015-12-22_spheres_center/read.lua` for a simple example on how to spawn points exactly at the position given in the position file.
For spawning points on a sphere, refer to `test/2015-12-22_onesphere/read.lua` for an example on how to spawn points on a spheres surface.
Calling pomelo on this examples creates some output which can then be visualized calling gnuplot.

This is thus the tool to make surface triangulations for your own particles. If you have any implementation of surface triangulation for your own particles, feel free to add a pull request so we can add them to the list.


## If you come from 'Analyzing X-ray tomographies of granular packings' or 'Howto calculate the lpf distribution'

 -  createBoundary: Use [this tool](https://github.com/spatialfruitsalad/pomeloTools/tree/master/createboundary) to create a boundary of surface points with label 0 around the actual particle positions. This will be used in pomelo to exclude the outest layer of particles.
Extract and use this program (with adjusted paths):
`./createBoundary  particles_centroids_ed5.dat particles_centroids_ed5.dat.border 513 513 513`
- use Pomelo GENERIC mode with the lua files from the create boundary program. Please adjust file paths in surfacevoxel.lua to your folder structure.
- copy particles_centroids_ed5.dat to particles_centroids_ed5.dat.wohead and remove the first three lines (two header lines as well as particle with label 0 (air)).
- create list of particles in correct ellip file format
`cat particles_centroids_ed5.dat.wohead | awk '{print $4, $1, $2, $3, 27.177/2, 27.17/2, 27.17/2}' > particles_centroids_ed5.dat.ellip` (radius obtained from fit of first peak from the pair correlation).
-  pull the latest version of [calcLPF_ellip](https://github.com/spatialfruitsalad/pomeloTools/tree/master/calcLPF_ellip) . The new version excludes cells with volume zero, wich exist due to pomelo boundary mode ignore (cells which share a common face with a cell that belongs to boundary 0 (the container wall) will automatically be set to have volume 0).
- make directory
`mkdir out`
- run calcLPF_ellip
 `./calcLPF_ellip pomeloOut/setVoronoiVolumes.dat particles_centroids_ed5.dat.ellip  out spheres 255 255 255 0 512`
 - That should give you a phig of 0.60.
- When increasing the cutoff, phig should increase to around 0.64 depending on the actual chosen cutoff values.
 `./calcLPF_ellip pomeloOut/setVoronoiVolumes.dat particles_centroids_ed5.dat.ellip  out spheres 255 255 120 100 400`
- The lpfs can be obtained from `out/spheres_lpf.dat`. a tool like [gsl-histogram](https://www.gnu.org/software/gsl/doc/html/histogram.html) can be used to create the histogram data.

## License
Pomelo is licensed under GPL3. See COPYING for further details.
