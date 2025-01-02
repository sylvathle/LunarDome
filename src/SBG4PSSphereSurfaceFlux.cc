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
//
//
// SBG4PSSphereSurfaceFlux
#include "SBG4PSSphereSurfaceFlux.hh"

#include "G4SystemOfUnits.hh"
#include "G4StepStatus.hh"
#include "G4Track.hh"
#include "G4VSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VPVParameterisation.hh"
#include "G4UnitsTable.hh"
#include "G4GeometryTolerance.hh"
////////////////////////////////////////////////////////////////////////////////
// (Description)
//   This is a primitive scorer class for scoring only Surface Flux.
//  Flux version assumes only for G4Sphere shape.
//
// Surface is defined  at the inside of sphere.
// Direction                  -Rmin   +Rmax
//   0  IN || OUT            ->|<-     |
//   1  IN                   ->|       |
//   2  OUT                    |<-     |
//
// Created: 2005-11-14  Tsukasa ASO, Akinori Kimura.
// 29-Mar-2007  T.Aso,  Bug fix for momentum direction at outgoing flux.
// 2010-07-22   Introduce Unit specification.
// 2010-07-22   Add weighted and divideByAre options
// 2011-02-21   Get correct momentum direction in Flux_Out.
// 2011-09-09   Modify comment in PrintAll().
// 2014-03-03  T.Aso,  To use always positive value for anglefactor.
///////////////////////////////////////////////////////////////////////////////

SBG4PSSphereSurfaceFlux::SBG4PSSphereSurfaceFlux(G4String name, G4int direction,
                                             G4int depth)
  : SBG4PSSphereSurfaceFlux(name, direction, "percm2", depth)
{}

SBG4PSSphereSurfaceFlux::SBG4PSSphereSurfaceFlux(G4String name, G4int direction,
                                             const G4String& unit, G4int depth)
  : G4VPrimitiveScorer(name, depth)
  , HCID(-1)
  , fDirection(direction)
  , EvtMap(nullptr)
  , weighted(true)
  , divideByArea(true)
{
  DefineUnitAndCategory();
  SetUnit(unit);
}

