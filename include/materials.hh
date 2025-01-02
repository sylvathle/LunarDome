#ifndef MATERIALS_HH
#define MATERIALS_HH

#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"

class Materials
{

	public:
		Materials();
		~Materials();

		G4Material* GetMaterial(G4String mat);
		G4Material* GetRegoAtDepth(G4double);

	private:

		G4NistManager *nist;

		// Monoatomic elements
		G4Element* elH;
		G4Element* elB;
		G4Element* elC;
		G4Element* elN;
		G4Element* elO;
		G4Element* elAl;
		G4Element* elP;
		G4Element* elCr;
		G4Element* elMn;

		// 
		G4Material *worldMat;
		G4Material *icruSphereMat;
		G4Material *AluMat;

		// Metal Alloys
		G4Material *Al2219;

		// Oxides
		G4Material *Cr2O3;
		G4Material *MnO;
		G4Material *P2O5;

		// Regolith based materials
		G4Material *RegoAp17;
		G4Material *RegoBrick;
		G4Material *EAC1A;

		//Hydrogen-rich
		G4Material *PLA;
		G4Material *LiqMethane;
		G4Material *LiqHydrogene;
		G4Material *Kevlar;
		G4Material *Borazine;

		void DefMonoatomic();
		void DefIcruMat();
		void DefOxides();
		void DefMetalAlloys();
		void DefHydrogenRich();
		void DefEAC1A(G4double d_EAC1A=1.45*g/cm3);
		G4Material *GetRegoAp17(G4double d_RegoAp17=1.5*g/cm3, G4String suffixName="");
		void DefRegoAp17(G4double d_RegoAp17=1.5*g/cm3);
		void DefRegoBrick(G4double d_RegoBrick=1.54*g/cm3, G4String regoStr="RegoAp17");
		void DefRegos();
		

};

#endif
