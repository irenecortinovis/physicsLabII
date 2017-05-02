//
// Author: Alfredo Guarnieri   24/03/17
//
// C4 P1 




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
#include <TStyle.h>
#include <TCanvas.h>
#include <TLinearFitter.h>


#include "../../stat1.h"
#include "../../check.h"
#include "../../funzioni.h"



 
//  Non Linear Fit
void NonLinearFit( TCanvas *, int );
void ShowFit();

//  Linear Fit
void LinearFit( TCanvas * ); 
 


// struttura dati elaborati
namespace gVar{
  
  // osservabili
  std::vector <double>  t   	;	// tempo
  std::vector <double>  Vb  	;	// Vb
  

  // raccoglitore dati
  class stat1* ptr[3];
  
 
  // etichette dati
  std::string CompoName[3];
  
  
  // Fit output
  TFitResultPtr Outptr[3][2];	// 4 componenti X 2 (modulo e fase)
    
    
  // Variabili ausiliarie
  double y, sy, x, sx, size, fMin, fMax;
  
  // resistori
  double r ;	// resistenza interna generatore
  double rL ;	// resistenza interna induttore
  double R[3];	// resistsenza resitori
  
  // capacità
  double C;
  
  // induttanze
  double I;
  
  // Errori relativi
  double errR = 1*exp10(-2);	//  ERRORE RELATIVO sulle resistenze 1.0%
    
  // Errori assoluti
  double sigmaV = 0.08;		//  ERRORE ASSOLUTO sulla ddp generata in [volt]

}	// end namespace gVar




// Utilities
void ComponentNameMessage( int );


// funzioni per Non Linear Fit




//////////////////////////////////////////////////////////////////////////////////
///
///


void P1_impedenze_fdt( TCanvas * Canv0 ) {



  ///////////////////////////////////////////////////////////
  /// Data input step

  std::string path  = "./c4/data/";
    std::string file1 = "P1_sotto.txt";
    std::string file2 = "P1_critico.txt";
    std::string file3 = "P1_sovra.txt";
  
  class stat1 sotto  ( path, file1 , 2, '\t', 3 );
  class stat1 criti  ( path, file2 , 2, '\t', 3 );
  class stat1 sovra ( path, file3 , 2, '\t', 3 );
  
  // raggruppo i quattro oggetti in un array
  gVar::ptr[0] = &sotto;
  gVar::ptr[1] = &criti;
  gVar::ptr[2] = &sovra;


  // etichette
  gVar::CompoName[0] = "Smz-Sotto";
  gVar::CompoName[1] = "Smz-Criti";
  gVar::CompoName[2] = "Smz-Sovra";
  
  // Resistenze
  gVar::r   = 50; 	// resistenza interna del generatore - Ohm
  gVar::rL  = 60; 	// resistenza interna dell'induttore - Ohm
  
  gVar::R[0] =  250; 	// Ohm
  gVar::R[1] = 3130;
  gVar::R[3] = 5000;
  
  // Capacità - valori iniziali
  gVar::C =    4.6 * exp10(-8);	// farad
  
  // Induttanze - valori iniziali
  gVar::I =   0.12; 		// henry

  
  /////////////////////////////////////////////////////




  // Non linear Fit

  for ( int i = 0; i<3; i++ ) NonLinearFit( Canv0, i );
  
  // NOT TO DO Linear Fit
  
  
    
  
return;}




  ///////////////////////////////////////////////////////////
  /// Non linear Fit


void NonLinearFit( TCanvas * Canv0, int comp )
{ 
  
  using namespace gVar;
  
  std::cout << " -------------- Non Linear Fit -------------- " << "\n";
  
  ComponentNameMessage( comp );
  
    
  t   	= ptr[comp]->pData->at(0);
  Vb  	= ptr[comp]->pData->at(1);
  
  size = Stat::fsize( &t );
  fMin = Stat::fmin ( &t );
  fMax = Stat::fmax ( &t );
  

  TGraphErrors* mod  = new TGraphErrors ( size );
    mod->GetXaxis()->SetTitle("log (Freq - Hz)");
    mod->GetYaxis()->SetTitle("|Z|");
    mod->SetTitle(" Modulo impedenza  ");
    mod->SetMarkerColor(4);
    mod->SetMarkerStyle(21);
    

  // riempito all'occorrenza
  TF1  *f1 = new TF1();

    
    
    
    for (int i = 0; i< size; i++) {
      
       y = fabs(Vb.at(i)) * ( R[comp] + r + rL );
      sy = sqrt( pow( sigmaV*( 1/ Vb.at(i) ), 2)  + pow( 3*errR, 2) ) * y;	// propagazione errore su y
      
      st = 0.01 * t;
      
      mod->SetPoint( i, t, y );
      mod->SetPointError( i, st, sy );
    }
    
    f1->Clear();
    
    if ( comp == 0 ){ // Sovra-smorzamento
      
      f1 = new TF1("ModImpedenzaC", ModImpedenzaC, fMin ,fMax, 2);
	// 1.0 / (  par[0] + 2 * M_PI * par[1] * exp10( x[0] ) )
	f1->FixParameter( 0, 0 );
	f1->SetParameter( 1,  C[comp] );
	f1->SetParName( 1, "C [F]");
	mod->Fit("ModImpedenzaC", "C", "", fMin, fMax);
    }
    
    else{	// induttori
      
      f1 = new TF1("ModImpedenzaL", ModImpedenzaL, fMin ,fMax, 2);
	// sqrt( par[0]*par[0] + par[1]*par[1]*exp10( 2 * x[0] ) * 4 * M_PI * M_PI )
	f1->FixParameter( 0,  rL );	// resitenza interna induttore
	f1->SetParameter( 1,  I[comp] );
	f1->SetParName( 0, "R [ohm]");
	f1->SetParName( 1, "L [H]");
	
	mod->Fit("ModImpedenzaL", "C", "", fMin, fMax);
    }

   
    
    
    Canv0->SetGrid();
      Canv0->Divide(2,1);
      gStyle->SetOptFit(1111);
      mod->Draw("AP");

      
      
    ///////////////////////////////////////////////////////////
    /// Print eps
    
    std::string OutFilePrefix = "./c4/C4_P1_";
    std::string OutFileExtension = ".eps";
    std::string OutFileName = OutFilePrefix+CompoName[comp]+OutFileExtension;
    
     Canv0->Print( OutFileName.c_str(), "eps");
     Canv0->Clear();

   return;}



void ComponentNameMessage( int i ) {
  using namespace gVar;
  std::cout << "\n\n" << CompoName[i] << "\n\n";
  return;}



double SottoDC_1 ( double * x, double * par ) 
{ 
  double y = par[0] * exp( -par[1] * x[0] ) * cos( par[2] * x[0] + par[3] ); 
  return fabs(y);  }

double SottoDC_2 ( double * x, double * par )
{ double y = par[0] * exp( -par[1] * x[0] ) * ( 
					      -par[1] * cos( par[2] * x[0] + par[3] )
					      -par[2] * sin( par[2] * x[0] + par[3] );
  return fabs(y);  
}










