#include "materials.hh"

Materials::Materials()
{
	nist = G4NistManager::Instance();
	DefMonoatomic();
	DefIcruMat();
	DefOxides();
	DefMetalAlloys();
	DefHydrogenRich();
	DefRegos();
}

Materials::~Materials()
{
	delete nist;
}

G4Material* Materials::GetMaterial(G4String mat)
{
	if (mat=="PLA") {return PLA;}
	if (mat=="RegoBrick") {return RegoBrick;}
	if (mat=="EAC1A") {return EAC1A;}
	if (mat=="LiqHydrogene") {return LiqHydrogene;}
	if (mat=="LiqMethane") {return LiqMethane;}
	if (mat=="RegoAp17") {return RegoAp17;}
	if (mat=="IcruMat") {return icruSphereMat;}
	//G4NistManager *nist = G4NistManager::Instance();
	return nist->FindOrBuildMaterial(mat);
}

G4Material * Materials::GetRegoAtDepth(G4double z)
{
	G4int ncomponents;
	G4double massfraction;

	G4double densityRego = 1.92*(abs(z/m)+12.2)/(abs(z/m)+18.0) *g/cm3;
	std::stringstream ss_rego_id;
	ss_rego_id << abs(z);
	std::string str_rego_id;
	ss_rego_id >> str_rego_id;

	return GetRegoAp17(densityRego,str_rego_id);
}

void Materials::DefMonoatomic()
{
	G4int z;
	G4double a;

	elH  = new G4Element("Hydrogen",  "H",  z=1.,  a=1.00784*g/mole);
	elB  = new G4Element("Boron",  "B",  z=5.,  a=10.81*g/mole);
	elC = new G4Element("Carbon", "C", z=6., a= 12.011*g/mole);
	elN = new G4Element("Nitrogen", "N", z=7., a= 14.001*g/mole);
	elO  = new G4Element("Oxygen",  "O",  z=8.,  a= 15.9994*g/mole);
	elAl  = new G4Element("Aluminum",  "Al",  z=13.,  a= 26.9815*g/mole);
	elP = new G4Element("Phosphorus", "P", z=15., a= 30.9738*g/mole);
	elCr = new G4Element("Chromium","Cr", z=24., a= 51.9961*g/mole);
	elMn = new G4Element("Manganese","Mn", z=25., a= 54.9380*g/mole);

}

void Materials::DefIcruMat()
{
	G4double d_icrusphere = 1.000 *g/cm3;
	icruSphereMat = new G4Material("icruSphereMat", d_icrusphere, 4);

	//76,2 % oxygen, 11,1 % carbon, 10,1 % hydrogen and 2,6 % nitrogen).
	icruSphereMat->AddElement(nist->FindOrBuildElement("H"),10.1*perCent);
	icruSphereMat->AddElement(nist->FindOrBuildElement("N"),2.6*perCent);
	icruSphereMat->AddElement(nist->FindOrBuildElement("O"),76.2*perCent);
	icruSphereMat->AddElement(nist->FindOrBuildElement("C"),11.1*perCent);	

}

void Materials::DefOxides()
{
	G4int natoms,nel;

	// Manganese Oxide
	G4double d_MnO = 5.43 *g/cm3;
	MnO = new G4Material("MnO", d_MnO, nel= 2);
	MnO-> AddElement(elMn, natoms=1);
	MnO-> AddElement(elO,  natoms=1);
	
	// Phosphorus Pentoxyde
	G4double d_P2O5 = 2.39 *g/cm3;
	P2O5 = new G4Material("P2O5", d_P2O5, nel= 2);
	P2O5-> AddElement(elP, natoms=2);
	P2O5-> AddElement(elO,  natoms=5);
	
	// Chromium oxyde
	G4double d_Cr2O3 = 5.22 *g/cm3;
	Cr2O3 = new G4Material("Cr2O3", d_Cr2O3, nel= 2);
	Cr2O3-> AddElement(elCr, natoms=2);
	Cr2O3-> AddElement(elO,  natoms=3);
	
}

