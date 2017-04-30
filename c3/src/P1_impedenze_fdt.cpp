//
// Author: Alfredo Guarnieri   24/03/17
//
// C3 P1 misura funzione di trasferimento e impedenza




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

//  Non Linear Fit
void NonLinearFit( TCanvas *, int );
void ShowFit();

//  Linear Fit
void LinearFit( TCanvas * ); 
 


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
  gVar::CompoName[0] = "Conduttore 1";
  gVar::CompoName[1] = "Conduttore 2";
  gVar::CompoName[2] = "Induttore 1";
  gVar::CompoName[3] = "Induttore 2";
  
  // Resistenze
  gVar::r   = 50; 	// resistenza interna del generatore - Ohm
  gVar::rL  = 50; 	// resistenza interna dell'induttore - Ohm
  
  gVar::R[0] = 14870; 	// Ohm
  gVar::R[1] =   677;
  gVar::R[2] =   gVar::R[1] + gVar::rL;
  gVar::R[3] =   gVar::R[1] + gVar::rL;
  
  // Capacità
  gVar::C[0] =    1.1 * exp10(-8); 	// farad
  gVar::C[1] =    4.6 * exp10(-8);
  
  // Induttanze
  gVar::I[2] =   0.12; 	// henry
  gVar::I[3] =   0.05;
  
  /////////////////////////////////////////////////////




  // Non linear Fit

  for ( int i = 0; i<4; i++ ) NonLinearFit( Canv0, i );
  
  // Linear Fit
  
  
    
  
return;}




  ///////////////////////////////////////////////////////////
  /// Non linear Fit


