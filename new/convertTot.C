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
  double bf3 = 0.;
  tree -> SetBranchAddress("f3", &bf3);  
  double bfUp0 = 0.;
  tree -> SetBranchAddress("fUp0", &bfUp0);
  double bfUp1 = 0.;
  tree -> SetBranchAddress("fUp1", &bfUp1);
  double bfUp2 = 0.;
  tree -> SetBranchAddress("fUp2", &bfUp2);
  double bfUp3 = 0.;
  tree -> SetBranchAddress("fUp3", &bfUp3);    
  
  const unsigned nEntries = tree->GetEntries();

  if(nEntries==1){
    tree->GetEntry(0);

    for(unsigned x=0; x<256; ++x){
      for(unsigned y=0; y<256; ++y){      
	// parameters
	m_parameters.f0[x][y]  =bf0;
	m_parameters.f1[x][y]  =bf1;
	m_parameters.f2[x][y]  =bf2;
	m_parameters.f3[x][y]  =bf3;
	m_parameters.fUp0[x][y]=bfUp0;
	m_parameters.fUp1[x][y]=bfUp1;
	m_parameters.fUp2[x][y]=bfUp2;
	m_parameters.fUp3[x][y]=bfUp3;
      }
    }
  } else{
    for(unsigned nEvt=0; nEvt<nEntries; ++nEvt){
      
      tree->GetEntry(nEvt);
      
      // parameters
      m_parameters.f0[bX][bY]=bf0;
      m_parameters.f1[bX][bY]=bf1;
      m_parameters.f2[bX][bY]=bf2;
      m_parameters.f3[bX][bY]=bf3;
      m_parameters.fUp0[bX][bY]=bfUp0;
      m_parameters.fUp1[bX][bY]=bfUp1;
      m_parameters.fUp2[bX][bY]=bfUp2;
      m_parameters.fUp3[bX][bY]=bfUp3;
    }
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
    fFit->SetParameter(3,m_parameters.f3[x][y]);
    //std::cout << "params: " << m_parameters.f0[x][y] << " " << m_parameters.f1[x][y] << " " << m_parameters.f2[x][y] << std::endl;
    
    fFitUp->SetParameter(0,m_parameters.fUp0[x][y]);
    fFitUp->SetParameter(1,m_parameters.fUp1[x][y]);
    fFitUp->SetParameter(2,m_parameters.fUp2[x][y]);      
    fFitUp->SetParameter(3,m_parameters.fUp3[x][y]);      
    //}

  energy = fFit->Eval(ToT);
  energy_resolution = fabs(fFitUp->Eval(ToT)-energy);
  // calibration from Fe55 data
  energy*=0.92;
  if(energy>1000.0) std::cout << "ERROR - ToT: " << ToT << " energy: " << energy << energy_resolution << std::endl;
  //std::cout << "ToT: " << ToT << " energy: " << energy << " reso: " << energy_resolution << std::endl;
  delete fFit;
  delete fFitUp;
}

void convertTot::DrawAll()
{
  std::string opt="";
  TCanvas *can = new TCanvas("fit_func","fit_func",500,600);
  TH2F *hh = new TH2F("density","density",50,0.0,50.0, 200,0.0,50.0);
  can->cd();
  for(unsigned x=0; x<NPIXELSX; ++x){
    std::cout << "x: " << x << std::endl;
    for(unsigned y=0; y<NPIXELSY; ++y){    
      char name[100];
      sprintf(name, "fFitX%03dY%03d", x, y);
      char nameup[100];
      sprintf(nameup, "fFitUpX%03dY%03d", x, y);
      
      TF1* fFit   = new TF1(name,   m_fitFunctionExpr, 3.0, 40.0);
      
      fFit->SetParameter(0,m_parameters.f0[x][y]);
      fFit->SetParameter(1,m_parameters.f1[x][y]);
      fFit->SetParameter(2,m_parameters.f2[x][y]);
      fFit->SetParameter(3,m_parameters.f3[x][y]);
      //std::cout << "params: " << m_parameters.f0[x][y] << " " << m_parameters.f1[x][y] << " " << m_parameters.f2[x][y] << std::endl;
      //energy = fFit->Eval(ToT);
      //delete fFit;
      //fFit->Draw(opt.c_str());
      //if(opt=="") opt="same";
      for(unsigned i=3; i<50; ++i){
	hh->Fill(i,fFit->Eval(double(i)));
      }
      delete fFit;
    } // end y
  } // end x
  std::cout << "Canvas!" << std::endl;
  hh->Draw("colz");
  can->Update();
  can->WaitPrimitive();
  can->SaveAs("h1.root");
  delete can;
}