void Materials::DefMetalAlloys()
{
	G4int ncomponents;
	G4double massfraction;
	G4double d_Al2219 = 2.84 *g/cm3;
	
	Al2219 = new G4Material("Al2219",d_Al2219,ncomponents=7);
	Al2219 -> AddMaterial(nist->FindOrBuildMaterial("G4_Al"),massfraction=0.925);
	Al2219 -> AddMaterial(nist->FindOrBuildMaterial("G4_Cu"),massfraction=0.065);
	Al2219 -> AddMaterial(nist->FindOrBuildMaterial("G4_Mn"),massfraction=0.003);
	Al2219 -> AddMaterial(nist->FindOrBuildMaterial("G4_Si"),massfraction=0.002);
	Al2219 -> AddMaterial(nist->FindOrBuildMaterial("G4_Fe"),massfraction=0.003);
	Al2219 -> AddMaterial(nist->FindOrBuildMaterial("G4_Zn"),massfraction=0.001);
	Al2219 -> AddMaterial(nist->FindOrBuildMaterial("G4_V"),massfraction=0.001);

}

void Materials::DefHydrogenRich()
{
	G4int ncomponents,natoms,nel;
	G4double d_PLA = 1.2387 *g/cm3;
	PLA = new G4Material("PLA",d_PLA,ncomponents = 3);
	PLA -> AddElement(elC,natoms=3);
	PLA -> AddElement(elH,natoms=4);
	PLA -> AddElement(elO,natoms=2);

        G4double d_LiqMethane = 0.42262 *g/cm3;
        LiqMethane = new G4Material("LiqMethane",d_LiqMethane,ncomponents = 2);
        LiqMethane -> AddElement(elC,natoms=1);
        LiqMethane -> AddElement(elH,natoms=4);

        G4double d_LiqHydrogene = 0.07085 *g/cm3;
        LiqHydrogene = new G4Material("LiqHydrogene",d_LiqHydrogene,ncomponents = 1);
        LiqHydrogene -> AddElement(elH,natoms=2);

	G4double d_kevlar = 1.44 *g/cm3;
	Kevlar = new G4Material("Kevlar",d_kevlar,nel = 4);
	Kevlar -> AddElement(elC,natoms=14);
	Kevlar -> AddElement(elO,natoms=2);
	Kevlar -> AddElement(elN,natoms=2);
	Kevlar -> AddElement(elH,natoms=10);

	G4double d_borazine = 0.78 *g/cm3;
	Borazine = new G4Material("Borazine",d_borazine,nel=3);
	Borazine -> AddElement(elB,natoms=1);
	Borazine -> AddElement(elH,natoms=6);
	Borazine -> AddElement(elN,natoms=1);
	
}

