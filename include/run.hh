#ifndef RUN_HH
#define RUN_HH

#include "G4UserRunAction.hh"
//#include "MyRun.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"


#include "shieldmaterial.hh"
#include "geometry.hh"
#include "generator.hh"
#include "ions.hh"

#include "TotalFlux.hh"

#include "ISRun.hh"
#include "TETRun.hh"
#include "TETModelImport.hh"

//#include "Bins.hh"
#include <vector>
#include <map>
#include <sys/stat.h>
#include <filesystem>

class MyRunAction : public G4UserRunAction
{
	public :
		explicit MyRunAction();
		~MyRunAction();
		
		virtual G4Run* GenerateRun() override;
		virtual void BeginOfRunAction(const G4Run*) override;
		virtual void EndOfRunAction(const G4Run*) override;


		void PrintResult(std::ostream &out);
		void UpdateFlux(G4String primaryName, G4double secondarykE, G4String secondaryName);
		void IterIncident(G4String primaryName);
		G4String GetCSVFluxName(G4String iparticle);
		

	private:  
		TETModelImport* fTetData;
		TETRun*         fRun;
		ISRun*         iRun;
 		G4int           fNumOfEvent;
 		G4int           fRunID;
		G4String	phantomType;
		
		G4String result_dir,labelCSV;
		
		G4GenericMessenger *fMessenger;
		std::map <G4String, TotalFlux> flux;
		
		void DefineCommands();
		void SetResultsDirectory(G4String);
};

void createDirectories(const std::string& path);

#endif
