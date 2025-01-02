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
// TETPSEnergyDeposit.cc
//
// Author: Haegin Han
// Reference: ICRP Publication 145. Ann. ICRP 49(3), 2020.
// Geant4 Contributors: J. Allison and S. Guatelli
//

#include "TETPSEnergyDeposit.hh"


TETPSEnergyDeposit::TETPSEnergyDeposit(G4String name, TETModelImport* _tetData)
  :G4PSEnergyDeposit(name), fTetData(_tetData),HCID(-1)
{
	//const MyPrimaryGenerator *generator = static_cast<const MyPrimaryGenerator*>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
	
	//particleList = generator->GetParticleList();
}

G4int TETPSEnergyDeposit::GetIndex(G4Step* aStep)
{
  // return the organ ID (= material index)
  G4int copyNo = aStep->GetPreStepPoint()->GetTouchable()->GetCopyNumber();
  return fTetData->GetMaterialIndex(copyNo);
}


void TETPSEnergyDeposit::Initialize(G4HCofThisEvent* HCE)
{
	EDEMap = new G4THitsMap<HitInfo>(GetMultiFunctionalDetector()->GetName(), 				GetName());
	//DoseMap = new G4THitsMap<G4double>(GetMultiFunctionalDetector()->GetName(), 				GetName());
	if(HCID < 0) {HCID = GetCollectionID(0);}
	
	HCE->AddHitsCollection(HCID, (G4VHitsCollection*)EDEMap);
}

void TETPSEnergyDeposit::EndOfEvent(G4HCofThisEvent*)
{;}

void TETPSEnergyDeposit::clear()
{
	EDEMap->clear();
	//DoseMap->clear();
}

G4bool TETPSEnergyDeposit::ProcessHits(G4Step* aStep,G4TouchableHistory *history)
{
	if (aStep->GetTrack()->GetParticleDefinition()->GetPDGCharge()==0) return FALSE;
	G4double total_edep = aStep->GetTotalEnergyDeposit();

	if ( total_edep == 0. ) return FALSE;

	G4double nonion_edep=aStep->GetNonIonizingEnergyDeposit();
	G4double dl=aStep->GetDeltaPosition().mag();
	G4double ion_edep=total_edep-nonion_edep; //Ionizing energy deposited
	
	G4double let = ion_edep/dl;
	G4double HT = QualityFactor(let)*ion_edep;

	G4int index = GetIndex(aStep);
	//G4cout << index << "  Q = " <<  QualityFactor(let) << " HT = " << HT << G4endl;
	//edep *= 2.0;
	HitInfo hit(&HT,&ion_edep);
	EDEMap->add(index,hit);  
	//DoseMap->add(index,ion_edep);  
	//G4cout << "AQUI" << G4endl;
	return TRUE;
}
