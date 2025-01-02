#include "fluxInfo.hh"

fluxInfo::fluxInfo()
{
	kE = 0;
	//ang = 0;
	particleName = "";
}


fluxInfo::fluxInfo(G4String particleName_,G4double kE_)
{
	kE = kE_;
	//ang = ang_;
	particleName = particleName_;
	//G4cout << kE << " " << particleName << G4endl;
}

// Overload + operator to add two Box objects.
fluxInfo fluxInfo::operator+=(const fluxInfo& b) 
{
         this->kE = this->kE + b.kE;
         return *this;
}

fluxInfo fluxInfo::operator+=(const G4double& d) 
{
         this->kE = this->kE + d;
         return *this;
}


fluxInfo::~fluxInfo() {}
