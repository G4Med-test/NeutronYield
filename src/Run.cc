#include "Run.hh"

#include "HistoManager.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4Material.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Run::Run()
: G4Run(),
fParticle(0), fEkin(0.)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Run::~Run()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::SetPrimary (G4ParticleDefinition* particle, G4double energy)
{
    fParticle = particle;
    fEkin     = energy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::Merge(const G4Run* run)
{
    const Run* localRun = static_cast<const Run*>(run);
    
    // pass information about primary particle
    fParticle = localRun->fParticle;
    fEkin     = localRun->fEkin;
    
    // accumulate sums
    G4Run::Merge(run);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::EndOfRun()
{
    std::ios::fmtflags mode = G4cout.flags();
    G4cout.setf(std::ios::fixed,std::ios::floatfield);
    G4int prec = G4cout.precision(2);
    
    //run conditions
    //
    G4String partName = fParticle->GetParticleName();
    
    G4cout << "\n ======================== run summary =====================\n";
    G4cout
    << "\n The run is " << numberOfEvent << " "<< partName << " of "
    << G4BestUnit(fEkin,"Energy") << G4endl;
    
    if (numberOfEvent == 0) {
        G4cout.setf(mode,std::ios::floatfield);
        G4cout.precision(prec);
        return;
    }
    
    // reset default formats
    G4cout.setf(mode,std::ios::floatfield);
    G4cout.precision(prec);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