G4bool SBG4PSSphereSurfaceFlux::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  //G4cout << "Process Hits" << G4endl;
  G4StepPoint* preStep = aStep->GetPreStepPoint();

  G4VPhysicalVolume* physVol       = preStep->GetPhysicalVolume();
  G4VPVParameterisation* physParam = physVol->GetParameterisation();
  G4VSolid* solid                  = nullptr;



  if(physParam != nullptr)
  {  // for parameterized volume
    G4int idx =
      ((G4TouchableHistory*) (aStep->GetPreStepPoint()->GetTouchable()))
        ->GetReplicaNumber(indexDepth);
    solid = physParam->ComputeSolid(idx, physVol);
    solid->ComputeDimensions(physParam, idx, physVol);
  }
  else
  {  // for ordinary volume
    solid = physVol->GetLogicalVolume()->GetSolid();
  }

  auto  sphereSolid = (G4Sphere*) (solid);

  G4int dirFlag = IsSelectedSurface(aStep, sphereSolid);
  if(dirFlag > 0)
  {
    if(fDirection == fFlux_InOut || fDirection == dirFlag)
    {
      G4StepPoint* thisStep = nullptr;
      if(dirFlag == fFlux_In)
      {
	//G4cout << "fluxIN" << G4endl;
  	//G4cout << aStep->GetTrack()->GetParticleDefinition()->GetParticleName() << " " << preStep->GetKineticEnergy() << " " << preStep->GetPosition() << G4endl;
        thisStep = preStep;
      }
      else if(dirFlag == fFlux_Out)
      {
	//G4cout << "fluxOut" << G4endl;
  	//G4cout << aStep->GetTrack()->GetParticleDefinition()->GetParticleName() << " " << preStep->GetKineticEnergy() << " " << preStep->GetPosition() << G4endl;
        thisStep = aStep->GetPostStepPoint();
        //G4cout << "fFlux_out " << dirFlag << " " << aStep->GetTrack()->GetParticleDefinition()->GetParticleName() << " " << preStep->GetKineticEnergy() << " " << preStep->GetPosition() << G4endl;
      }
      else
      {
        //G4cout << "return " << dirFlag << " " << aStep->GetTrack()->GetParticleDefinition()->GetParticleName() << " " << preStep->GetKineticEnergy() << " " << preStep->GetPosition() << G4endl;
        return false;
      }

      if (thisStep->GetPosition()[2]<0) {return false;}
     
      //G4cout << aStep->GetTrack()->GetParticleDefinition()->GetParticleName() << " " << thisStep->GetKineticEnergy() << " " << thisStep->GetPosition() << " " << dirFlag << " " << fDirection << G4endl;

      /*G4TouchableHandle theTouchable = thisStep->GetTouchableHandle();
      G4ThreeVector pdirection       = thisStep->GetMomentS32 16531.8 (-381.973,135.314,292.894umDirection();
      G4ThreeVector localdir =
        theTouchable->GetHistory()->GetTopTransform().TransformAxis(pdirection);
      G4double localdirL2 = localdir.x() * localdir.x() +
                            localdir.y() * localdir.y() +
                            localdir.z() * localdir.z();
      G4ThreeVector stppos1 = aStep->GetPreStepPoint()->GetPosition();
      G4ThreeVector localpos1 =
        theTouchable->GetHistory()->GetTopTransform().TransformPoint(stppos1);
      G4double localR2 = localpos1.x() * localpos1.x() +
                         localpos1.y() * localpos1.y() +
                         localpos1.z() * localpos1.z();
      G4double anglefactor =
        (localdir.x() * localpos1.x() + localdir.y() * localpos1.y() +
         localdir.z() * localpos1.z()) /
        std::sqrt(localdirL2) / std::sqrt(localR2);*/

      //if(anglefactor < 0.0) anglefactor *= -1.0;

      fluxInfo flux = fluxInfo(aStep->GetTrack()->GetParticleDefinition()->GetParticleName(),preStep->GetKineticEnergy());
      
      //G4cout << aStep->GetTrack()->GetParticleDefinition()->GetParticleName()<< " " << preStep->GetKineticEnergy() << G4endl;

      //G4double current = 1.0 / anglefactor;
      //if(weighted)
      //  current *= thisStep->GetWeight();  // Flux (Particle Weight)
      //if(divideByArea)                     // Flux with angle.
      //{
      //  G4double radi = sphereSolid->GetInnerRadius();
      //  G4double dph  = sphereSolid->GetDeltaPhiAngle() / radian;
      //  G4double stth = sphereSolid->GetStartThetaAngle() / radian;
      //  G4double enth = stth + sphereSolid->GetDeltaThetaAngle() / radian;
      //  current /= radi * radi * dph * (-std::cos(enth) + std::cos(stth));
      //}
      index++;

      //G4int index = GetIndex(aStep);
      EvtMap->add(index, flux);
      //G4cout << "event ADDED " << G4endl;
    }
  }

  return true;
}

