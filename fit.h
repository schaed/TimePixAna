#ifndef FIT_HH
#define FIT_HH

#include <iostream>
#include <fstream>
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
#define NSIGMALEFT 5.
#define NSIGMARIGHT 1.
#define NBINSDISTRIBUTIONS 100

const char *elementInfoFileName = "fluorescenceData.dat";
const char *fitFunctionExpr = "[0] * exp(- ((x-[1]) * (x-[1])) / (2. * [2] * [2])) / (TMath::Sqrt(2. * TMath::Pi()) * [2])";

/////////////////////////////////////////////////////////////////////////////////////////////////////
// element //////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

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
  unsigned int nLines = 0;
  while(getline(fileIn, line)){
    nLines ++;
  }
  fileIn.close();

  cout << __PRETTY_FUNCTION__ << ": reading data" << endl;
  fileIn.open(fileName);
  unsigned int pixelX = 0;
  unsigned int pixelY = 0;
  unsigned int ToT = 0;
  double ToA = 0.;
  unsigned int count = 0;
  while(getline(fileIn, line)){
    if(line[0] != '[') continue;
    sscanf(line.c_str(), "[%d, %d, %d, %lf]", &pixelX, &pixelY, &ToT, &ToA);
    h1Spectrum[pixelX][pixelY] -> Fill(ToT);
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

  // fitting
  cout << __PRETTY_FUNCTION__ << ": fitting" << endl;
  for(unsigned int ix=0; ix<NPIXELSX; ix++){
    for(unsigned int iy=0; iy<NPIXELSY; iy++){
      char name[100];
      sprintf(name, "fFitX%03dY%03d", ix, iy);
      // fit range initialization
      const double rangeMin = parameters.mu[ix][iy] - NSIGMALEFT * parameters.mu[ix][iy];
      const double rangeMax = parameters.mu[ix][iy] + NSIGMARIGHT * parameters.mu[ix][iy];
      TF1 *fFit = new TF1(name, fitFunctionExpr, rangeMin, rangeMax); 
      // first fit
      fFit -> SetParNames("scale", "#mu", "#sigma");
      fFit -> SetParLimits(0, 0., 1000000.);
      fFit -> SetParLimits(1, 0., 1000000.);
      fFit -> SetParLimits(2, 0., 1000000.);
      fFit -> SetParameter(0, parameters.scale[ix][iy]);
      fFit -> SetParameter(1, parameters.mu[ix][iy]);
      fFit -> SetParameter(2, parameters.sigma[ix][iy]);
      h1Spectrum[ix][iy] -> Fit(fFit, "Q && R"); // memory leak in the TLinearFitter class, bug in some root versions
      // second fit
      fFit -> SetRange(fFit -> GetParameter(1) - NSIGMALEFT * fFit -> GetParameter(2), fFit -> GetParameter(1) + NSIGMARIGHT * fFit -> GetParameter(2));
      fFit -> SetParameter(0, fFit -> GetParameter(0));
      fFit -> SetParameter(1, fFit -> GetParameter(1));
      fFit -> SetParameter(2, fFit -> GetParameter(2));
      h1Spectrum[ix][iy] -> Fit(fFit, "Q && R"); // memory leak in the TLinearFitter class, bug in some root versions
      // retrieving parameters
      parameters.scale[ix][iy] = fFit -> GetParameter(0);
      parameters.mu[ix][iy] = fFit -> GetParameter(1);
      parameters.sigma[ix][iy] = fFit -> GetParameter(2);
      parameters.scaleErr[ix][iy] = fFit -> GetParError(0);
      parameters.muErr[ix][iy] = fFit -> GetParError(1);
      parameters.sigmaErr[ix][iy] = fFit -> GetParError(2);
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