void convertTot::DrawAllReso()
{
  std::string opt="";
  TCanvas *can = new TCanvas("fit_func","fit_func",500,600);
  TH2F *hh = new TH2F("density","density",30,0.0,30.0, 200,0.0,2.0);
  can->cd();
  for(unsigned x=0; x<NPIXELSX; ++x){
    std::cout << "x: " << x << std::endl;
    for(unsigned y=0; y<NPIXELSY; ++y){    
      char name[100];
      sprintf(name, "fFitX%03dY%03d", x, y);
      char nameup[100];
      sprintf(nameup, "fFitUpX%03dY%03d", x, y);
      
      TF1* fFit   = new TF1(name,   m_fitFunctionExpr, 3.0, 40.0);
      
      fFit->SetParameter(0,m_parameters.f0[x][y]);
      fFit->SetParameter(1,m_parameters.f1[x][y]);
      fFit->SetParameter(2,m_parameters.f2[x][y]);
      fFit->SetParameter(3,m_parameters.f3[x][y]);

      TF1* fFitUp = new TF1(nameup, m_fitFunctionExpr, 3.0, 40.0);

      fFitUp->SetParameter(0,m_parameters.fUp0[x][y]);
      fFitUp->SetParameter(1,m_parameters.fUp1[x][y]);
      fFitUp->SetParameter(2,m_parameters.fUp2[x][y]); 
      fFitUp->SetParameter(3,m_parameters.fUp3[x][y]); 
      
      //energy = fFit->Eval(ToT);
      //delete fFit;
      //fFit->Draw(opt.c_str());
      //if(opt=="") opt="same";
      for(unsigned i=3; i<30; ++i){
	hh->Fill(i,(fFitUp->Eval(double(i)) - fFit->Eval(double(i)))/fFit->Eval(double(i)));
      }
      delete fFit;
      delete fFitUp;
    } // end y
  } // end x
  std::cout << "Canvas!" << std::endl;
  hh->Draw("colz");
  can->Update();
  can->WaitPrimitive();
  can->SaveAs("h2.root");
  delete can;
}

// tot for 8keV & tot for a source
// draw the energy value  for each source width

void convertTot::CheckAll()
{
  std::string opt="";
  //TCanvas *can = new TCanvas("fit_func","fit_func",500,600);
  //can->cd();
  for(unsigned x=0; x<NPIXELSX; ++x){
    //std::cout << "x: " << x << std::endl;
    for(unsigned y=0; y<NPIXELSY; ++y){    
      char name[100];
      sprintf(name, "fFitX%03dY%03d", x, y);
      char nameup[100];
      sprintf(nameup, "fFitUpX%03dY%03d", x, y);
      
      TF1* fFit   = new TF1(name,   m_fitFunctionExpr, 3.0, 40.0);
      
      fFit->SetParameter(0,m_parameters.f0[x][y]);
      fFit->SetParameter(1,m_parameters.f1[x][y]);
      fFit->SetParameter(2,m_parameters.f2[x][y]);
      fFit->SetParameter(3,m_parameters.f3[x][y]);
      
      double energy = fFit->Eval(40.0);
      //if(energy>25.0) std::cout << "ToT: 25 en: " << energy << " x " << x << " y " << y << std::endl;
      //energy = fFit->Eval(5.0);      
      if(energy<18.0) std::cout << "ToT: 40 en: " << energy << " x " << x << " y " << y << std::endl;      
      
      delete fFit;
      //fFit->Draw(opt.c_str());
      //if(opt=="") opt="same";
    } // end y
  } // end x
  //std::cout << "Canvas!" << std::endl;
  //can->Update();
  //can->WaitPrimitive();
  //can->SaveAs("h1.root");
  //delete can;
}

