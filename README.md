# LunarDome
Code for Dose calculation of ICRP145 and IcruSphere shielded with regolith dome

## ICRP145
Data of the ICRP 145 must be set up to run the code, the global environment variable _PHANTOM\_PATH_ is needed.
A simple way is to run the advanced example of the ICRP145\_HumanPhantoms.

The 
```
export PHANTOM_PATH=geant4dir/examples/advanced/ICRP145_HumanPhantoms/build/ICRP145data/
```

## Install
After cloning the project, the usual Geant4 C++ commands must be run:

```
cd LunarDome
mkdir build
cmake ..
make -j8
```
