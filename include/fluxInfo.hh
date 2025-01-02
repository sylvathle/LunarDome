#ifndef FLUXINFO_HH
#define FLUXINFO_HH

#include "G4AnalysisManager.hh"

class fluxInfo
{
	public:
		fluxInfo();
		fluxInfo(G4String particleName_,G4double kE_);
		~fluxInfo();
		
		fluxInfo operator+=(const fluxInfo& b);
		fluxInfo operator+=(const G4double& d);
	
		G4double kE ;
		//G4double ang ;
		G4String particleName;
};

#endif
