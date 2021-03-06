#ifndef FIT_HH
#define FIT_HH

#include <iostream>
#include <fstream>
#include <algorithm>    // std::sort
#include <vector> 
using namespace std;

#include "progressBar.h"

#include <TH1.h>
#include <TF1.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TError.h>
#include <TPad.h>
#include <TCanvas.h>

#define NPIXELSX 256
#define NPIXELSY 256
#define TOTNBINS 100
#define TOTMIN 0
#define TOTMAX TOTNBINS
#define SIGMATOFWHM 2.355
#define NSIGMALEFT 3.
#define NSIGMARIGHT 3.
#define NBINSDISTRIBUTIONS 100
#define MAXMEAN 24.0 //  Ca 24 GaAs 29 Ag 53 Cu 24 Sn 60 Zr 35 Ti 20
 
const char *elementInfoFileName = "fluorescenceData.dat";
//const char *fitFunctionExpr = "[0] * exp(- ((x-[1]) * (x-[1])) / (2. * [2] * [2])) / (TMath::Sqrt(2. * TMath::Pi()) * [2])";
//const char *fitFunctionExpr = "[0]*exp(-0.5*((x-[1])/[2])^2)";
//const char *fitFunctionExpr = "[0]*exp(-0.5*((x-[1])/[2])^2)+ (([3]+[4]*x)>0 ? ([3]+[4]*x) : 0.0 )";
const char *fitFunctionExpr = "[0]*exp(-0.5*((x-[1])/[2])^2) + [3]*exp(-0.5*((x-[4])/[5])^2)";
/////////////////////////////////////////////////////////////////////////////////////////////////////
// element //////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

int fitSPIDR(const char *fileNameBase="/tmp/data_Ca",
	const char *elementName="");
int DoFittingSPIDR(const char *fileNameBase="/tmp/data_Ca",
	const char *elementName="");

typedef struct element{
  string name;
  unsigned int Z;
  double ka;
  double kb;
  void print(){
    cout << __PRETTY_FUNCTION__ << ": name = " << name << endl;
    cout << __PRETTY_FUNCTION__ << ": Z = " << Z << endl;
    cout << __PRETTY_FUNCTION__ << ": ka = " << ka << endl;
    cout << __PRETTY_FUNCTION__ << ": kb = " << kb << endl;
    return ;
  }
} element_t;

/////////////////////////////////////////////////////////////////////////////////////////////////////
// parameters ///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

struct evt{
  std::string a;
  double time;
};
// sorting function
bool myTimeSort(evt left, evt right){ return left.time < right.time; }

typedef struct parameters{
  double scale[NPIXELSX][NPIXELSY];  
  double mu[NPIXELSX][NPIXELSY];
  double sigma[NPIXELSX][NPIXELSY];
  double scaleErr[NPIXELSX][NPIXELSY];    
  double muErr[NPIXELSX][NPIXELSY];
  double sigmaErr[NPIXELSX][NPIXELSY];
  double chi2[NPIXELSX][NPIXELSY];
  double ndof[NPIXELSX][NPIXELSY];
} parameters_t;

/////////////////////////////////////////////////////////////////////////////////////////////////////
// getElementInfo ///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

