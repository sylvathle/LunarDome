#include <iostream>

//#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

//#include "QBBC_HP.hh"
#include "QBBC.hh"
#include "QGSP_BERT.hh"

#include "geometry.hh"
//#include "sphere_geometry.hh"
#include "physics.hh"
#include "action.hh"

#include "shieldmaterial.hh"

int main(int argc, char** argv)
{
	time_t start,end;

	time(&start);

	//#ifdef G4MULTITHREADED
	//	G4MTRunManager *runManager = new G4MTRunManager();
	//	G4cout << "Multithresaded" << G4endl;
	//#else
		G4RunManager *runManager = new G4RunManager();
	//	G4cout << "One core" << G4endl;
	//#endif

	runManager->SetVerboseLevel(0);
	G4UIExecutive *ui = nullptr;

	//G4String *shieldmaterial= new G4String("G4_Galactic");
	//G4String *shieldmaterialname= new G4String("G4_Al");
	//ShieldMaterial* shieldmaterial = new ShieldMaterial(shieldmaterialname);
	//G4cout << "argc " << argc << G4endl;
	//if (argc==3)
	//{
	//	
	//	shieldmaterialname= new G4String(argv[2]);
	//	shieldmaterial=new ShieldMaterial(shieldmaterialname);
	//	G4cout << shieldmaterial << G4endl;
	//}

	if (argc<=1) {ui = new G4UIExecutive(argc,argv);}

	//G4String phantomType = "ICRP145"; //"ICRP145" "IcruSphere"
	//phantomType = "IcruSphere"; //"ICRP145" "IcruSphere"


	//runManager->SetUserInitialization(new MyGeometry());

	runManager->SetUserInitialization(new MyGeometry(ui));
	//runManager->SetUserInitialization(new MyPhysicsList());
	runManager->SetUserInitialization(new QBBC(0));
	runManager->SetUserInitialization(new MyActionInitialization());

	//G4VModularPhysicsList* physics = new QGSP_BERT();
	//G4VModularPhysicsList* physics = new QBBC();
    	//physics->RegisterPhysics(new G4HadronPhysicsQGSP_BIC_HP());
	//runManager->SetUserInitialization(physics);
	

	const MyGeometry *detectorConstruction = static_cast<const MyGeometry*> (runManager->GetUserDetectorConstruction());
	G4String phantomType = detectorConstruction->GetPhantomType();
	
	G4UImanager *UImanager = G4UImanager::GetUIpointer();

	

	if ( ! ui)
	{
		G4String command = "/control/execute ";
		G4String fileName = argv[1];
		UImanager->ApplyCommand(command+fileName);

	}
	else
	{
		G4VisManager *visManager = new G4VisExecutive;
		visManager->SetVerboseLevel(0);
		visManager->Initialize();
		//G4cout << G4String("/control/execute vis")+phantomType+G4String(".mac") << G4endl;
		UImanager->ApplyCommand(G4String("/control/execute vis")+phantomType+G4String(".mac"));
		ui->SessionStart();
		delete visManager;
	}

	delete runManager;
	delete ui;

	time(&end);


	double time_taken = double(end-start);

	//auto man = G4AnalysisManager::Instance();

	// Create an output file stream object
	//std::ofstream outFile("../time/runningTime.txt",std::ios::app);

	// Check if the file is successfully opened
	//if (!outFile) {
	//	std::cerr << "Error: Unable to open file for writing!" << std::endl;
	//	return 1; // Exit with an error code
	//}

	// Write the value of t to the file
	//outFile << argv[1] << ","<< time_taken << std::endl;

	// Close the file
	//outFile.close();


}
