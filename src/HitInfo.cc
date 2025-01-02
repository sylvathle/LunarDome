#include "HitInfo.hh"

HitInfo::HitInfo(): EDE(0.), Dose(0.) {}

HitInfo::HitInfo(G4double* ede, G4double* dose): EDE(*ede), Dose(*dose) {}

HitInfo::~HitInfo() {}

HitInfo& HitInfo::operator+=(const HitInfo& obj)
{
	EDE += obj.EDE;
	Dose += obj.Dose;
	return (HitInfo&)(*this);
}

G4double QualityFactor(G4double let)
{
	G4double let_kev_um(let); // Assuming it comes in MeV/mm, so multiply by 1e3 (MeV to keV) and by 1e3 (mm to um)
	if (let_kev_um<=10.0) {return 1.0;}
	else if (let_kev_um<=100.0) {return 0.32*let_kev_um-2.210;}
	else {return 300.0/sqrt(let_kev_um);}
}






