#include "shieldmaterial.hh"

//ShieldMaterial::ShieldMaterial(G4NistManager *nist, G4String *g4ShieldMatName)
ShieldMaterial::ShieldMaterial(G4String g4ShieldMatName)
{
	//G4cout << "SHIELDMATNAME " << *shieldMatName << G4endl;

	G4NistManager *nist = G4NistManager::Instance();
	//G4MaterialPropertiesTable *mptShield = new G4MaterialPropertiesTable();
	//mptShield->AddProperty("RINDEX", energy, rindexShield, 2);
	shieldMatName = g4ShieldMatName;
	
	G4int z,natoms,ncomponents,nel;
	G4double a,massfraction;
	
	//G4Material *SiO2 = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
	G4Element* elH  = new G4Element("Hydrogen",  "H",  z=1.,  a=1.00784*g/mole);
	G4Element* elC = new G4Element("Carbon", "C", z=6., a= 12.011*g/mole);
	G4Element* elO  = new G4Element("Oxygen",  "O",  z=8.,  a= 15.9994*g/mole);
	G4Element* elP = new G4Element("Phosphorus", "P", z=15., a= 30.9738*g/mole);
	G4Element* elMn = new G4Element("Manganese","Mn", z=25., a= 54.9380*g/mole);
	
	// Manganese Oxide
	density = 5.43 *g/cm3;
	G4Material* MnO = new G4Material("MnO", density, nel= 2);
	MnO-> AddElement(elMn, natoms=1);
	MnO-> AddElement(elO,  natoms=1);
	
	// Phosphorus Pentoxyde
	density = 2.39 *g/cm3;
	G4Material* P2O5 = new G4Material("P2O5", density, nel= 2);
	P2O5-> AddElement(elP, natoms=2);
	P2O5-> AddElement(elO,  natoms=5);
	
	G4double densityEAC1A = 1.45 *g/cm3;
	G4Material *EAC1A = new G4Material("EAC1A", densityEAC1A, ncomponents = 10);
	EAC1A -> AddMaterial(nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE"), massfraction=.437*perCent);
	EAC1A -> AddMaterial(nist->FindOrBuildMaterial("G4_TITANIUM_DIOXIDE"), massfraction=.024*perCent);
	EAC1A -> AddMaterial(nist->FindOrBuildMaterial("G4_ALUMINUM_OXIDE"), massfraction=.126*perCent);
	EAC1A -> AddMaterial(nist->FindOrBuildMaterial("G4_FERRIC_OXIDE"), massfraction=.12*perCent);
	//EAC1A -> AddMaterial(MnO, massfraction=.002*perCent);
		EAC1A -> AddMaterial(MnO, massfraction=.002*perCent);
	EAC1A -> AddMaterial(nist->FindOrBuildMaterial("G4_MAGNESIUM_OXIDE"), massfraction=.119*perCent);
	EAC1A -> AddMaterial(nist->FindOrBuildMaterial("G4_CALCIUM_OXIDE"), massfraction=.108*perCent);
	EAC1A -> AddMaterial(nist->FindOrBuildMaterial("G4_SODIUM_MONOXIDE"), massfraction=.0029*perCent);
	EAC1A -> AddMaterial(nist->FindOrBuildMaterial("G4_POTASSIUM_OXIDE"), massfraction=.0013*perCent);
	EAC1A -> AddMaterial(P2O5, massfraction=.006*perCent);

	G4double densityPLA = 1.2387 *g/cm3;
	G4Material *PLA = new G4Material("PLA",density,ncomponents = 3);
	PLA -> AddElement(elC,natoms=3);
	PLA -> AddElement(elH,natoms=4);
	PLA -> AddElement(elO,natoms=2);

	G4double densityBrick = 1.54 *g/cm3;
	G4Material *RegoBrick = new G4Material("RegoBrick",density,ncomponents=2);
	RegoBrick -> AddMaterial(EAC1A, massfraction=0.9);
	RegoBrick -> AddMaterial(PLA, massfraction=0.1);
	isComposite=true;
	

	if  (g4ShieldMatName==G4String("EAC1A"))
	{
		shieldMat = EAC1A;
		density = densityEAC1A;
	}
	else if (g4ShieldMatName==G4String("RegoBrick"))
	{
		shieldMat = RegoBrick;
		density = densityBrick;
	}
	else if (g4ShieldMatName==G4String("PLA"))
	{
		shieldMat = PLA;
		density = densityPLA;
	}		
	else
	{
		shieldMat=nist->FindOrBuildMaterial(g4ShieldMatName);
		density = shieldMat->GetDensity();
		isComposite=false;
	}

	
	
}

ShieldMaterial::~ShieldMaterial()
{}

char* ShieldMaterial::GetName()
{
	return shieldMatName.data();
}


