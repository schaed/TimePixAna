#include "fitSpectra.h"
#include "TGraph.h"
#include "TGraphErrors.h"

int fitSpectra(const char *fileNameAg,
	       const char *fileNameZr,
	       const char *fileNameCu,
	       const char *fileNameCa,
	       const char *fileNameSn,
	       const char *fileNameGaAs,
	       const char *fileNameTi){

  TFile *fileAg = TFile::Open(fileNameAg, "READ");
  if(fileAg == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameAg << endl;
    return 1;
  }

  TFile *fileZr = TFile::Open(fileNameZr, "READ");
  if(fileAg == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameZr << endl;
    return 1;
  }

  TFile *fileCu = TFile::Open(fileNameCu, "READ");
  if(fileCu == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameCu << endl;
    return 1;
  }

  TFile *fileCa = TFile::Open(fileNameCa, "READ");
  if(fileCu == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameCa << endl;
    return 1;
  }  
  TFile *fileSn = TFile::Open(fileNameSn, "READ");
  if(fileCu == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameSn << endl;
    return 1;
  }
  TFile *fileGaAs = TFile::Open(fileNameGaAs, "READ");
  if(fileCu == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameGaAs << endl;
    return 1;
  }

  TFile *fileTi = TFile::Open(fileNameTi, "READ");
  if(fileCu == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameTi << endl;
    return 1;
  }

  TTree *treeAg   = static_cast<TTree *>(fileAg->Get("fit"));
  TTree *treeZr   = static_cast<TTree *>(fileZr->Get("fit"));
  TTree *treeCu   = static_cast<TTree *>(fileCu->Get("fit"));
  TTree *treeCa   = static_cast<TTree *>(fileCa->Get("fit"));
  TTree *treeSn   = static_cast<TTree *>(fileSn->Get("fit"));
  TTree *treeGaAs = static_cast<TTree *>(fileGaAs->Get("fit"));
  TTree *treeTi   = static_cast<TTree *>(fileTi->Get("fit"));

  std::cout << "Trees: " << treeAg << " " << treeZr << " " << treeCu << " " << treeCa << " " << treeSn << " " << treeGaAs << " " << treeTi << std::endl;
  
  unsigned int bXAg   = 0, bYAg   = 0;
  unsigned int bXZr   = 0, bYZr   = 0;
  unsigned int bXCu   = 0, bYCu   = 0;
  unsigned int bXCa   = 0, bYCa   = 0;
  unsigned int bXSn   = 0, bYSn   = 0;
  unsigned int bXGaAs = 0, bYGaAs = 0;
  unsigned int bXTi   = 0, bYTi   = 0;

  double bScaleAg   = 0.0, bMuAg  =0.0, bSigmaAg  =0.0;  
  double bScaleZr   = 0.0, bMuZr  =0.0, bSigmaZr  =0.0;  
  double bScaleCu   = 0.0, bMuCu  =0.0, bSigmaCu  =0.0;  
  double bScaleCa   = 0.0, bMuCa  =0.0, bSigmaCa  =0.0;  
  double bScaleSn   = 0.0, bMuSn  =0.0, bSigmaSn  =0.0;  
  double bScaleGaAs = 0.0, bMuGaAs=0.0, bSigmaGaAs=0.0;  
  double bScaleTi   = 0.0, bMuTi  =0.0, bSigmaTi  =0.0;

  double bMuErrAg   = 0.0;  
  double bMuErrZr   = 0.0;  
  double bMuErrCu   = 0.0;  
  double bMuErrCa   = 0.0;  
  double bMuErrSn   = 0.0;  
  double bMuErrGaAs = 0.0;  
  double bMuErrTi   = 0.0;    
  
  treeAg   -> SetBranchAddress("x", &bXAg  );
  treeZr   -> SetBranchAddress("x", &bXZr  );
  treeCu   -> SetBranchAddress("x", &bXCu  );
  treeCa   -> SetBranchAddress("x", &bXCa  );
  treeSn   -> SetBranchAddress("x", &bXSn  );
  treeGaAs -> SetBranchAddress("x", &bXGaAs);
  treeTi   -> SetBranchAddress("x", &bXTi);
  treeAg   -> SetBranchAddress("y", &bYAg  );
  treeZr   -> SetBranchAddress("y", &bYZr  );
  treeCu   -> SetBranchAddress("y", &bYCu  );
  treeCa   -> SetBranchAddress("y", &bYCa  );
  treeSn   -> SetBranchAddress("y", &bYSn  );
  treeGaAs -> SetBranchAddress("y", &bYGaAs);
  treeTi   -> SetBranchAddress("y", &bYTi);
  treeAg   -> SetBranchAddress("scale", &bScaleAg  );
  treeZr   -> SetBranchAddress("scale", &bScaleZr  );
  treeCu   -> SetBranchAddress("scale", &bScaleCu  );
  treeCa   -> SetBranchAddress("scale", &bScaleCa  );
  treeSn   -> SetBranchAddress("scale", &bScaleSn  );
  treeGaAs -> SetBranchAddress("scale", &bScaleGaAs);
  treeTi   -> SetBranchAddress("scale", &bScaleTi);
  treeAg   -> SetBranchAddress("mu", &bMuAg  );
  treeZr   -> SetBranchAddress("mu", &bMuZr  );
  treeCu   -> SetBranchAddress("mu", &bMuCu  );
  treeCa   -> SetBranchAddress("mu", &bMuCa  );
  treeSn   -> SetBranchAddress("mu", &bMuSn  );
  treeGaAs -> SetBranchAddress("mu", &bMuGaAs);
  treeTi   -> SetBranchAddress("mu", &bMuTi);

  treeAg   -> SetBranchAddress("muErr", &bMuErrAg  );
  treeZr   -> SetBranchAddress("muErr", &bMuErrZr  );
  treeCu   -> SetBranchAddress("muErr", &bMuErrCu  );
  treeCa   -> SetBranchAddress("muErr", &bMuErrCa  );
  treeSn   -> SetBranchAddress("muErr", &bMuErrSn  );
  treeGaAs -> SetBranchAddress("muErr", &bMuErrGaAs);
  treeTi   -> SetBranchAddress("muErr", &bMuErrTi);


  treeAg   -> SetBranchAddress("sigma", &bSigmaAg  );
  treeZr   -> SetBranchAddress("sigma", &bSigmaZr  );
  treeCu   -> SetBranchAddress("sigma", &bSigmaCu  );
  treeCa   -> SetBranchAddress("sigma", &bSigmaCa  );
  treeSn   -> SetBranchAddress("sigma", &bSigmaSn  );
  treeGaAs -> SetBranchAddress("sigma", &bSigmaGaAs);
  treeTi   -> SetBranchAddress("sigma", &bSigmaTi);  

  Double_t x[100], y[100], xe[100], ye[100];
  TCanvas *can = new  TCanvas("c", "c", 500, 500);

  cout << __PRETTY_FUNCTION__ << ": opening root tree" << endl;  
  TFile *file = TFile::Open("fout.root", "RECREATE");
  if(file == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file fout.root" << endl;
    return 1;
  }
  TTree *tree = new TTree("fit", "fit");
  unsigned int bX = 0;
  tree -> Branch("x", &bX);
  unsigned int bY = 0;
  tree -> Branch("y", &bY);
  double bf0 = 0.;
  tree -> Branch("f0", &bf0);
  double bf1 = 0.;
  tree -> Branch("f1", &bf1);
  double bf2 = 0.;
  tree -> Branch("f2", &bf2);
  double bfUp0 = 0.;
  tree -> Branch("fUp0", &bfUp0);
  double bfUp1 = 0.;
  tree -> Branch("fUp1", &bfUp1);
  double bfUp2 = 0.;
  tree -> Branch("fUp2", &bfUp2);    

  unsigned nEntries = treeAg->GetEntries();
  std::cout << "Processing: " << nEntries << std::endl;
  for(unsigned nEvt=0; nEvt<nEntries; ++nEvt){
    treeAg  ->GetEntry(nEvt);
    treeZr  ->GetEntry(nEvt);
    treeCu  ->GetEntry(nEvt);
    treeCa  ->GetEntry(nEvt);
    treeSn  ->GetEntry(nEvt);
    treeGaAs->GetEntry(nEvt);
    treeTi->GetEntry(nEvt);

    //if(nEvt%5000==0) std::cout << "Process: " << nEvt << std::endl;

//Ca - temperature 67.1C - 15kV and 5mA - 3.691keV
//Sn  - temperature 63.9C - 45kV and 50mA - 25.272keV - no large peak
//Cu  - temperature 63.9C - 45kV and 50mA - 8.040/8.93
//Ag - 22.166
//Ti - 4.51
//Zr - 15.776
//GaAr - 9.25/10.54
    
    // energy
    x[0]=3.691;
    x[1]=4.51;        
    x[2]=8.5;
    x[3]=10.0;    
    x[4]=15.776;    
    x[5]=22.166;
    x[6]=25.272;

    xe[0]=0.0;
    xe[1]=0.0;     
    xe[2]=0.0;
    xe[3]=0.0; 
    xe[4]=0.0;   
    xe[5]=0.0;
    xe[6]=0.0;   

    //value
    y[0]=bMuCa;
    y[1]=bMuTi;
    y[2]=bMuCu;    
    y[3]=bMuGaAs;
    y[4]=bMuZr;
    y[5]=bMuAg;
    y[6]=bMuSn;

    //value
    ye[0]=bMuErrCa;
    ye[1]=bMuErrTi;
    ye[2]=bMuErrCu;    
    ye[3]=bMuErrGaAs;
    ye[4]=bMuErrZr;
    ye[5]=bMuErrAg;
    ye[6]=bMuErrSn;    

    char name[100];
    sprintf(name, "fFitX%03dY%03d", bXAg, bYAg);
    char nameup[100];
    sprintf(nameup, "fFitUpX%03dY%03d", bXAg, bYAg);
    
    //TGraph* gr = new TGraph(7,x,y);
    TGraphErrors* gr = new TGraphErrors(7,x,y,xe,ye);
    gr->GetXaxis()->SetTitle("Energy [keV]");
    gr->GetYaxis()->SetTitle("ToT");
    
    TF1 *fFit = new TF1(name, fitFunctionExpr, 3.0, 26.0); 
    // first fit
    fFit -> SetParNames("scale", "#mu", "#sigma");
    fFit -> SetParLimits(0, 0., 100.);
    fFit -> SetParLimits(1, 0., 100.);
    fFit -> SetParLimits(2, 0., 100.);
    fFit -> SetParameter(0, 1.0);
    fFit -> SetParameter(1, 1.0);
    fFit -> SetParameter(2, 1.0);
    gr -> Fit(fFit, "Q && R"); // memory leak in the TLinearFitter class, bug in some root versions

    // Error up
    //value + sigma
    y[0]=bMuCa+bSigmaCa;
    y[1]=bMuTi+bSigmaTi;
    y[2]=bMuCu+bSigmaCu;    
    y[3]=bMuGaAs+bSigmaGaAs;
    y[4]=bMuZr+bSigmaZr;
    y[5]=bMuAg+bSigmaAg;
    y[6]=bMuSn+bSigmaSn;

    TGraphErrors* grup = new TGraphErrors(7,x,y,xe,ye);
    grup->GetXaxis()->SetTitle("Energy [keV]");
    grup->GetYaxis()->SetTitle("ToT");
    
    TF1 *fFitUp = new TF1(nameup, fitFunctionExpr, 3.0, 26.0); 
    // first fit
    fFitUp -> SetParNames("scale", "#mu", "#sigma");
    fFitUp -> SetParLimits(0, 0., 100.);
    fFitUp -> SetParLimits(1, 0., 100.);
    fFitUp -> SetParLimits(2, 0., 100.);
    fFitUp -> SetParameter(0, 1.0);
    fFitUp -> SetParameter(1, 1.0);
    fFitUp -> SetParameter(2, 1.0);
    grup -> Fit(fFitUp, "Q && R"); // memory leak in the TLinearFitter class, bug in some root versions

    
    if(false){
      can->Clear();
      can->cd();
      gr->Draw();
      can->Update();
      can->WaitPrimitive();
      
      std::cout <<"mean: " << fFit -> GetParameter(0) << " " << fFit -> GetParameter(1) << " " << fFit -> GetParameter(2) << std::endl;
      std::cout <<"mean: " << fFitUp -> GetParameter(0) << " " << fFitUp -> GetParameter(1) << " " << fFitUp -> GetParameter(2) << std::endl;      
    }

    bX = bXAg;
    bY = bYAg;
    bf0 = fFit -> GetParameter(0);
    bf1 = fFit -> GetParameter(1);
    bf2 = fFit -> GetParameter(2);

    bfUp0 = fFitUp -> GetParameter(0);
    bfUp1 = fFitUp -> GetParameter(1);
    bfUp2 = fFitUp -> GetParameter(2);    
    tree->Fill();

    loadBar(nEvt,nEntries);

    delete fFit;
    delete fFitUp;
  }

  tree -> Write();
  delete tree;
  
  // closing root file
  cout << __PRETTY_FUNCTION__ << ": closing root file" << endl;
  file -> Close();															   
  
  return 1;
}
