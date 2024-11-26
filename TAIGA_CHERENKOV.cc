
#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "FTFP_BERT.hh"
#include "QGSP_BERT.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4SteppingVerbose.hh"
#include "G4OpticalParameters.hh"
#include "G4OpticalPhysics.hh"

/* #ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else */
#include "G4RunManager.hh"
//#endif

#include "G4UImanager.hh"
//#include "QBBC.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"
#include <fstream>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int Nevent=0;
char Title[200]={0};
std::ofstream PhEffect;
std::ofstream CHARGED;
std::ifstream rdata;

int main(int argc,char** argv)
{
  rdata.open("C:\\Geant4\\G4work\\TAIGA\\TAIGA_vis\\ModSpPart03.dat", std::ios_base::in);
  if(rdata.is_open()) rdata.getline(Title, 200);

  G4UIExecutive* ui = 0;                                                          
  if ( argc == 1 ) {                                                                
    ui = new G4UIExecutive(argc, argv);                                             
  }      

  G4int precision = 4;
  G4SteppingVerbose::UseBestUnit(precision);                                                                       

  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  
  // Construct the default run manager
  //
//#ifdef G4MULTITHREADED
  //G4MTRunManager* runManager = new G4MTRunManager;
//#else
  G4RunManager* runManager = new G4RunManager;
//#endif

  // Set mandatory initialization classes
  //
  // Detector construction
  runManager->SetUserInitialization(new DetectorConstruction());
  
  // Physics list
  G4VModularPhysicsList* physicsList = new QGSP_BERT;
  auto opticalParams = G4OpticalParameters::Instance();

  opticalParams->SetWLSTimeProfile("delta");
  physicsList->ReplacePhysics(new G4EmStandardPhysics_option4());
  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
  physicsList->RegisterPhysics(opticalPhysics);
  runManager->SetUserInitialization(physicsList);
    
  // User action initialization
  runManager->SetUserInitialization(new ActionInitialization());
  
  // Initialize visualization                                                      
  //                                                                               
  G4VisManager* visManager = new G4VisExecutive;
   //G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.     
   //G4VisManager* visManager = new G4VisExecutive("Quiet");                        
   visManager->Initialize();                                                      

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();                            

  // Process macro or start UI session
  //
  if (!ui) {
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }                                                                                
  else { 
    // interactive mode
    UImanager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
    delete ui;
  } 

  //runManager->Initialize();
  //G4int numberOfEvent = 25000;//50000
  //runManager->BeamOn(numberOfEvent);

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted 
  // in the main() program !

  if (PhEffect.is_open()) PhEffect.close();
  if (CHARGED.is_open()) CHARGED.close();
  if (rdata.is_open()) rdata.close();

  delete visManager;                                                             //Switching off visualisation
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
