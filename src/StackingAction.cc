#include "StackingAction.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StackingAction::StackingAction()
 : G4UserStackingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StackingAction::~StackingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ClassificationOfNewTrack 
StackingAction::ClassifyNewTrack(const G4Track* )
{
  G4ClassificationOfNewTrack     classification = fUrgent;
  
  // Only track protons and neutrons
  //G4int pdg = aTrack->GetDefinition()->GetPDGEncoding();
  //if( pdg != 2112 && pdg != 2212  ) classification = fKill;
  
  return classification;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
