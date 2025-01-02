#include "geometry.hh"

MyGeometry::MyGeometry(): regoWidth(0.05*m), aluWidth(0.01*m), rPhantom(0.15*m),phantomType("IcruSphere"), innerRadius(0.4*m), thickDome1(0.005*m), thickDome2(0.01*m), thickDome3(0.005*m), thickDome4(0.31*m)
//MyGeometry::MyGeometry(): regoWidth(0.05*m), aluWidth(0.01*m), rPhantom(0.15*m),phantomType("ICRP145")
{
	materials = new Materials();
	DefineCommands();
}

MyGeometry::MyGeometry(G4UIExecutive *ui_): regoWidth(0.05*m), aluWidth(0.01*m), rPhantom(0.15*m),phantomType("IcruSphere"), innerRadius(0.4*m), thickDome1(0.005*m), thickDome2(0.01*m), thickDome3(0.005*m), thickDome4(0.31*m)
//MyGeometry::MyGeometry(G4UIExecutive *ui_): regoWidth(0.05*m), aluWidth(0.01*m), rPhantom(0.15*m),phantomType("ICRP145")
{

	ui = ui;
	materials = new Materials();
	DefineCommands();
}
	

MyGeometry::~MyGeometry()
{
}

G4VPhysicalVolume *MyGeometry::Construct()
{

	// Import TET model for the ICRP 145 HP
	if (phantomType == "ICRP145")
	{

		fTetData = new TETModelImport(true,ui);
		fPhantomSize     = fTetData -> GetPhantomSize();
		fPhantomBoxMin   = fTetData -> GetPhantomBoxMin();
		fPhantomBoxMax   = fTetData -> GetPhantomBoxMax();
		fNOfTetrahedrons = fTetData -> GetNumTetrahedron();
	}
	
	// Dimensions of the world
	G4double xWorld = 10.0*m;
	G4double yWorld = 10.0*m;
	G4double zWorld = 5.0*m;

	// Create world of simulation.
	solidWorld = new G4Box("solidWorld", xWorld, yWorld, zWorld);
	logicWorld = new G4LogicalVolume(solidWorld,materials->GetMaterial("G4_Galactic"),"logicWorld");
	physWorld = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), logicWorld, "physWorld", 0, false, 0, true);
	
	// Create the multilayers with varying densities of the lunar regolith
	G4double totalDepth=5*m;

	G4int Ndepth = 10;
	G4double sliceThickness = totalDepth/G4double(Ndepth)/2.0;
	G4double depth = -sliceThickness;
	
	G4int idepth = 1;
	std::stringstream ss_rego_id;
	std::string str_rego_id;
	
	
	while (depth>-totalDepth)
	{
		ss_rego_id << idepth;
		ss_rego_id >> str_rego_id;
		
		//G4cout << "depth = " << depth << " " << "solidRego"+G4String(str_rego_id) << " " << GetRegoAtDepth(depth)->GetDensity() << G4endl;
		G4Box *solidRegoSlice = new G4Box("solidRego"+G4String(str_rego_id), 10.0*m, 10.0*m, sliceThickness);
		G4LogicalVolume* logicRegoSlice = new G4LogicalVolume(solidRegoSlice,materials->GetRegoAtDepth(depth),"logicRego"+G4String(str_rego_id));
		//G4LogicalVolume* logicRegoSlice = new G4LogicalVolume(solidRegoSlice,materials->GetMaterial("G4_Galactic"),"logicRego"+G4String(str_rego_id));
		G4VPhysicalVolume* physRegoSlice = new G4PVPlacement(0, G4ThreeVector(0., 0., depth), logicRegoSlice, "physRego"+G4String(str_rego_id), logicWorld, false, 0, true);
	
		solidRego.push_back(solidRegoSlice);
		logicRego.push_back(logicRegoSlice);
		physRego.push_back(physRegoSlice);
	
		depth-=2*sliceThickness;
		++idepth;
	}


	
	
	innerRadius = 0.4*m;
	solidAir = new G4Sphere("solidAir", 0.0*m, innerRadius, 0*deg,360*deg, 0*deg, 90*deg);
	logicAir = new G4LogicalVolume(solidAir,materials->GetMaterial("G4_AIR"),"logicAir");
	//logicAir = new G4LogicalVolume(solidAir,materials->GetMaterial("G4_Galactic"),"logicAir");
	physAir = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.0*m), logicAir, "physAir", logicWorld, false, 0, true);
	
	
	G4double out_radius1 = innerRadius+thickDome1;
	G4double out_radius2 = out_radius1+thickDome2;
	G4double out_radius3 = out_radius2+thickDome3;
	G4double out_radius4 = out_radius3+thickDome4;
	
	solidDome1 = new G4Sphere("solidDome1", innerRadius, out_radius1, 0*deg, 360*deg, 0*deg, 90*deg);
	logicDome1 = new G4LogicalVolume(solidDome1,materials->GetMaterial("G4_Galactic"),"logicDome1");
	physDome1 = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.0*m), logicDome1, "physDome1", logicWorld, false, 0, true);

	G4double thicknessground = 4.0*mm;
	solidGround = new G4Tubs("solidGround",0.0*m,innerRadius,thicknessground,0*deg,360*deg);
	logicGround = new G4LogicalVolume(solidGround,materials->GetRegoAtDepth(depth),"logicGround");
	physGround = new G4PVPlacement(0, G4ThreeVector(0., 0., sliceThickness-thicknessground), logicGround, "physGround", logicRego[0], false, 0, true);

	solidDome2 = new G4Sphere("solidDome2", out_radius1, out_radius2, 0*deg, 360*deg, 0*deg, 90*deg);
	logicDome2 = new G4LogicalVolume(solidDome2,materials->GetMaterial("G4_Galactic"),"logicDome2");
	physDome2 = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.0*m), logicDome2, "physDome2", logicWorld, false, 0, true);

	solidDome3 = new G4Sphere("solidDome3", out_radius2, out_radius3, 0*deg, 360*deg, 0*deg, 90*deg);
	logicDome3 = new G4LogicalVolume(solidDome3,materials->GetMaterial("G4_Galactic"),"logicDome3");
	physDome3 = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.0*m), logicDome3, "physDome3", logicWorld, false, 0, true);
	
	solidDome4 = new G4Sphere("solidDome4", out_radius3, out_radius4, 0*deg, 360*deg, 0*deg, 90*deg);
	logicDome4 = new G4LogicalVolume(solidDome4,materials->GetMaterial("G4_Galactic"),"logicDome4");
	physDome4 = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.0*m), logicDome4, "physDome4", logicWorld, false, 0, true);
	


 	// Define the phantom container (10-cm margins from the bounding box of phantom)
 	//

	if (phantomType=="ICRP145")
	{
		//G4cout << "Geometry, setting up ICRP145" << G4endl; 
 		G4Box* containerSolid = new G4Box("phantomBox", fPhantomSize.x()/2 + 10.*cm,
					           fPhantomSize.y()/2 + 10.*cm,
						   fPhantomSize.z()/2 + 10.*cm);

 		fContainer_logic = new G4LogicalVolume(containerSolid, materials->GetMaterial("G4_Galactic"), "logicPhantom");

		new G4PVPlacement(nullptr, G4ThreeVector(0.0*m,0.0*m,fPhantomSize.z()/2 ), fContainer_logic, "PhantomPhysical", logicAir, false, 0);
		//new G4PVPlacement(nullptr, G4ThreeVector(0.0*m,0.0*m,fPhantomSize.z()/2 ), fContainer_logic, "physicPhantom", logicWorld, false, 0);

		fContainer_logic->SetOptimisation(TRUE);
		fContainer_logic->SetSmartless( 0.5 ); // for optimization (default=2)

		// Define the tetrahedral mesh phantom as a parameterised geometry
		//
		// solid and logical volume to be used for parameterised geometry
		G4VSolid* tetraSolid = new G4Tet("TetSolid", G4ThreeVector(),  G4ThreeVector(1.*cm,0,0), G4ThreeVector(0,1.*cm,0),  G4ThreeVector(0,0,1.*cm));
	
		fTetLogic = new G4LogicalVolume(tetraSolid, materials->GetMaterial("G4_Galactic"), "TetLogic");
		//fScoringVolume = fTetLogic;
		// physical volume (phantom) constructed as parameterised geometry
		new G4PVParameterised("wholePhantom",fTetLogic,fContainer_logic, kUndefined, fTetData->GetNumTetrahedron(), new TETParameterisation(fTetData));

	}
	else if (phantomType=="IcruSphere")
	{
		solidPhantom = new G4Sphere("solidPhantom", 0., rPhantom, 0, 360*deg, 0, 180*deg);
		//logicPhantom = new G4LogicalVolume(solidPhantom, materials->GetMaterial("G4_Galactic"), "logicPhantom");
		logicPhantom = new G4LogicalVolume(solidPhantom, materials->GetMaterial("IcruMat"), "logicPhantom");
		//physPhantom = new G4PVPlacement(0, G4ThreeVector(0,-0.001*m+rPhantom), logicPhantom, "physPhantom", logicAir, false, 0, true);
		physPhantom = new G4PVPlacement(0, G4ThreeVector(0.,0.*m,rPhantom), logicPhantom, "physPhantom", logicAir, false, 0, true);
		//physPhantom = new G4PVPlacement(0, G4ThreeVector(0.,0.*m,rPhantom), logicPhantom, "physPhantom", logicWorld, false, 0, true);
	}

	G4cout << "Geometry created" << G4endl;
	
	return physWorld;
}

