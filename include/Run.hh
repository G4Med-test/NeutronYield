#ifndef Run_h
#define Run_h 1

#include "G4Run.hh"

class DetectorConstruction;
class G4ParticleDefinition;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class Run : public G4Run
{
public:
    Run();
    ~Run();
    
public:
    void SetPrimary(G4ParticleDefinition* particle, G4double energy);
    
    virtual void Merge(const G4Run*);
    void EndOfRun();
    
private:
    G4ParticleDefinition*  fParticle;
    G4double  fEkin;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

