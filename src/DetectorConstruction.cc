#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4EllipticalTube.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4RunManager.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4RegionStore.hh"
#include "G4VisAttributes.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
:fMaterial_World(0), fMaterial_Target(0), fMaterial_Scorer(0),
fPhysicalWorld(0), fDetectorMessenger(0)
{
    // Commands
    fDetectorMessenger = new DetectorMessenger(this);
    
    // materials
    DefineMaterials();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
    delete fDetectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    return ConstructVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
    G4double a, z, density;
    G4int ncomponents;
    G4double fractionmass;
    
    // define Elements
    G4Element* H  =  new G4Element("Hydrogen", "H",  z= 1, a=  1.001*g/mole);
    G4Element* C  = new G4Element("Carbon",   "C",  z= 6, a=  12.0107*g/mole);
    G4Element* N  = new G4Element("Nitrogen", "N",  z= 7, a=  14.0067*g/mole);
    G4Element* O  = new G4Element("Oxygen",   "O",  z= 8, a=  15.9994*g/mole);
    G4Element* Al = new G4Element("Aluminum", "Al", z=13, a=  26.98154*g/mole);
    G4Element* Ar = new G4Element("Argon",    "Ar", z=18, a=  39.9480*g/mole);
    G4Element* Fe = new G4Element("Iron",     "Fe", z=26, a=  55.845*g/mole);
    G4Element* Be = new G4Element("Beryllium","Be", z=4,  a=  9.0122*g/mole);
    // Vacuum
    G4Material* Vacuum = new G4Material("Vacuum", density=1e-25*g/cm3, ncomponents=4,
                                        kStateGas, 2.73*kelvin, 3.0e-18*pascal);
    Vacuum->AddElement(C, fractionmass=0.000124);
    Vacuum->AddElement(N, fractionmass=0.755268);
    Vacuum->AddElement(O, fractionmass=0.231781);
    Vacuum->AddElement(Ar,fractionmass=0.012827);
    
    // assign materials
    fMaterial_World  = Vacuum;
    fMaterial_Scorer = Vacuum;
    
    fAluminum = new G4Material("Aluminum", density=2.699*g/cm3, ncomponents=1);
    fAluminum->AddElement(Al, fractionmass=1.0);
    
    //fCarbon = new G4Material("Carbon", density=1.867*g/cm3, ncomponents=1); // For 256 MeV
    fCarbon = new G4Material("Carbon", density=1.618*g/cm3, ncomponents=1); // For 256 MeV
    fCarbon->AddElement(C, fractionmass=1.0);
    fCarbon->GetIonisation()->SetMeanExcitationEnergy(78*eV);
    
    fIron = new G4Material("Iron", density=7.869*g/cm3, ncomponents=1);
    fIron->AddElement(Fe, fractionmass=1.0);

    fBeryllium = new G4Material("Beryllium", density=/*1.5*/1.848*g/cm3, ncomponents=1);
    fBeryllium->AddElement(Be, fractionmass=1.0);
    
    fAcrylic = new G4Material("Acrylic", density=1.19*g/cm3, ncomponents=3);
    fAcrylic->AddElement(H, 0.080538);
    fAcrylic->AddElement(C, 0.599848);
    fAcrylic->AddElement(O, 0.319614);
    fAcrylic->GetIonisation()->SetMeanExcitationEnergy(74*eV);
    
    fWater = G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");
    
    fAir = new G4Material("Air", density=1.20484*mg/cm3, ncomponents=4);
    fAir->AddElement(C, 0.000124);
    fAir->AddElement(N, 0.755268);
    fAir->AddElement(O, 0.231781);
    fAir->AddElement(Ar, 0.012827);
    fAir->GetIonisation()->SetMeanExcitationEnergy(85.7*eV);
    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::ConstructVolumes()
{
    // Cleanup old geometry
    //
    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();
    
    G4bool useAlternativeGeometry = false;
    G4bool usingBeryllium = false;
 
    if ( fTarget_Material_Name == "Carbon" )
        fMaterial_Target = fCarbon;
    else if ( fTarget_Material_Name == "Aluminum" )
        fMaterial_Target = fAluminum;
    else if ( fTarget_Material_Name == "Iron")
        fMaterial_Target = fIron;
    else if ( fTarget_Material_Name == "Beryllium") {
        fMaterial_Target = fBeryllium;
	usingBeryllium = true;
    } else if ( fTarget_Material_Name == "G4_WATER") {
        fMaterial_Target = fWater;
        fMaterial_World = fAir;
        useAlternativeGeometry = true;
    } else {
        fMaterial_Target = G4NistManager::Instance()->FindOrBuildMaterial(fTarget_Material_Name);
        fMaterial_World = fAir;
    }
    
    // World
    G4Box* solid_World = new G4Box("World",
                                   1000.0*cm,
                                   1000.0*cm,
                                   1000.0*cm);
    
    
    
    G4LogicalVolume* logical_World = new G4LogicalVolume(solid_World,
                                                         fMaterial_World,
                                                         "World");
    fPhysicalWorld = new G4PVPlacement(0,
                                       G4ThreeVector(),
                                       logical_World,
                                       "World",
                                       0,
                                       false,
                                       0);
    
    G4VisAttributes * visWorld = new G4VisAttributes(G4Colour(0.,1.,1.));
    visWorld->SetVisibility(false);
    logical_World->SetVisAttributes(visWorld);
    
    if ( !useAlternativeGeometry ) {
        // Target
        G4Tubs* solid_Target = new G4Tubs("Target",
                                          0*cm,
                                          fTarget_Radius,
                                          fTarget_Thickness/2,
                                          0*deg,
                                          360*deg);
        
        G4cout << "---- Building target with dimensions: " << G4endl;
        G4cout << "----                       thickness: " << fTarget_Thickness/mm << " mm " << G4endl;
        G4cout << "----                       radius   : " << fTarget_Radius/mm << " mm " << G4endl;
        
        G4LogicalVolume* logical_Target = new G4LogicalVolume(solid_Target,
                                                              fMaterial_Target,
                                                              "Target");
        
        new G4PVPlacement(0,
                          G4ThreeVector(0*mm,0*mm,0*mm),
                          logical_Target,
                          "Target",
                          logical_World,
                          false,
                          0);

	if ( usingBeryllium ) {
		G4Tubs* carbonTube = new G4Tubs("envelope", fTarget_Radius, fTarget_Radius + 3 * mm, fTarget_Thickness/2, 0*deg, 360*deg);
		G4LogicalVolume* lcarbonTube = new G4LogicalVolume(carbonTube, fCarbon, "envelope");
		new G4PVPlacement(0, G4ThreeVector(), lcarbonTube, "envelope", logical_World,false,0);
			
		G4Tubs* cap = new G4Tubs("cap", 0*mm, fTarget_Radius + 3 * mm, 1*mm, 0*deg, 360*deg);
		G4LogicalVolume* lcap = new G4LogicalVolume(cap, fAluminum, "cap");
		new G4PVPlacement(0, G4ThreeVector(0, 0, fTarget_Thickness/2 + 1*mm), lcap, "cap", logical_World, false, 0); 
	}

    } else {
        // If highest energy
        G4RotationMatrix* rot = new G4RotationMatrix();
        
        // if the smalles energy set this too
        rot->rotateX(90*deg);     
	rot->rotateZ(90*deg);
        G4Box* phantom = new G4Box("phantom", 10*cm, 16*cm, 22.5*cm);
        G4LogicalVolume* lphantom = new G4LogicalVolume(phantom, fMaterial_World, "phantom");
        new G4PVPlacement(rot, G4ThreeVector(0), lphantom, "phantom", logical_World, 0, 0, 0);
        
        G4Box* shell = new G4Box("shell", 10*cm, 6*cm, 22.5*cm);
        G4LogicalVolume* lshell = new G4LogicalVolume(shell, fMaterial_Target, "shell");
        new G4PVPlacement(0, G4ThreeVector(0), lshell, "shell", lphantom, 0, 0, 0);
        G4Box* core_w = new G4Box("core_w", 9*cm, 6*cm, 21.5*cm);
        G4LogicalVolume* lcore_w = new G4LogicalVolume(core_w, fMaterial_Target, "core_w");
        new G4PVPlacement(0, G4ThreeVector(0), lcore_w, "core_w", lshell, 0, 0, 0);
        
        G4Tubs* shell_l = new G4Tubs("shell_l", 0.0, 10.0*cm, 22.5*cm, 0, 180*deg);
        G4LogicalVolume* lshell_l = new G4LogicalVolume(shell_l, fMaterial_Target, "shell_l");
        new G4PVPlacement(0, G4ThreeVector(0, 6*cm, 0), lshell_l, "shell_l", lphantom, 0, 0, 0);
        G4Tubs* core_l = new G4Tubs("core_l", 0.0, 9.0*cm, 21.5*cm, 0, 180*deg);
        G4LogicalVolume* lcore_l = new G4LogicalVolume(core_l, fMaterial_Target, "core_l");
        new G4PVPlacement(0, G4ThreeVector(0, -0.0*cm, 0), lcore_l, "core_l", lshell_l, 0, 0, 0);
        
        G4Tubs* shell_r = new G4Tubs("shell_r", 0.0, 10.0*cm, 22.5*cm, 180*deg, 180*deg);
        G4LogicalVolume* lshell_r = new G4LogicalVolume(shell_r, fMaterial_Target, "shell_r");
        new G4PVPlacement(0, G4ThreeVector(0, -6*cm, 0), lshell_r, "shell_r", lphantom, 0, 0, 0);
        G4Tubs* core_r = new G4Tubs("core_r", 0.0, 9.0*cm, 21.5*cm, 180*deg, 180*deg);
        G4LogicalVolume* lcore_r = new G4LogicalVolume(core_r, fMaterial_Target, "core_r");
        new G4PVPlacement(0, G4ThreeVector(0, 0.0*cm, 0), lcore_r, "core_r", lshell_r, 0, 0, 0);
   }         
        G4double STheta;
        G4String ScorerName;
        G4double rMin = 900.0*cm;
        G4double rMax = 900.005*cm;
        G4double SPhi = 0.0*deg;
        G4double DPhi = 360*deg;
        G4double DTheta = 4.0*deg;
        //
        const G4double angles[10] = {0.0*deg, 7.5*deg, 30.0*deg, 60.0*deg, 90.0*deg, 120.0*deg, 150.0*deg, 15.0*deg, 90.0*deg, 110*deg};
        
        // Scoring 1
        STheta = angles[0];
        ScorerName = "Scorer_0.0_deg";
        G4Sphere* solid_Scorer1 = new G4Sphere(ScorerName, rMin, rMax, SPhi, DPhi, STheta, DTheta);
        
        G4LogicalVolume* logical_Scorer1 = new G4LogicalVolume(solid_Scorer1,
                                                               fMaterial_Scorer,
                                                               ScorerName);
        
        new G4PVPlacement(0, G4ThreeVector(),
                          logical_Scorer1,
                          ScorerName,
                          logical_World,
                          0,
                          0,
                          0);
        // Scoring 2
        STheta = angles[1]-(DTheta/2)*deg;
        ScorerName = "Scorer_7.5_deg";
        G4Sphere* solid_Scorer2 = new G4Sphere(ScorerName, rMin, rMax, SPhi, DPhi, STheta, DTheta);
        
        G4LogicalVolume* logical_Scorer2 = new G4LogicalVolume(solid_Scorer2,
                                                               fMaterial_Scorer,
                                                               ScorerName);
        
        new G4PVPlacement(0, G4ThreeVector(0),
                          logical_Scorer2,
                          ScorerName,
                          logical_World,
                          0,
                          0,
                          0);
        // Scoring 3a
        STheta = angles[7]-(DTheta/2)*deg;
        ScorerName = "Scorer_15.0_deg";
        G4Sphere* solid_Scorer3a = new G4Sphere(ScorerName, rMin, rMax, SPhi, DPhi, STheta, DTheta);
        
        G4LogicalVolume* logical_Scorer3a = new G4LogicalVolume(solid_Scorer3a,
                                                               fMaterial_Scorer,
                                                               ScorerName);
        
        new G4PVPlacement(0, G4ThreeVector(0),
                          logical_Scorer3a,
                          ScorerName,
                          logical_World,
                          0,
                          0,
                          0);
        // Scoring 3b
        STheta = angles[8]-(DTheta/2)*deg;
        ScorerName = "Scorer_90.0_deg";
        G4Sphere* solid_Scorer3b = new G4Sphere(ScorerName, rMin, rMax, SPhi, DPhi, STheta, DTheta);
        
        G4LogicalVolume* logical_Scorer3b = new G4LogicalVolume(solid_Scorer3b,
                                                               fMaterial_Scorer,
                                                               ScorerName);
        
        new G4PVPlacement(0, G4ThreeVector(0),
                          logical_Scorer3b,
                          ScorerName,
                          logical_World,
                          0,
                          0,
                          0);
         // Scoring 3
        STheta = angles[2]-(DTheta/2)*deg;
        ScorerName = "Scorer_30.0_deg";
        G4Sphere* solid_Scorer3 = new G4Sphere(ScorerName, rMin, rMax, SPhi, DPhi, STheta, DTheta);
        
        G4LogicalVolume* logical_Scorer3 = new G4LogicalVolume(solid_Scorer3,
                                                               fMaterial_Scorer,
                                                               ScorerName);
        
        new G4PVPlacement(0, G4ThreeVector(0),
                          logical_Scorer3,
                          ScorerName,
                          logical_World,
                          0,
                          0,
                          0);
        
        // Scoring 4
        STheta = angles[3]-(DTheta/2)*deg;
        ScorerName = "Scorer_60.0_deg";
        G4Sphere* solid_Scorer4 = new G4Sphere(ScorerName, rMin, rMax, SPhi, DPhi, STheta, DTheta);
        
        G4LogicalVolume* logical_Scorer4 = new G4LogicalVolume(solid_Scorer4,
                                                               fMaterial_Scorer,
                                                               ScorerName);
        
        new G4PVPlacement(0, G4ThreeVector(0),
                          logical_Scorer4,
                          ScorerName,
                          logical_World,
                          0,
                          0,
                          0);
        
        // Scoring 5
        STheta = angles[4]-(DTheta/2)*deg;
        ScorerName = "Scorer_90.0_deg";
        G4Sphere* solid_Scorer5 = new G4Sphere(ScorerName, rMin, rMax, SPhi, DPhi, STheta, DTheta);
        
        G4LogicalVolume* logical_Scorer5 = new G4LogicalVolume(solid_Scorer5,
                                                               fMaterial_Scorer,
                                                               ScorerName);
        
        new G4PVPlacement(0, G4ThreeVector(0),
                          logical_Scorer5,
                          ScorerName,
                          logical_World,
                          0,
                          0,
                          0);
        
        // Scoring 6a
        STheta = angles[9]-(DTheta/2)*deg;
        ScorerName = "Scorer_110.0_deg";
        G4Sphere* solid_Scorer6a = new G4Sphere(ScorerName, rMin, rMax, SPhi, DPhi, STheta, DTheta);
        
        G4LogicalVolume* logical_Scorer6a = new G4LogicalVolume(solid_Scorer6a,
                                                               fMaterial_Scorer,
                                                               ScorerName);
        
        new G4PVPlacement(0, G4ThreeVector(0),
                          logical_Scorer6a,
                          ScorerName,
                          logical_World,
                          0,
                          0,
                          0);
         // Scoring 6
        STheta = angles[5]-(DTheta/2)*deg;
        ScorerName = "Scorer_120.0_deg";
        G4Sphere* solid_Scorer6 = new G4Sphere(ScorerName, rMin, rMax, SPhi, DPhi, STheta, DTheta);
        
        G4LogicalVolume* logical_Scorer6 = new G4LogicalVolume(solid_Scorer6,
                                                               fMaterial_Scorer,
                                                               ScorerName);
        
        new G4PVPlacement(0, G4ThreeVector(0),
                          logical_Scorer6,
                          ScorerName,
                          logical_World,
                          0,
                          0,
                          0);
        
        // Scoring 7
        STheta = angles[6]-(DTheta/2)*deg;
        ScorerName = "Scorer_150.0_deg";
        G4Sphere* solid_Scorer7 = new G4Sphere(ScorerName, rMin, rMax, SPhi, DPhi, STheta, DTheta);
        
        G4LogicalVolume* logical_Scorer7 = new G4LogicalVolume(solid_Scorer7,
                                                               fMaterial_Scorer,
                                                               ScorerName);
        
        new G4PVPlacement(0, G4ThreeVector(0),
                          logical_Scorer7,
                          ScorerName,
                          logical_World,
                          0,
                          0,
                          0);
        
             
        G4Box* scorer = new G4Box("scorer", 12.7/2 * cm, 12.7/2 * cm, 12.7/2 * cm);
        G4LogicalVolume* lscorer = new G4LogicalVolume(scorer, fMaterial_World, "scorer");
        
        for ( int i = 0; i < 13; i++ ) {
            G4double phi = 90*deg + 15*deg * i;
            
            G4RotationMatrix rotm = G4RotationMatrix();
            rotm.rotateX(90*deg);
            rotm.rotateY(-phi);
            G4ThreeVector uz = G4ThreeVector(std::cos(phi),  0, std::sin(phi));
            G4ThreeVector position = (350*cm)*uz;
            G4Transform3D transform = G4Transform3D(rotm,position);
            G4String aName = "Scorer_" + G4UIcommand::ConvertToString(i)+"_deg";
            new G4PVPlacement(transform,            //rotation,position
                              lscorer,                //logical volume
                              aName,          //name
                              logical_World,         //mother volume (ring)
                              false,                //no boolean operation
                              i,                    //copy number
                              false);
        }
    return fPhysicalWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetTargetThickness(G4double val)
{
    fTarget_Thickness = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void DetectorConstruction::SetTargetRadius(G4double val)
{
    fTarget_Radius = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void DetectorConstruction::SetTargetMaterial(G4String name)
{
    fTarget_Material_Name = name;
}

#include "G4RunManager.hh"

void DetectorConstruction::UpdateGeometry()
{
    G4RunManager::GetRunManager()->DefineWorldVolume(ConstructVolumes());
}



