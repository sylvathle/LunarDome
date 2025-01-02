#ifndef EVENT_HH
#define EVENT_HH

#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "G4THitsMap.hh"
#include "G4AnalysisManager.hh"

#include "fluxInfo.hh"
#include "run.hh"
#include "generator.hh"

class MyEventAction : public G4UserEventAction
{
	public:
		MyEventAction(MyRunAction* myrun);
		//MyEventAction(MyPrimaryGenerator*);
		~MyEventAction();
		
		virtual void BeginOfEventAction(const G4Event*);
		virtual void EndOfEventAction(const G4Event*);

		//void AddEdep(G4double edep)
		//{
		//	fEdep += edep;
			//G4cout << " Add edep = " << edep << " | fPrimaryExp  " << pow(10,fPrimaryExp ) << G4endl;
		//}
		
		//G4double GetPrimaryEnergyExp() {return fPrimaryExp;}
		//MyPrimaryGenerator *gen;
		
		//void SetPrimaryParticleEnergy(G4double energy) {fPrimary=energy;}
	
	private:
		//G4double fEdep;
		MyRunAction* myRun;
		G4int totalSurfFluxID;

		//G4THitsMap<fluxInfo> totalSurfFlux;
};

#endif
