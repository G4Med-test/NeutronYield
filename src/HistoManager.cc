#include "HistoManager.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::HistoManager()
: fFileName("NeutronYield")
{
    Book();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::~HistoManager()
{
    delete G4AnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Book()
{
    // Create or get analysis manager
    // The choice of analysis technology is done via selection of a namespace
    // in HistoManager.hh
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetFileName(fFileName);
    analysisManager->SetActivation(true);
    analysisManager->SetVerboseLevel(1);
    
    // Define histograms start values*/
    const G4int nTuples = 23;
    
    const G4String name[nTuples] =
    {
        "Scorer_0.0_deg",
        "Scorer_7.5_deg",
        "Scorer_30.0_deg",
        "Scorer_60.0_deg",
        "Scorer_90.0_deg",
        "Scorer_120.0_deg",
        "Scorer_150.0_deg",
        "Scorer_15.0_deg",
        "Scorer_90.0_deg",
        "Scorer_110.0_deg",
        "Scorer_0_deg","Scorer_1_deg",
        "Scorer_2_deg","Scorer_3_deg",
        "Scorer_4_deg","Scorer_5_deg",
        "Scorer_6_deg","Scorer_7_deg",
        "Scorer_8_deg","Scorer_9_deg",
        "Scorer_10_deg","Scorer_11_deg",
        "Scorer_12_deg"
    };
    G4int ih; 
 
    for ( int i = 0; i < nTuples; i++ ) {
	 ih = analysisManager->CreateH1(name[i], name[i], 100, 0.4, 114.0);
	 analysisManager->SetH1Activation(ih, false);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