void MyGeometry::ConstructSDandField()
{
	G4SDManager* pSDman = G4SDManager::GetSDMpointer();
	G4String phantomSDname = "PhantomSD";
	// MultiFunctional detector
 	//auto* MFDet = new MyMultiDetector(phantomSDname);
 	auto* MFDet = new G4MultiFunctionalDetector(phantomSDname);
 	pSDman->AddNewDetector( MFDet );
		

	if (phantomType=="ICRP145")
	{
		//G4cout << "Sensitive dec " << phantomType << G4endl;
		// scorer for energy depositon in each organ
		MFDet->RegisterPrimitive(new TETPSEnergyDeposit("EDE", fTetData));
		//MFDet->RegisterPrimitive(new TETPSEnergyDeposit("Dose", fTetData));

	//	// attach the detector to logical volume for parameterised geometry (phantom geometry)
		SetSensitiveDetector(fTetLogic, MFDet);
		
	}
	else
	{
		//G4cout << "Sensitive dec " << phantomType << G4endl;
		MFDet->RegisterPrimitive(new ISPSEnergyDeposit("EDE"));

		SetSensitiveDetector(logicPhantom, MFDet);
	}

	//MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");
	//logicShield->SetSensitiveDetector(sensDet);
	
	//if (shieldWidth!=0)
	//{
	G4MultiFunctionalDetector* myScorer = new G4MultiFunctionalDetector("myCellScorer");
	G4SDManager::GetSDMpointer()->AddNewDetector(myScorer);
	G4VPrimitiveScorer* totalSurfFlux = new SBG4PSSphereSurfaceFlux("TotalSurfFlux",0);
	myScorer->RegisterPrimitive(totalSurfFlux);
	logicAir->SetSensitiveDetector(myScorer);

	G4cout << "ConstructSDandField" << G4endl;
}