G4int SBG4PSSphereSurfaceFlux::IsSelectedSurface(G4Step* aStep,
                                               G4Sphere* sphereSolid)
{
  G4TouchableHandle theTouchable =
    aStep->GetPreStepPoint()->GetTouchableHandle();
  G4double kCarTolerance =
    G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();

  //G4cout << aStep->GetTrack()->GetParticleDefinition()->GetParticleName() << " " << aStep->GetPreStepPoint()->GetKineticEnergy() << G4endl;

  if(aStep->GetPreStepPoint()->GetStepStatus() == fGeomBoundary)
  {
    //G4cout << "---Pre " <<  sphereSolid->GetName() <<  G4endl;
    // Entering Geometry
    G4ThreeVector stppos1 = aStep->GetPreStepPoint()->GetPosition();
    G4ThreeVector localpos1 =
      theTouchable->GetHistory()->GetTopTransform().TransformPoint(stppos1);
    G4double localR2 = localpos1.x() * localpos1.x() +
                       localpos1.y() * localpos1.y() +
                       localpos1.z() * localpos1.z();
    // G4double InsideRadius2 =
    //  sphereSolid->GetInsideRadius()*sphereSolid->GetInsideRadius();
    // if(std::fabs( localR2 - InsideRadius2 ) < kCarTolerance ){
    G4double InsideRadius = sphereSolid->GetOuterRadius();
    //G4cout << "-- " << InsideRadius << G4endl;
    if(localR2 >
         (InsideRadius - kCarTolerance) * (InsideRadius - kCarTolerance) &&
       localR2 <
         (InsideRadius + kCarTolerance) * (InsideRadius + kCarTolerance))
    {
      return fFlux_In;
    }
  }

  /*if(aStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary)
  {
    //G4cout << "---Post " << sphereSolid->GetName() << G4endl;
    // Exiting Geometry
    G4ThreeVector stppos2 = aStep->GetPostStepPoint()->GetPosition();
    G4ThreeVector localpos2 =
      theTouchable->GetHistory()->GetTopTransform().TransformPoint(stppos2);
    G4double localR2 = localpos2.x() * localpos2.x() +
                       localpos2.y() * localpos2.y() +
                       localpos2.z() * localpos2.z();
    // G4double InsideRadius2 =
    //  sphereSolid->GetInsideRadius()*sphereSolid->GetInsideRadius();
    // if(std::facb(localR2 - InsideRadius2) ) < kCarTolerance ){
    G4double InsideRadius = sphereSolid->GetOuterRadius();
    //G4cout << "-- kCarTolerance " << kCarTolerance << "  Inside radius " << InsideRadius << " localR2 " << localR2 << G4endl;
    if(localR2 >
         (InsideRadius - kCarTolerance) * (InsideRadius - kCarTolerance) &&
       localR2 <
         (InsideRadius + kCarTolerance) * (InsideRadius + kCarTolerance))
    {
      return fFlux_Out;
    }
  }*/

  return -1;
}

void SBG4PSSphereSurfaceFlux::Initialize(G4HCofThisEvent* HCE)
{
  EvtMap = new G4THitsMap<fluxInfo>(detector->GetName(), GetName());
  index = 0;
  if(HCID < 0)
    HCID = GetCollectionID(0);
  HCE->AddHitsCollection(HCID, (G4VHitsCollection*) EvtMap);
}

void SBG4PSSphereSurfaceFlux::clear() { EvtMap->clear(); }

void SBG4PSSphereSurfaceFlux::PrintAll()
{
  G4cout << " MultiFunctionalDet  " << detector->GetName() << G4endl;
  G4cout << " PrimitiveScorer " << GetName() << G4endl;
  G4cout << " Number of entries " << EvtMap->entries() << G4endl;
  for(const auto& [copy, flux] : *(EvtMap->GetMap()))
  {
    G4cout << "  copy no.: " << copy
           << "  Flux  : " << (flux)->kE << " " << (flux)->particleName << " ["
           << GetUnit() << "]" << G4endl;
  }
}

void SBG4PSSphereSurfaceFlux::SetUnit(const G4String& unit)
{
  if(divideByArea)
  {
    CheckAndSetUnit(unit, "Per Unit Surface");
  }
  else
  {
    if(unit.empty())
    {
      unitName  = unit;
      unitValue = 1.0;
    }
    else
    {
      G4String msg = "Invalid unit [" + unit + "] (Current  unit is [" +
                     GetUnit() + "] ) for " + GetName();
      G4Exception("SBG4PSSphereSurfaceFlux::SetUnit", "DetPS0016", JustWarning,
                  msg);
    }
  }
}

void SBG4PSSphereSurfaceFlux::DefineUnitAndCategory()
{
  // Per Unit Surface
  new G4UnitDefinition("percentimeter2", "percm2", "Per Unit Surface",
                       (1. / cm2));
  new G4UnitDefinition("permillimeter2", "permm2", "Per Unit Surface",
                       (1. / mm2));
  new G4UnitDefinition("permeter2", "perm2", "Per Unit Surface", (1. / m2));
}
