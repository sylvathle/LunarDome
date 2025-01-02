#ifndef HitInfo_hh
#define HitInfo_hh

#include "G4SDManager.hh"
#include "generator.hh"

class HitInfo {
	private:
		G4double EDE; //Effective Dose Equivalent (mSv * mass)
		G4double Dose; //Dose (Gray * mass)
	
	public:
		HitInfo();
		HitInfo(G4double*, G4double*);
		~HitInfo();
		
		HitInfo& operator+=(const HitInfo& obj);
		
		G4double GetEDE() const {return EDE;}
		G4double GetDose() const {return Dose;}
};


G4double QualityFactor(G4double let);

#endif
