#include "PhysicsList.hh"
#include "PhysicsListMessenger.hh"

#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4PhysListFactory.hh"
#include "G4VPhysicsConstructor.hh"

#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsFTFP_BERT_HP.hh"
#include "G4HadronPhysicsShielding.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmExtraPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4IonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4ShortLivedConstructor.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4LossTableManager.hh"
#include "G4UnitsTable.hh"
#include "G4ProcessManager.hh"
#include "G4IonFluctuations.hh"
#include "G4IonParametrisedLossModel.hh"
#include "G4EmParameters.hh"
#include "G4ParallelWorldPhysics.hh"
#include "G4AutoDelete.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList:: PhysicsList() : G4VModularPhysicsList()
{
    G4LossTableManager::Instance();
    defaultCutValue = 0.05*mm;
    fCutForGamma     = defaultCutValue;
    fCutForElectron  = defaultCutValue;
    fCutForPositron  = defaultCutValue;
    fCutForProton    = defaultCutValue;
    
    fMessenger = new  PhysicsListMessenger(this);
    
    SetVerboseLevel(1);
    
    
    // ******     Definition of defaults for the physics processes *****
    // ******     in case no physics is called by the macro file   *****
    //
    // Based on Zacharatou and Paganetti IEEE.
    
    // Elecromagnetic physics
    //
    fEmPhysicsList = new G4EmStandardPhysics_option3();
    fEmName = G4String("emstandard_opt3");
    
    // Hadronic physics
    //
    fProtonPhys.push_back( new G4HadronPhysicsQGSP_BIC_HP());
    fProtonPhys.push_back( new G4DecayPhysics());
    fProtonPhys.push_back( new G4IonBinaryCascadePhysics());
    fProtonPhys.push_back( new G4HadronElasticPhysicsHP());
    fProtonPhys.push_back( new G4StoppingPhysics());
    
    // Decay physics
    fDecay_List = new G4DecayPhysics();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::~ PhysicsList()
{
    delete fMessenger;
    delete fEmPhysicsList;
    delete fDecay_List;
    fProtonPhys.clear();
    
    for(size_t i=0; i<fProtonPhys.size(); i++) {
        delete fProtonPhys[i];
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void  PhysicsList::ConstructParticle()
{
    fDecay_List -> ConstructParticle();

  G4LeptonConstructor lepton;
  lepton.ConstructParticle();

  G4BosonConstructor boson;
  boson.ConstructParticle();

  G4MesonConstructor meson;
  meson.ConstructParticle();

  G4BaryonConstructor baryon;
  baryon.ConstructParticle();

  G4ShortLivedConstructor shortLived;
  shortLived.ConstructParticle();

  G4IonConstructor ion;
  ion.ConstructParticle();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void  PhysicsList::ConstructProcess()
{
    // Transportation
    //
    AddTransportation();
    
    // Electromagnetic physics
    //
    fEmPhysicsList -> ConstructProcess();
    fEmConf.AddModels();
    
    // Hadronic physics
    //
    for(size_t i=0; i < fProtonPhys.size(); i++)
    {
        fProtonPhys[i] -> ConstructProcess();
    }
    
    return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void  PhysicsList::AddPhysicsList(const G4String& name)
{
    if (verboseLevel>1) {
        G4cout << "PhysicsList::AddPhysicsList: <" << name << ">" << G4endl;
    }
    if (name == fEmName) return;
    
    if (name == "standard_opt4") {
        fEmName = name;
        delete fEmPhysicsList;
        fProtonPhys.clear();
        fEmPhysicsList = new G4EmStandardPhysics_option4();
        G4RunManager::GetRunManager() -> PhysicsHasBeenModified();
        
    } else if (name == "standard_opt3") {
        fEmName = name;
        delete fEmPhysicsList;
        fProtonPhys.clear();
        fEmPhysicsList = new G4EmStandardPhysics_option3();
        G4RunManager::GetRunManager() -> PhysicsHasBeenModified();
        
    } else if (name == "proton_BERT_em_standard_opt4") {
        // QGSP_BIC_HP + EmStandard_opt4
        
        AddPhysicsList("standard_opt4");
        fProtonPhys.push_back( new G4HadronPhysicsQGSP_BERT_HP());
        fProtonPhys.push_back( new G4DecayPhysics());
        fProtonPhys.push_back( new G4IonBinaryCascadePhysics());
        fProtonPhys.push_back( new G4HadronElasticPhysicsHP());
        fProtonPhys.push_back( new G4StoppingPhysics());
        fProtonPhys.push_back( new G4RadioactiveDecayPhysics());
        
        G4cout << "proton_BERT_em_standard_opt4 PHYSICS LIST has been activated" << G4endl;
        
     } else if (name == "proton_BIC_em_standard_opt4") {
        // QGSP_BIC_HP + EmStandard_opt4
        
        AddPhysicsList("standard_opt4");
        fProtonPhys.push_back( new G4HadronPhysicsQGSP_BIC_HP());
        //fProtonPhys.push_back( new G4DecayPhysics());
        fProtonPhys.push_back( new G4IonBinaryCascadePhysics());
        fProtonPhys.push_back( new G4HadronElasticPhysicsHP());
        fProtonPhys.push_back( new G4StoppingPhysics());
        //fProtonPhys.push_back( new G4RadioactiveDecayPhysics());
        
        G4cout << "proton_BIC_em_standard_opt4 PHYSICS LIST has been activated" << G4endl;
        
    } else if (name == "proton_BIC_em_standard_opt3") {
        // QGSP_BIC_HP + EmStandard_opt3
        
        AddPhysicsList("standard_opt3");
        fProtonPhys.push_back( new G4HadronPhysicsQGSP_BIC_HP());
        fProtonPhys.push_back( new G4DecayPhysics());
        fProtonPhys.push_back( new G4IonBinaryCascadePhysics());
        fProtonPhys.push_back( new G4HadronElasticPhysicsHP());
        fProtonPhys.push_back( new G4StoppingPhysics());
        fProtonPhys.push_back( new G4RadioactiveDecayPhysics());
 
        G4cout << "proton_BIC_em_standard_opt3 PHYSICS LIST has been activated" << G4endl;

    } else if (name == "proton_BERT_em_standard_opt3") {
        // QGSP_BERT_HP + EmStandard_opt3
        
        AddPhysicsList("standard_opt3");
        fProtonPhys.push_back( new G4HadronPhysicsQGSP_BERT_HP());
        fProtonPhys.push_back( new G4DecayPhysics());
        fProtonPhys.push_back( new G4IonBinaryCascadePhysics());
        fProtonPhys.push_back( new G4HadronElasticPhysicsHP());
        fProtonPhys.push_back( new G4StoppingPhysics());
        fProtonPhys.push_back( new G4RadioactiveDecayPhysics());
        
        G4cout << "proton_BERT_em_standard_opt3 PHYSICS LIST has been activated" << G4endl;
    
    } else if (name == "proton_FTFPBERT_em_standard_opt4") {
        // QGSP_BERT_HP + EmStandard_opt3

        AddPhysicsList("standard_opt4");
        fProtonPhys.push_back( new G4HadronPhysicsFTFP_BERT_HP());
        fProtonPhys.push_back( new G4DecayPhysics());
        fProtonPhys.push_back( new G4IonBinaryCascadePhysics());
        fProtonPhys.push_back( new G4HadronElasticPhysicsHP());
        fProtonPhys.push_back( new G4StoppingPhysics());
        fProtonPhys.push_back( new G4RadioactiveDecayPhysics());

        G4cout << "proton_FTFPBERT_em_standard_opt4 PHYSICS LIST has been activated" << G4endl;

    } else if (name == "proton_Shielding_em_standard_opt4") {
        // QGSP_BERT_HP + EmStandard_opt3

        AddPhysicsList("standard_opt4");
        fProtonPhys.push_back( new G4HadronPhysicsShielding());
        //fProtonPhys.push_back( new G4DecayPhysics());
        fProtonPhys.push_back( new G4IonBinaryCascadePhysics());
        fProtonPhys.push_back( new G4HadronElasticPhysicsHP());
        fProtonPhys.push_back( new G4StoppingPhysics());
        //fProtonPhys.push_back( new G4RadioactiveDecayPhysics());

        G4cout << "proton_Shielding_em_standard_opt4 PHYSICS LIST has been activated" << G4endl;
 
    } else if (name == "hadron_em_standard_opt4") {
        // A'la hadrontherapy advanced example
        
        AddPhysicsList("standard_opt4");
        fProtonPhys.push_back( new G4DecayPhysics());
        fProtonPhys.push_back( new G4RadioactiveDecayPhysics());
        fProtonPhys.push_back( new G4IonBinaryCascadePhysics());
        fProtonPhys.push_back( new G4EmExtraPhysics());
        fProtonPhys.push_back( new G4HadronElasticPhysicsHP());
        fProtonPhys.push_back( new G4StoppingPhysics());
        fProtonPhys.push_back( new G4HadronPhysicsQGSP_BIC_HP());
        
        G4cout << "hadron_em_standard_opt4 PHYSICS LIST has been activated" << G4endl;
    } else {
        G4cout << "PhysicsList::AddPhysicsList: <" << name << ">"
        << " is not defined"
        << G4endl;
    }
    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void  PhysicsList::SetCuts()
{
    
    if (verboseLevel >0){
        G4cout << "PhysicsList::SetCuts:";
        G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length") << G4endl;
    }
    
    SetCutValue(fCutForGamma, "gamma");
    SetCutValue(fCutForElectron, "e-");
    SetCutValue(fCutForPositron, "e+");
    SetCutValue(fCutForProton, "proton");
   
/*    G4EmParameters* opt = G4EmParameters::Instance(); 
    //G4EmProcessOptions opt;
    opt->SetMinEnergy(100.*eV);
    opt->SetMaxEnergy(500.*MeV);
    opt->SetNumberOfBins(220);
    opt->SetFluo(false);
    opt->SetAuger(false);
    opt->SetPixe(false);*/
    
}

