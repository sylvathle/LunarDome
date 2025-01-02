#include "physics.hh"

MyPhysicsList::MyPhysicsList()
{
	RegisterPhysics(new G4EmStandardPhysics());
	//RegisterPhysics(new G4OpticalPhysics());
	RegisterPhysics(new G4HadronElasticPhysics());
	RegisterPhysics(new G4EmExtraPhysics());
	RegisterPhysics(new G4HadronPhysicsFTFP_BERT());
	RegisterPhysics(new G4HadronPhysicsQGSP_BIC_HP());
	//RegisterPhysics(new G4HadronInelasticQBBC());
	
	
	//G4PhysListFactory *physListFactory = new G4PhysListFactory(); 
	//const std::vector<G4String> v = physListFactory->AvailablePhysLists(); 
	//G4cout << v << G4endl; 
}

MyPhysicsList::~MyPhysicsList()
{}
