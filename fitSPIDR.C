#include "fitSPIDR.h"

int fitSPIDR(const char *fileNameBase,
	     const char *elementName){

  gErrorIgnoreLevel = kWarning; // to suppress output

  std::cout << "check" << std::endl;

  char fileNameIn[1000];
  sprintf(fileNameIn, "%s.dat", fileNameBase);
  char fileNameSortIn[1000];
  sprintf(fileNameSortIn, "%s_sort.dat", fileNameBase);
  char fileNameRoot[1000];
  sprintf(fileNameRoot, "%s.root", fileNameBase);  
  char fileNamePar[1000];
  sprintf(fileNamePar, "%s.par", fileNameBase);
  cout << __PRETTY_FUNCTION__ << ": data will be read from " << fileNameIn << endl;
  cout << __PRETTY_FUNCTION__ << ": spectra will be saved in " << fileNameRoot << endl;
  cout << __PRETTY_FUNCTION__ << ": fit results will be saved in " << fileNamePar << endl;  

  /////////////////////////////////
  // allocating spectrum histograms
  /////////////////////////////////  
  cout << __PRETTY_FUNCTION__ << ": allocating spectrum histograms" << endl;  
  TH1F ***h1Spectrum = allocateSpectra();  

  ////////
  // sort spectrum
  ////////
  cout << __PRETTY_FUNCTION__ << ": sorting spectra" << endl;
  if(sortSpectra(fileNameIn, fileNameSortIn)){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot sort spectra" << endl;
    return 1;
  }

  ///////////////////
  // building spectra
  ///////////////////  
  cout << __PRETTY_FUNCTION__ << ": building spectra" << endl;
  if(buildSpectra(fileNameSortIn, h1Spectrum)){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot build spectra" << endl;
    return 1;
  }

  //////////////////
  // fitting spectra
  //////////////////
  cout << __PRETTY_FUNCTION__ << ": fitting spectra" << endl;
  parameters_t parameters;
  //if(fitSpectra(h1Spectrum, parameters, fileNamePar)){
  //  cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot fit spectra" << endl;    
  //  return 1;
  //}

  /////////////////
  // saving results
  /////////////////
  // opening root file
  cout << __PRETTY_FUNCTION__ << ": opening root tree" << endl;  
  TFile *file = TFile::Open(fileNameRoot, "RECREATE");
  if(file == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameRoot << endl;
    return 1;
  }

  // saving spectra
  cout << __PRETTY_FUNCTION__ << ": saving spectra" << endl;  
  for(unsigned int ix=0; ix<NPIXELSX; ix++){
    for(unsigned int iy=0; iy<NPIXELSY; iy++){
      h1Spectrum[ix][iy] -> SetLineColor(1);
      h1Spectrum[ix][iy] -> SetLineWidth(2);
      h1Spectrum[ix][iy] -> GetXaxis() -> SetTitle("ToT");
      h1Spectrum[ix][iy] -> GetYaxis() -> SetTitle("#frac{dN}{dToT}");
      h1Spectrum[ix][iy] -> Write();
    }
    loadBar(ix, NPIXELSX);
  }

  // writing tree
  cout << __PRETTY_FUNCTION__ << ": writing tree" << endl;  
  TTree *tree = new TTree("fit", "fit");
  unsigned int bX = 0;
  tree -> Branch("x", &bX);
  unsigned int bY = 0;
  tree -> Branch("y", &bY);
  double bScale = 0.;
  tree -> Branch("scale", &bScale);
  double bMu = 0.;
  tree -> Branch("mu", &bMu);
  double bSigma = 0.;
  tree -> Branch("sigma", &bSigma);
  double bScaleErr = 0.;
  tree -> Branch("scaleErr", &bScaleErr);
  double bMuErr = 0.;
  tree -> Branch("muErr", &bMuErr);
  double bSigmaErr = 0.;
  tree -> Branch("sigmaErr", &bSigmaErr);
  double bChi2 = 0.;
  tree -> Branch("chi2", &bChi2);
  double bNdof = 0.;
  tree -> Branch("ndof", &bNdof);
  for(unsigned int ix=0; ix<NPIXELSX; ix++){
    for(unsigned int iy=0; iy<NPIXELSY; iy++){
      bX = ix;
      bY = iy;
      bScale = parameters.scale[ix][iy];
      bMu = parameters.mu[ix][iy];
      bSigma = parameters.sigma[ix][iy];      
      bScaleErr = parameters.scale[ix][iy];
      bMuErr = parameters.muErr[ix][iy];
      bSigmaErr = parameters.sigmaErr[ix][iy];
      bChi2 = parameters.chi2[ix][iy];
      bNdof = parameters.ndof[ix][iy];      
      tree -> Fill();
    }
    loadBar(ix, NPIXELSX);    
  }
  tree -> Write();
  delete tree;
  
  // closing root file
  cout << __PRETTY_FUNCTION__ << ": closing root file" << endl;
  file -> Close();
  delete file;

  ///////////////////////////////
  // deleting spectrum histograms
  ///////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": deleting spectrum histograms" << endl;  
  deleteSpectra(h1Spectrum);
  
  return 0;
}
