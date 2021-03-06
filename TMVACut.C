//Test del metodo Cuts
//
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TMVA/TMVAGui.h"
#include <TMVA/Config.h>

using namespace std;

int TMVATest(){
    
    TMVA::Tools::Instance();
     
    //SigTree
    TString sname = "./HH.root";
    TFile* Sig_TFile = TFile::Open( sname );
    
    //BkgTree
    TString bname = "./ttbar.root";
    TFile* Bkg_TFile = TFile::Open( bname );
    
    TTree* Sig_Tree = (TTree*)Sig_TFile->Get("tree");
    TTree* Bkg_Tree = (TTree*)Bkg_TFile->Get("tree");
   
    //output file
    TString outfileName( "TMVAtest.root" );
    TFile* outputFile = TFile::Open( outfileName, "RECREATE" );  
    
    TMVA::Factory *TMVAtest = new TMVA::Factory( "TMVAClassificationTest", outputFile,
                                               "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );
    TMVA::DataLoader *dataloader = new TMVA::DataLoader("datasetTest");

    //Add trainingVariables
    dataloader->AddVariable("bb_pt", 'F');
    dataloader->AddVariable("vbs_pt", 'F');
    dataloader->AddVariable("lep_pt", 'F');
    dataloader->AddVariable("Ptm", 'F');
    dataloader->AddVariable("Htnu", 'F');
   
    //Add Sig and Bkg trees
    dataloader->AddSignalTree(Sig_Tree, 1.);
    dataloader->AddBackgroundTree(Bkg_Tree, 1.);

     
    //Add SpectatorVariables
    dataloader->AddSpectator ("mbb", 'F');
    dataloader->AddSpectator ("mww", 'F');
	dataloader->AddSpectator ("mvbs", 'F');
   
    TCut mycuts = "";
    TCut mycutb = "";
    
    dataloader->PrepareTrainingAndTestTree( mycuts, mycutb, "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V" );
    TMVAtest->BookMethod(dataloader, TMVA::Types::kCuts,"Cuts", "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart") ;

    // start the training
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
    
    TMVAtest->TrainAllMethods();
    TMVAtest->TestAllMethods();
    TMVAtest->EvaluateAllMethods();
    
    outputFile->Close();
    
    delete TMVAtest;
    delete dataloader;

    if (!gROOT->IsBatch()) TMVA::TMVAGui( outfileName );
    return 0 ;
}

int main(int argc, char** argv ){
    return TMVATest();
}