void convertTot::DrawTot()
{
  std::string opt="";
  TCanvas *can = new TCanvas("fit_func","fit_func",500,600);
  can->cd();
  TFile *f = TFile::Open("data/data_Cu.root","READ");
  TH1F *hh=NULL;
  for(unsigned x=0; x<NPIXELSX; ++x){
    std::cout << "x: " << x << std::endl;
    for(unsigned y=0; y<NPIXELSY; ++y){    
      char name[100];
      sprintf(name, "h1SpectrumX%03dY%03d", x, y);
      TH1F* h   = static_cast<TH1F*>(f->Get(name));
      if(x==0 && y==0) hh = static_cast<TH1F*>(h->Clone());
      hh->Add(h);
      delete h;
    } // end y
  } // end x
  //std::cout << "Canvas!" << std::endl;
  hh->Draw();
  can->Update();
  can->WaitPrimitive();
  //can->SaveAs("h1.root");
  //delete can;
}

void convertTot::Correlate()
{

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
  
  std::string opt="";
  TCanvas *can = new TCanvas("fit_func","fit_func",500,600);
  can->cd();
  for(unsigned x=0; x<NPIXELSX; ++x){
    std::cout << "x: " << x << std::endl;
    for(unsigned y=0; y<NPIXELSY; ++y){    
      char name[100];
      sprintf(name, "fFitX%03dY%03d", x, y);
      char nameup[100];
      sprintf(nameup, "fFitUpX%03dY%03d", x, y);
      
      TF1* fFit   = new TF1(name,   m_fitFunctionExpr, 1.0, 40.0);
      TF1* fFitCharge   = new TF1(name,   m_fitFunctionExpr, 1.0, 40.0);      
      
      fFit->SetParameter(0,m_parameters.f0[x][y]);
      fFit->SetParameter(1,m_parameters.f1[x][y]);
      fFit->SetParameter(2,m_parameters.f2[x][y]);
      fFit->SetParameter(3,3.0);

      double ratio =charge_parameters.f0[x][y]/ m_parameters.f0[x][y];
      std::cout << "ratio: " << ratio << std::endl;
      std::cout << "charge_parameters.f0[x][y]: " << charge_parameters.f0[x][y] << " " << m_parameters.f0[x][y] << std::endl;
      std::cout << "charge_parameters.f1[x][y]: " << charge_parameters.f1[x][y] << " " << m_parameters.f1[x][y] << std::endl;
      std::cout << "charge_parameters.f2[x][y]: " << charge_parameters.f2[x][y] << " " << m_parameters.f2[x][y] << std::endl;
      std::cout << "charge_parameters.f3[x][y]: " << charge_parameters.f3[x][y] << " " << m_parameters.f3[x][y] << std::endl;
      fFitCharge->SetParameter(0,m_parameters.f0[x][y]);
      fFitCharge->SetParameter(1,m_parameters.f1[x][y]);
      fFitCharge->SetParameter(2,charge_parameters.f2[x][y]);
      fFitCharge->SetParameter(3,ratio*charge_parameters.f3[x][y]);            

      fFitCharge->SetLineColor(1);
      fFit->Draw();
      fFitCharge->Draw("same");
      can->Update();
      can->WaitPrimitive();
      
      delete fFit;
      delete fFitCharge;      

    } // end y
  } // end x
  std::cout << "Canvas!" << std::endl;
  //hh->Draw("colz");
  //can->Update();
  //can->WaitPrimitive();
  //can->SaveAs("h2.root");
  delete can;
}
