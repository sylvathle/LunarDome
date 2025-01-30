# LunarDome
Code for Dose calculation of ICRP145 and IcruSphere shielded with regolith dome

## Running Geant4 simulations

### ICRP145
Data of the ICRP 145 must be set up to run the code, the global environment variable _PHANTOM\_PATH_ is needed.
A simple way is to run the advanced example of the ICRP145\_HumanPhantoms.

The 
```
export PHANTOM_PATH=geant4dir/examples/advanced/ICRP145_HumanPhantoms/build/ICRP145data/
```

### Install
After cloning the project, the usual Geant4 C++ commands must be run:

```
cd LunarDome
mkdir build
cmake ..
make -j8
```
### First run
The code can be run using the test.mac macros in the inputs folder

```
./sim ../inputs/test.mac
```
If everything runs fine a folder called "results/Icrusphere/test/" should be created where are created csv files containing the results.
Each run creates 5 files with pattern YYYYMMDD-HHmmss-XXXXXXX\_nt\_type.csv, where XXXXXXX is a random string of numbers and type can take the values:

- Doses: total doses in mGy for each primary ion
- InnerFlux: fluxes for each primary ion
- NParticles: number of primary ion simulated
- TotalBody: total doses in the human phantom
- weightParticle: total flux of particles per ion

### Macros

The macros should work with the usual macro commands plus some that have been defined for the simulation of a human phantom in a lunar dome (see test.mac in the inputs folder as example).


```
/SIM/scoring/phantom HP
```
Define the human phantom to be simulated: HP can take the values IcruSphere or ICRP145.
A folder called IcruSphere or ICRP145 will be created inside the results folder to store the results.

```
/SIM/gcr/gcrFile csvFile
```
Indicate the primary fluxes to use for the simulation, in csv format, csvFile must give the location of the file.
More information provided at https://doi.org/10.5281/zenodo.14622711 on the format of this file.


```
/SIM/scoring/resDir folderName
```
Indicating where the csv files created in the simulations should be stored. A folder called folderName will be created in results/HumanPhantom/ and used to store the results.


```
/SIM/scoring/sampleSize Nsample
```
Nsample stands for the number of particle that should be generated before storing the results, if Nsample if bigger Nsimulated with the /run/beamOn command then no data will be stored.
If Nsample = 10 and 100 particles where simulated, then 10 runs will be saved.

```
/SIM/geometry/domeIn/radius innerRadius
```
Give the inner radius of the dome in milimeters.


```
/SIM/geometry/dome1/thick thick1
```
Give the thickness of the first layer in milimeters.


```
/SIM/geometry/dome1/material material
```
material here informs the material to be used for the first layer (e.g. G4\_Galactic, Al2219, RegoAp17 ....)
All native geant4 materials can be used plus some materials defined for this projects which name can be found in the ../src/material.cc file.

For the two last commands up to 4 layers can be stacked, informing their thickness and material. No layer can take thickness 0, therefore if some should not be used they can be replaced by the material of the world (generally G4\_Galactic).


```
/SIM/scoring/radbeam radius mm
```
radius stands for the radius of the hemisphere from where primaries are generated. For logical reason the user should make sure it is a bigger number than the radius of the last layer of the dome. (innerRadius plus the 4 thicknesses of the 4 layers).


```
/SIM/generate/rat Z w
```
Weight w used to generate the ion of charge Z. By default all 28 ions are generated with equal probability. w can take any positive number, the code renormalize the weight between 0 and 1.
If the user want to simulate iron only, the weight of all other ions must be set to 0.





## Analysis

This sections assumes the geant4 simulations ran following a specific way of naming the folders containing the csv files.
The data has been resumes and shared at https://doi.org/10.5281/zenodo.14622711, they can be downloaded and moved to a data folder inside the analysis folder to run the analysis.

A folder called figures must be created at the same level than the analysis folder (outside the analysis folder).

```
cd LunarDome
mkdir figures
```

The analysis related to doses calculations is run with:

```
python3 plotAnalysis.py
```

This file used the Icrudose.csv and ICRPdose.csv files to generate the figures. 
Other input are used like EDE\_matthiae.csv to compare the results from other studies.

The analysis related to fluxes can be run with:

```
python3 plotFluxes.py arg
```
The argument can be "All" is all primaries should be included, but it can also be replaced by any of the primary ions (alpha, O16, Fe56...) to obtain the fluxes penetrating the dome from a specific ion.


The model of dependency of the Doses in a human phantom with regolith thickness (in g/cm2) can be used with the regomodel.py file.

e.g.
```
python3 regomodel.py 100 DE_IFHP
```

The first argument (here 100) is a float number stading for the regolith thickness (g/cm2). The second argument (here DE\_IFHP) corresponds to the scenario/quantity of interest:

- DE\_ICRU: Dose equivalent for the ICRU sphere
- DE\_IFHP: Dose equivalent all organs for the ICRP 145 female human phantom
- EDE\_IFHP: Effective dose equivalent for the ICRP 145 female human phantom




