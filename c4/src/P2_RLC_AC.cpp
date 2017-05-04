//
// Author: Alfredo Guarnieri
//
// C4 P2 misura funzione di trasferimento e impedenza




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
void P2_AC_Fit( TCanvas *, int );



// struttura dati elaborati
namespace gVar1{
  
  // osservabili
  std::vector <double>  v   	;	// Frequenza
  std::vector <double>  Va  	;	// Va
  std::vector <double>  Vb  	;	// Vb
  std::vector <double>  Vba 	;
  std::vector <double>  fC2 	;
  std::vector <double>  sfC2 	;
  std::vector <double>  fC1 	;
  std::vector <double>  sfC1 	;
  

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
  double R;	// resistsenza resitori
  
  // ddp
  double V0 = 5;
  
  // capacità
  double C;
  
  // induttanze
  double I;
  
  // Errori relativi
  double errR = 1*exp10(-2);	//  ERRORE RELATIVO sulle resistenze 1.0%
    
  // Errori assoluti
  double sigmaV = 0.08;		//  ERRORE ASSOLUTO sulla ddp generata in [volt]

}	// end namespace gVar



// Fit functions
double ModFdtR ( double *, double * );
double ModFdtL ( double *, double * );
double ModFdtC ( double *, double * );

double ArgFdtR ( double *, double * );
double ArgFdtL ( double *, double * );
double ArgFdtC ( double *, double * );



//////////////////////////////////////////////////////////////////////////////////
///
///


void P2_RLC_AC( TCanvas * Canv0 ) {



  ///////////////////////////////////////////////////////////
  /// Data input step

  std::string path  = "./c4/data/";
    std::string file1 = "P2_R.txt";
    std::string file2 = "P2_L.txt";
    std::string file3 = "P2_C.txt";

  
  class stat1 Rdata  ( path, file1 , 8, '\t', 3 );
  class stat1 Ldata  ( path, file2 , 8, '\t', 3 );
  class stat1 Cdata  ( path, file3 , 8, '\t', 3 );
  
  
  // raggruppo i quattro oggetti in un array
  gVar1::ptr[0] = &Rdata;
  gVar1::ptr[1] = &Ldata;
  gVar1::ptr[2] = &Cdata;

  // etichette
  gVar1::CompoName[0] = "Res";
  gVar1::CompoName[1] = "Ind";
  gVar1::CompoName[2] = "Cond";
  
  
  // Resistenze
  gVar1::r   = 50; 	// resistenza interna del generatore - Ohm
  gVar1::rL  = 50; 	// resistenza interna dell'induttore - Ohm
  
  gVar1::R = 1000; 	// Ohm
    
  
  // Capacità - valori iniziali
//   gVar1::C[0] =    1.1 * exp10(-8); 	// farad
  gVar1::C =    4.6 * exp10(-8);
  
  // Induttanze - valori iniziali
  gVar1::I =   0.0675; 	// henry
//   gVar1::I[3] =   0.05;


  // Non linear Fit

   for ( int i = 0; i<3; i++ ) P2_AC_Fit( Canv0, i );
  
  // NOT TO DO Linear Fit
  
  
    
  
return;}