void MyGeometry::SetHumanPhantom(G4String phantom_) {phantomType = phantom_;}

void MyGeometry::SetInnerRadius(G4double innerrad)
{
	innerRadius = innerrad;
	out_radius1 = innerRadius+thickDome1;
	out_radius2 = out_radius1+thickDome2;
	out_radius3 = out_radius2+thickDome3;
	out_radius4 = out_radius3+thickDome4;

	solidAir->SetOuterRadius(innerRadius);
	solidGround->SetOuterRadius(innerRadius);
	solidDome1->SetInnerRadius(innerRadius);
	solidDome1->SetOuterRadius(out_radius1);
	solidDome2->SetInnerRadius(out_radius1);
	solidDome2->SetOuterRadius(out_radius2);
	solidDome3->SetInnerRadius(out_radius2);
	solidDome3->SetOuterRadius(out_radius3);
	solidDome4->SetInnerRadius(out_radius3);
	solidDome4->SetOuterRadius(out_radius4);
	//G4cout << "InnerRadius" << G4endl;
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

void MyGeometry::SetThickDome1(G4double t1)
{
	thickDome1 = t1;
	out_radius1 = innerRadius+thickDome1;
	out_radius2 = out_radius1+thickDome2;
	out_radius3 = out_radius2+thickDome3;
	out_radius4 = out_radius3+thickDome4;

	solidDome1->SetOuterRadius(out_radius1);
	solidDome2->SetInnerRadius(out_radius1);
	solidDome2->SetOuterRadius(out_radius2);
	solidDome3->SetInnerRadius(out_radius2);
	solidDome3->SetOuterRadius(out_radius3);
	solidDome4->SetInnerRadius(out_radius3);
	solidDome4->SetOuterRadius(out_radius4);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

void MyGeometry::SetThickDome2(G4double t2)
{
	thickDome2 = t2;
	out_radius2 = out_radius1+thickDome2;
	out_radius3 = out_radius2+thickDome3;
	out_radius4 = out_radius3+thickDome4;

	solidDome2->SetInnerRadius(out_radius1);
	solidDome2->SetOuterRadius(out_radius2);
	solidDome3->SetInnerRadius(out_radius2);
	solidDome3->SetOuterRadius(out_radius3);
	solidDome4->SetInnerRadius(out_radius3);
	solidDome4->SetOuterRadius(out_radius4);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

void MyGeometry::SetThickDome3(G4double t3)
{
	thickDome3 = t3;
	out_radius3 = out_radius2+thickDome3;
	out_radius4 = out_radius3+thickDome4;

	solidDome3->SetInnerRadius(out_radius2);
	solidDome3->SetOuterRadius(out_radius3);
	solidDome4->SetInnerRadius(out_radius3);
	solidDome4->SetOuterRadius(out_radius4);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

void MyGeometry::SetThickDome4(G4double t4)
{
	thickDome4 = t4;
	out_radius4 = out_radius3+thickDome4;

	solidDome4->SetInnerRadius(out_radius3);
	solidDome4->SetOuterRadius(out_radius4);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

void MyGeometry::SetMaterialDome1(G4String mat)
{
	logicDome1->SetMaterial(materials->GetMaterial(mat));
	if (mat==G4String("G4_Galactic")) {logicGround->SetMaterial(materials->GetRegoAtDepth(0));}
	else {logicGround->SetMaterial(materials->GetMaterial(mat));}
}

void MyGeometry::SetMaterialDome2(G4String mat)
{
	logicDome2->SetMaterial(materials->GetMaterial(mat));
}

void MyGeometry::SetMaterialDome3(G4String mat)
{
	logicDome3->SetMaterial(materials->GetMaterial(mat));
}

void MyGeometry::SetMaterialDome4(G4String mat)
{
	logicDome4->SetMaterial(materials->GetMaterial(mat));
}


void MyGeometry::DefineCommands()
{
	fMessenger = new G4GenericMessenger(this, "/SIM/scoring/","Set scoring phantom");
	auto& phantomCmd = fMessenger->DeclareMethod("phantom",&MyGeometry::SetHumanPhantom,"Folder for results");
	phantomCmd.SetParameterName("phantom", true);
	phantomCmd.SetDefaultValue("IcruSphere");
	
	fMessenger = new G4GenericMessenger(this, "/SIM/geometry/dome1/","Set dome 1 material");
	auto& matdome1Cmd = fMessenger->DeclareMethod("material",&MyGeometry::SetMaterialDome1,"set  material dome 1");
	matdome1Cmd.SetParameterName("material", true);
	matdome1Cmd.SetDefaultValue("G4_Galactic");
	
	fMessenger = new G4GenericMessenger(this, "/SIM/geometry/dome2/","Set dome 2 material");
	auto& matdome2Cmd = fMessenger->DeclareMethod("material",&MyGeometry::SetMaterialDome2,"set  material dome 2");
	matdome2Cmd.SetParameterName("material", true);
	matdome2Cmd.SetDefaultValue("G4_Galactic");	

	fMessenger = new G4GenericMessenger(this, "/SIM/geometry/dome3/","Set dome 3 material");
	auto& matdome3Cmd = fMessenger->DeclareMethod("material",&MyGeometry::SetMaterialDome3,"set  material dome 3");
	matdome3Cmd.SetParameterName("material", true);
	matdome3Cmd.SetDefaultValue("G4_Galactic");
	
	fMessenger = new G4GenericMessenger(this, "/SIM/geometry/dome4/","Set dome 4 material");
	auto& matdome4Cmd = fMessenger->DeclareMethod("material",&MyGeometry::SetMaterialDome4,"set  material dome 4");
	matdome4Cmd.SetParameterName("material", true);
	matdome4Cmd.SetDefaultValue("G4_Galactic");

	fMessenger = new G4GenericMessenger(this, "/SIM/geometry/domeIn/","Set Dims inner dome radius");
	auto& dimindomeCmd = fMessenger->DeclareMethod("radius",&MyGeometry::SetInnerRadius,"set inner dome radius");
	dimindomeCmd.SetParameterName("radius", true);
	dimindomeCmd.SetDefaultValue("400");

	fMessenger = new G4GenericMessenger(this, "/SIM/geometry/dome1/","Set Dims inner dome radius");
	auto& thickdome1Cmd = fMessenger->DeclareMethod("thick",&MyGeometry::SetThickDome1,"set dome 1 thickness");
	thickdome1Cmd.SetParameterName("thick", true);
	thickdome1Cmd.SetDefaultValue("4");

	fMessenger = new G4GenericMessenger(this, "/SIM/geometry/dome2/","Set Dims inner dome radius");
	auto& thickdome2Cmd = fMessenger->DeclareMethod("thick",&MyGeometry::SetThickDome2,"set dome 2 thickness");
	thickdome2Cmd.SetParameterName("thick", true);
	thickdome2Cmd.SetDefaultValue("10");

	fMessenger = new G4GenericMessenger(this, "/SIM/geometry/dome3/","Set Dims inner dome radius");
	auto& thickdome3Cmd = fMessenger->DeclareMethod("thick",&MyGeometry::SetThickDome3,"set dome 3 thickness");
	thickdome3Cmd.SetParameterName("thick", true);
	thickdome3Cmd.SetDefaultValue("4");

	fMessenger = new G4GenericMessenger(this, "/SIM/geometry/dome4/","Set Dims inner dome radius");
	auto& thickdome4Cmd = fMessenger->DeclareMethod("thick",&MyGeometry::SetThickDome4,"set dome 4 thickness");
	thickdome4Cmd.SetParameterName("thick", true);
	thickdome4Cmd.SetDefaultValue("310");


}

