//
// Author: Alfredo Guarnieri   24/03/17
//
// C1 P3 parte 1 diodo




#include <iostream>
#include <iomanip>
#include <cmath>    // pow(), tgamma()

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
#include <TStyle.h>
#include <TApplication.h>

#include "../../stat1.h"
#include "../../check.h"
#include "../../funzioni.h"




void P3_1_diod( TCanvas * Canv0 ) {

    TApplication* Grafica = new TApplication("Grafica", 0, NULL);
    gStyle->SetOptFit(1111);




  /// TGraph

  
  TGraphErrors* gr = new TGraphErrors ("./c1/data/P3_1_diod.txt");
    
    // parametri grafici
    gr->GetXaxis()->SetTitle("V");
    gr->GetYaxis()->SetTitle("mA");
    gr->SetTitle(" Diodo led: Corr.te [mA] in funzione di Diff.Pot. [V] ");
    gr->SetMarkerColor(4);
    gr->SetMarkerStyle(21);
    
    Canv0->SetGrid();
    gr->Draw("AP");



  ///////////////////////////////////////////////////////////
  /// Adattamento
  
  double f1Min = 2.4;
  double f1Max = 3.0;
    
  TF1  *f1 = new TF1("f1","(exp( [0] * x ) - 1)*[1] + x*[2]", f1Min ,f1Max);
    f1->SetParameter(0, 4);
    f1->SetParameter(1, 0);
    f1->SetParameter(2, 0);
    f1->SetParName(0,"[0]: q/(ktg)");
    f1->SetParName(1,"[1]: I_0 [mA]");
    f1->SetParName(2,"[2]: a [1/ohm]");


  gr->Fit("f1", "S", "", f1Min, f1Max);



    
    //TFitResultPtr pf1 = gr->Fit("f1","S", "", f1Min ,f1Max);



  ///////////////////////////////////////////////////////////
  /// Calcolo Chi2
  
  
  ///////////////////////////////////////////////////////////
  /// Legenda
  
  TLegend * leg = new TLegend(0.15,0.7,0.6,0.9);
    leg->SetTextSize(0.045);
    leg->AddEntry( gr, "data (I,V)","p");
    leg->AddEntry( f1, "(e^{ [0] * x } - 1)*[1] + x*[2]","l");

    
    leg->Draw();
    
  
  Grafica->Run("");


  return;}







