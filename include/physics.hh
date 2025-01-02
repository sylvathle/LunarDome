#ifndef PHYSICS_HH
#define PHYSICS_HH

#include "G4VModularPhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4OpticalPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4EmExtraPhysics.hh"

#include "G4PhysListFactory.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"
#include "QBBC.hh"

class MyPhysicsList: public G4VModularPhysicsList
{
	public:
		MyPhysicsList();
		~MyPhysicsList();
};


#endif