int getElementInfo(const char *elementName,
		   element_t &element){

  element.name = elementName;
  
  ifstream file(elementInfoFileName);
  if(!file){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << elementInfoFileName << endl;
    return 1;
  }

  bool found = false;
  string line = "";
  char tmp[2];
  while(getline(file, line)){
    if(line[0] == elementName[0] && line[1] == elementName[1]){
      found = true;
      sscanf(line.c_str(), "%s\t%d\t%lf\t%lf", tmp, &element.Z, &element.ka, &element.kb);
    }
  }  
  file.close();

  if(!found){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - element not found in database: " << elementName << endl;
    return 1;
  }

  element.print();
  
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// allocateSpectra //////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

TH1F ***allocateSpectra(){
  TH1F ***h1Spectrum = new TH1F**[NPIXELSX];
  char name[100];
  char title[100];
  for(unsigned int ix=0; ix<NPIXELSX; ix++){
    h1Spectrum[ix] = new TH1F*[NPIXELSY];
    for(unsigned int iy=0; iy<NPIXELSY; iy++){
      sprintf(name, "h1SpectrumX%03dY%03d", ix, iy);
      sprintf(title, "spectrum X %03d Y %03d", ix, iy);
      h1Spectrum[ix][iy] = new TH1F(name, title, TOTNBINS, TOTMIN, TOTMAX);
    }
  }
  return h1Spectrum;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// deleteSpectra ////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void deleteSpectra(TH1F ***h1Spectrum){  
  for(unsigned int ix=0; ix<NPIXELSX; ix++){
    for(unsigned int iy=0; iy<NPIXELSY; iy++){
      delete h1Spectrum[ix][iy];
    }
    delete[] h1Spectrum[ix];
  }
  delete[] h1Spectrum;
  return ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// time sort /////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
int sortSpectra(const char *fileName,const char *fileNameOut){
  
  cout << __PRETTY_FUNCTION__ << ": sorting..." << endl;
  ifstream fileIn(fileName);
  if(!fileIn){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileName << endl;
    return 1;
  }
  ofstream fileOut(fileNameOut,std::ofstream::out);
  if(!fileOut){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameOut << endl;
    return 1;
  }
  string line = "";
  std::vector<evt> evt_vec, evt_vec2;
  
  unsigned int pixelX = 0;
  unsigned int pixelY = 0;
  unsigned int ToT = 0;
  double ToA = 0.;
  double dummy = 0.0;
  double time = 0.0;
  //unsigned int nLines = 0;
  unsigned count =0;
  unsigned nRestartTimer=0;
  while(getline(fileIn, line)){

    //std::cout << "line: " << line << std::endl;
    if(count%10000000==0) cout << __PRETTY_FUNCTION__ << ": Events proc: " << count <<std::endl;

    if(line=='[') { continue; }

    sscanf(line.c_str(), "%d %d %d %lf %lf %lf", &pixelX, &pixelY, &ToT, &ToA, &dummy, &time);
    evt e;
    e.a = line;
    e.time = time;

    if(time<10000.0){
      evt_vec2.push_back(e);
      ++nRestartTimer;
    }else{
      evt_vec.push_back(e);
      if(nRestartTimer>0) ++nRestartTimer;
    }
    if(nRestartTimer>100){
      std::sort(evt_vec.begin(), evt_vec.end(), myTimeSort);
      //write sorted
      for(unsigned i=0; i<evt_vec.size(); ++i){ fileOut << evt_vec.at(i).a << std::endl; }
      // clear
      evt_vec.clear();
      evt_vec = evt_vec2;
      evt_vec2.clear();
      nRestartTimer=0;
    }

    count ++;
    //loadBar(count, nLines);
  }

  std::sort(evt_vec.begin(), evt_vec.end(), myTimeSort);
  //write sorted
  for(unsigned i=0; i<evt_vec.size(); ++i){ fileOut << evt_vec.at(i).a << std::endl; }
  evt_vec.clear();

  fileOut.close();
  fileIn.close();
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// buildSpectra /////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

int buildSpectra(const char *fileName,
		 TH1F ***h1Spectrum){

  cout << __PRETTY_FUNCTION__ << ": measuring file size" << endl;
  ifstream fileIn(fileName);
  if(!fileIn){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileName << endl;
    return 1;
  }
  string line = "";
  string prevline = "";
  unsigned int nLines = 0;
  while(getline(fileIn, line)){
    if(nLines%10000000==0) cout << __PRETTY_FUNCTION__ << ": Events proc: " << nLines <<std::endl;
    nLines ++;
    //if(nLines>1000) break;
  }
  fileIn.close();

  cout << __PRETTY_FUNCTION__ << ": reading data" << endl;
  fileIn.open(fileName);
  unsigned int pixelX = 0;
  unsigned int pixelY = 0;
  unsigned int ToT = 0;
  double ToA = 0.;
  double dummy = 0.0;
  double time = 0.0;

  unsigned int prev_pixelX = 0;
  unsigned int prev_pixelY = 0;
  unsigned int prev_ToT = 0;
  double prev_ToA = 0.;
  double prev_dummy = 0.0;
  double prev_time = 0.0;

  unsigned int count = 0;
  bool MoreHits=false;
  while(getline(fileIn, line)){

    if(count==0) { prevline=line; getline(fileIn, line);
      sscanf(prevline.c_str(), "%d %d %d %lf %lf %lf", &prev_pixelX, &prev_pixelY, &prev_ToT, &prev_ToA, &prev_dummy, &prev_time);
      ++count; continue; 
    }
    else{
      //prevline=line; 
      sscanf(prevline.c_str(), "%d %d %d %lf %lf %lf", &prev_pixelX, &prev_pixelY, &prev_ToT, &prev_ToA, &prev_dummy, &prev_time);
    }
    if(line=='[') { continue; }

    sscanf(line.c_str(), "%d %d %d %lf %lf %lf", &pixelX, &pixelY, &ToT, &ToA, &dummy, &time);

    prevline=line; 
    if(fabs(prev_time-time)<5.0){
      MoreHits=true;
      continue;
    }else if(MoreHits){
      MoreHits=false;
      continue;
    } else{
      MoreHits=false;
    }

    /*cout << __PRETTY_FUNCTION__ << "pixelX: " << pixelX
	 << " pixelY: " << pixelY
	 << " ToT: " << ToT
	 << " ToA: " << ToA
	 << " time: " << time
	 << endl;*/
    h1Spectrum[prev_pixelX][prev_pixelY] -> Fill(prev_ToT);
    count ++;
    loadBar(count, nLines);
  }  
  fileIn.close();

  return 0;
}

int saveParameters(const char *fileName,
		   parameters_t parameters){
  
  cout << __PRETTY_FUNCTION__ << ": " << fileName << endl;
  
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// saveResults //////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

int saveResults(const char *fileName,
		TH1F ***h1Spectrum,
		const parameters_t parameters){

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// initializeParameters /////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void initializeParameters(TH1F ***h1Spectrum,
			  parameters_t &parameters){

  for(unsigned int ix=0; ix<NPIXELSX; ix++){
    for(unsigned int iy=0; iy<NPIXELSY; iy++){

      // maximum bin
      unsigned int maxBin = h1Spectrum[ix][iy] -> GetMaximumBin();

      // half integral, used to initialize the fit
      double halfIntegral = 0.;
      for(unsigned int iBin=0; iBin<maxBin; iBin++){
	halfIntegral += h1Spectrum[ix][iy] -> GetBinContent(iBin);
      }
      halfIntegral += 0.5 * h1Spectrum[ix][iy] -> GetBinContent(maxBin);
      
      // peak position, used to initialize the fit = X position of maximum bin
      double peakPosition = h1Spectrum[ix][iy] -> GetBinLowEdge(maxBin);

      // FWHM, measured on the left of the peak, used to initialize the fit and set the range
      const double halfMaximum = h1Spectrum[ix][iy] -> GetBinContent(maxBin) / 2.;
      double FWHM = 0.;
      for(unsigned int iBin=maxBin; iBin>=1; --iBin){
	const double val = h1Spectrum[ix][iy] -> GetBinContent(iBin);
	if(val < halfMaximum){
	  FWHM = 2. * (peakPosition - h1Spectrum[ix][iy] -> GetBinCenter(iBin));
	  break;
	}
      }

      // sigma, computed from the FWHM, used to initialize the fit and set the range
      const double sigma = FWHM / SIGMATOFWHM;

      parameters.scale[ix][iy] = 2. * halfIntegral;      
      parameters.mu[ix][iy] = peakPosition;
      parameters.sigma[ix][iy] = sigma;
      parameters.scaleErr[ix][iy] = 0.;
      parameters.muErr[ix][iy] = 0.;
      parameters.sigmaErr[ix][iy] = 0.;
      parameters.chi2[ix][iy] = 0.;
      parameters.ndof[ix][iy] = 0;      
    }
    loadBar(ix, NPIXELSX);
  }
  
  return ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// fitSpectra ///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

int fitSpectra(TH1F ***h1Spectrum,
	       parameters_t &parameters,
	       const char *fileName){

  // initializing parameters
  cout << __PRETTY_FUNCTION__ << ": initializing parameters" << endl;
  initializeParameters(h1Spectrum, parameters);

  std::string CompName ="Ca";
  
  TCanvas *can = new  TCanvas("c", "c", 500, 500);
  
  // fitting
  cout << __PRETTY_FUNCTION__ << ": fitting" << endl;
  for(unsigned int ix=0; ix<NPIXELSX; ix++){
    for(unsigned int iy=0; iy<NPIXELSY; iy++){
      char name[100];
      sprintf(name, "fFitX%03dY%03d", ix, iy);
      // fit range initialization
      const double rangeMin = parameters.mu[ix][iy] - NSIGMALEFT * parameters.mu[ix][iy];
      const double rangeMax = parameters.mu[ix][iy] + NSIGMARIGHT * parameters.mu[ix][iy];
      //TF1 *fFit = new TF1(name, fitFunctionExpr, rangeMin, rangeMax);
      TF1 *fFit= NULL;
      if(CompName=="Sn")        fFit = new TF1(name, fitFunctionExpr, 25.0, MAXMEAN); 
      else if(CompName=="Zr")   fFit = new TF1(name, fitFunctionExpr, 15.0, MAXMEAN); 
      else if(CompName=="Cu")   fFit = new TF1(name, fitFunctionExpr, 4.0, 24.0);  
      else if(CompName=="GaAs") fFit = new TF1(name, fitFunctionExpr, 4.0, 26.0);  
      else if(CompName=="Ca")   fFit = new TF1(name, fitFunctionExpr, 4.0, 19.0); 
      else if(CompName=="Ag")   fFit = new TF1(name, fitFunctionExpr, 25.0, MAXMEAN); 
      else if(CompName=="Ti")   fFit = new TF1(name, fitFunctionExpr, 4.0, 26.0); 

      // first fit
      fFit -> SetParNames("scale", "#mu", "#sigma");

      // Zn
      if(CompName=="Zr") {
	fFit -> SetParLimits(0, 5., 500.);
	fFit -> SetParLimits(1, 7.0, MAXMEAN);
	fFit -> SetParLimits(2, 0.2, 5.);
	fFit -> SetParLimits(3, 2., 50.);
	fFit -> SetParLimits(4, 3.0, 24.0);
	fFit -> SetParLimits(5, 3.0, 10.);
	fFit -> SetParameter(0, 150.0);
	fFit -> SetParameter(1, 25.0);
	fFit -> SetParameter(2, 0.9);
	fFit -> SetParameter(3, 18.0);
	fFit -> SetParameter(4, 20.0);       
	fFit -> SetParameter(5, 5.0);
      }
      
      // Sn
      if(CompName=="Sn"){
	fFit -> SetParLimits(0, 30., 150.);
	fFit -> SetParLimits(1, 30.0, MAXMEAN);
	fFit -> SetParLimits(2, 0.75, 5.);
	//fFit -> SetParLimits(3, 0., 100.);
	//fFit -> SetParLimits(4, -20., 0.);
	fFit -> SetParLimits(3, 2., 15.);
	fFit -> SetParLimits(4, 20.0, MAXMEAN);      
	fFit -> SetParLimits(5, 2.5, 10.);      
	/*
	  fFit -> SetParameter(0, parameters.scale[ix][iy]);
	  fFit -> SetParameter(1, parameters.mu[ix][iy]);
	  fFit -> SetParameter(2, parameters.sigma[ix][iy]);
	  fFit -> SetParameter(3, 3.0);
	*/
	
	fFit -> SetParameter(0, 35.0);
	fFit -> SetParameter(1, 40.0);
	fFit -> SetParameter(2, 1.0);
	//fFit -> SetParameter(3, 3.0);
	//fFit -> SetParameter(4, -1.0);
	fFit -> SetParameter(3, 5.0);
	fFit -> SetParameter(4, 30.0);            
	fFit -> SetParameter(5, 5.0);
      }

      // Cu
      if(CompName=="Cu"){
	fFit -> SetParLimits(0, 5., 500.);
	fFit -> SetParLimits(1, 7.0, MAXMEAN);
	fFit -> SetParLimits(2, 0.2, 5.);
	fFit -> SetParLimits(3, 2., 120.);
	fFit -> SetParLimits(4, 3.0, 10.0);
	fFit -> SetParLimits(5, 3.0, 10.);
	fFit -> SetParameter(0, 150.0);
	fFit -> SetParameter(1, 15.0);
	fFit -> SetParameter(2, 0.5);
	fFit -> SetParameter(3, 25.0);
	fFit -> SetParameter(4, 5.0);       
	fFit -> SetParameter(5, 5.0);
      }
      // Ag
      if(CompName=="Ag"){
	fFit -> SetParLimits(0, 5., 200.);
	fFit -> SetParLimits(1, 25.0, MAXMEAN);
	fFit -> SetParLimits(2, 0.2, 5.);
	fFit -> SetParLimits(3, 2., 120.);
	fFit -> SetParLimits(4, 3.0, 40.0);
	fFit -> SetParLimits(5, 3.0, 10.);
	fFit -> SetParameter(0, 150.0);
	fFit -> SetParameter(1, 33.0);
	fFit -> SetParameter(2, 0.5);
	fFit -> SetParameter(3, 10.0);
	fFit -> SetParameter(4, 30.0);       
	fFit -> SetParameter(5, 5.0);
      }
      
      // GaAs
      if(CompName=="GaAs"){
	fFit -> SetParLimits(0, 5., 500.);
	fFit -> SetParLimits(1, 7.0, MAXMEAN);
	fFit -> SetParLimits(2, 0.2, 5.);
	fFit -> SetParLimits(3, 2., 120.);
	fFit -> SetParLimits(4, 3.0, 10.0);
	fFit -> SetParLimits(5, 3.0, 10.);
	fFit -> SetParameter(0, 150.0);
	fFit -> SetParameter(1, 15.0);
	fFit -> SetParameter(2, 0.5);
	fFit -> SetParameter(3, 25.0);
	fFit -> SetParameter(4, 5.0);       
	fFit -> SetParameter(5, 5.0);
      }
      // Ca
      if(CompName=="Ca"){
	fFit -> SetParLimits(0, 5., 500.);
	fFit -> SetParLimits(1, 7.0, MAXMEAN);
	fFit -> SetParLimits(2, 0.2, 3.);
	fFit -> SetParLimits(3, 2., 500.);
	fFit -> SetParLimits(4, 3.0, 14.0);
	fFit -> SetParLimits(5, 0.2, 3.);
	fFit -> SetParameter(0, 80.0);
	fFit -> SetParameter(1, 15.0);
	fFit -> SetParameter(2, 0.5);
	fFit -> SetParameter(3, 150.0);
	fFit -> SetParameter(4, 7.0);       
	fFit -> SetParameter(5, 2.5);
      }
      // Ti
      if(CompName=="Ti"){
	fFit -> SetParLimits(0, 5., 500.);
	fFit -> SetParLimits(1, 4.0, MAXMEAN);
	fFit -> SetParLimits(2, 0.2, 5.);
	fFit -> SetParLimits(3, 2., 50.);
	fFit -> SetParLimits(4, 3.0, 14.0);
	fFit -> SetParLimits(5, 3, 7.);
	fFit -> SetParameter(0, 250.0);
	fFit -> SetParameter(1, 9.0);
	fFit -> SetParameter(2, 1.0);
	fFit -> SetParameter(3, 5.0);
	fFit -> SetParameter(4, 7.0);       
	fFit -> SetParameter(5, 4);
      }
            
      h1Spectrum[ix][iy] -> Fit(fFit, "Q && R M P"); // memory leak in the TLinearFitter class, bug in some root versions
      // second fit
      /*
      fFit -> SetRange(fFit -> GetParameter(1) - NSIGMALEFT * fFit -> GetParameter(2), fFit -> GetParameter(1) + NSIGMARIGHT * fFit -> GetParameter(2));
      fFit -> SetParameter(0, fFit -> GetParameter(0));
      fFit -> SetParameter(1, fFit -> GetParameter(1));
      fFit -> SetParameter(2, fFit -> GetParameter(2));
      fFit -> SetParameter(3, fFit -> GetParameter(3));
      //fFit -> SetParameter(4, fFit -> GetParameter(4));      
      h1Spectrum[ix][iy] -> Fit(fFit, "Q && R"); // memory leak in the TLinearFitter class, bug in some root versions
      */
      if(fFit -> GetParameter(2)>30.0){
	can->Clear();
	h1Spectrum[ix][iy] ->Draw();
	can->Update();
	can->WaitPrimitive();
	std::cout <<"mean: " << fFit -> GetParameter(0) << " " << fFit -> GetParameter(1) << " " << fFit -> GetParameter(2) << " " << fFit -> GetParameter(3)
		  << " " << fFit -> GetParameter(4)
		  << " " << h1Spectrum[ix][iy]->GetBinContent(20)
		  << " " << h1Spectrum[ix][iy]->GetBinError(20)	  
		  << std::endl;	
	}
      
      // retrieving parameters
      if(CompName=="Ca"){
	parameters.scale[ix][iy] = fFit -> GetParameter(3);
	parameters.mu[ix][iy] = fFit -> GetParameter(4);
	parameters.sigma[ix][iy] = fFit -> GetParameter(5);
	parameters.scaleErr[ix][iy] = fFit -> GetParError(3);
	parameters.muErr[ix][iy] = fFit -> GetParError(4);
	parameters.sigmaErr[ix][iy] = fFit -> GetParError(5);
      }else{
	parameters.scale[ix][iy] = fFit -> GetParameter(0);
	parameters.mu[ix][iy] = fFit -> GetParameter(1);
	parameters.sigma[ix][iy] = fFit -> GetParameter(2);
	parameters.scaleErr[ix][iy] = fFit -> GetParError(0);
	parameters.muErr[ix][iy] = fFit -> GetParError(1);
	parameters.sigmaErr[ix][iy] = fFit -> GetParError(2);
      }
      parameters.chi2[ix][iy] = fFit -> GetChisquare();
      parameters.ndof[ix][iy] = fFit -> GetNDF();


      
      delete fFit;
    }
    loadBar(ix, NPIXELSX);    
  }
  
  // saving parameters
  cout << __PRETTY_FUNCTION__ << ": saving parameters" << endl;
  ofstream file(fileName);
  if(!file){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileName << endl;
    return 1;
  }  
  for(unsigned int ix=0; ix<NPIXELSX; ix++){
    for(unsigned int iy=0; iy<NPIXELSY; iy++){
      file << parameters.scale[ix][iy] << "\t"
  	   << parameters.mu[ix][iy] << "\t"
  	   << parameters.sigma[ix][iy] << "\t"
  	   << parameters.scaleErr[ix][iy] << "\t"
  	   << parameters.muErr[ix][iy] << "\t"
  	   << parameters.sigmaErr[ix][iy] << "\t"
  	   << parameters.chi2[ix][iy] << "\t"
  	   << parameters.ndof[ix][iy] << endl;
    }
    loadBar(ix, NPIXELSX);
  }
  file.close();

  return 0;
}

#endif
