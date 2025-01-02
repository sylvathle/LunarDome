#include "ions.hh"

Ion::Ion() 
{
	name = "H";
	Z = 1;
	A = 1;
}

Ion::Ion(G4String name_, G4int Z_, G4int A_)
{
	name = name_;
	Z = Z_;
	A = A_;
}

Ion::~Ion() {}


std::map<G4String,Ion> getIons()
{
	std::map<G4String,Ion> ions;
	ions["H"] = Ion("H",1,1); ions["proton"] = Ion("H",1,1);
	ions["He"] = Ion("He",2,4); ions["alpha"] = Ion("He",2,4);
	ions["Li"] = Ion("Li",3,7); ions["Li7"] = Ion("Li",3,7);
	ions["Be"] = Ion("Be",4,9); ions["Be9"] = Ion("Be",4,9);
	ions["B"] = Ion("B",5,11); ions["B11"] = Ion("B",5,11);
	ions["C"] = Ion("C",6,12); ions["C12"] = Ion("C",6,12);
	ions["N"] = Ion("N",7,14); ions["N14"] = Ion("N",7,14);
	ions["O"] = Ion("O",8,16); ions["O16"] = Ion("O",8,16);
	ions["F"] = Ion("F",9,19); ions["F19"] = Ion("F",9,19);
	ions["Ne"] = Ion("Ne",10,21); ions["Ne21"] = Ion("Ne",10,21);
	ions["Na"] = Ion("Na",11,23); ions["Na23"] = Ion("Na",11,23);
	ions["Mg"] = Ion("Mg",12,24); ions["Mg24"] = Ion("Mg",12,24);
	ions["Al"] = Ion("Al",13,27); ions["Al27"] = Ion("Al",13,27);
	ions["Si"] = Ion("Si",14,28); ions["Si28"] = Ion("Si",14,28);
	ions["P"] = Ion("P",15,30); ions["P30"] = Ion("P",15,30);
	ions["S"] = Ion("S",16,32); ions["S32"] = Ion("S",16,32);
	ions["Cl"] = Ion("Cl",17,35); ions["Cl35"] = Ion("Cl",17,35);
	ions["Ar"] = Ion("Ar",18,40); ions["Ar40"] = Ion("Ar",18,40);
	ions["K"] = Ion("K",19,39); ions["K39"] = Ion("K",19,39);
	ions["Ca"] = Ion("Ca",20,40); ions["Ca40"] = Ion("Ca",20,40);
	ions["Sc"] = Ion("Sc",21,45); ions["Sc45"] = Ion("Sc",21,45);
	ions["Ti"] = Ion("Ti",22,48); ions["Ti48"] = Ion("Ti",22,48);
	ions["V"] = Ion("V",23,51); ions["V51"] = Ion("V",23,51);
	ions["Cr"] = Ion("Cr",24,52); ions["Cr52"] = Ion("Cr",24,52);
	ions["Mn"] = Ion("Mn",25,55); ions["Mn55"] = Ion("Mn",25,55);
	ions["Fe"] = Ion("Fe",26,56); ions["Fe56"] = Ion("Fe",26,56);
	ions["Co"] = Ion("Co",27,59); ions["Co59"] = Ion("Co",27,59);
	ions["Ni"] = Ion("Ni",28,59); ions["Ni59"] = Ion("Ni",28,59);
	return ions;

}
