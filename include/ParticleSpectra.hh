#ifndef PARTICLESPECTRA_HH
#define PARTICLESPECTRA_HH

#include "G4AnalysisManager.hh"
#include <vector>

// Structure that inform the number of particle of a certain kind produced in after crossing the target.
// It should be associated to one specific input particle and a specific ibin energy
struct ispectra
{
	double bin; //The spectra of outgoing particle 
	std::vector<int> spectra; // Number of particle for each obin 
};


// Class informing the full spectra of a specific outgoing particle type associated with a specific incoming particle
class ParticleSpectra
{
	public:
		ParticleSpectra();
		ParticleSpectra(G4int Nbin_);
		~ParticleSpectra();
		
		void Update(G4int oebin) 
		{
			//G4cout << iabin << " " << iebin << " " << oabin << " " << oebin << " " << iabin*Nbin+iebin << " " << oabin*Nbin+oebin << G4endl;
			oflux[oebin]++;
		}
		void print();
		
		// Get value of the energy bin
		G4int GetBin(G4int oebin) {return oflux[oebin];}
		
		// Get value of the angular bin
		//G4int GetABin(G4int ibin, G4int abin) {return oflux[ibin].angles[abin];}
		
		void SetBin(G4int oebin, G4int N) {oflux[oebin] = N;}
		G4int GetNbin() {return Nbin;}

	private: 
		G4int Nbin;
		std::vector <int> oflux; // One spectra for each ibin	
		
};

#endif
