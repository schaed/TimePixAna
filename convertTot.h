#ifndef CONVERTTOT
#define CONVERTTOT
#include <iostream>
#include <fstream>
#include <algorithm>    // std::sort
#include <vector> 
using namespace std;

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

class convertTot{

 public:  
  //convertTot(const char *fileName="data/fout.root", const char *fitFunc = "[0]*x + [1] - [2]/(x-3.0)");
  convertTot(const char *fileName="data/foutnew.root", const char *fitFunc = "-([1]-3*[0]-x)/2.0/[0] + sqrt(([1]-3*[0]-x)*([1]-3*[0]-x) +4.0*[0]*(3*[1]+[2]-3.0*x) )/2.0/[0]");
  virtual ~convertTot();

  
  void Load();

  void GetEnergyFromToT(const unsigned x, const unsigned y, const double ToT, double &energy, double &energy_resolution);
  void DrawAll();
  void CheckAll();  
 typedef struct parameters{
 double f0[NPIXELSX][NPIXELSY];  
 double f1[NPIXELSX][NPIXELSY];
 double f2[NPIXELSX][NPIXELSY];
 double fUp0[NPIXELSX][NPIXELSY];    
 double fUp1[NPIXELSX][NPIXELSY];
 double fUp2[NPIXELSX][NPIXELSY];
 } parameters;

 private:

  const char *m_fileName;
  const char *m_fitFunctionExpr;

  parameters m_parameters;
  //TF1 *fFit[NPIXELSX][NPIXELSY];
  //TF1 *fFitUp[NPIXELSX][NPIXELSY];  
};
#endif
