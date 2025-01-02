#ifndef IONS_H
#define IONS_H

#include "G4IonTable.hh"
#include <iostream>

class Ion{
	private:
		G4String name;
		G4int Z;
		G4int A;

	public:
		Ion();
		Ion(G4String name_, G4int Z_, G4int A_);
		~Ion();

		G4int getZ() const {return Z;}
		G4int getA() const {return A;}
	
	
};


std::map<G4String,Ion> getIons();

#endif
