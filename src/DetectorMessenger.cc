#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"

#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction * det)
:fDetector(det),
fDetDir(0), fThickCmd(0), fRadiusCmd(0), fUpdateCmd(0)
{
    
    fDetDir = new G4UIdirectory("/NeutronYield/Geometry/");
    fDetDir->SetGuidance("detector construction commands");
    
    fThickCmd = new G4UIcmdWithADoubleAndUnit("/NeutronYield/Geometry/Target/SetThickness",this);
    fThickCmd->SetGuidance("Set Thickness of the target");
    fThickCmd->SetParameterName("thickness",false);
    fThickCmd->SetRange("thickness>0.");
    fThickCmd->SetUnitCategory("Length");
    fThickCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fRadiusCmd = new G4UIcmdWithADoubleAndUnit("/NeutronYield/Geometry/Target/SetRadius",this);
    fRadiusCmd->SetGuidance("Set Radius of the target");
    fRadiusCmd->SetParameterName("radius",false);
    fRadiusCmd->SetRange("radius>0.");
    fRadiusCmd->SetUnitCategory("Length");
    fRadiusCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
     
    fMaterialCmd = new G4UIcmdWithAString("/NeutronYield/Geometry/Target/SetMaterial",this);
    fMaterialCmd->SetGuidance("Set Material (from NIST) of the target");
    fMaterialCmd->SetParameterName("material",false);
    fMaterialCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fUpdateCmd = new G4UIcmdWithoutParameter("/NeutronYield/Geometry/Update",this);
    fUpdateCmd->SetGuidance("Update geometry.");
    fUpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
    fUpdateCmd->SetGuidance("if you changed geometrical value(s).");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
    delete fThickCmd;
    delete fRadiusCmd;
    delete fMaterialCmd;
    delete fUpdateCmd;
    delete fDetDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
    if ( command == fThickCmd )
        fDetector->SetTargetThickness(fThickCmd->GetNewDoubleValue(newValue));

    if ( command == fRadiusCmd )
        fDetector->SetTargetRadius(fRadiusCmd->GetNewDoubleValue(newValue));

    if ( command == fMaterialCmd )
        fDetector->SetTargetMaterial(newValue);
 
    if  ( command == fUpdateCmd )
        fDetector->UpdateGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
