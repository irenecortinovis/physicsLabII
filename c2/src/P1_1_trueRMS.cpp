//
// Author: Alfredo Guarnieri   24/03/17
//
// C2 P1 parte 1 True RMS




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




void P1_1_trueRMS( TCanvas * Canv0 ) {



  ///////////////////////////////////////////////////////////
  /// Data input step

  std::string path  = "./c2/data/";
  std::string file1 = "C2_P1.txt";

  class stat1 import ( path, file1 , 10, '\t', 3 );
  

  ///////////////////////////////////////////////////////////
  /* column mapping of import.pData->at( .. )
  
  0 ω
  1 log(ω)
  2 ImultPP
  3 Ib PP
  4 Imult/Ib
  5 err
  6 VmultPP
  7 V(b-a)PP
  8 Vmult/V(b-a)
  9 err
  
  */
  
  std::vector <double>  w = import.pData->at(0);
  std::vector <double>  logw = import.pData->at(1);
  std::vector <double>  curr_ratio = import.pData->at(4);
  std::vector <double>  volt_ratio = import.pData->at(8);
  
  std::vector <double>  curr_ratio_err = import.pData->at(5);
  std::vector <double>  volt_ratio_err = import.pData->at(9);

  
  
  ///////////////////////////////////////////////////////////
  /// TGraph - current ratio

  int   size = Stat::fsize( &logw );
  
  TGraphErrors* grI = new TGraphErrors ( size );
  TGraphErrors* grV = new TGraphErrors ( size );
    
    
    
    for (int i = 0; i< size; i++) {
      grI->SetPoint( i, logw.at(i), curr_ratio.at(i) );
      grI->SetPointError( i, .02 * logw.at(i) / fabs( w.at(i) ), curr_ratio_err.at(i) );
    }
    
    for (int i = 0; i< size; i++) {
      grV->SetPoint( i, logw.at(i), volt_ratio.at(i) );
      grV->SetPointError( i, .02 * logw.at(i) / fabs( w.at(i) ), volt_ratio_err.at(i) );
    }


    /// grafico I
    grI->GetXaxis()->SetTitle("lg10( f [Hz] )");
    grI->GetYaxis()->SetTitle("Current ratio");
    grI->SetTitle("Measurements ratio: multimeter / oscilloscope  ");
    grI->SetMarkerColor(4);
    grI->SetMarkerStyle(21);
    
      Canv0->SetGrid();
      grI->Draw("AP");
      Canv0->Print("./c2/C2_P1_trueRMS_I.eps", "eps");

    
    /// grafico V
    grV->GetXaxis()->SetTitle("lg10( f [Hz] )");
    grV->GetYaxis()->SetTitle("Voltage ratio");
    grV->SetTitle(" Measurements ratio: multimeter / oscilloscope  ");
    grV->SetMarkerColor(4);
    grV->SetMarkerStyle(21);
    
      Canv0->SetGrid();
      grV->Draw("AP");
      Canv0->Print("./c2/C2_P1_trueRMS_V.eps", "eps");




  ///////////////////////////////////////////////////////////
  /// Adattamento
  
//   double f1Min = 2.3;
//   double f1Max = 3.0;
//     
//   TF1  *f1 = new TF1("f1","(exp( [0] * x ) - 1)*[1] + x*[2]", f1Min ,f1Max);
//     f1->SetParameter(0, 1 );
//     f1->SetParameter(1, 1 );
//     f1->SetParameter(2, exp10(-2) );
//     
//     
//     TFitResultPtr pf1 = gr->Fit("f1","S", "", f1Min ,f1Max);



  ///////////////////////////////////////////////////////////
  /// Calcolo Chi2
  
//   double chi2 = 0;
//   double * x = gr->GetX();
//   double * y = gr->GetY();
//   
//   for (int i = 0; i< gr->GetN(); i++) {
//     
//     // selects points in the fitting range
//     if ( x[i] >= f1Min && x[i] <= f1Max ) chi2 +=  pow( y[i] - f1->Eval(x[i]) ,2) / gr->GetErrorY(i);
//     }
// 
//   int dof  = f1->GetNumberFitPoints() - f1->GetNpar() - 1;
//     std::cout << "dof : "<< dof << "\n";			//check
//     std::cout << "chi2 / dof : "<< chi2 / dof << "\n";		//check



  ///////////////////////////////////////////////////////////
  /// Legenda
  
//   TLegend * leg = new TLegend(0.15,0.7,0.6,0.9);
//     leg->SetTextSize(0.045);
//     leg->SetHeader("MLE - fit");
//     leg->AddEntry( gr, "data (I,V)","p");
//     leg->AddEntry( f1, "(e^{ [0] * x } - 1)*[1] + x*[2]","l");
// 
//     
//     leg->Draw();
    
  
   
  ///////////////////////////////////////////////////////////
  /// Fit result table
    
//    TPaveText *pt = new TPaveText(0.15,0.3,0.4,0.55);
//     pt->SetFillColor(18);
//     pt->SetTextAlign(12);
//     pt->AddText("qui");
//     pt->AddText("qui");
//     pt->Draw();

      
  ///////////////////////////////////////////////////////////
  /// Print eps
      

      
  return;}