void NonLinearFit( TCanvas * Canv0, int comp )
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

    // riempito all'occorrenza
    TF1  *f1 = new TF1();

    
    
    // Mod impedenza
    for (int i = 0; i< size; i++) {
      
       y = ( Vba.at(i) / Vb.at(i) ) * ( R[comp] + r );
      sy = ( sigmaV*( 1/Vba.at(i) + 1/ Vb.at(i) )  + errR ) * y;	// propagazione errore su y
      
       x = log10( v.at(i) );
      sx = log10( v.at(i) +1 ) - log10( v.at(i) -1);	// precisione all'ultima cifra in scala log10
      
      mod->SetPoint( i, x, y );
      mod->SetPointError( i, sx, sy );
    }
    
    f1->Clear();
    
    if ( comp <  2 ){ // condensatori
      
      f1 = new TF1("ModImpedenzaC", ModImpedenzaC, fMin ,fMax, 2);
	// 1.0 / (  par[0] + 2 * M_PI * par[1] * exp10( x[0] ) )
	f1->FixParameter( 0, 0 );
	f1->SetParameter( 1,  C[comp] );
	mod->Fit("ModImpedenzaC", "C", "", fMin, fMax);
    }
    
    else{	// induttori
      
      f1 = new TF1("ModImpedenzaL", ModImpedenzaL, fMin ,fMax, 2);
	// sqrt( par[0]*par[0] + par[1]*par[1]*exp10( 2 * x[0] ) * 4 * M_PI * M_PI )
	f1->FixParameter( 0,  rL );	// resitenza interna induttore
	f1->SetParameter( 1,  I[comp] );
	mod->Fit("ModImpedenzaL", "C", "", fMin, fMax);
    }


    
    
    // Arg impedenza - fase Channel 2
    for (int i = 0; i< size; i++) {
      
       y = - 6.28* fC2.at(i)*v.at(i)*exp10(-6);		// dati in microsecondi
      sy =   6.28*sfC2.at(i)*v.at(i)*exp10(-6);
      
       x = log10( v.at(i) );
      sx = log10( v.at(i) +1 ) - log10( v.at(i) -1);	// precisione all'ultima cifra in scala log10      
      
      if ( comp >=  2 ) y = M_PI + y;	// induttore
      
      arg->SetPoint( i, x, y );
      arg->SetPointError( i, sx, sy  );
    }

    f1->Clear();
    
    if ( comp <  2 ){ // condensatori
      f1 = new TF1 ("ArgImpedenzaC", ArgImpedenzaC, fMin ,fMax, 2);
	// par[0] + par[1] * x[0]
	f1->SetParameter( 0, M_PI*0.5 );
	f1->FixParameter( 1, 0.0 );

	arg->Fit("ArgImpedenzaC", "C", "", fMin, fMax);
    }

    else{	// induttori
      
      f1 = new TF1 ("ArgImpedenzaL", ArgImpedenzaL, fMin ,fMax, 2);
	// atan( 2 * M_PI * exp10( x[0] ) * par[1] / par[0] )
	f1->FixParameter( 0, rL );
	f1->SetParameter( 1, I[comp] );
	
      	arg->Fit("ArgImpedenzaL", "C", "", fMin, fMax);
    }
    
    
    // Modulo fdt
    for (int i = 0; i< size; i++) {
      
       y = Vba.at(i) / Va.at(i);
      sy = y*( errR + sigmaV*( 1/ Vba.at(i) + 1/ Va.at(i) ) );	  // propagazione errore su y
      
       x = log10( v.at(i) );
      sx = log10( v.at(i) +1 ) - log10( v.at(i) -1);	// precisione all'ultima cifra in scala log10      
      
      tmod->SetPoint( i, x, y );
      tmod->SetPointError( i, sx, sy  );
    }
    

    f1->Clear();
    
    if ( comp <  2 ){ // condensatori
      
      f1 = new TF1 ("ModFdtC", ModFdtC, fMin ,fMax, 2);
	// 1.0 / sqrt( 1 + par[0] * par[0] * par[1] * par[1] * 4 * M_PI * M_PI * exp10( 2 * x[0] ) )
	f1->FixParameter( 0, R[comp] );
	f1->SetParameter( 1, C[comp] );

	tmod->Fit("ModFdtC", "C", "", fMin, fMax);
    }
    
     else { 	// induttori
      
       f1 = new TF1 ("ModFdtL", ModFdtL, fMin ,fMax, 2);
	// sqrt( 1 / ( 1 + exp10( -2 * x[0] ) * 4 * M_PI * M_PI * par[0] * par[0] / (par[1] * par[1]) ) )
	f1->FixParameter( 0, rL );
	f1->SetParameter( 1, I[comp] );

	tmod->Fit("ModFdtL", "C", "", fMin, fMax);
    }
    
    
    // Arg fdt
    for (int i = 0; i< size; i++) {
      
       y =   2 * M_PI * fC1.at(i)*v.at(i)*exp10(-6);	// dati in microsecondi
      sy =   2 * M_PI * sfC1.at(i)*v.at(i)*exp10(-6);
      
       x = log10( v.at(i) );
      sx = log10( v.at(i) +1 ) - log10( v.at(i) -1);	// precisione all'ultima cifra in scala log10      
      
      targ->SetPoint( i, x, y - M_PI );
      targ->SetPointError( i, sx, sy  );
      
    }

    f1->Clear();
    
    if ( comp <  2 ){ // condensatori
 
    f1 = new TF1 ("ArgFdtC", ArgFdtC, fMin ,fMax, 2);
    // atan( - 2 * M_PI * exp10( x[0] ) * par[0] * par[1] )
      f1->FixParameter( 0, R[comp] + r );
      f1->SetParameter( 1, C[comp] );

      targ->Fit("ArgFdtC", "L", "", fMin, fMax);
    }
    
    else {
      
    f1 = new TF1 ("ArgFdtL", ArgFdtL, fMin ,fMax, 2);
    // atan( - exp10( - x[0] ) * par[0] / ( par[1] * 2 * M_PI )  )
      f1->FixParameter( 0, rL );
      f1->SetParameter( 1, I[comp] );

      targ->Fit("ArgFdtC", "L", "", fMin, fMax);
      
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
    
    std::string OutFilePrefix = "C3_P1_NonLinFit_";
    std::string OutFileExtension = ".eps";
    std::string OutFileName = OutFilePrefix+CompoName[comp]+OutFileExtension;
    
     Canv0->Print( OutFileName.c_str(), "eps");
     Canv0->Clear();
     


  
  
  
  



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

return;
}

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
{ return atan( - exp10( - x[0] ) * par[0] / ( par[1] * 2 * M_PI )  ); }












