#include "TotalFlux.hh"

TotalFlux::TotalFlux()
{
	// Initialize data collection
	Nbin = 200; //bins.GetNbins();
	logemin = -6; //logMeV //bins.GetMinE(); //1 eV
	logemax = 6.8; //logMeV //bins.GetMaxE(); //100 GeV

	nIncident = 0;
}

TotalFlux::TotalFlux(G4String csvSource)
{
	this->FirstUse();
}


TotalFlux::~TotalFlux()
{}

void TotalFlux::FirstUse()
{
	// Initialize data collection
	Nbin = 200;// bins.GetNbins();
	logemin = -6; //bins.GetMinE(); //100 KeV
	logemax = 6.8; //bins.GetMaxE(); //100 GeV
	
	nIncident = 0;

}

void TotalFlux::IterIncident()
{
	nIncident++;
}


// Update total flux considering energy of primary, secondary and secondary identity
// If energies out of spectra boundaries, abort update and leave function
bool TotalFlux::Update(G4double secondarykE, G4String secondaryName)
{

	// We don't register neutrinos
	if (secondaryName=="nu_mu" || secondaryName=="nu_e" || secondaryName=="anti_nu_mu" || secondaryName=="anti_nu_e") {return false;}
	G4double logsegkE = log10(secondarykE);

	if (logsegkE<logemin) {return false;}
	
	G4int oebin = Nbin*(logsegkE-logemin)/(logemax-logemin);
	if (oebin<0) return false;
	if (oebin>=Nbin)
	{
		G4cout << "Warning ibin too high energy particle for the energy range provided, ignoring" << G4endl; 
		return false;
	}

	G4String particleName(secondaryName);

	if (ofluxes.count(particleName)==0) {ofluxes[particleName]=ParticleSpectra(Nbin);}
	ofluxes[particleName].Update(oebin);
	
	return true;
}

void TotalFlux::Print()
{

	std::map<G4int, ParticleSpectra>::iterator it;
	for (auto const& x : ofluxes)
	{
		std::cout << x.first << std::endl; 
		ParticleSpectra spe = x.second; // string's value 
		spe.print();
	}
	G4cout << "nIncident " << nIncident << G4endl;
}

