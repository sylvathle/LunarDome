//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// TETRun.cc
//
// Author: Haegin Han - Adapted by Sylvain Blunier
// Reference: ICRP Publication 145. Ann. ICRP 49(3), 2020.
// Geant4 Contributors: J. Allison and S. Guatelli
//

#include "TETRun.hh"


TETRun::TETRun()
:G4Run()
{
	id_event=0;
	id_entry=0;

	const MyGeometry *detectorConstruction = static_cast<const MyGeometry*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	fTetData = detectorConstruction->GetICRP145phantom();
	massMap = fTetData->GetMassMap();
	
	N_organ= massMap.size();
	
	for (auto itr : massMap) 
	{
		massMap[itr.first]=massMap[itr.first]/kg;
	}
	wT = GetwT("../scripts/organsInfo.csv");
	
	const MyPrimaryGenerator *generator = static_cast<const MyPrimaryGenerator*>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
	mission_factor = generator->GetMissionFactor()/joule;
	n_event_record = generator->GetSampleSize();

	for (G4int i=0;i<generator->GetNIons();i++)
	{
		GCRParticleWeights.push_back(mission_factor*generator->GetTotalParticleNumber(i));
	}
	for (G4int Z=1;Z<=28;Z++) {Nparts.push_back(0);}

}

TETRun::~TETRun()
{
 	fEDEMap.clear();
 	fDoseMap.clear();
	Nparts.clear();
}

void TETRun::RecordEvent(const G4Event* event)
{
	// Hits collections
	++id_event;
	
	G4HCofThisEvent* HCE = event->GetHCofThisEvent();
	if(!HCE) return;

 	auto  EDE_id = G4SDManager::GetSDMpointer()->GetCollectionID("PhantomSD/EDE");
  	
  	G4int Z = (G4int)event->GetPrimaryVertex()->GetPrimary()->GetParticleDefinition()->GetPDGCharge();
	Nparts[Z-1]++;
  	
	auto* EDEMap = static_cast<G4THitsMap<HitInfo>*>(HCE->GetHC(EDE_id));

	// sum up the energy deposition and the square of it
	for (auto itr : *EDEMap->GetMap()) {
		G4int id = itr.first+Z*100000; 
		fEDEMap[id]  += itr.second->GetEDE(); //sum
		fDoseMap[id]  += itr.second->GetDose();  //sum
	}
	
	if (id_event==n_event_record)
	{
		G4AnalysisManager *man = G4AnalysisManager::Instance();
		for (G4int iZ=0;iZ<=27;iZ++){man->FillNtupleIColumn(0,iZ,Nparts[iZ]);}
		man->AddNtupleRow(0);
		G4int iter(1),id(0);
		G4double fac;
		G4double ede(0.0),HT(0.0),dose(0.0);

		for (auto itr : massMap) {
			man->FillNtupleIColumn(iter,0,id_entry);
			man->FillNtupleIColumn(iter,1,itr.first);
			for (G4int iZ=0;iZ<=27;iZ++)
			{
				id = itr.first+(iZ+1)*100000;
				if (Nparts[iZ]>0)
				{
					fac = GCRParticleWeights[iZ]/massMap[itr.first]/Nparts[iZ]*1e3;
					HT = fac*fEDEMap[id];
					dose = fac*fDoseMap[id];
				}
				else {HT=0; dose=0;}
				man->FillNtupleDColumn(iter,2*(iZ+1),HT);
				man->FillNtupleDColumn(iter,2*(iZ+1)+1,dose);
				
				ede += HT*wT[itr.first];
			}
			man->AddNtupleRow(1);
		}
		++iter;
		man->FillNtupleDColumn(iter,0,ede);
		man->AddNtupleRow(iter);	
		
		fEDEMap.clear();
		fDoseMap.clear();
		for (G4int iZ=0;iZ<=27;iZ++) {Nparts[iZ]=0;}
	
		id_event=0;
		++id_entry;
	}
}

void TETRun::Merge(const G4Run* run)
{
	 // merge the data from each thread
	 EMAP localMap = static_cast<const TETRun*>(run)->fEDEMap;
	 G4cout << "Merging " << G4endl;
	 for(auto itr : localMap){
		 fEDEMap[itr.first]  += itr.second;
		}

	G4Run::Merge(run);
}




