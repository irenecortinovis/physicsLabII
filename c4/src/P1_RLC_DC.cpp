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
  std::vector <double>  eV  	;	// errore assoluto su V
  std::vector <double>  et  	;	// errore assoluto su t
  

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
  double errR = 1*exp10(-2);	//  ERRORE RELATIVO su ogni resistenza 1.0%
    
  // Errori assoluti
  double sigmaV = 0.08;		//  ERRORE ASSOLUTO sulla ddp generata in [volt]

}	// end namespace gVar




// funzioni per Non Linear Fit
double SottoDC ( double *, double *  );
double SovraDC          ( double *, double *  );


// funzioni calcolo valori iniziali
double Gamma( double, double );
double Pulsa( double, double, double );
double Fase ( double, double, double );
double Q0   ( double, double );
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
  
  class stat1 sotto ( path, file1 , 4, '\t', 3 );
  class stat1 criti ( path, file2 , 4, '\t', 3 );
  class stat1 sovra ( path, file3 , 4, '\t', 3 );
  
  // raggruppo i quattro oggetti in un array
  gVar4::ptr[0] = &sotto;
  gVar4::ptr[1] = &criti;
  gVar4::ptr[2] = &sovra;

  // etichette
  gVar4::CompoName[0] = "Sotto-smorz";
  gVar4::CompoName[1] = "Smorz-critic";
  gVar4::CompoName[2] = "Sovra-smorz";
  
  // Resistenze
  gVar4::r   = 50; 	// resistenza interna del generatore - Ohm
  gVar4::rL  = 50; 	// resistenza interna dell'induttore - Ohm
  
  gVar4::R[0] =   100; 	// Ohm
  gVar4::R[1] =  2400;	// critico
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
  
  std::cout << " --------------------- " << CompoName[comp] << " --------------------- " << "\n";
      
  t   	= ptr[comp]->pData->at(0);
  Vb  	= ptr[comp]->pData->at(1);
  eV   	= ptr[comp]->pData->at(2);
  et   	= ptr[comp]->pData->at(3);
  
  size = Stat::fsize( &t );
  fMin = Stat::fmin ( &t );
  fMax = Stat::fmax ( &t );
  
  
  TGraphErrors* mod  = new TGraphErrors ( size );    

  TF1  *fsotto = new TF1();
  TF1  *fsovra = new TF1();
  TF1  *fcriti = new TF1();


  
  PrintParIniz( R[comp] + r + rL, I, C, V0 );
  
  
  
    for (int i = 0; i< size; i++) {
      
//        y = ( Vb.at(i) * exp10(-3) ) / ( R[comp] + r + rL );
//       sy = fabs( eV.at(i) * exp10(-3) / ( R[comp] + r + rL ) ) + errR *fabs(y) ;
      
       y = ( Vb.at(i) * exp10(-3) );
      sy = 0.15*fabs( eV.at(i) * exp10(-3) );
      
       x = t.at(i) * exp10(-6);
      sx = fabs( et.at(i)* exp10(-6) );
      
      mod->SetPoint( i, x, y );
      mod->SetPointError( i, sx, sy );
    }

    
    
    if ( comp == 0 ){ 		// Sotto-smorzamento
      
      
      fsotto = new TF1( "SottoDC", SottoDC, fMin ,fMax, 3 );

	fsotto->SetParName  ( 0, "ampiezz:");
	fsotto->SetParameter( 0,  C*V0*(R[comp] + r + rL) );
	
	fsotto->SetParName  ( 1, "gamma:");
	fsotto->SetParameter( 1,  Gamma     ( R[comp] + r + rL, I ) );
	
	fsotto->SetParName  ( 2, "pulsaz:");
	fsotto->SetParameter( 2,  Pulsa( R[comp] + r + rL, I, C ) );

	mod->Fit("SottoDC", "C", "", fMin, fMax);

	Canv0->cd();
	mod->GetXaxis()->SetTitle("t [s]");
	mod->GetYaxis()->SetTitle("V(t) [mV]");
	mod->SetTitle( CompoName[comp].c_str() );
	mod->SetMarkerColor(4);
	mod->SetMarkerStyle(21);

	double c = fsotto->GetParameter(0)/ (V0*(R[comp] + r + rL));
	double l1 = 1 / ( 2*fsotto->GetParameter(1) ) * (R[comp] + r + rL);
	double l2 = pow( fsotto->GetParameter(1), 2) + pow(fsotto->GetParameter(2), 2);
	       l2 = 1/(l2*c);
	
        double ec  = fsotto->GetParError(0) / fsotto->GetParameter(0) + 0.01 + 0.01;
	double el1 = fsotto->GetParError(1) / fsotto->GetParameter(1) + 0.01;
   


	std::cout << "C: "   << c  << "\n";
	std::cout << "eC%: " << ec*100 << "\n";

	std::cout << "L1: "   << l1  << "\n";
	std::cout << "eL1%: " << el1*100 << "\n";
	
	std::cout << "L2: "   << l2  << "\n";
	std::cout << "eL2%: " << " " << "\n";

	

    }
    

    
    if ( comp == 1 ) { 	// critico
      
      fcriti = new TF1("critico","[0]*[1]*[1]*x*exp(-[1]*x)",fMin,fMax); 
      
// 	fcriti->SetParName(0,"RQ_0");
// 	  fcriti->SetParameter( 0,  V0/ ( 2*I*Pulsa( R[comp] + r + rL, I, C ) )  );

	fcriti->SetParName  ( 0, "ampiezz:");
	  fcriti->SetParameter( 0,  C*V0*(R[comp] + r + rL) );
	  
	fcriti->SetParName(1,"gamma");
	  fcriti->SetParameter( 1,  Gamma     ( R[comp] + r + rL, I ) );
	
	
      mod->Fit("critico", "C", "", fMin, fMax);
      
      Canv0->cd();
	mod->GetXaxis()->SetTitle("t [s]");
	mod->GetYaxis()->SetTitle("V(t) [mV]");
	mod->SetTitle( CompoName[comp].c_str() );
	mod->SetMarkerColor(4);
	mod->SetMarkerStyle(21);

	double c = fcriti->GetParameter(0)/ (V0*(R[comp] + r + rL));
	double l1 = 1 / ( 2*fcriti->GetParameter(1) ) * (R[comp] + r + rL);

        double ec  = fcriti->GetParError(0) / fcriti->GetParameter(0) + 0.01 + 0.01;
	double el1 = fcriti->GetParError(1) / fcriti->GetParameter(1) + 0.01;
   

	std::cout << "R: "   << R[comp] + r + rL  << "\n";
	
	std::cout << "C: "   << c  << "\n";
	std::cout << "eC%: " << ec*100 << "\n";

	std::cout << "L1: "   << l1  << "\n";
	std::cout << "eL1%: " << el1*100 << "\n";
      
    }

    
   // risetto gli errori sperimentali
   for (int i = 0; i< size; i++) {
     
     sy = 0.5 * fabs( eV.at(i) * exp10(-3) );
     sx = fabs( et.at(i)* exp10(-6) );
      
     mod->SetPointError( i, sx, sy );
    }
    

    if ( comp == 2 ){	// Sovra-smorzamento
      
        fsovra = new TF1("SovraDC", SovraDC, fMin ,fMax, 3);

	fsovra->SetParName  ( 0, "V0*Q0:");
	fsovra->SetParameter( 0,  0.0023 );

	fsovra->SetParName  ( 1, "theta:");
	fsovra->SetParameter( 1,  2300 );

	fsovra->SetParName  ( 2, "eta:");
	fsovra->SetParameter( 2,  15000 );

// 	fsovra->SetParName  ( 1, "gamm:");
// 	fsovra->SetParameter( 1,  Gamma     ( R[comp] + r + rL, I ) );
// 
// 	fsovra->SetParName  ( 2, "pul.prop:");
// 	fsovra->SetParameter( 2,  1.0 / (C*I) );

    mod->Fit("SovraDC", "C", "", fMin, fMax);
    
    Canv0->cd();
      mod->GetXaxis()->SetTitle("t [s]");
      mod->GetYaxis()->SetTitle("V(t) [mV]");
      mod->SetTitle( CompoName[comp].c_str() );
      mod->SetMarkerColor(4);
      mod->SetMarkerStyle(21);

      
    double c = fsovra->GetParameter(0)/ (V0*(R[comp] + r + rL));
    double l1 = fsovra->GetParameter(2) - fsovra->GetParameter(1);
           l1 = (R[comp] + r + rL) / l1;

    double ec  = fsovra->GetParError(0) / fsovra->GetParameter(0) + 0.01 + 0.01;
    double el1 = fsovra->GetParError(2) / fsovra->GetParameter(2) + 0.01;
   
    std::cout << "R: "   << R[comp] + r + rL  << "\n";

    std::cout << "C: "   << c  << "\n";
    std::cout << "eC%: " << ec*100 << "\n";

    std::cout << "L1: "   << l1  << "\n";
    std::cout << "eL1%: " << el1*100 << "\n";
      
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





double SottoDC ( double * x, double * par )
{ double y = par[0] * exp( -par[1] * x[0] ) * ( 
					      +par[1] * cos( par[2] * x[0] )
					      +par[2] * sin( par[2] * x[0] )
					      );
  return y;}


  
  
  
double SovraDC ( double * x, double * par ) 
{ 
  double A     = par[0];
  double theta = par[1];
  double eta   = par[2];
  
  double B     = pow( 0.5*(eta-theta),2) +  pow( 0.5*(eta+theta),2);
         B     = B / (theta + eta);
  
  double  y = A * B * ( exp( -theta*x[0] ) - exp( -eta*x[0] ) );
  
  return y;}


  

double Gamma ( double R, double L )          { return R / ( 2*L ); };
double Pulsa ( double R, double L, double C) { return sqrt( fabs( -(R*R / (4*L*L)) + (1/(C*L)) ) ); };
double Fase  ( double R, double L, double C) { return atan( ( (1.0/(R*C)) - (0.5*R/L) )/ Pulsa( R, L, C ) ); };
double Q0    ( double C, double V )          { return C*V; };


void PrintParIniz( double R, double L, double C, double V ){
  using namespace gVar4;
  std::cout << "\n---------------------------------" << "\n"
  << "Q0 (sotto)     : "<<  Q0   ( C, V) 	<< "\n"
  << "Gamma       : " 	<<  Gamma( R, L )	  	<< "\n"
  << "Pulsa       : " 	<<  Pulsa( R, L, C) 	<< "\n"
  << "Fase (sotto)  : "	<<  Fase ( R, L, C) 	<< "\n"
  << "---------------------------------" << "\n\n";
  
return; };