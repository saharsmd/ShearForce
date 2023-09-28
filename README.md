# Vertex model simulations of mechanochemical ERK waves with a persistent random walk

This software was developed using the user project template of
Chaste. See the [User
Projects](https://chaste.cs.ox.ac.uk/trac/wiki/ChasteGuides/UserProjects)
guide on the Chaste wiki for more information.

![](img/model_schematic.png)

The software runs vertex model simulations based on the models
presented in the following publications/preprints:

+ Boocock et al Nature Physics (2021) doi: [10.1038/s41567-020-01037-7](https://doi.org/10.1038/s41567-020-01037-7)
+ Boocock et al bioRxiv (2023) doi: [10.1101/2023.03.24.534111](https://doi.org/10.1101/2023.03.24.534111)

The code can be used to reproduce the simulation results presented in
Boocock et al (2023).

# Installation 

Before installation of this user project you will first need to
download and install a version of
[Chaste](https://github.com/Chaste/Chaste). Simulations in Boocock et
al (2023) were performed using [Chaste
2019.1](https://github.com/Chaste/Chaste/tree/release_2019.1) and
[Chaste 2021.1](https://github.com/Chaste/Chaste/tree/release_2021.1)
(recommended).

There are various ways of [obtaining
Chaste](https://chaste.cs.ox.ac.uk/trac/wiki/GettingStarted). One
method is to clone from the [Chaste github
repository](https://github.com/Chaste/Chaste) and change branch to a
specific release:

    git clone https://github.com/Chaste/Chaste.git
	cd Chaste
    git checkout release_2021.1
	
You then need to follow instructions to [install
dependencies](https://chaste.cs.ox.ac.uk/trac/wiki/InstallGuides/InstallGuide)
and for [first time configuration and
build](https://chaste.cs.ox.ac.uk/trac/wiki/ChasteGuides/CmakeFirstRun).

Once you have a working installation of Chaste, clone the erk\_waves
repository to the user projects directory and compile following the
instructions in the [User
Projects](https://chaste.cs.ox.ac.uk/trac/wiki/ChasteGuides/UserProjects)
guide. When running setup.py you only need to select yes for the
cell\_based component. To speed up simulations change
"CMAKE\_BUILD\_TYPE" from "Debug" to "Release".

    cd Chaste/projects
	git clone https://github.com/DBoocock/erk_waves.git
	cd erk_waves
	python setup_project.py
	cd path/to/chaste_build    # e.g. cd ~/home/chaste_build
	ccmake path/to/Chaste    # e.g. ccmake ~/home/Chaste
	make TestERKWaveWithSelfPropulsionNoAlignment

The "python" directory within the cloned repository contains a Python
script for specifying combinations of parameter values and launching
parralel simulations. Copy or move this directory to a convenient
location and modify "batch\_erk\_propulsion\_no\_alignment.py" to
point to the location of the compiled C++ executable in your build
(default
"~/chaste\_build/projects/erk\_waves/test/TestERKWaveWithSelfPropulsionNoAlignment").

To run:

    python batch_erk_propulsion_no_alignment.py



Simulation output will be saved to a location specified by the system
environment variable "CHASTE\_TEST\_OUTPUT" which you may set as
desired. The output directory includes a file "params.txt" documenting
parameter values as well as a csv file
"results\_from\_time\_xxxx/cell\_data.dat" containing simulation data
(the data to save is specified in the ErkPropulsionWriterNoAlignment
class). Within each row of "cell\_data.dat" the first column gives the
simulation timepoint, then columns repeat in blocks of 8 containing
the following information for each cell:

    location_index    # Redundant with cell_id
    cell_id    # Unique cell identifier
	x, y    # Cell centroid location
    erk    # ERK level
    A0    # Target area
    A    # Cell area
    theta    # Angle of self-propulsion

Simulations can be visualized using
[ParaView](https://www.paraview.org/download/): open ParaView and
simply import the pvd file from the location
"results\_from\_time\_xxxx/results.pvd" (consult the [Chaste
Tutorials](https://chaste.cs.ox.ac.uk/chaste/tutorials/release_2021.1/UserTutorials.html)
for further information).

The analysis of simulation output published in Boocock et al (2023)
was performed using a suite of Python scripts and modules that are
available with data using the following [download
link](https://seafile.ist.ac.at/f/7538561b4dba4bf69d49/) (850MB
download).

# Installation via Docker
You can install Chaste by following the steps mentioned in [Docker Github repo](https://github.com/Chaste/chaste-docker).
Once the container has launched successfully, you can start building the project as follows:

First you should clone the repository to the user projects directory.

    cd projects
	git clone https://github.com/DBoocock/erk_waves.git
	cd erk_waves
	python setup_project.py
 
When running setup.py you only need to select yes for the
cell\_based component.

Then you should build the project using the instruction:

    cd scripts
	. build_project.sh erk_waves c
	cd lib
	make TestERKWaveWithSelfPropulsionNoAlignment
 
To run:

    cd projects/erk_waves/python
	python batch_erk_propulsion_no_alignment.py
	
 
Note that you should change the path to the executable in batch_erk_propulsion_no_alignment.py before doing this step.
   
#### Developing the code within the container
You can use the instructions in [Developing the code within the container](https://github.com/Chaste/chaste-docker#developing-code-within-the-container) to access the files within your container and edit the codes.
#### Share data between the host and container
Simulation output will be saved in the testoutput folder. You can use a [bind mount](https://github.com/Chaste/chaste-docker#sharing-data-between-the-host-and-container) to gain a direct access to this folder from the host.

   
   
   
   

