#include "SteppingAction.hh"
#include "Run.hh"
#include "HistoManager.hh"

#include "G4SteppingManager.hh"
#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction()
:G4UserSteppingAction()
{
    fVolumeNames.push_back("Scorer_0.0_deg");
    fVolumeNames.push_back("Scorer_7.5_deg");
    fVolumeNames.push_back("Scorer_30.0_deg");
    fVolumeNames.push_back("Scorer_60.0_deg");
    fVolumeNames.push_back("Scorer_90.0_deg");
    fVolumeNames.push_back("Scorer_120.0_deg");
    fVolumeNames.push_back("Scorer_150.0_deg");
    fVolumeNames.push_back("Scorer_15.0_deg");
    fVolumeNames.push_back("Scorer_90.0_deg");
    fVolumeNames.push_back("Scorer_110.0_deg");
    fVolumeNames.push_back("Scorer_0_deg");
    fVolumeNames.push_back("Scorer_1_deg");
    fVolumeNames.push_back("Scorer_2_deg");
    fVolumeNames.push_back("Scorer_3_deg");
    fVolumeNames.push_back("Scorer_4_deg");
    fVolumeNames.push_back("Scorer_5_deg");
    fVolumeNames.push_back("Scorer_6_deg");
    fVolumeNames.push_back("Scorer_7_deg");
    fVolumeNames.push_back("Scorer_8_deg");
    fVolumeNames.push_back("Scorer_9_deg");
    fVolumeNames.push_back("Scorer_10_deg");
    fVolumeNames.push_back("Scorer_11_deg");
    fVolumeNames.push_back("Scorer_12_deg");

    fNbVolumes = G4int(fVolumeNames.size());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    G4StepPoint* stepPoint = step->GetPreStepPoint();
    
    if ( stepPoint->GetStepStatus() == fGeomBoundary) {
        G4int particle = step->GetTrack()->GetDefinition()->GetPDGEncoding();

        if ( particle == 2112 ) {

            G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
            const G4TouchableHandle touchable = stepPoint->GetTouchableHandle();
            
            G4String volume = touchable->GetVolume()->GetName();
            G4double ekin = stepPoint->GetKineticEnergy();
            for ( int i = 0; i < fNbVolumes; i++) {
                if ( volume == fVolumeNames[i] ) {
                    analysisManager->FillH1(i, ekin); 
                    step->GetTrack()->SetTrackStatus(fStopAndKill);
		    break;
                }
            }
        }
        
    } else {
        return;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