void Materials::DefEAC1A(G4double d_EAC1A)
{
	G4int ncomponents;
	G4double massfraction;

	EAC1A = new G4Material("EAC1A", d_EAC1A, ncomponents = 10);
	G4double massSiO2(0.437), massTiO2(0.024),massAl2O3(.126),massFeO(.12),massMnO(.002),massMgO(.119),massCaO(.108),massNaO(.0029),massK2O(.0013),massP2O5(.006);
	G4double totMassEAC1A=massSiO2+massTiO2+massAl2O3+massFeO+massMnO+massMgO+massCaO+massNaO+massK2O+massP2O5;
	EAC1A -> AddMaterial(nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE"), massfraction=massSiO2/totMassEAC1A);
	EAC1A -> AddMaterial(nist->FindOrBuildMaterial("G4_TITANIUM_DIOXIDE"), massfraction=massTiO2/totMassEAC1A);
	EAC1A -> AddMaterial(nist->FindOrBuildMaterial("G4_ALUMINUM_OXIDE"), massfraction=massAl2O3/totMassEAC1A);
	EAC1A -> AddMaterial(nist->FindOrBuildMaterial("G4_FERRIC_OXIDE"), massfraction=massFeO/totMassEAC1A);
	EAC1A -> AddMaterial(MnO, massfraction=massMnO/totMassEAC1A);
	EAC1A -> AddMaterial(nist->FindOrBuildMaterial("G4_MAGNESIUM_OXIDE"), massfraction=massMgO/totMassEAC1A);
	EAC1A -> AddMaterial(nist->FindOrBuildMaterial("G4_CALCIUM_OXIDE"), massfraction=massCaO/totMassEAC1A);
	EAC1A -> AddMaterial(nist->FindOrBuildMaterial("G4_SODIUM_MONOXIDE"), massfraction=massNaO/totMassEAC1A);
	EAC1A -> AddMaterial(nist->FindOrBuildMaterial("G4_POTASSIUM_OXIDE"), massfraction=massK2O/totMassEAC1A);
	EAC1A -> AddMaterial(P2O5, massfraction=massP2O5/totMassEAC1A);
}


void Materials::DefRegoAp17(G4double d_RegoAp17)
{
	RegoAp17 = GetRegoAp17(d_RegoAp17,"");
}

G4Material* Materials::GetRegoAp17(G4double d_RegoAp17, G4String suffixName)
{
	G4int ncomponents;
	G4double massfraction;

	G4String prefix = "RegoAp17";

	if (suffixName != "") {prefix=prefix+"_";}

	G4Material *rego = new G4Material(prefix+suffixName, d_RegoAp17, ncomponents = 10);
	G4double massSiO2(0.434), massTiO2(0.032), massAl2O3(.181), massFeO(.108), massMnO(.00145), massMgO(.120), massCaO(.128), massK2O=(.001), massNa2O(0.0038),massCr2O3(0.0027);
	G4double totMass=massSiO2+massTiO2+massAl2O3+massFeO+massMnO+massMgO+massCaO+massNa2O+massK2O+massCr2O3;
	rego -> AddMaterial(nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE"), massfraction=massSiO2/totMass);
	rego -> AddMaterial(nist->FindOrBuildMaterial("G4_TITANIUM_DIOXIDE"), massfraction=massTiO2/totMass);
	rego -> AddMaterial(nist->FindOrBuildMaterial("G4_ALUMINUM_OXIDE"), massfraction=massAl2O3/totMass);
	rego -> AddMaterial(nist->FindOrBuildMaterial("G4_FERRIC_OXIDE"), massfraction=massFeO/totMass);
	rego -> AddMaterial(MnO, massfraction=massMnO/totMass);
	rego -> AddMaterial(nist->FindOrBuildMaterial("G4_MAGNESIUM_OXIDE"), massfraction=massMgO/totMass);
	rego -> AddMaterial(nist->FindOrBuildMaterial("G4_CALCIUM_OXIDE"), massfraction=massCaO/totMass);
	rego -> AddMaterial(nist->FindOrBuildMaterial("G4_SODIUM_MONOXIDE"), massfraction=massNa2O/totMass);
	rego -> AddMaterial(nist->FindOrBuildMaterial("G4_POTASSIUM_OXIDE"), massfraction=massK2O/totMass);
	rego -> AddMaterial(Cr2O3, massfraction=massCr2O3/totMass);

	return rego;
	
}

void Materials::DefRegoBrick(G4double d_RegoBrick, G4String regoStr)
{
	G4int ncomponents;
	G4double massfraction;
	G4Material* Rego = GetMaterial(regoStr);
	RegoBrick = new G4Material("RegoBrick",d_RegoBrick,ncomponents=2);
	RegoBrick -> AddMaterial(Rego, massfraction=0.9);
	RegoBrick -> AddMaterial(PLA, massfraction=0.1);
}


void Materials::DefRegos()
{
	DefEAC1A();
	DefRegoAp17();
	DefRegoBrick();
}

