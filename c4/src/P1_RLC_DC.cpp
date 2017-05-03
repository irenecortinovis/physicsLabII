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
void FitData( TCanvas *, int );

 


// struttura dati elaborati
namespace gVar4{
  
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
  double R[3];	// resistenza resitori
  
  // ddp
  double V0 = 5;	//volt
  
  // capacità
  double C;
  
  // induttanze
  double I;
  
  // Errori relativi
  double errR = 1*exp10(-2);	//  ERRORE RELATIVO sulle resistenze 1.0%
    
  // Errori assoluti
  double sigmaV = 0.08;		//  ERRORE ASSOLUTO sulla ddp generata in [volt]

}	// end namespace gVar




// funzioni per Non Linear Fit
double SottoDC_ridotta  ( double *, double *  );
double SottoDC_completa ( double *, double *  );
double SovraDC          ( double *, double *  );

// funzioni calcolo valori iniziali
double Gamma( double, double );
double Pulsa( double, double, double );
double SottoFase ( double, double, double );
double SottoA    ( double, double, double );
void   PrintParIniz( double, double, double, double );



//////////////////////////////////////////////////////////////////////////////////
///
///


void P1_RLC_DC( TCanvas * Canv0 ) {

  

  ///////////////////////////////////////////////////////////
  /// Data input step

  std::string path  = "./c4/data/";
    std::string file1 = "P1_sotto.txt";
    std::string file2 = "P1_critico.txt";
    std::string file3 = "P1_sovra.txt";
  
  class stat1 sotto ( path, file1 , 2, '\t', 3 );
  class stat1 criti ( path, file2 , 2, '\t', 3 );
  class stat1 sovra ( path, file3 , 2, '\t', 3 );
  
  Stat::calc( &sovra.pData->at(0) );
  Stat::calc( &sovra.pData->at(1) );
  
  // raggruppo i quattro oggetti in un array
  gVar4::ptr[0] = &sotto;
  gVar4::ptr[1] = &criti;
  gVar4::ptr[2] = &sovra;

  // etichette
  gVar4::CompoName[0] = "Smz-Sotto";
  gVar4::CompoName[1] = "Smz-Criti";
  gVar4::CompoName[2] = "Smz-Sovra";
  
  // Resistenze
  gVar4::r   = 50; 	// resistenza interna del generatore - Ohm
  gVar4::rL  = 50; 	// resistenza interna dell'induttore - Ohm
  
  gVar4::R[0] =  100; 	// Ohm
  gVar4::R[1] = 2400;	// critico
  gVar4::R[2] = 10000;	// sovra
  
  // Capacità - valori iniziali
  gVar4::C =    4.6 * exp10(-8);	// farad
  
  // Induttanze - valori iniziali
  gVar4::I =   0.0657; 		// henry

  
  /////////////////////////////////////////////////////


  for ( int i = 0; i<3; i++ ) FitData( Canv0, i );
  
  
return;}







