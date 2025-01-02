#ifndef GCRFlux_h
#define GCRFlux_h

#include "G4GenericMessenger.hh"

#include <vector>
#include <map>
#include <random>
#include <iostream>
#include <fstream>

#include "ions.hh"

// Class containing information of particle content and flux (m-2.sr-1.s-1)

class GCRFlux
{
	public:
		GCRFlux();
		~GCRFlux();

		G4int GetNIons() const { return nIonsKind;}
		G4String GetIonName(G4int i) const { return ionsName[i];}
		G4int GetNbins(G4int Z) const {return Nbins.at(Z);}
		G4double GetEneVal(G4int Z, G4int ie) const {return binsEnergy.at(Z)[ie];}
		G4double GetFluxVal(G4int Z, G4int ie) const {return binsFlux.at(Z)[ie];}
		G4int GetA(G4String s) const {return ions.at(s).getA();}
		G4int GetZ(G4String s) const {return ions.at(s).getZ();}
		
		G4double GetParticleFlux(G4int Z) const {return listIntegratedFluxParticleKind[Z];}
		
		G4double GetEnergyFlux(G4int Z, G4double E) const;

	private:
		
		G4GenericMessenger *fMessenger;
		G4String csvFile;

		std::map< G4int , std::vector<G4double> > binsEnergy; //Energy bins for each particles
		std::map< G4int , std::vector<G4double> > binsFlux; //Flux bins for each particles
		std::map< G4int , G4int > Nbins; //N bins for eahc particle ()

		G4double total_particles;
		G4int nIonsKind;

		std::vector<G4double> listIntegratedFluxParticleKind;
		std::vector<G4double> listParticleWeight;
		std::vector<G4String> ionsName;
		std::map<G4String,Ion> ions;
		
		void LoadGCRFromCSV();
		void DefineCommands();
		void SetGCRcsv(G4String csv);
	
};

#endif
