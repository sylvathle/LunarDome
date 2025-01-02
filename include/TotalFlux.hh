#ifndef TOTALFLUX_HH
#define TOTALFLUX_HH

#include "ParticleSpectra.hh"
//#include "Bins.hh"

// Total outgoing flux associated to one incident particle
// Should also be assiociated to one material and one width of this material

class TotalFlux
{

	public:
		TotalFlux();
		TotalFlux(G4String csvSource);
		~TotalFlux();
		
		void FirstUse();
		bool Update(G4double secondarykE, G4String secondaryName);
		void Print();
		//void ToCSV(G4String fileName) const;
		void IterIncident();

		std::map<G4String,ParticleSpectra> GetFluxes() {return ofluxes;}
		
		//std::vector <G4double> GetEdges(G4String scale="log");
	
	private:
		G4int Nbin;
		G4double logemax, logemin; //maximum and minimum used energies
		std::map<G4String,ParticleSpectra> ofluxes; //Set of fluxes for each outgoing particle kind
		//std::vector<G4double> oangles;
		//int Nbin;
		//G4double logemax, logemin; //maximum and minimum used energies
		G4int nIncident; // Number of incident particle per ibin
		//Bins bins;


};

#endif
