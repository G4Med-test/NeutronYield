#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"

#include "globals.hh"

class G4Material;
class DetectorMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    
    DetectorConstruction();
    ~DetectorConstruction();
    
public:
    
    void SetTargetThickness(G4double);
    void SetTargetRadius(G4double);
    void SetTargetMaterial(G4String);
    
    virtual G4VPhysicalVolume* Construct();
    
    void UpdateGeometry();
        
public:
    
    G4double    GetTargetThickness()       {return fTarget_Thickness;};
    G4double    GetTargetRadius()          {return fTarget_Radius;};
    G4String    GetTargetMaterial()        {return fTarget_Material_Name;};
    
    const G4VPhysicalVolume* GetPhysicalWorld() {return fPhysicalWorld;};

private:
    void DefineMaterials();

    G4double        fTarget_Thickness;
    G4double	    fTarget_Radius;
    G4Material*     fMaterial_World;
    G4Material*     fMaterial_Target;
    G4Material*     fMaterial_Scorer;
    G4Material*     fBeryllium;
    G4Material*     fAluminum;
    G4Material*     fCarbon;
    G4Material*     fIron;
    G4Material*     fWater;
    G4Material*     fAir;
    G4Material*     fAcrylic;

    G4String        fTarget_Material_Name;
    
    G4VPhysicalVolume* fPhysicalWorld;
    
    DetectorMessenger* fDetectorMessenger;
    
    G4VPhysicalVolume* ConstructVolumes();
    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#endif