/*
void LinearFit( TCanvas * Canv0 )
{
  
  using namespace gVar;
  
    std::cout << " -------------- Linear Fit -------------- " << "\n";
  

  
  /////////////////////////////////////////////////////////
  // Condensatore 1
  
  comp = 0; // Cond1
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
  
  TGraphErrors* Fitmod  = new TGraphErrors ( size );
  TGraphErrors* Fitarg  = new TGraphErrors ( size );
  

  
  // Mod impedenza
  
  for (int i = 0; i< size; i++) {
      
       y = ( Vba.at(i) / Vb.at(i) ) * ( R[ comp ] + 0.0 );			// R[0] = 14870 om
      sy = ( sigmaV*( 1/Vba.at(i) + 1/ Vb.at(i) )  + errR ) / y;	// propagazione errore su y
      
       x = 2 * M_PI * v.at(i);	// in rad/s
      sx = 2 * M_PI;
      
      Fitmod->SetPoint( i, x, 1/y );
      Fitmod->SetPointError( i, sx, sy );
    }
   
  // Fit
  
  fMin = Stat::fmin( &v ) * 2 * M_PI;
  fMax = Stat::fmax( &v ) * 2 * M_PI;

  Outptr[comp][0] = Fitmod->Fit("pol1","S", "L", fMin ,fMax);  


    
  
  
  // Arg impedenza - fase Channel 2
  for (int i = 0; i< size; i++) {
      
       y = - 6.28* fC2.at(i)*v.at(i)*exp10(-6);		// dati in microsecondi
      sy =   6.28*sfC2.at(i)*v.at(i)*exp10(-6);
      
       x = 2 * M_PI * v.at(i);
      sx = 2 * M_PI;
      
      Fitarg->SetPoint( i, x, y );
      Fitarg->SetPointError( i, sx, sy  );
    }
    
  // Fit

  Outptr[0][1] = Fitarg->Fit("pol1","S", "L", fMin ,fMax);  






  /////////////////////////////////////////////////////////
  // Condensatore 2
  
  comp = 1; // Cond1
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
  


  Fitmod->Clear();
  
  // Mod impedenza
   for (int i = 0; i< size; i++) {
      
       y = ( Vba.at(i) / Vb.at(i) ) * ( R[ comp ] + 0.0 );
      sy = ( sigmaV*( 1/Vba.at(i) + 1/ Vb.at(i) )  + errR ) / y;	// propagazione errore su y
      
       x = 2 * M_PI * v.at(i);	// in rad/s
      sx = 2 * M_PI;
      
      Fitmod->SetPoint( i, x, 1/y );
      Fitmod->SetPointError( i, sx, sy );
    }
    
 
  // Fit
  
  fMin = Stat::fmin( &v ) * 2 * M_PI;
  fMax = Stat::fmax( &v ) * 2 * M_PI;
  
  Outptr[1][0] = Fitmod->Fit("pol1","S", "L", fMin ,fMax);  


    
  
    
  Fitarg->Clear();
  
  // Arg impedenza - fase Channel 2
  for (int i = 0; i< size; i++) {
      
       y = - 6.28* fC2.at(i)*v.at(i)*exp10(-6);		// dati in microsecondi
      sy =   6.28*sfC2.at(i)*v.at(i)*exp10(-6);
      
       x = 2 * M_PI * v.at(i);
      sx = 2 * M_PI;
      
      Fitarg->SetPoint( i, x, y );
      Fitarg->SetPointError( i, sx, sy  );
    }
    
  // Fit

  Outptr[comp][1] = Fitarg->Fit("pol1","S", "L", fMin ,fMax);  

  

  
    
  /////////////////////////////////////////////////////////  
  // Induttore 1
  
  comp = 2;
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
  
  
  
  Fitmod->Clear();
  
  // Mod impedenza
   for (int i = 0; i< size; i++) {
      
       y = ( Vba.at(i) / Vb.at(i) ) * ( R[ comp ] + 0.0 );
       y = pow( y, 2 );
      sy = ( sigmaV*( 1/Vba.at(i) + 1/ Vb.at(i) )  + errR ) * y * 2;	// propagazione errore su y
      
       x = 2 * M_PI * v.at(i);	// in rad/s
       x = pow( x, 2 );
      sx = 2 * M_PI * 2;
      
      Fitmod->SetPoint( i, x, y );
      Fitmod->SetPointError( i, sx, sy );
    }
    
 
  // Fit
  
  fMin = Stat::fmin( &v ) * 2 * M_PI;
  fMax = Stat::fmax( &v ) * 2 * M_PI;
  
  Outptr[comp][0] = Fitmod->Fit("pol1","S", "L", fMin ,fMax);  


    
  
    
  Fitarg->Clear();
  
  // Arg impedenza - fase Channel 2
  for (int i = 0; i< size; i++) {
      
       y = - 6.28* fC2.at(i)*v.at(i)*exp10(-6);		// dati in microsecondi
      sy =   6.28*sfC2.at(i)*v.at(i)*exp10(-6);
      
       x = 2 * M_PI * v.at(i);
      sx = 2 * M_PI;
      
      Fitarg->SetPoint( i, x, y );
      Fitarg->SetPointError( i, sx, sy  );
    }
    
  // Fit

  Outptr[comp][1] = Fitarg->Fit("pol1","S", "L", fMin ,fMax);  


  
  /////////////////////////////////////////////////////////  
  // Induttore 2
  
  comp = 3;
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
  
  
  
  Fitmod->Clear();
  
  // Mod impedenza
   for (int i = 0; i< size; i++) {
      
       y = ( Vba.at(i) / Vb.at(i) ) * ( R[comp] + 0.0 );
       y = pow( y, 2 );
      sy = ( sigmaV*( 1/Vba.at(i) + 1/ Vb.at(i) )  + errR ) * y * 2;	// propagazione errore su y
      
       x = 2 * M_PI * v.at(i);	// in rad/s
       x = pow( x, 2 );
      sx = 2 * M_PI * 2;
      
      Fitmod->SetPoint( i, x, y );
      Fitmod->SetPointError( i, sx, sy );
    }
    
 
  // Fit
  
  fMin = Stat::fmin( &v ) * 2 * M_PI;
  fMax = Stat::fmax( &v ) * 2 * M_PI;
  
  Outptr[comp][0] = Fitmod->Fit("pol1","S", "L", fMin ,fMax);  


    
  
    
  Fitarg->Clear();
  
  // Arg impedenza - fase Channel 2
  for (int i = 0; i< size; i++) {
      
       y = - 6.28* fC2.at(i)*v.at(i)*exp10(-6);		// dati in microsecondi
      sy =   6.28*sfC2.at(i)*v.at(i)*exp10(-6);
      
       x = 2 * M_PI * v.at(i);
      sx = 2 * M_PI;
      
      Fitarg->SetPoint( i, x, y );
      Fitarg->SetPointError( i, sx, sy  );
    }
    
  // Fit

  Outptr[comp][1] = Fitarg->Fit("pol1","S", "L", fMin ,fMax);  

  
  
  return;
}
*/

