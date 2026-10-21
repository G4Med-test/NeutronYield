#include "PhysicsListMessenger.hh"
#include "PhysicsList.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsListMessenger::PhysicsListMessenger(PhysicsList* pPhys)
:fPhysicsList(pPhys)
{
    fPhysDir = new G4UIdirectory("/NeutronYield/PhysicsList/");
    fPhysDir->SetGuidance("Commands to activate physics models and set cuts");
    
    fListCmd = new G4UIcmdWithAString("/NeutronYield/PhysicsList/AddPhysics",this);
    fListCmd->SetGuidance("Add physics list.");
    fListCmd->SetParameterName("PList",false);
    fListCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsListMessenger::~PhysicsListMessenger()
{
    delete fPhysDir;
    delete fListCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsListMessenger::SetNewValue(G4UIcommand* command,
                                       G4String newValue)
{
    if( command == fListCmd )
        fPhysicsList->AddPhysicsList(newValue);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
