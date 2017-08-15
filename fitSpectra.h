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
#define MAXMEAN 20.0

//const char *fitFunctionExpr = "[0] * exp(- ((x-[1]) * (x-[1])) / (2. * [2] * [2])) / (TMath::Sqrt(2. * TMath::Pi()) * [2])";
//const char *fitFunctionExpr = "[0]*x + [1] - [2]/(x-[3])";
const char *fitFunctionExpr = "[0]*x + [1] - [2]/(x-3.0)";

int fitSpectra(const char *fileNameAg="data/data_Ag_tree.root",
	       const char *fileNameZn="data/data_Zr_tree.root",
	       const char *fileNameCu="data/data_Cu_tree.root",
	       const char *fileNameCa="data/data_Ca_tree_lower.root",
	       const char *fileNameSn="data/data_Sn_tree.root",
	       const char *fileNameGaAs="data/data_GaAs_tree.root",
	       const char *fileNameTi  ="data/data_Ti_tree.root");
