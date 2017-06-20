//
//



#include <iostream>
#include <iomanip>
#include <cmath>		// pow(), tgamma()
#include <root/TGraphErrors.h>
#include <root/TAxis.h>

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
#include <TCanvas.h>


#include "../../stat1.h"
#include "../../check.h"
#include "../../funzioni.h"




double ModFdt1C ( double *, double * );
double ModFdt2C ( double *, double * );
double ArgFdt1C ( double *, double * );
double ArgFdt2C ( double *, double * );



void P2_capacita( TCanvas * Canv0 ) {

  std::string path  = "./c2/data/";
  std::string file1 = "C2_P2.txt";
  
  class stat1 cond1  ( path, file1 , 11, '\t', 3 );

  std::vector<double> v   = cond1.pData->at(0);
  std::vector<double> Va  = cond1.pData->at(2);
  std::vector<double> Vb  = cond1.pData->at(3);
  std::vector<double> Vba = cond1.pData->at(4);
  std::vector<double> fa2  = cond1.pData->at(6);
  std::vector<double> sfa2 = cond1.pData->at(7);
  std::vector<double> fa1  = cond1.pData->at(9);
  std::vector<double> sfa1 = cond1.pData->at(10);
  
 
  int size = Stat::fsize( &v );
  double fMin = log10( Stat::fmin( &v ) ) ;
  double fMax = log10( Stat::fmax( &v ) ) ;
  
  // parametri di stampa
  std::string OutFilePrefix = "./c2/C2_P2_";
  std::string OutFileExtension = ".eps";
  
  
  TGraphErrors* graf1  = new TGraphErrors ( size );
    TGraphErrors* graf2  = new TGraphErrors ( size );
      TGraphErrors* graf3  = new TGraphErrors ( size );
        TGraphErrors* graf4  = new TGraphErrors ( size );

  TF1  *f1    = new TF1();
    TF1  *f2    = new TF1();
      TF1  *f3    = new TF1();
        TF1  *f4    = new TF1();

  TCanvas     * Canv1 = new TCanvas     ( "Canv1", "Canv1", 0,0,900,500 );
      TCanvas     * Canv2 = new TCanvas     ( "Canv2", "Canv2", 0,0,900,500 );
        TCanvas     * Canv3 = new TCanvas     ( "Canv3", "Canv3", 0,0,900,500 );
	  TCanvas     * Canv4 = new TCanvas     ( "Canv4", "Canv4", 0,0,900,500 );
    
  double y, sy, x, sx;
  
  double R = 14870;	// ohm
  double r = 50;		// ohm
  double c = 11E-9;		// nF
  
  double sigmaV = 0.02;	// errore assoluto su misura ddp
  
  double errR   = 0.01;	// errore relativo su misura ddp
  
  int flag = 0;		// ricalcolo chi2
  double chi2 = 0;
  

  
  //////////////////////////////////////
  /// Mod fdt1
  
  for (int i = 0; i< size; i++) {

       y = ( Vba.at(i) / Vb.at(i) );
      sy = sqrt( pow( sigmaV*( 2/Vba.at(i) + 1/ Vb.at(i) ), 2)  ) * y;	// propagazione errore su y
      if (flag == 1 ) sy = sqrt( chi2 ); // errori ricalcolati

       x = log10( v.at(i) );
      sx = 0.5 * ( log10( v.at(i) +1 ) - log10( v.at(i) -1) );	// precisione all'ultima cifra in scala log10

      graf1->SetPoint( i, x, y );
      graf1->SetPointError( i, sx, sy );
 
  }
 

 
  f1 = new TF1("ModFdt1C", ModFdt1C, fMin ,fMax,1);
    f1->SetParameter( 0,  c*R );	// condensatore
    f1->SetParName(   0, "RC [ohm*F]");

	Canv1->Clear();
	Canv1->cd();
	graf1->GetXaxis()->SetTitle("log (Freq - Hz)");
	graf1->GetXaxis()->CenterTitle();
	graf1->SetTitle("Modulo FdT Vb-a / Vb");
	graf1->SetMarkerColor(4);
	graf1->SetMarkerStyle(21);


	gStyle->SetOptFit(1111);
	graf1->Fit("ModFdt1C", "C", "", fMin, fMax);

	// ricalcolo del chi2
	chi2 = 0;
	y = 0; x=0;
	for ( int i = 0; i< f1->GetNumberFitPoints(); i++ ) {

	  graf1->GetPoint( i, x, y);

	  chi2 += pow( f1->Eval(x) - y  ,2) / f1->GetNDF();

	  std::cout << f1->Eval(x) << ";" << y << "\n";
	}
	
	graf1->Draw("AP");

// 	TLegend * leg = new TLegend(0.6,0.3,0.8,0.5);
// 	leg->AddEntry(f1, "y = 1/(#omega C)", "l");
// 	leg->AddEntry(graf, "data", "p");
// 	leg->Draw();

	std::string OutObj = "ModFdt1_";
	std::string OutFileName = OutFilePrefix+OutObj+OutFileExtension;

	Canv1->Print( OutFileName.c_str(), "eps");






  //////////////////////////////////////
  /// Arg fdt1

    for (int i = 0; i< size; i++) {
      
       y =   2 * M_PI * fa2.at(i)*v.at(i)*exp10(-6);	// dati in microsecondi
       y = y - M_PI;
      sy =   sqrt( pow( sfa2.at(i) / fa2.at(i), 2) + pow( 1 / v.at(i), 2) ) * fabs(y);
      if (flag == 1 ) sy = sqrt( chi2 ); // errori ricalcolati
      
       x = log10( v.at(i) );
      sx = 0.5 * ( log10( v.at(i) +1 ) - log10( v.at(i) -1) ) ;	// precisione all'ultima cifra in scala log10      
      
      graf2->SetPoint( i, x, y );
      graf2->SetPointError( i, sx, sy  );
      
    }


    
    f2 = new TF1 ("ArgFdt1C", ArgFdt1C, fMin ,fMax, 1);
      f2->SetParameter( 0, c*(R+r) );
      f2->SetParName  ( 0, "RC [ohm*F]");
      
      Canv2->Clear();
      Canv2->cd();
      graf2->GetXaxis()->SetTitle("log (Freq - Hz)");
      graf2->GetXaxis()->CenterTitle();
      graf2->SetTitle("Arg. FdT Vb-a / Vb");
      graf2->SetMarkerColor(4);
      graf2->SetMarkerStyle(21);
	
      gStyle->SetOptFit(1111);
      graf2->Fit("ArgFdt1C", "C", "", fMin, fMax);
      
      
	// ricalcolo del chi2
	chi2 = 0;
	y = 0; x=0;
	for ( int i = 0; i< f2->GetNumberFitPoints(); i++ ) {
	  
	  graf2->GetPoint( i, x, y);
	  
	  chi2 += pow( f2->Eval(x) - y  ,2) / f2->GetNDF();
	  
	  std::cout << f2->Eval(x) << ";" << y << "\n";
	  
	}
      
      
      graf2->Draw("AP");
	
//       TLegend * leg = new TLegend(0.16,0.25,0.45,0.5);
//       leg->AddEntry(f1, " y = arctan{  -w * (RC) ) }", "l");
//       leg->AddEntry(targ, "data", "p");
//       leg->Draw();

      OutObj = "ArgFdt1_";
      OutFileName = OutFilePrefix+OutObj+OutFileExtension;

      Canv2->Print( OutFileName.c_str(), "eps");




      
      
  //////////////////////////////////////
  /// Mod fdt2
  
  for (int i = 0; i< size; i++) {

       y = ( Vb.at(i) / Va.at(i) );
      sy = sqrt( pow( sigmaV*( 1/Vb.at(i) + 1/ Va.at(i) ), 2)  ) * y;	// propagazione errore su y
      if (flag == 1 ) sy = sqrt( chi2 ); // errori ricalcolati

       x = log10( v.at(i) );
      sx = 0.5 * ( log10( v.at(i) +1 ) - log10( v.at(i) -1) );	// precisione all'ultima cifra in scala log10

      graf3->SetPoint( i, x, y );
      graf3->SetPointError( i, sx, sy );
 
  }
 

 
  f3 = new TF1("ModFdt2C", ModFdt2C, fMin ,fMax,1);
    f3->SetParameter( 0,  c*R );	// condensatore
    f3->SetParName(   0, "RC [ohm*F]");

	Canv3->Clear();
	Canv3->cd();
	graf3->GetXaxis()->SetTitle("log (Freq - Hz)");
	graf3->GetXaxis()->CenterTitle();
	graf3->SetTitle("Modulo FdT Vb / Va");
	graf3->SetMarkerColor(4);
	graf3->SetMarkerStyle(21);


	gStyle->SetOptFit(1111);
	graf3->Fit("ModFdt2C", "C", "", fMin, fMax);

	// ricalcolo del chi2
	chi2 = 0;
	y = 0; x=0;
	for ( int i = 0; i< f3->GetNumberFitPoints(); i++ ) {

	  graf3->GetPoint( i, x, y);

	  chi2 += pow( f3->Eval(x) - y  ,2) / f3->GetNDF();

	  std::cout << f3->Eval(x) << ";" << y << "\n";
	}
	
	graf3->Draw("AP");

// 	TLegend * leg = new TLegend(0.6,0.3,0.8,0.5);
// 	leg->AddEntry(f1, "y = 1/(#omega C)", "l");
// 	leg->AddEntry(graf, "data", "p");
// 	leg->Draw();

	OutObj = "ModFdt2_";
	OutFileName = OutFilePrefix+OutObj+OutFileExtension;

	Canv3->Print( OutFileName.c_str(), "eps");






  //////////////////////////////////////
  /// Arg fdt2

    for (int i = 0; i< size; i++) {
      
       y =   2 * M_PI * fa2.at(i)*v.at(i)*exp10(-6);	// dati in microsecondi
       y = y - M_PI;
      sy =   sqrt( pow( sfa2.at(i) / fa2.at(i), 2) + pow( 1 / v.at(i), 2) ) * fabs(y);
      if (flag == 1 ) sy = sqrt( chi2 ); // errori ricalcolati
      
       x = log10( v.at(i) );
      sx = 0.5 * ( log10( v.at(i) +1 ) - log10( v.at(i) -1) ) ;	// precisione all'ultima cifra in scala log10      
      
      graf4->SetPoint( i, x, y );
      graf4->SetPointError( i, sx, sy  );
      
    }


    
    f4 = new TF1 ("ArgFdt2C", ArgFdt2C, fMin ,fMax, 1);
      f4->SetParameter( 0, c*(R+r) );
      f4->SetParName  ( 0, "RC [ohm*F]");
      
      Canv4->Clear();
      Canv4->cd();
      graf4->GetXaxis()->SetTitle("log (Freq - Hz)");
      graf4->GetXaxis()->CenterTitle();
      graf4->SetTitle("Arg. FdT Vb / Va");
      graf4->SetMarkerColor(4);
      graf4->SetMarkerStyle(21);
	
      gStyle->SetOptFit(1111);
      graf4->Fit("ArgFdt2C", "C", "", fMin, fMax);
      
      
	// ricalcolo del chi2
	chi2 = 0;
	y = 0; x=0;
	for ( int i = 0; i< f4->GetNumberFitPoints(); i++ ) {
	  
	  graf4->GetPoint( i, x, y);
	  
	  chi2 += pow( f4->Eval(x) - y  ,2) / f4->GetNDF();
	  
	  std::cout << f4->Eval(x) << ";" << y << "\n";
	  
	}
      
      
      graf4->Draw("AP");
	
//       TLegend * leg = new TLegend(0.16,0.25,0.45,0.5);
//       leg->AddEntry(f1, " y = arctan{  -w * (RC) ) }", "l");
//       leg->AddEntry(targ, "data", "p");
//       leg->Draw();

      OutObj = "ArgFdt2_";
      OutFileName = OutFilePrefix+OutObj+OutFileExtension;

      Canv4->Print( OutFileName.c_str(), "eps");
      





  


return;}




double ModFdt1C ( double * x, double * par )
{ 
  double w = 2 * M_PI * exp10( x[0] );
  return 1.0 / ( par[0]*w );
}

double ModFdt2C ( double * x, double * par )
{ 
  double w = 2 * M_PI * exp10( x[0] );
  return w*par[0] / ( sqrt( 1 + par[0]*par[0]*w*w ) );
}

double ArgFdt1C ( double * x, double * par )
{ 
  double w = 2 * M_PI * exp10( x[0] );
  return -atan( w*par[0] );
}

double ArgFdt2C ( double * x, double * par )
{ 
  double w = 2 * M_PI * exp10( x[0] );
  return -atan( -1.0 / w*par[0] );
}