void FitData( TCanvas * Canv0, int comp )
{ 
  
  using namespace gVar4;
  
  std::cout << " ---------------------------- " << "\n";
      
  t   	= ptr[comp]->pData->at(0);
  Vb  	= ptr[comp]->pData->at(1);
  
  size = Stat::fsize( &t );
  fMin = Stat::fmin ( &t );
  fMax = Stat::fmax ( &t );
  
  PrintParIniz( R[comp] + r + rL, I, C, V0 );

  TGraphErrors* mod  = new TGraphErrors ( size );
    mod->GetXaxis()->SetTitle("t [s]");
    mod->GetYaxis()->SetTitle("I(t) [mu A]");
    mod->SetTitle( " title " );
    mod->SetMarkerColor(4);
    mod->SetMarkerStyle(21);
    

  // riempito all'occorrenza
  TF1  *f1 = new TF1();

    
    for (int i = 0; i< size; i++) {
      
       y = (Vb.at(i) * exp10(-3)) / ( R[comp] + r + rL );
      sy = sqrt( pow( sigmaV*( 1/ Vb.at(i) ), 2)  + pow( 3*errR, 2) ) * fabs(y);	// propagazione errore su y
       
       x = t.at(i) * exp10(-6);
      sx = 0.000001;
      
      mod->SetPoint( i, x, y );
      mod->SetPointError( i, sx, sy );
    }

    
    f1->Clear();
        
    
    if ( comp == 0 ){ 		// Sotto-smorzamento
      
      f1 = new TF1("SottoDC_completa", SottoDC_completa, fMin ,fMax, 4);
	// 
	
	f1->SetParName  ( 0, " A:");
	f1->SetParameter( 0,  SottoA    ( R[comp] + r + rL, I, C ) * V0 );
	
	f1->SetParName  ( 1, "gamm:");
	f1->SetParameter( 1,  Gamma     ( R[comp] + r + rL, I ) );
	
	f1->SetParName  ( 2, "puls:");
	f1->SetParameter( 2,  Pulsa( R[comp] + r + rL, I, C ) );
	  
	f1->SetParName  ( 3, "fase:");
	f1->SetParameter( 3,  SottoFase ( R[comp] + r + rL, I, C) );

	mod->Fit("SottoDC_completa", "C", "", fMin, fMax);
    }
    

    
    if ( comp == 1 ) { 	// critico
      
      f1 = new TF1("critico","[0]*[1]*[1]*x*exp(-[1]*x)",fMin,fMax); 
	//
      
	f1->SetParName(0,"RQ_0");
	  f1->SetParameter( 0,  V0/ ( 2*I*Pulsa( R[comp] + r + rL, I, C ) )  );
	
	f1->SetParName(1,"gamma");
	  f1->SetParameter( 1,  Gamma     ( R[comp] + r + rL, I ) );
	
	
      mod->Fit("critico", "C", "", fMin, fMax);
      
    }


    if ( comp == 2 ){	// Sovra-smorzamento
      
//         f1 = new TF1("SovraDC", SovraDC, fMin ,fMax, 3);
// 	// 
// 	
// 	f1->SetParName  ( 0, " A:");
// 	f1->SetParameter( 0,  V0/ ( 2*I*Pulsa( R[comp] + r + rL, I, C ) )  );
// 	
// 	f1->SetParName  ( 1, "gamm:");
// 	f1->FixParameter( 1,  Gamma     ( R[comp] + r + rL, I ) );
// 	
// 	f1->SetParName  ( 2, "puls:");
// 	f1->FixParameter( 2,  Pulsa     ( R[comp] + r + rL, I, C ) );
	
    f1 = new TF1("sovrasmorzato","[0]*(exp(-x*([2]-[1])) - exp(-x*([2]+[1])) )",fMin,fMax);
    //[0]=1/2*Q0R*(omega0)^2 [1]= beta [2]=gamma
    f1->SetParName(0,"1/2*RQ_0*(omega_0)^2");
    f1->SetParName(1,"beta");
    f1->SetParName(2,"gamma");
	
    f1->SetParameter(0, 5.0/(0.0657 * 2.0 * 74000) );
    f1->SetParameter(1, 74000);
    f1->SetParameter(2, 76000);
	
    mod->Fit("sovrasmorzato", "C", "", fMin, fMax);
    
    }

    
    
    ///////////////////////////////////////////////////////////
    /// Print eps
    
    Canv0->SetGrid();
    gStyle->SetOptFit(1111);
    
    mod->Draw("AP");
    
    std::string OutFilePrefix = "./c4/C4_P1_";
    std::string OutFileExtension = ".eps";
    std::string OutFileName = OutFilePrefix+CompoName[comp]+OutFileExtension;
    
     Canv0->Print( OutFileName.c_str(), "eps");
     Canv0->Clear();

   return;}





double SottoDC_ridotta ( double * x, double * par ) 
{ 
  double y = par[0] * exp( -par[1] * x[0] ) * cos( par[2] * x[0] + par[3] ); 
  return y;  }

double SottoDC_completa ( double * x, double * par )
{ double y = par[0] * exp( -par[1] * x[0] ) * ( 
					      -par[1] * cos( par[2] * x[0] + par[3] )
					      -par[2] * sin( par[2] * x[0] + par[3] )
					      );
  return y;}

double SovraDC ( double * x, double * par ) 
{ 
  double y = par[0] * exp( (-par[1] + sqrt( par[1]*par[1] - par[2] ) ) * x[0] ) -  
	     par[0] * exp( (-par[1] - sqrt( par[1]*par[1] - par[2] ) ) * x[0] ) ; 
  return y;}



double Gamma( double R, double L )          { return 0.5* R/L; };
double Pulsa( double R, double L, double C) { return sqrt( fabs( -(R*R / (4*L*L)) + (1/(C*L)) ) ); };
double SottoFase ( double R, double L, double C) { return atan( ( (1.0/(R*C)) - (0.5*R/L) )/ Pulsa( R, L, C ) ); };
double SottoA    ( double R, double L, double C) { return - C / cos( SottoFase(R, L, C)); };

void PrintParIniz( double R, double L, double C, double V ){
  using namespace gVar4;
  std::cout << "\n---------------------------------" << "\n"
  << "Gamma       : " 	<<  Gamma( R, L )	  	<< "\n"
  << "Pulsa       : " 	<<  Pulsa( R, L, C) 	<< "\n"
  << "Sotto-Fase  : "	<<  SottoFase ( R, L, C) 	<< "\n"
  << "Sotto-A     : " 	<<  SottoA    ( R, L, C)*V 	<< "\n"
  << "---------------------------------" << "\n\n";
  
return; };


