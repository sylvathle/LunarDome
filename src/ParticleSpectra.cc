#include "ParticleSpectra.hh"

ParticleSpectra::ParticleSpectra(): Nbin(50)
{
	//oflux.resize(Nbin);
//	for (int i=0;i<Nbin*Nangles;i++)
//	{
	for (int i=0;i<Nbin;i++) {oflux.push_back(0);}
		//for (int j=0;j<Nangles;j++) {oflux[i].angles.push_back(0);}
//	}
}

ParticleSpectra::ParticleSpectra(int Nbin_): Nbin(Nbin_)
{
	//oflux.resize(Nbin); 
	//G4cout << "Define particle spectra" << G4endl;
	//for (int i=0;i<Nbin*Nangles;i++)
	//{
	for (int j=0;j<Nbin;j++) {oflux.push_back(0);}
		//for (int j=0;j<Nangles;j++) {oflux[i].angles.push_back(0);}
	//}
}

ParticleSpectra::~ParticleSpectra()
{
	//delete oflux;
}

//void ParticleSpectra::Update(G4int iebin, G4int oebin, G4int oabin)
//{
//	oflux[iebin].spectra[oabin*Nbin+oebin]++;
	//oflux[ibin].angles[abin]++;
//}

//void ParticleSpectra::SetBin(G4int ibin, G4int obin, G4int abin, G4int N)
//{
//	oflux[ibin].spectra[abin*Nbin+obin] = N;
//}

void ParticleSpectra::print()
{
	//for (int ibin=0;ibin<Nbin;ibin++)
	//{
		for (int obin=0;obin<Nbin;obin++)
		{
			if (oflux[obin]!=0)
			{
				G4cout <<  obin << " "  << oflux[obin] << G4endl;
			}
		}
	//}
}
