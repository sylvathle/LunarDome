#ifndef GEOMETRY_HH
#define GEOMETRY_HH

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4CSGSolid.hh"
#include "G4SubtractionSolid.hh"

#include "G4NistManager.hh"

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"

#include "G4SystemOfUnits.hh"
#include "G4MultiFunctionalDetector.hh"

//#include "SBG4PSFlatSurfaceFlux.hh"
#include "SBG4PSSphereSurfaceFlux.hh"

#include "G4VPrimitiveScorer.hh"
#include "G4RunMessenger.hh"
//#include "G4GenericMessenger.hh"
//#include "G4UIcmdWithADoubleAndUnit.hh"

//#include "detector.hh"
//#include "SBMultiFunctionalDetector.hh"
//#include "shieldmaterial.hh"

#include <sstream>


#include "G4SDManager.hh"

#include "G4RunManager.hh"

#include "TETModelImport.hh"
#include "TETParameterisation.hh"
#include "TETPSEnergyDeposit.hh"
#include "ISPSEnergyDeposit.hh"

#include "materials.hh"


class MyGeometry : public G4VUserDetectorConstruction
{
	public:		
		MyGeometry();
		MyGeometry(G4UIExecutive *ui_);
		~MyGeometry();
		
		G4LogicalVolume *GetLogicIS() const { return logicPhantom; }

		virtual G4VPhysicalVolume *Construct();
		TETModelImport * GetICRP145phantom() const {return fTetData;}
		G4String GetPhantomType() const {return phantomType;}
	
	private:
		G4UIExecutive *ui;
		
		// rPhantom stands for the radius of the Human Phantom
		//void DefineMaterials();
		void SetHumanPhantom(G4String);
		
		void SetInnerRadius(G4double);
		void SetThickDome1(G4double);
		void SetThickDome2(G4double);
		void SetThickDome3(G4double);
		void SetThickDome4(G4double);

		//void SetDimDome1(G4double inner_radius, G4double outer_radius);
		void SetMaterialDome1(G4String mat);
		
		///void SetDimDome2(G4double inner_radius, G4double outer_radius);
		void SetMaterialDome2(G4String mat);

		//void SetDimDome3(G4double inner_radius, G4double outer_radius);
		void SetMaterialDome3(G4String mat);

		//void SetDimDome4(G4double inner_radius, G4double outer_radius);
		void SetMaterialDome4(G4String mat);
				
		//void SetRegolithDome(G4double inner_radius, G4double outer_radius);
		void DefineCommands();
		
		//G4Material* GetMaterial(G4String mat);
		//G4Material* GetRegoAtDepth(G4double);
		
		
	protected:
		//G4Material *worldMat, *icruSphereMat, *RegoBrick, *AluMat, *EAC1A, *PLA, *LiqMethane, *LiqHydrogene, *RegoAp17, *Cr2O3, *MnO, *P2O5;

		Materials *materials;
	
		G4double regoWidth, aluWidth, rPhantom ;
		//G4LogicalVolume *fScoringVolume;
		G4GenericMessenger *fMessenger;
		
		std::vector <G4Box*> solidRego;
		std::vector <G4LogicalVolume*> logicRego;
		std::vector <G4VPhysicalVolume*> physRego;
		G4Sphere *solidPhantom, *solidAir, *solidDome1,*solidDome2, *solidDome3, *solidDome4, *solidBrickDome;
		G4Box *solidWorld;
		G4Tubs *solidGround;
		TETModelImport *fTetData;
		G4LogicalVolume *logicWorld, *logicAir, *logicDome1, *logicDome2, *logicDome3, *logicDome4, *logicPhantom, *fTetLogic, *fContainer_logic, *logicGround;
		G4VPhysicalVolume *physWorld, *physAir, *physDome1, *physDome2, *physDome3, *physDome4, *physPhantom, *physGround;

		G4double innerRadius;
		G4double thickDome1, out_radius1;
		G4double thickDome2, out_radius2;
		G4double thickDome3, out_radius3;
		G4double thickDome4, out_radius4;

		G4String phantomType;

		// rPhantom stands for the radius of the Human Phantom
		//G4double zWorld;
		G4ThreeVector      fPhantomSize;
		G4ThreeVector      fPhantomBoxMin, fPhantomBoxMax;
		G4int              fNOfTetrahedrons;

		virtual void ConstructSDandField();	

		
};



#endif
