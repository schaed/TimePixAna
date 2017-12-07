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
  if(fileCa == NULL){
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

  ////////
  // read inputs
  //std::stringstream os("data/tot_toa_calibration/a_tot.dat");
  ifstream readFile("data/tot_toa_calibration/a_tot.dat");
  double a;
  std::string line;
  unsigned b=0,c=0;
  parameters charge_parameters;
  while(getline(readFile,line)){
    std::stringstream os(line);
    while(os >> a){
      if((b%NPIXELSX)==0 && b!=0) ++c;
      charge_parameters.f0[c][(b%NPIXELSY)] = a*1.0e3;
      ++b;
      //std::cout << c << " " << (b%NPIXELSY) << std::endl;
    }
  }
  readFile.close();

  // param 2
  readFile.open("data/tot_toa_calibration/b_tot.dat");
  b=0; c=0;
  while(getline(readFile,line)){
    std::stringstream os(line);
    while(os >> a){
      if((b%NPIXELSX)==0 && b!=0) ++c;
      charge_parameters.f1[c][(b%NPIXELSY)] = a;
      ++b;
      //std::cout << c << " " << (b%NPIXELSY) << std::endl;
    }
  }
  readFile.close();
  // param 3
  readFile.open("data/tot_toa_calibration/c_tot.dat");
  b=0; c=0;
  while(getline(readFile,line)){
    std::stringstream os(line);
    while(os >> a){
      if((b%NPIXELSX)==0 && b!=0) ++c;
      charge_parameters.f2[c][(b%NPIXELSY)] = a/1.0e3;
      ++b;
      //std::cout << c << " " << (b%NPIXELSY) << std::endl;
    }
  }
  readFile.close();

  // param 3
  readFile.open("data/tot_toa_calibration/t_tot.dat");
  b=0; c=0;
  while(getline(readFile,line)){
    std::stringstream os(line);
    while(os >> a){
      if((b%NPIXELSX)==0 && b!=0) ++c;
      charge_parameters.f3[c][(b%NPIXELSY)] = a/1.0e3;
      ++b;
      //std::cout << c << " " << (b%NPIXELSY) << std::endl;
    }
  }
  readFile.close();  

  ////////
  //end
  ////////
  
  Double_t xval[100], yval[100], xe[100], ye[100], yvalup[100];
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
  double bf3 = 0.;
  tree -> Branch("f3", &bf3);
  double bfUp0 = 0.;
  tree -> Branch("fUp0", &bfUp0);
  double bfUp1 = 0.;
  tree -> Branch("fUp1", &bfUp1);
  double bfUp2 = 0.;
  tree -> Branch("fUp2", &bfUp2);
  double bfUp3 = 0.;
  tree -> Branch("fUp3", &bfUp3);

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
    xval[0]=3.691;
    xval[1]=4.51;        
    xval[2]=8.5;
    xval[3]=10.0;    
    xval[4]=15.776;    
    xval[5]=22.166;
    xval[6]=25.272;

    xe[0]=0.0;
    xe[1]=0.0;     
    xe[2]=0.0;
    xe[3]=0.0; 
    xe[4]=0.0;   
    xe[5]=0.0;
    xe[6]=0.0;   

    //value
    yval[0]=bMuCa;
    yval[1]=bMuTi;
    yval[2]=bMuCu;    
    yval[3]=bMuGaAs;
    yval[4]=bMuZr;
    yval[5]=bMuAg;
    yval[6]=bMuSn;

    //value
    ye[0]=bMuErrCa;
    ye[1]=bMuErrTi;
    ye[2]=bMuErrCu;    
    ye[3]=bMuErrGaAs;
    ye[4]=bMuErrZr;
    ye[5]=bMuErrAg;
    ye[6]=bMuErrSn;

    //ye[0]=bSigmaCa;
    //ye[1]=bSigmaTi;
    //ye[2]=bSigmaCu;    
    //ye[3]=bSigmaGaAs;
    //ye[4]=bSigmaZr;
    //ye[5]=bSigmaAg;
    //ye[6]=bSigmaSn;

    if(bXAg==22 && bYAg==9)    ye[4]=20.0;
    if(bXAg==35 && bYAg==152)  ye[4]=20.0;
    if(bXAg==119 && bYAg==81)  ye[4]=20.0;
    if(yval[4]<yval[3])  ye[4]=20.0;
    if(yval[1]<yval[0])  ye[0]=20.0;
    if(yval[4]>yval[5])  ye[4]=20.0;
    if(yval[6]<yval[5])  ye[6]=20.0;        
    if(ye[1]<0.3) ye[1]=0.35;
    unsigned nLarge=0;
    for(unsigned a=0; a<7;++a) if(ye[a]>3.0) ++nLarge;
    if(nLarge>3) for(unsigned a=0; a<7;++a) if(ye[a]<3.0) ye[a]=5.0;
    char name[100];
    sprintf(name, "fFitX%03dY%03d", bXAg, bYAg);
    char nameup[100];
    sprintf(nameup, "fFitUpX%03dY%03d", bXAg, bYAg);
    
    //TGraph* gr = new TGraph(7,x,y);
    TGraphErrors* gr = new TGraphErrors(7,xval,yval,xe,ye);
    gr->GetXaxis()->SetTitle("Energy [keV]");
    gr->GetYaxis()->SetTitle("ToT");


    const char* m_fitFunctionExprA = "[0]*x + [1] - [2]/(x-2.0)";
    TF1 *fFit = new TF1(name, m_fitFunctionExprA, 3.0, 26.0); 
    // first fit
    fFit -> SetParNames("scale", "#mu", "#sigma");
    fFit -> SetParLimits(0, 0., 100.);
    fFit -> SetParLimits(1, 0., 100.);
    fFit -> SetParLimits(2, 0., 100.);
    //fFit -> SetParLimits(3, 2.5, 3.2);
    fFit -> SetParameter(0, 1.0);
    fFit -> SetParameter(1, 1.0);
    fFit -> SetParameter(2, 1.0);
    //fFit -> SetParameter(3, 3.0);
    gr -> Fit(fFit, "Q && R"); // memory leak in the TLinearFitter class, bug in some root versions

    // Error up
    //value + sigma
    yvalup[0]=bMuCa+bSigmaCa;
    yvalup[1]=bMuTi+bSigmaTi;
    yvalup[2]=bMuCu+bSigmaCu;    
    yvalup[3]=bMuGaAs+bSigmaGaAs;
    yvalup[4]=bMuZr+bSigmaZr;
    yvalup[5]=bMuAg+bSigmaAg;
    yvalup[6]=bMuSn+bSigmaSn;

    TGraphErrors* grup = new TGraphErrors(7,xval,yvalup,xe,ye);
    grup->GetXaxis()->SetTitle("Energy [keV]");
    grup->GetYaxis()->SetTitle("ToT");
    
    TF1 *fFitUp = new TF1(nameup, fitFunctionExpr, 3.0, 26.0); 
    // first fit
    fFitUp -> SetParNames("scale", "#mu", "#sigma");
    fFitUp -> SetParLimits(0, 0., 100.);
    fFitUp -> SetParLimits(1, 0., 100.);
    fFitUp -> SetParLimits(2, 0., 100.);
    fFitUp -> SetParLimits(3, 1., 5.);
    fFitUp -> SetParameter(0, 1.0);
    fFitUp -> SetParameter(1, 1.0);
    fFitUp -> SetParameter(2, 1.0);
    fFitUp -> SetParameter(3, 3.0);
    grup   -> Fit(fFitUp, "Q && R"); // memory leak in the TLinearFitter class, bug in some root versions
    
    //if(bXAg==0 && bYAg==1){
    bX = bXAg;
    bY = bYAg;
    const char* m_fitFunctionExpr = "[0]*x + [1] - [2]/(x-[3])";
    //const char* m_fitFunctionExpr = "[0]*x + [1] - [2]/(x-2.0)";
    TF1* fFitCharge   = new TF1(name,   m_fitFunctionExpr, 1.0, 40.0);    
    TF1* fFitChargeUp   = new TF1(name,   m_fitFunctionExpr, 1.0, 40.0);
    TF1 *fFit1 = NULL;
    TF1 *fFit2 = NULL;
    TF1 *fFitUp1 = NULL;
    TF1 *fFitUp2 = NULL;    
    double my_intercept=1.0;
    double my_intercept_up=1.0;    
    
    if(true){

      unsigned x= bX;
      unsigned y= bY;

      double ratio =charge_parameters.f0[x][y]/ fFit->GetParameter(0);
      fFitCharge->SetParameter(0,fFit->GetParameter(0));
      fFitCharge->SetParameter(1,fFit->GetParameter(1));
      fFitCharge->SetParameter(2,ratio*charge_parameters.f2[x][y]);
      fFitCharge->SetParameter(2,fFit->GetParameter(2));
      fFitCharge->SetParameter(3,ratio*charge_parameters.f3[x][y]);
      fFitCharge->SetLineColor(1);
      //std::cout << "loop0" << std::endl;
      fFit1 = FitLoop(x, y, ratio, fFit1, fFit, charge_parameters.f2[x][y], charge_parameters.f3[x][y], my_intercept, gr, yval);
      //std::cout << "loop10" << std::endl;
      //delete fFit1;
      char nameFitAA[100];
      sprintf(nameFitAA, "[0]*x + [1] - [2]/(x-%f)", my_intercept);      
      const char* m_fitFunctionExprCC = nameFitAA;    
      fFit2 = new TF1(name, m_fitFunctionExprCC, 1.0, 26.0);
      //std::cout << "loop11 << " << fFit1 << std::endl;
      fFit2 -> SetParNames("scale", "#mu", "#sigma");
      fFit2 -> SetParLimits(0, 0., 100.);
      fFit2 -> SetParLimits(1, 0., 100.);
      fFit2 -> SetParLimits(2, 0., 100.);
      fFit2 -> SetParameter(0, fFit1->GetParameter(0));
      fFit2 -> SetParameter(1, fFit1->GetParameter(1));
      fFit2 -> SetParameter(2, 3.0*ratio*charge_parameters.f2[x][y]);
      fFit2->SetLineColor(4);
      gr -> Fit(fFit2, "Q && R");
      //std::cout << "loop1" << std::endl;
      fFitUp1 = FitLoop(x, y, ratio, fFitUp1, fFitUp, charge_parameters.f2[x][y], charge_parameters.f3[x][y], my_intercept_up, grup, yvalup);

      char nameFitM[100];
      sprintf(nameFitM, "[0]*x + [1] - [2]/(x-%f)", my_intercept_up);      
      const char* m_fitFunctionExprM = nameFitM;    
      fFitUp2 = new TF1(name, m_fitFunctionExprM, 1.0, 26.0);
      fFitUp2 -> SetParNames("scale", "#mu", "#sigma");
      fFitUp2 -> SetParLimits(0, 0., 100.);
      fFitUp2 -> SetParLimits(1, 0., 100.);
      fFitUp2 -> SetParLimits(2, 0., 100.);
      fFitUp2 -> SetParameter(0, fFitUp1->GetParameter(0));
      fFitUp2 -> SetParameter(1, fFitUp1->GetParameter(1));
      fFitUp2 -> SetParameter(2, 3.0*ratio*charge_parameters.f2[x][y]);
      fFitUp2->SetLineColor(4);
      grup -> Fit(fFitUp2, "Q && R");
      
      can->Clear();
      //if(bX==25 && bY==20){
      if(false){
	can->cd();
	gr->Draw();
	fFitCharge->Draw("same");	
	fFit->Draw("same");
	fFit2->Draw("same");
	can->Update();
	can->WaitPrimitive();
	std::cout << "cA: " << bMuCa << std::endl;
	std::cout <<"mean: " << fFit -> GetParameter(0) << " " << fFit -> GetParameter(1) << " " << fFit -> GetParameter(2) << " x " << bX << " y " << bY << std::endl;
	std::cout <<"mean: " << fFitUp -> GetParameter(0) << " " << fFitUp -> GetParameter(1) << " " << fFitUp -> GetParameter(2) << std::endl;
      }
    }

    bX = bXAg;
    bY = bYAg;
    bf0 = fFit -> GetParameter(0);
    bf1 = fFit -> GetParameter(1);
    bf2 = fFit -> GetParameter(2);

    /*bf0 = fFitCharge -> GetParameter(0);
    bf1 = fFitCharge -> GetParameter(1);
    bf2 = fFitCharge -> GetParameter(2);    
    bf3 = fFitCharge -> GetParameter(3);    
    */
    bf0 = fFit2 -> GetParameter(0);
    bf1 = fFit2 -> GetParameter(1);
    bf2 = fFit2 -> GetParameter(2);
    bf3 = my_intercept;
    
    bfUp0 = fFitUp2 -> GetParameter(0);
    bfUp1 = fFitUp2 -> GetParameter(1);
    bfUp2 = fFitUp2 -> GetParameter(2);    
    bfUp3 = my_intercept_up;
    //std::cout << "my_par0: " << fFit2 -> GetParameter(0) << " " << fFitUp2 -> GetParameter(0) << std::endl;
    //std::cout << "my_intercept: " << my_intercept << " " << my_intercept_up << std::endl;
    tree->Fill();

    loadBar(nEvt,nEntries);

    delete fFit;
    delete fFit1;
    delete fFit2;
    delete fFitCharge;
    delete fFitChargeUp;
    delete fFitUp;
  }

  tree -> Write();
  delete tree;
  
  // closing root file
  cout << __PRETTY_FUNCTION__ << ": closing root file" << endl;
  file -> Close();															   
  
  return 1;
}