void ShowFit()
{
  
  using namespace gVar;
  
  
  std::cout << "\n\n" << "-------------------------" << "\n\n";
  
  for ( int i = 0; i < 2 ; i++ ){
    
    std::cout << " C" << i+1 << " | "
    <<  Outptr[i][0]->Parameter(1) << " | "
    <<  Outptr[i][0]->ParError(1) << " | "
    <<  Outptr[i][0]->Prob() << " | "
    <<  "\n";
    
  }
  
    for ( int i = 0; i < 2 ; i++ ){
    
    std::cout << " Fase-" << i+1 << " | "
    <<  Outptr[i][1]->Parameter(0) << " | "
    <<  Outptr[i][1]->ParError(0) << " | "
    <<  Outptr[i][1]->Prob() << " | "
    <<  "\n";
    
  }


  std::cout << "\n\n" << "-------------------------" << "\n\n";
  
  for ( int i = 2; i < 4 ; i++ ){
    
    std::cout << " L" << i+1 << " | "
    <<  sqrt( Outptr[i][0]->Parameter(1) ) << " | "
    <<  0.5 * Outptr[i][0]->ParError(1) << " | "
    <<  Outptr[i][0]->Prob() << " | "
    <<  "\n";
    
  }
  
    for ( int i = 0; i < 4 ; i++ ){
    
    std::cout << " Fase-" << i+1 << " | "
    <<  Outptr[i][1]->Parameter(0) << " | "
    <<  Outptr[i][1]->ParError(0) << " | "
    <<  Outptr[i][1]->Prob() << " | "
    <<  "\n";
    
  }  
  
  return;
}