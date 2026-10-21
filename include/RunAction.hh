#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"

class Run;
class PrimaryGeneratorAction;
class HistoManager;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class RunAction : public G4UserRunAction
{
public:
    RunAction(DetectorConstruction*, PhysicsList*,PrimaryGeneratorAction*);
    ~RunAction();
    
public:
    virtual void   BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);
    virtual G4Run* GenerateRun();
    
private:
    PrimaryGeneratorAction* fPrimary;
    Run*                    fRun;
    HistoManager*	    fHistoManager;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