TF1 * FitLoop(unsigned x, unsigned y, float ratio, TF1 *fFit1, TF1 *fit, double f2, double f3, double &my_intercept, TGraphErrors *gr, Double_t yval[]){

  char name[100];
    sprintf(name, "fFitX%03dY%03d", x, y);
    char nameFit[100];
    sprintf(nameFit, "[0]*x + [1] - [2]/(x-%f)", (ratio*f3));
    const char* m_fitFunctionExprB = nameFit;
    fFit1 = new TF1(name, m_fitFunctionExprB, 1.0, 26.0); 
    fFit1 -> SetParNames("scale", "#mu", "#sigma");
    fFit1 -> SetParLimits(0, 0., 100.);
    fFit1 -> SetParLimits(1, 0., 100.);
    fFit1 -> SetParLimits(2, 0., 100.);
    fFit1 -> SetParameter(0, fit->GetParameter(0));
    fFit1 -> SetParameter(1, fit->GetParameter(1));
    fFit1 -> SetParameter(2, ratio*f2);
    fFit1->SetLineColor(4);
    gr -> Fit(fFit1, "Q && R");
    my_intercept = ratio*f3;
    unsigned nTry=0;
    //std::cout << "loop2" << std::endl;
    double my_eval = fFit1->Eval(3.691);
    while(my_eval-yval[0]>0.1) {
      //std::cout << "loop8" << std::endl;      
      char nameFitA[100];
      if((my_intercept)<1.0){
	my_intercept=1.5;
      }else if((my_intercept)<1.5){
	my_intercept=2.0;
      }else if((my_intercept)<2.0){
	my_intercept=2.5;
      }else if((my_intercept)<2.5){
	my_intercept=2.6;
      }else if((my_intercept)<2.7){
	my_intercept=2.8;
      }else if((my_intercept)<2.9){
	my_intercept=3.0;
      }      
      sprintf(nameFitA, "[0]*x + [1] - [2]/(x-%f)", my_intercept);      
      const char* m_fitFunctionExprC = nameFitA;
      
      TF1 *fFit3 = new TF1(name, m_fitFunctionExprC, 1.0, 26.0);
      fFit3 -> SetParNames("scale", "#mu", "#sigma");
      fFit3 -> SetParLimits(0, 0., 100.);
      fFit3 -> SetParLimits(1, 0., 100.);
      fFit3 -> SetParLimits(2, 0., 100.);
      fFit3 -> SetParameter(0, fFit1->GetParameter(0));
      fFit3 -> SetParameter(1, fFit1->GetParameter(1));
      fFit3 -> SetParameter(2, 2.0*ratio*f2);
      fFit3->SetLineColor(4);
      gr -> Fit(fFit3, "Q && R");
      //std::cout << "loop4" << std::endl;
      my_eval = fFit3->Eval(3.691);
      //std::cout << "loop4b" << std::endl;      
      delete fFit3;
      ++nTry;
      if(nTry>5) break;
    }
    //std::cout << "loop3" << std::endl;
    return fFit1;
}
