#include "run.hh"
#include "ions.hh"

MyRunAction::MyRunAction(): fRun(nullptr), fNumOfEvent(0), fRunID(0)
{
	
	//organsGrouped = GetOrgansGroup("organsInfo.csv");
	//std::map<G4int, G4int> mapGroupedOrgans = generateNumberedMap(organsGrouped);
	
	std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::tm* timeInfo = std::localtime(&currentTime);
	std::stringstream ss;
	ss << std::put_time(timeInfo, "%Y%m%d-%H%M%S");
        labelCSV = G4String(ss.str());

        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

        G4String rd_label = G4String(std::to_string(nanoseconds.count()%100000000));
        labelCSV = labelCSV + G4String("-") + rd_label;

	DefineCommands();

}

MyRunAction::~MyRunAction()
{
	delete fMessenger;
}


G4Run* MyRunAction::GenerateRun()
{
	const MyGeometry *detectorConstruction = static_cast<const MyGeometry*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	
	phantomType = detectorConstruction->GetPhantomType();
	//SetResultsDirectory("results/"+phantomType+G4String("/"));
	G4cout << "GenerateRun " << phantomType << G4endl;
	if (phantomType=="ICRP145") 
	{	
		fRun = new TETRun();
		return fRun;
	}
	else
	{
		iRun = new ISRun();
		return iRun;
	}
}


void MyRunAction::UpdateFlux(G4String primaryName, G4double secondarykE, G4String secondaryName)
{
	flux[primaryName].Update(secondarykE, secondaryName);
}

void MyRunAction::IterIncident(G4String primaryName)
{
	if (flux.count(primaryName)==0)
	{
		//G4String csvSource = this->GetCSVFluxName(primaryName);
		flux[primaryName].FirstUse();
	}
	flux[primaryName].IterIncident();
}

// Prepare arrays to store the data
void MyRunAction::BeginOfRunAction(const G4Run* aRun)
{
	auto man = G4AnalysisManager::Instance();
	
	const MyGeometry *detectorConstruction = static_cast<const MyGeometry*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());

        G4String rd_label = G4String(std::to_string(CLHEP::RandFlat::shootInt(1e9)));
	labelCSV = labelCSV + G4String("-") + rd_label;
	
	if (phantomType=="ICRP145") 
	{
		fTetData = detectorConstruction->GetICRP145phantom();
		auto massMap = fTetData->GetMassMap();
	}
	const MyPrimaryGenerator *generator = static_cast<const MyPrimaryGenerator*>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
	std::vector<G4String> particleList;

	for (G4int i=0;i<generator->GetNIons();i++) 
	{
		particleList.push_back(generator->GetIonName(i));
	}

	man->CreateNtuple("NParticles","NParticles");
	for (int i=0;i<particleList.size();i++)
	{
		man->CreateNtupleIColumn(particleList[i]);
	}
	man->FinishNtuple(0);


	man->CreateNtuple("Doses","Doses");
	man->CreateNtupleIColumn("idEvent");
	man->CreateNtupleIColumn("organId");
	for (int i=0;i<particleList.size();i++)
	{
		man->CreateNtupleDColumn(particleList[i]+G4String("_EDE"));
		man->CreateNtupleDColumn(particleList[i]+G4String("_Dose"));
	}
	man->FinishNtuple(1);

	
	man->CreateNtuple("TotalBody","TotalBody");
	man->CreateNtupleDColumn("EDE");
	man->FinishNtuple(2);

	man->CreateNtuple("InnerFlux","InnerFlux");
	man->CreateNtupleSColumn("Iparticle");
	man->CreateNtupleSColumn("Oparticle");
	man->CreateNtupleDColumn("okE");
	man->CreateNtupleDColumn("count");
	man->FinishNtuple(3);

	man->CreateNtuple("weightParticle","weightParticle");
	man->CreateNtupleSColumn("Iparticle");
	man->CreateNtupleIColumn("Z");
	man->CreateNtupleDColumn("wP");
	man->FinishNtuple(4);

	
	if (phantomType=="ICRP145") {
		// print the progress at the interval of 10%
		fNumOfEvent=aRun->GetNumberOfEventToBeProcessed();
		G4RunManager::GetRunManager()->SetPrintProgress(int(fNumOfEvent*0.1));
	}
	
}

