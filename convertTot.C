#include "convertTot.h"

convertTot::convertTot(const char *fileName, const char *fitFunc): m_fileName(fileName),
								   m_fitFunctionExpr(fitFunc)
{
  Load();
}

convertTot::~convertTot() {}


void convertTot::Load(){

  TFile *file = TFile::Open(m_fileName, "READ");
  if(file == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << m_fileName << endl;
    return;
  }

  TTree *tree   = static_cast<TTree *>(file->Get("fit"));

  unsigned int bX = 0;
  tree -> SetBranchAddress("x", &bX);
  unsigned int bY = 0;
  tree -> SetBranchAddress("y", &bY);
  double bf0 = 0.;
  tree -> SetBranchAddress("f0", &bf0);
  double bf1 = 0.;
  tree -> SetBranchAddress("f1", &bf1);
  double bf2 = 0.;
  tree -> SetBranchAddress("f2", &bf2);
  double bfUp0 = 0.;
  tree -> SetBranchAddress("fUp0", &bfUp0);
  double bfUp1 = 0.;
  tree -> SetBranchAddress("fUp1", &bfUp1);
  double bfUp2 = 0.;
  tree -> SetBranchAddress("fUp2", &bfUp2);  
  
  const unsigned nEntries = tree->GetEntries();
  for(unsigned nEvt=0; nEvt<nEntries; ++nEvt){

    tree->GetEntry(nEvt);

    // parameters
    m_parameters.f0[bX][bY]=bf0;
    m_parameters.f1[bX][bY]=bf1;
    m_parameters.f2[bX][bY]=bf2;
    m_parameters.fUp0[bX][bY]=bfUp0;
    m_parameters.fUp1[bX][bY]=bfUp1;
    m_parameters.fUp2[bX][bY]=bfUp2;
  }

  delete tree;
  file->Close();
}

void convertTot::GetEnergyFromToT(const unsigned x, const unsigned y, const double ToT, double &energy, double &energy_resolution)
{

  if(x>NPIXELSX || y>NPIXELSY){
    std::cout << "ERROR - pixel x or y exceeds limit" << std::endl;
    return;
  }

  char name[100];
  sprintf(name, "fFitX%03dY%03d", x, y);
  char nameup[100];
  sprintf(nameup, "fFitUpX%03dY%03d", x, y);

  //if(fFit[x][y]==NULL){
    TF1* fFit   = new TF1(name,   m_fitFunctionExpr, 3.0, 100.0);
    TF1* fFitUp = new TF1(nameup, m_fitFunctionExpr, 3.0, 100.0);

    fFit->SetParameter(0,m_parameters.f0[x][y]);
    fFit->SetParameter(1,m_parameters.f1[x][y]);
    fFit->SetParameter(2,m_parameters.f2[x][y]);
    
    fFitUp->SetParameter(0,m_parameters.fUp0[x][y]);
    fFitUp->SetParameter(1,m_parameters.fUp1[x][y]);
    fFitUp->SetParameter(2,m_parameters.fUp2[x][y]);      
    //}

  energy = fFit->Eval(ToT);
  energy_resolution = fabs(fFitUp->Eval(ToT)-energy);
  if(energy>1000.0) std::cout << "ERROR - ToT: " << ToT << " energy: " << energy << energy_resolution << std::endl;
  //std::cout << "ToT: " << ToT << " energy: " << energy << " reso: " << energy_resolution << std::endl;
  delete fFit;
  delete fFitUp;
}
