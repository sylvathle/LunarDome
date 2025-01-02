#ifndef ReadCSV_hh
#define ReadCSV_hh

#include "G4SDManager.hh"

std::map<G4int, G4double> GetwT(const G4String& filename);
std::map<G4int, G4String> GetOrgansGroup(const G4String& filename);
std::map<G4int, G4int> generateNumberedMap(const std::map<G4int, G4String>& organs);
G4int GetNGroups(std::map<G4int, G4int>);

#endif
