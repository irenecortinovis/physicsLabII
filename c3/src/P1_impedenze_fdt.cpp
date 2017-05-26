//
// Author: Alfredo Guarnieri   24/03/17
//
// C3 P1 misura funzione di trasferimento e impedenza




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
#include <TLinearFitter.h>


#include "../../stat1.h"
#include "../../check.h"
#include "../../funzioni.h"



 
//  il programma è diviso in tre parti:

//  Parte 0:
//  comume, importazione dati e definzione costanti

//   0 Freq
//   1 Va
//   2 Vb
//   3 Vb-a (MATH)
//   4 -Fase (MATH-CH2)
//   5 errFase CH2
//   6 -Fase (MATH-CH1)
//   7 errFase CH1  


void C3P1_Fit( TCanvas *, int );
 


// struttura dati elaborati
namespace gVar{
  
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
  class stat1* ptr[4];
  
 
  // etichette dati
  std::string CompoName[4];
  
  
  // Fit output
  TFitResultPtr Outptr[4][2];	// 4 componenti X 2 (modulo e fase)
    
    
  // Variabili ausiliarie
  double y, sy, x, sx, size, fMin, fMax;
  
  // resistori
  double r ;	// resistenza interna generatore
  double rL ;	// resistenza interna induttore
  double R[4];	// resistsenza resitori
  
  // capacità
  double C[2];
  
  // induttanze
  double I[2];
  
  // Errori relativi
  double errR = 1*exp10(-2);	//  ERRORE RELATIVO sulle resistenze 1.0%
    
  // Errori assoluti
  double sigmaV = 0.08;		//  ERRORE ASSOLUTO sulla ddp generata in [volt]

}	// end namespace gVar




// Utilities
void ComponentNameMessage( int );


// funzioni per Non Linear Fit
double ModImpedenzaC( double*, double* );
double ArgImpedenzaC( double*, double* );
double ModFdtC( double*, double* );
double ArgFdtC( double*, double* );
double ModImpedenzaL( double*, double* );
double ArgImpedenzaL( double*, double* );
double ModFdtL( double*, double* );
double ArgFdtL( double*, double* );



//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


void P1_impedenze_fdt( TCanvas * Canv0 ) {



  ///////////////////////////////////////////////////////////
  /// Data input step

  std::string path  = "./c3/data/";
    std::string file1 = "P1_condens_1.txt";
    std::string file2 = "P1_condens_2.txt";
    std::string file3 = "P1_indut_1.txt";
    std::string file4 = "P1_indut_2.txt";
  
  class stat1 cond1  ( path, file1 , 8, '\t', 3 );
  class stat1 cond2  ( path, file2 , 8, '\t', 3 );
  class stat1 indut1 ( path, file3 , 8, '\t', 3 );
  class stat1 indut2 ( path, file4 , 8, '\t', 3 );
  
  // raggruppo i quattro oggetti in un array
  gVar::ptr[0] = &cond1;
  gVar::ptr[1] = &cond2;
  gVar::ptr[2] = &indut1;
  gVar::ptr[3] = &indut2;

  // etichette
  gVar::CompoName[0] = "cond1";
  gVar::CompoName[1] = "cond2";
  gVar::CompoName[2] = "ind1";
  gVar::CompoName[3] = "ind2";
  
  // Resistenze
  gVar::r   = 50; 	// resistenza interna del generatore - Ohm
  gVar::rL  = 60; 	// resistenza interna dell'induttore - Ohm
  
  gVar::R[0] = 14870; 	// Ohm
  gVar::R[1] =   677;
  gVar::R[2] =   gVar::R[1] + gVar::rL;
  gVar::R[3] =   gVar::R[1] + gVar::rL;
  
  // Capacità - valori iniziali
  gVar::C[0] =    1.1 * exp10(-8); 	// farad
  gVar::C[1] =    4.6 * exp10(-8);
  
  // Induttanze - valori iniziali
  gVar::I[2] =   0.12; 	// henry
  gVar::I[3] =   0.05;
  
  /////////////////////////////////////////////////////




  for ( int i = 0; i<4; i++ ) C3P1_Fit( Canv0, i );
  
 
  
    
  
return;}




