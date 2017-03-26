//
// Author: Alfredo Guarnieri   24/03/17
//
// C1 P3 parte 1 diodo




#include <iostream>
#include <iomanip>
#include <cmath>		// pow(), tgamma()

#include <TH1D.h>
#include <TF1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TMath.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TMatrixT.h>
#include <TMatrixDBase.h>
#include <TMatrixD.h>
#include <TPaveText.h>
#include <TLegend.h>

#include "../../stat1.h"
#include "../../check.h"
#include "../../funzioni.h"




void P3_1_diod( TCanvas * Canv0 ) {



  ///////////////////////////////////////////////////////////
  /// Data input step

  std::string path  = "./c1/data/";
  std::string file1 = "P3_1_diod.txt";

  class stat1 import ( path, file1 , 2, '\t', 3 );
    std::vector<double> voltage = import.pData->at(0);
    std::vector<double> current = import.pData->at(1);
    
    // quick check
    Stat::calc( & voltage );  // volt
    Stat::calc( & current );  // mA



  ///////////////////////////////////////////////////////////
  /// TGraph

  int   size = Stat::fsize( & voltage );
  
  TGraphErrors* gr = new TGraphErrors ( size );
    
    // parametri grafici
    gr->GetXaxis()->SetTitle("V");
    gr->GetYaxis()->SetTitle("mA");
    gr->SetTitle(" Diodo led: Corr.te [mA]  +/- 3.92% in funzione di Dif.Pot. [V]  +/- 0.01% ");
    gr->SetMarkerColor(4);
    gr->SetMarkerStyle(21);
    
        
    for (int i = 0; i< size; i++) {
      gr->SetPoint( i, voltage.at(i), current.at(i) );
      gr->SetPointError( i, .01/100, 3.92/100 );
    }
    
    Canv0->SetGrid();
    gr->Draw("AP");



  ///////////////////////////////////////////////////////////
  /// Adattamento
  
  // double c1 = 38.6;	// V^-1 costante fornita dalle istruzioni

  TF1  *f1 = new TF1("f1","(exp( [0] * x ) - 1)*[1] + x*[2]",2.2 ,3);
    f1->SetRange(2.2, 3);
    f1->SetParameter(0, 1 );
    f1->SetParameter(1, 1 );
    f1->SetParameter(2, 1 );
    
    
    TFitResultPtr pf1 = gr->Fit(f1,"S");

    

  ///////////////////////////////////////////////////////////
  /// Calcolo Chi2
  
  double chi2 = 0;
  double x,y;
  
  // int nfit = f1->GetNumberFitPoints();
  int npar = f1->GetNDF();
  int dof = size - npar;
    std::cout << "f1->GetNDF() : "<< f1->GetNDF() << "\n";	//check
    std::cout << "dof : "<< dof << "\n";			//check
    
    
  for (int i = 0; i< size; i++) {
      gr->GetPoint(i, x, y );
      chi2 +=  pow( y - f1->Eval(x) ,2) / gr->GetErrorY(i);
    }
    
    std::cout << "chi2 / dof : "<< chi2 / dof << "\n";	//check



  ///////////////////////////////////////////////////////////
  /// Legenda
  
  TLegend * leg = new TLegend(0.15,0.7,0.6,0.9);
    leg->SetTextSize(0.045);
    leg->SetHeader("MLE - fit");
    leg->AddEntry( gr, "data (I,V)","p");
    leg->AddEntry( f1, "(e^{ [0] * x } - 1)*[1] + x*[2]","l");

    
    leg->Draw();
    
  
   
  ///////////////////////////////////////////////////////////
  /// Fit result table
    
//    TPaveText *pt = new TPaveText(0.15,0.3,0.4,0.55);
//     pt->SetFillColor(18);
//     pt->SetTextAlign(12);
//     pt->AddText("qui");
//     pt->AddText("qui");
//     pt->Draw();

  return;}







