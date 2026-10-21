#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "StackingAction.hh"
#include "SteppingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ActionInitialization::ActionInitialization(DetectorConstruction* detector,
                                           PhysicsList* physics)
: G4VUserActionInitialization(),
fDetector(detector),
fPhysics(physics)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ActionInitialization::~ActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::BuildForMaster() const
{
    RunAction* runAction = new RunAction(fDetector, fPhysics, 0);
    SetUserAction(runAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::Build() const
{
    PrimaryGeneratorAction* kinematics = new PrimaryGeneratorAction();
    SetUserAction(kinematics);
    
    SetUserAction(new RunAction(fDetector, fPhysics, kinematics));
        
    SetUserAction(new SteppingAction());

    SetUserAction(new StackingAction()); 
}

