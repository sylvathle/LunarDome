#include "event.hh"
#include <csignal>

MyEventAction::MyEventAction(MyRunAction* myrun)
{
	myRun = myrun;
}

MyEventAction::~MyEventAction()
{}


void MyEventAction::BeginOfEventAction(const G4Event*)
{
	totalSurfFluxID = 1;
}



void MyEventAction::EndOfEventAction(const G4Event* evt)
{
	G4String primaryName = evt->GetPrimaryVertex()->GetPrimary()->GetParticleDefinition()->GetParticleName();
	myRun->IterIncident(primaryName);
	G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
	G4THitsMap<fluxInfo>* eventTotalSurfFlux = (G4THitsMap<fluxInfo>*)(HCE->GetHC(totalSurfFluxID));
	std::map<G4int, fluxInfo*>::iterator it;
	for (it = eventTotalSurfFlux->GetMap()->begin(); it != eventTotalSurfFlux->GetMap()->end(); it++)
	{
		myRun->UpdateFlux(primaryName,it->second->kE,it->second->particleName);
		//G4cout << "event.cc  UpdateFLUX done" << G4endl;
	}
}