void MyRunAction::EndOfRunAction(const G4Run* aRun)
{

	const MyPrimaryGenerator *generator = static_cast<const MyPrimaryGenerator*>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
	
	auto man = G4AnalysisManager::Instance();

	std::map<G4String, TotalFlux>::iterator itF;

	std::map<G4String,Ion> Ions = getIons();
	
	
	// Record fluxes	
	for (itF = flux.begin(); itF != flux.end(); itF++)
	{
		G4String iParticle = itF->first;
		G4String oParticle;
		G4int iZ = Ions[iParticle].getZ();

		man->FillNtupleSColumn(4,0,iParticle);
		man->FillNtupleIColumn(4,1,iZ);
		man->FillNtupleDColumn(4,2,generator->GetTotalParticleNumber(iZ-1));
		man->AddNtupleRow(4);

		std::map<G4String,ParticleSpectra> ofluxes = itF->second.GetFluxes();
		std::map<G4String,ParticleSpectra>::iterator itoF;

		for (itoF = ofluxes.begin(); itoF != ofluxes.end();itoF++)
		{
			oParticle = itoF->first;
			for (G4int oebin=0; oebin<itoF->second.GetNbin();oebin++)
			{
				man->FillNtupleSColumn(3,0,iParticle);
				man->FillNtupleSColumn(3,1,oParticle);
				man->FillNtupleDColumn(3,2,oebin);
				man->FillNtupleDColumn(3,3,itoF->second.GetBin(oebin));
				man->AddNtupleRow(3);
			}
		}

	}



	man->Write();
	man->CloseFile();

 	// print the result only in the Master
 	if(!isMaster) return;

 	// get the run ID
 	fRunID = aRun->GetRunID();

}

void MyRunAction::SetResultsDirectory(G4String dir)
{
	const MyGeometry *detectorConstruction = static_cast<const MyGeometry*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	
	phantomType = detectorConstruction->GetPhantomType();

	auto man = G4AnalysisManager::Instance();
	G4cout << "Set Dir Phantom " << phantomType << " " << dir << G4endl;
	result_dir = "../results/"+phantomType+G4String("/")+dir+G4String("/");
	
	createDirectories(result_dir);
	
	man->SetDefaultFileType("csv");
	man->OpenFile(result_dir+labelCSV+ G4String(".csv"));
}

void MyRunAction::DefineCommands()
{
	fMessenger = new G4GenericMessenger(this, "/SIM/scoring/","Set scoring folder");

	auto& resdirCmd = fMessenger->DeclareMethod("resDir",&MyRunAction::SetResultsDirectory,"Folder for results");
	resdirCmd.SetParameterName("resDir", true);
	resdirCmd.SetDefaultValue("../results/"+phantomType+G4String("/"));
}


void MyRunAction::PrintResult(std::ostream &out)
{

if (phantomType!="ICRP145") {return;}
 // Print run result
 //
 using namespace std;
 EMAP edepMap = *fRun->GetEDEMap();

    out << G4endl
	 << "=====================================================================" << G4endl
	 << " Run #" << fRunID << " / Number of event processed : "<< fNumOfEvent    << G4endl
	 << "=====================================================================" << G4endl
	 << "organ ID| "
	 << setw(19) << "Organ Mass (g)"
         << setw(19) << "Dose (Gy/source)"
	 //<< setw(19) << "Relative Error" 
	 << setw(34) << "Name of organ" << G4endl;

    out.precision(3);
    auto massMap = fTetData->GetMassMap();
    for(auto itr : massMap){
		G4double meanDose    = edepMap[itr.first]  / itr.second / fNumOfEvent;
		//G4double squareDose =  (edepMap[itr.first].second)/ (itr.second*itr.second);
		//G4double variance    = ((squareDose/fNumOfEvent) - (meanDose*meanDose))/fNumOfEvent;
		G4double relativeE(1);
		//if(meanDose > 0) relativeE   = sqrt(variance)/meanDose;

		out << setw(8)  << itr.first << "| "
			<< setw(19) << fixed      << itr.second/g;
		out	<< setw(19) << scientific << meanDose/(joule/kg);
		//out	<< setw(19) << fixed      << relativeE ; 
		out	<< setw(34) << fTetData->GetMaterial(itr.first)->GetName() << G4endl;
	}
	out << "=====================================================================" << G4endl << G4endl;
}

void createDirectories(const std::string& path) {
    std::string currentPath = "";
    for (char c : path) {
        if (c != '/') {
            currentPath += c;
        } else {
            currentPath += '/';
            if (!std::filesystem::exists(currentPath)) {
                std::filesystem::create_directory(currentPath);
            }
        }
    }
    if (!std::filesystem::exists(currentPath)) {
        std::filesystem::create_directory(currentPath);
    }
}