void P2_AC_Fit( TCanvas * Canv0, int comp )
{ 
  
  using namespace gVar1;
  
 
  v   	= ptr[comp]->pData->at(0);
  Va  	= ptr[comp]->pData->at(1);
  Vb  	= ptr[comp]->pData->at(2);
  Vba 	= ptr[comp]->pData->at(3);
  fC2 	= ptr[comp]->pData->at(4);
  sfC2 	= ptr[comp]->pData->at(5);
  fC1 	= ptr[comp]->pData->at(6);
  sfC1 	= ptr[comp]->pData->at(7);
  
  size = Stat::fsize( &v );
  fMin = log10( Stat::fmin( &v ) ) ;
  fMax = log10( Stat::fmax( &v ) ) ;
  
  TGraphErrors* tmod = new TGraphErrors ( size );
  TGraphErrors* targ = new TGraphErrors ( size );
  
    tmod->GetXaxis()->SetTitle("log (Freq - Hz)");
    tmod->GetYaxis()->SetTitle("|Z'/Z|");
    tmod->SetTitle(" Modulo fdt  ");
    tmod->SetMarkerColor(4);
    tmod->SetMarkerStyle(21);
    
    targ->GetXaxis()->SetTitle("log (Freq - Hz)");
    targ->GetYaxis()->SetTitle("Arg(Z'/Z)");
    targ->SetTitle(" Argomento fdt  ");
    targ->SetMarkerColor(4);
    targ->SetMarkerStyle(21);

    // riempito all'occorrenza
    TF1  *f1 = new TF1();

    
    
    // Modulo fdt
    for (int i = 0; i< size; i++) {
      
       y = Vb.at(i) / Va.at(i);
      sy = fabs(y) * sqrt( pow(errR,2) + pow( sigmaV*( 1/ Vb.at(i) + 1/ Va.at(i) ), 2) );	  // propagazione errore su y
            
       x = log10( v.at(i) );
      sx = 0.5 * ( log10( v.at(i) +1 ) - log10( v.at(i) -1) );	// precisione all'ultima cifra in scala log10      
            
      tmod->SetPoint( i, x, y );
      tmod->SetPointError( i, sx, sy  );
    }
    

    if ( comp == 0 ){ // resistenza
      
      f1 = new TF1("ModFdtR", ModFdtR , fMin, fMax, 3 );
	// y = ( x[0] - ( par[0]/x[0] ) ) / ( 2*par[1] )
        // par[2] / sqrt( 1 + pow(y,2)
	f1->SetParName( 0, "w0^2");
	  f1->SetParameter( 0,  1/(I*C) );
	f1->SetParName( 1, "2gamma");
	  f1->SetParameter( 1, (R+r+rL)/I );
	f1->SetParName( 2, "R/Rtot");
	  f1->FixParameter( 2, R/(R+r+rL) );
	
 	tmod->Fit("ModFdtR", "C", "", fMin, fMax);
    }
    
    if ( comp == 1 ) { 	// induttori
      

    }
    
    if ( comp == 2 ) { 	// condensatore
      
    }
    
    
    // Arg fdt
    for (int i = 0; i< size; i++) {
      
       y =   2 * M_PI * fC1.at(i)*v.at(i)*exp10(-6);	// dati in microsecondi
      sy =   sqrt( pow( sfC1.at(i) / fC1.at(i), 2) + pow( 1 / v.at(i), 2) ) * fabs(y);
      
       x = log10( v.at(i) );
      sx = 0.5 * ( log10( v.at(i) +1 ) - log10( v.at(i) -1) ) ;	// precisione all'ultima cifra in scala log10      
      
      targ->SetPoint( i, x, y );
      targ->SetPointError( i, sx, sy  );      
    }

    
    if ( comp == 0 ){ // resistenza
      
      f1 = new TF1("ArgFdtR", ArgFdtR, fMin ,fMax, 2);
	// y = ( x[0] - ( par[0]/x[0] ) ) / ( 4*par[1]*par[1] )
	f1->SetParName( 0, "w0^2");
	  f1->SetParameter( 0,  1/(I*C) );
	f1->SetParName( 1, "2gamma");
	  f1->SetParameter( 1,  (R+r+rL) / I );
	
 	targ->Fit("ArgFdtR", "CR", "", fMin, fMax);
// 	f1->Draw("AP");
    }

    
//     if ( comp <  2 ){ // condensatori
//  
//     }
//     
//     else {
//       
//       
//     }

    
    
    Canv0->SetGrid();
    Canv0->Divide(2,1); 
    gStyle->SetOptFit(1111);
    
      Canv0->cd(1); tmod->Draw("AP");
      Canv0->cd(2); targ->Draw("AP");
      

      

    ///////////////////////////////////////////////////////////
    /// Print eps
    
    std::string OutFilePrefix = "./c4/C4_P2_";
    std::string OutFileExtension = ".eps";
    std::string OutFileName = OutFilePrefix+CompoName[comp]+OutFileExtension;
    
     Canv0->Print( OutFileName.c_str(), "eps");
     Canv0->Clear();
     

return;}




double ModFdtR ( double * x, double * par )
{ 
  double w = 2 * M_PI * exp10(x[0]);
  double y = ( w - ( par[0]/w ) ) / ( 2*par[1] ) ;
  return par[2] / sqrt( 1 + pow(y,2)  );  }
  
// double ModFdtL ( double *, double * )
// double ModFdtC ( double *, double * )


double ArgFdtR ( double * x, double * par )
{ 
  double w = 2 * M_PI * exp10(x[0]);
  double y = ( w - ( par[0]/w ) ) / ( 2*par[1] ) ;
  return atan( -y);  }

// double ArgFdtL ( double *, double * )
// double ArgFdtC ( double *, double * )