void C3P1_Fit( TCanvas * Canv0, int comp )
{ 
  
  using namespace gVar;
  
  std::cout << " -------------- Non Linear Fit -------------- " << "\n";
  
  ComponentNameMessage( comp );
  
    
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

  

  TGraphErrors* mod  = new TGraphErrors ( size );
  TGraphErrors* arg  = new TGraphErrors ( size );
  TGraphErrors* tmod = new TGraphErrors ( size );
  TGraphErrors* targ = new TGraphErrors ( size );
  
    mod->GetXaxis()->SetTitle("log (Freq - Hz)");
    mod->GetYaxis()->SetTitle("|Z|");
    mod->SetTitle(" Modulo impedenza  ");
    mod->SetMarkerColor(4);
    mod->SetMarkerStyle(21);
    
    arg->GetXaxis()->SetTitle("log (Freq - Hz)");
    arg->GetYaxis()->SetTitle("Arg(Z)");
    arg->SetTitle(" Argomento impedenza  ");
    arg->SetMarkerColor(4);
    arg->SetMarkerStyle(21);

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

    

    TF1  *f1    = new TF1();	// non linear fit function
    
    TLinearFitter * regressione = new TLinearFitter();
    

    
    
    // Mod impedenza
    TF1 *lf1 = new TF1("lf1", "1++x", fMin, fMax);
    regressione->SetFormula(lf1);
    
    
    for (int i = 0; i< size; i++) {
      
       y = ( Vba.at(i) / Vb.at(i) ) * ( R[comp] + r );
      sy = sqrt( pow( sigmaV*( 2/Vba.at(i) + 1/ Vb.at(i) ), 2)  + pow(errR,2) ) * y;	// propagazione errore su y
      
       x = log10( v.at(i) );
      sx = 0.5 * ( log10( v.at(i) +1 ) - log10( v.at(i) -1) );	// precisione all'ultima cifra in scala log10
      
      mod->SetPoint( i, x, y );
      mod->SetPointError( i, sx, sy );
      

    }
    
    f1->Clear();
    
    if ( comp <  2 ){ // condensatori
      
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


    
    
    // Arg impedenza - fase Channel 2
    for (int i = 0; i< size; i++) {
      
       y = - 2 * M_PI * fC2.at(i)*v.at(i)*exp10(-6);		// dati in microsecondi
      sy =   sqrt( pow( sfC2.at(i) / fC2.at(i), 2) + pow( 1 / v.at(i), 2) ) * fabs(y);
      
       x = log10( v.at(i) );
      sx = 0.5 * ( log10( v.at(i) +1 ) - log10( v.at(i) -1) );	// precisione all'ultima cifra in scala log10      
      
      if ( comp >=  2 ) y = M_PI + y;	// induttore
      
      arg->SetPoint( i, x, y );
      arg->SetPointError( i, sx, sy  );
    }

    f1->Clear();
    
    if ( comp <  2 ){ // condensatori
      f1 = new TF1 ("ArgImpedenzaC", ArgImpedenzaC, fMin ,fMax, 2);
	// par[0] + par[1] * x[0]
	f1->SetParameter( 0, 0.0 );
	f1->FixParameter( 1, 0.0 );
	f1->SetParName( 0, "phi [rad]");
	arg->Fit("ArgImpedenzaC", "C", "", fMin, fMax);
    }

    else{	// induttori
      
      f1 = new TF1 ("ArgImpedenzaL", ArgImpedenzaL, fMin ,fMax, 2);
	// atan( 2 * M_PI * exp10( x[0] ) * par[1] / par[0] )
	f1->FixParameter( 0, rL );
	f1->SetParameter( 1, I[comp] );
	f1->SetParName  ( 0, "R [ohm]");
	f1->SetParName  ( 1, "L [H]");
	
      	arg->Fit("ArgImpedenzaL", "C", "", fMin, fMax);
    }
    
    
    // Modulo fdt
    for (int i = 0; i< size; i++) {
      
       y = Vba.at(i) / Va.at(i);
      sy = y * sqrt( pow(errR,2) + pow( sigmaV*( 2/ Vba.at(i) + 1/ Va.at(i) ), 2) );	  // propagazione errore su y
      
       x = log10( v.at(i) );
      sx = 0.5 * ( log10( v.at(i) +1 ) - log10( v.at(i) -1) );	// precisione all'ultima cifra in scala log10      
      
      tmod->SetPoint( i, x, y );
      tmod->SetPointError( i, sx, sy  );
    }
    

    f1->Clear();
    
    if ( comp <  2 ){ // condensatori
      
      f1 = new TF1 ("ModFdtC", ModFdtC, fMin ,fMax, 2);
	// 1.0 / sqrt( 1 + par[0] * par[0] * par[1] * par[1] * 4 * M_PI * M_PI * exp10( 2 * x[0] ) )
	f1->FixParameter( 0, R[comp] + r );
	f1->SetParameter( 1, C[comp] );
	f1->SetParName  ( 0, "R [ohm]");
	f1->SetParName  ( 1, "C [F]");

	tmod->Fit("ModFdtC", "C", "", fMin, fMax);
    }
    
     else { 	// induttori
      
       f1 = new TF1 ("ModFdtL", ModFdtL, fMin ,fMax, 2);
	// sqrt( 1 / ( 1 + exp10( -2 * x[0] ) * 4 * M_PI * M_PI * par[0] * par[0] / (par[1] * par[1]) ) )
	f1->FixParameter( 0, rL );
	f1->SetParameter( 1, I[comp] );
	f1->SetParName  ( 0, "R [ohm]");
	f1->SetParName  ( 1, "L [h]");

	tmod->Fit("ModFdtL", "C", "", fMin, fMax);
    }
    
    
    
    // Arg fdt
    for (int i = 0; i< size; i++) {
      
       y =   2 * M_PI * fC1.at(i)*v.at(i)*exp10(-6);	// dati in microsecondi
      sy =   sqrt( pow( sfC1.at(i) / fC1.at(i), 2) + pow( 1 / v.at(i), 2) ) * fabs(y);
      
      if ( comp < 2  )  y = y - M_PI;
      if ( comp >= 2 )  y = - y + M_PI*0.5;
      
      
       x = log10( v.at(i) );
      sx = 0.5 * ( log10( v.at(i) +1 ) - log10( v.at(i) -1) ) ;	// precisione all'ultima cifra in scala log10      
      
      targ->SetPoint( i, x, y );
      targ->SetPointError( i, sx, sy  );
      
    }

    f1->Clear();
    
    if ( comp <  2 ){ // condensatori
 
    f1 = new TF1 ("ArgFdtC", ArgFdtC, fMin ,fMax, 2);
    // atan( - 2 * M_PI * exp10( x[0] ) * par[0] * par[1] )
      f1->FixParameter( 0, R[comp] + r );
      f1->SetParameter( 1, C[comp] );
      f1->SetParName  ( 0, "R [ohm]");
      f1->SetParName  ( 1, "C [F]");
	
      targ->Fit("ArgFdtC", "C", "", fMin, fMax);
    }
    
    else {
      
    f1 = new TF1 ("ArgFdtL", ArgFdtL, fMin ,fMax, 2);
    // atan( - 1.0 * ( par[0] / ( par[1] * 2 * M_PI * exp10( x[0] ) ) ) )
      f1->FixParameter( 0, rL );
      f1->SetParameter( 1, I[comp] );
      f1->SetParName  ( 0, "R [ohm]");
      f1->SetParName  ( 1, "L [H]");

      targ->Fit("ArgFdtL", "C", "", fMin, fMax);
      
    }

    
    
    Canv0->SetGrid();
    Canv0->Divide(2,2); 
    gStyle->SetOptFit(1111);
    
      Canv0->cd(1);  mod->Draw("AP");
      Canv0->cd(2);  arg->Draw("AP");
      Canv0->cd(3); tmod->Draw("AP");
      Canv0->cd(4); targ->Draw("AP");
      
      
      
    ///////////////////////////////////////////////////////////
    /// Print eps
    
    std::string OutFilePrefix = "./c3/C3_P1_";
    std::string OutFileExtension = ".eps";
    std::string OutFileName = OutFilePrefix+CompoName[comp]+OutFileExtension;
    
     Canv0->Print( OutFileName.c_str(), "eps");
     Canv0->Clear();
     
 
  
return;}



void ComponentNameMessage( int i )
{
  
  using namespace gVar;
  
  std::cout << "\n\n" << CompoName[i] << "\n\n";
  
  return;
}



double ModImpedenzaC ( double * x, double * par ) // x[0] è la frequenza [Hz]. Sul TGraphErrors è in scala log10
{ return 1.0 / (  par[0] + 2 * M_PI * par[1] * exp10( x[0] ) );  }

double ArgImpedenzaC ( double * x, double * par )
{ return par[0] + par[1] * x[0];  }

double ModFdtC ( double * x, double * par )
{ return 1.0 / sqrt( 1 + par[0] * par[0] * par[1] * par[1] * 4 * M_PI * M_PI * exp10( 2 * x[0] ) ); }

double ArgFdtC ( double * x, double * par )
{ return atan( - 2 * M_PI * exp10( x[0] ) * par[0] * par[1] ); }

double ModImpedenzaL ( double * x, double * par )
{ return sqrt( par[0]*par[0] + par[1]*par[1]*exp10( 2 * x[0] ) * 4 * M_PI * M_PI ); }

double ArgImpedenzaL ( double * x, double * par )
{ return atan( 2 * M_PI * exp10( x[0] ) * par[1] / par[0] ); }

double ModFdtL ( double * x, double * par )
{ return sqrt( 1 / ( 1 + exp10( -2 * x[0] ) * 4 * M_PI * M_PI * par[0] * par[0] / (par[1] * par[1]) ) ); }

double ArgFdtL ( double * x, double * par )
{ return atan( - 1.0 * ( par[0] / ( par[1] * 2 * M_PI * exp10( x[0] ) ) ) ); }