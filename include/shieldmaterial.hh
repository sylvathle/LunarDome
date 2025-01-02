#ifndef SHIELDMATERIAL_HH
#define SHIELDMATERIAL_HH

#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"

class ShieldMaterial
{
	private:
		G4String shieldMatName;
		

	public:
		//ShieldMaterial(G4NistManager *nist, G4String *g4ShieldMatName);
		ShieldMaterial(G4String g4ShieldMatName);
		~ShieldMaterial();
		G4Material *shieldMat;
		char* GetName();

		bool isComposite;
		double density;
		double A;
		double Z;
};



#endif
