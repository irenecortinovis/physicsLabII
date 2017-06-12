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


void C3P1_Fit( TCanvas *, int, int );
 


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
  
  // Chi2 ricalcolati
  double Chi2[16];
  
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


  // inizializza a 0 i Chi2 ricalcolati
  for (int i = 0; i < 16 ; i++) gVar::Chi2[i] =0;
  
  // prima stima ricalcola i Chi2
  for ( int i = 0; i<4; i++ )  C3P1_Fit( Canv0, i, 0 );
  
  // poi ristima con i nuovi errori su y
  for ( int i = 0; i<4; i++ ) C3P1_Fit( Canv0, i, 1 );
  
return;}




void C3P1_Fit( TCanvas * Canv0, int comp, int flag )
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
  
  
  // parametri di stampa
  std::string OutFilePrefix = "./c3/C3_P1_";
  std::string OutFileExtension = ".eps";
      

  TF1  *f1    = new TF1();	// non linear fit function
       
    
    // Mod impedenza
    for (int i = 0; i< size; i++) {
      
       y = ( Vba.at(i) / Vb.at(i) ) * ( R[comp] + r );
      sy = sqrt( pow( sigmaV*( 2/Vba.at(i) + 1/ Vb.at(i) ), 2)  + pow(errR,2) ) * y;	// propagazione errore su y
      if (flag == 1 ) sy = sqrt( Chi2[ comp + 0 ] ); // errori ricalcolati
      
       x = log10( v.at(i) );
      sx = 0.5 * ( log10( v.at(i) +1 ) - log10( v.at(i) -1) );	// precisione all'ultima cifra in scala log10
      
      mod->SetPoint( i, x, y );
      mod->SetPointError( i, sx, sy );
      
    }
    

    /// Condensatori
    //
    if ( comp <  2 ){
      
      f1 = new TF1("ModImpedenzaC", ModImpedenzaC, fMin ,fMax, 2);
      	f1->SetParameter( 0, 0 );		// resistenza
	f1->SetParameter( 1,  C[comp] );	// condensatore
	f1->SetParName( 0, "R [ohm]");
	f1->SetParName( 1, "C [F]");
	
	Canv0->Clear();
	Canv0->cd();
	mod->GetXaxis()->SetTitle("log (Freq - Hz)");
	mod->GetXaxis()->CenterTitle();
	mod->SetTitle(" Modulo impedenza");
	mod->SetMarkerColor(4);
	mod->SetMarkerStyle(21);
	
	gStyle->SetOptFit(1111);
	mod->Fit("ModImpedenzaC", "C", "", fMin, fMax);
	
	// ricalcolo del chi2
	Chi2[ comp + 0] = 0;
	y = 0; x=0;
	for ( int i = 0; i< f1->GetNumberFitPoints(); i++ ) {
	  
	  mod->GetPoint( i, x, y);
	  
	  Chi2[ comp + 0] += pow( f1->Eval(x) - y  ,2) / f1->GetNDF();
	}
	
	mod->Draw("AP");
	
	TLegend * leg = new TLegend(0.6,0.3,0.89,0.6);
	leg->AddEntry(f1, " y = #sqrt{ R^2+#frac{1}{(#omega C)^2} }", "l");
	leg->AddEntry(mod, "data", "p");
	leg->Draw();
	
	std::string OutObj = "ModImp_";
	std::string OutFileName = OutFilePrefix+OutObj+CompoName[comp]+OutFileExtension;

	Canv0->Print( OutFileName.c_str(), "eps");
	
    }
    
    
    /// Induttori
    //    
    else{
      
      f1 = new TF1("ModImpedenzaL", ModImpedenzaL, fMin ,fMax, 2);
	
	if (comp == 2) f1->SetParameter( 0,  2*rL );	// resitenza interna induttore
	if (comp == 3) f1->SetParameter( 0,  rL );	// resitenza interna induttore
	f1->SetParameter( 1,  I[comp] );
	f1->SetParName( 0, "R [ohm]");
	f1->SetParName( 1, "L [H]");
	
	
	Canv0->Clear();
	Canv0->cd();
	mod->GetXaxis()->SetTitle("log (Freq - Hz)");
	mod->GetXaxis()->CenterTitle();
	mod->SetTitle("Modulo impedenza");
	mod->SetMarkerColor(4);
	mod->SetMarkerStyle(21);
	
	gStyle->SetOptFit(1111);
	
	mod->Fit("ModImpedenzaL", "C", "", fMin, fMax);
	
	// ricalcolo del chi2
	Chi2[ comp + 0] = 0;
	y = 0; x=0;
	for ( int i = 0; i< f1->GetNumberFitPoints(); i++ ) {
	  
	  mod->GetPoint( i, x, y);
	  
	  Chi2[ comp + 0] += pow( f1->Eval(x) - y  ,2) / f1->GetNDF();
	}
	
	mod->Draw("AP");
	
	TLegend * leg = new TLegend(0.2,0.3,0.5,0.6);
	leg->AddEntry(f1, " y = #sqrt{ R^2+(#omega L)^2 }", "l");
	leg->AddEntry(mod, "data", "p");
	leg->Draw();

	std::string OutObj = "ModImp_";
	std::string OutFileName = OutFilePrefix+OutObj+CompoName[comp]+OutFileExtension;
	
	Canv0->Print( OutFileName.c_str(), "eps");     

    }


    
    
    // Arg impedenza - fase Channel 2
    for (int i = 0; i< size; i++) {
      
       y = - 2 * M_PI * fC2.at(i)*v.at(i)*exp10(-6);		// dati in microsecondi
      sy =   sqrt( pow( sfC2.at(i) / fC2.at(i), 2) + pow( 1 / v.at(i), 2) ) * fabs(y);
      sy = sy * 0.5;
      // if (flag == 1 ) sy = sqrt( Chi2[ comp + 4 ] ); // errori ricalcolati
      
       x = log10( v.at(i) );
      sx = 0.5 * ( log10( v.at(i) +1 ) - log10( v.at(i) -1) );	// precisione all'ultima cifra in scala log10      
      
      if ( comp >=  2 ) y = M_PI + y;	// induttore
      
      arg->SetPoint( i, x, y );
      arg->SetPointError( i, sx, sy  );
    }

    f1->Clear();
    

    /// Condensatori
    ///
    if ( comp <  2 ){
      
       f1 = new TF1 ("ArgImpedenzaC", ArgImpedenzaC, fMin ,fMax, 1);
     
	f1->SetParameter( 0, 50*C[comp] );
	f1->SetParName( 0, "RC [ohm*F]");
	arg->Fit("ArgImpedenzaC", "C", "", fMin, fMax);
    
	Canv0->Clear();
	Canv0->cd();
	arg->GetXaxis()->SetTitle("log (Freq - Hz)");
	arg->GetXaxis()->CenterTitle();
	arg->SetTitle("Argomento impedenza");
	arg->SetMarkerColor(4);
	arg->SetMarkerStyle(21);
	
	gStyle->SetOptFit(1111);
	
	if (comp == 0) arg->Fit("ArgImpedenzaC", "C", "", fMin, fMax);
	
	// escludo l'ultimo dato dalla stima --> outlier
	if (comp == 1) arg->Fit("ArgImpedenzaC", "C", "", fMin, 4.2);
	
	
	// ricalcolo del chi2
	Chi2[ comp + 4] = 0;
	y = 0; x=0;
	for ( int i = 0; i< f1->GetNumberFitPoints(); i++ ) {
	  
	  mod->GetPoint( i, x, y);
	  
	  Chi2[ comp + 4] += pow( f1->Eval(x) - y  ,2) / f1->GetNDF();
	}
	
	
	arg->Draw("AP");
	
	TLegend * leg = new TLegend(0.1,0.75,0.5,0.90);
	leg->AddEntry(f1, " y = -arctan{ 1 / ( w *(RC) ) }", "l");
	leg->AddEntry(arg, "data", "p");
	leg->Draw();

	std::string OutObj = "ArgImp_";
	std::string OutFileName = OutFilePrefix+OutObj+CompoName[comp]+OutFileExtension;

	Canv0->Print( OutFileName.c_str(), "eps");     

    }


    else{	// induttori
      
      f1 = new TF1 ("ArgImpedenzaL", ArgImpedenzaL, fMin ,fMax, 1);
	f1->SetParameter( 0, I[comp] / ( r + rL ) );
	f1->SetParName  ( 0, "L/R [H/ohm]");

      // f2 = new TF1 ("fitted", ArgImpedenzaL, fMin ,fMax, 1);
	// if (comp == 3) f2->SetParameter( 0, 0.120 / 117 );
	// if (comp == 4) f2->SetParameter( 0, 0.046 /  67 );
	
	Canv0->Clear();
	Canv0->cd();
	arg->GetXaxis()->SetTitle("log (Freq - Hz)");
	arg->GetXaxis()->CenterTitle();
	arg->SetTitle("Argomento impedenza");
	arg->SetMarkerColor(4);
	arg->SetMarkerStyle(21);
	
	gStyle->SetOptFit(1111);
	
	// esclud l'ultimo dato
	if ( comp == 2 ) arg->Fit("ArgImpedenzaL", "C", "", fMin, 4.2);
	if ( comp == 3 ) arg->Fit("ArgImpedenzaL", "C", "", fMin, fMax);
	
	
	// ricalcolo del chi2
	Chi2[ comp + 4] = 0;
	y = 0; x=0;
	for ( int i = 0; i< f1->GetNumberFitPoints(); i++ ) {
	  
	  mod->GetPoint( i, x, y);
	  
	  Chi2[ comp + 4] += pow( f1->Eval(x) - y  ,2) / f1->GetNDF();
	}
	
	
	arg->Draw("AP");
	
	
	TLegend * leg = new TLegend(0.6,0.25,0.89,0.5);
	leg->AddEntry(f1, " y = arctan{  w * (L/R) ) }", "l");
	leg->AddEntry(arg, "data", "p");
	leg->Draw();

	std::string OutObj = "ArgImp_";
	std::string OutFileName = OutFilePrefix+OutObj+CompoName[comp]+OutFileExtension;

	Canv0->Print( OutFileName.c_str(), "eps");     

    }
    


    /////////////////////// Funzioni di Trasferimento
    
    
    // Modulo fdt
    for (int i = 0; i< size; i++) {
      
       y = Vba.at(i) / Va.at(i);
      sy = y * sqrt( pow(errR,2) + pow( sigmaV*( 2/ Vba.at(i) + 1/ Va.at(i) ), 2) );	  // propagazione errore su y
      sy = sy * 0.15;
      //if (flag == 1 ) sy = sqrt( Chi2[ comp + 8 ] ); // errori ricalcolati
      
       x = log10( v.at(i) );
      sx = 0.5 * ( log10( v.at(i) +1 ) - log10( v.at(i) -1) );	// precisione all'ultima cifra in scala log10      
      
      tmod->SetPoint( i, x, y );
      tmod->SetPointError( i, sx, sy  );
    }
    


    
    if ( comp <  2 ){ // condensatori
      
      f1->Clear();
      f1 = new TF1 ("ModFdtC", ModFdtC, fMin ,fMax, 1);
	
	f1->SetParameter( 0, ( R[comp] + r )*C[comp] );
	f1->SetParName  ( 0, "RC [ohm*F]");
	
	Canv0->Clear();
	Canv0->cd();
	tmod->GetXaxis()->SetTitle("log (Freq - Hz)");
	tmod->GetXaxis()->CenterTitle();
	tmod->SetTitle("Modulo Funz. di trasferimento");
	tmod->SetMarkerColor(4);
	tmod->SetMarkerStyle(21);
	
	gStyle->SetOptFit(1111);
	tmod->Fit("ModFdtC", "C", "", fMin, fMax);
	
	
	// ricalcolo del chi2
	Chi2[ comp + 8] = 0;
	y = 0; x=0;
	for ( int i = 0; i< f1->GetNumberFitPoints(); i++ ) {
	  
	  mod->GetPoint( i, x, y);
	  
	  Chi2[ comp + 8] += pow( f1->Eval(x) - y  ,2) / f1->GetNDF();
	}
	
	
	tmod->Draw("AP");
	
	TLegend * leg = new TLegend(0.15,0.2,0.50,0.4);
	leg->AddEntry(f1, " y = { 1 + (#omega^2)*(RC)^2 }^{-1/2}", "l");
	leg->AddEntry( tmod, "data", "p");
	leg->Draw();

	std::string OutObj = "ModFdT_";
	std::string OutFileName = OutFilePrefix+OutObj+CompoName[comp]+OutFileExtension;
	
	Canv0->Print( OutFileName.c_str(), "eps");
    }
    
    
    
     else { 	// induttori
      
       f1 = new TF1 ("ModFdtL", ModFdtL, fMin ,fMax, 2);
	
	f1->SetParameter( 0, R[comp] + r );
	f1->SetParameter( 1, I[comp] );
	f1->SetParName  ( 0, "R [ohm]");
	f1->SetParName  ( 1, "L [h]");

	Canv0->Clear();
	Canv0->cd();
	tmod->GetXaxis()->SetTitle("log (Freq - Hz)");
	tmod->GetXaxis()->CenterTitle();
	tmod->SetTitle("Modulo Funz. di trasferimento");
	tmod->SetMarkerColor(4);
	tmod->SetMarkerStyle(21);
	
	gStyle->SetOptFit(1111);
	tmod->Fit("ModFdtL", "C", "", fMin, fMax);
	
	
	// ricalcolo del chi2
	Chi2[ comp + 8] = 0;
	y = 0; x=0;
	for ( int i = 0; i< f1->GetNumberFitPoints(); i++ ) {
	  
	  mod->GetPoint( i, x, y);
	  
	  Chi2[ comp + 8] += pow( f1->Eval(x) - y  ,2) / f1->GetNDF();
	}
	
	tmod->Draw("AP");
	
	TLegend * leg = new TLegend(0.6,0.15,0.9,0.40);
	leg->AddEntry(f1, " y = w*L /#sqrt{ R^2+ (#omega^2)*(L)^2 }", "l");
	leg->AddEntry( tmod, "data", "p");
	leg->Draw();

	std::string OutObj = "ModFdT_";
	std::string OutFileName = OutFilePrefix+OutObj+CompoName[comp]+OutFileExtension;
	
	Canv0->Print( OutFileName.c_str(), "eps");

    }
    
    
    
    // Arg fdt
    for (int i = 0; i< size; i++) {
      
       y =   2 * M_PI * fC1.at(i)*v.at(i)*exp10(-6);	// dati in microsecondi
      sy =   sqrt( pow( sfC1.at(i) / fC1.at(i), 2) + pow( 1 / v.at(i), 2) ) * fabs(y);
      
      if ( comp < 2  )  y = y - M_PI;
      if ( comp >= 2 )  y = y - M_PI;
      
      
       x = log10( v.at(i) );
      sx = 0.5 * ( log10( v.at(i) +1 ) - log10( v.at(i) -1) ) ;	// precisione all'ultima cifra in scala log10      
      
      targ->SetPoint( i, x, y );
      targ->SetPointError( i, sx, sy  );
      
    }


    
    if ( comp <  2 ){ // condensatori
 
    f1 = new TF1 ("ArgFdtC", ArgFdtC, fMin ,fMax, 1);
      f1->SetParameter( 0, C[comp]*(R[comp]+r) );
      f1->SetParName  ( 0, "RC [ohm*F]");
      
      Canv0->Clear();
      Canv0->cd();
      targ->GetXaxis()->SetTitle("log (Freq - Hz)");
      targ->GetXaxis()->CenterTitle();
      targ->SetTitle("Argomento Funz. di trasferimento");
      targ->SetMarkerColor(4);
      targ->SetMarkerStyle(21);
	
      gStyle->SetOptFit(1111);
      targ->Fit("ArgFdtC", "C", "", fMin, fMax);
      
      
      	// ricalcolo del chi2
	Chi2[ comp + 12] = 0;
	y = 0; x=0;
	for ( int i = 0; i< f1->GetNumberFitPoints(); i++ ) {
	  
	  mod->GetPoint( i, x, y);
	  
	  Chi2[ comp + 12] += pow( f1->Eval(x) - y  ,2) / f1->GetNDF();
	}
      
      
      targ->Draw("AP");
	
      TLegend * leg = new TLegend(0.16,0.25,0.45,0.5);
      leg->AddEntry(f1, " y = arctan{  -w * (RC) ) }", "l");
      leg->AddEntry(targ, "data", "p");
      leg->Draw();

      std::string OutObj = "ArgFdT_";
      std::string OutFileName = OutFilePrefix+OutObj+CompoName[comp]+OutFileExtension;

      Canv0->Print( OutFileName.c_str(), "eps");     

    }
    
    
    else {
      
    f1 = new TF1 ("ArgFdtL", ArgFdtL, fMin ,fMax, 1);
      f1->SetParameter( 0, ( R[comp] + r ) / I[comp] );
      f1->SetParName  ( 0, "R/L [ohm/H]");

      Canv0->Clear();
      Canv0->cd();
      targ->GetXaxis()->SetTitle("log (Freq - Hz)");
      targ->GetXaxis()->CenterTitle();
      targ->SetTitle("Argomento Funz. di trasferimento");
      targ->SetMarkerColor(4);
      targ->SetMarkerStyle(21);
	
      gStyle->SetOptFit(1111);
      
      targ->Fit("ArgFdtL", "C", "", fMin, fMax);
      
      
      // ricalcolo del chi2
      Chi2[ comp + 12] = 0;
      y = 0; x=0;
      for ( int i = 0; i< f1->GetNumberFitPoints(); i++ ) {
	  
	mod->GetPoint( i, x, y);
	  
	Chi2[ comp + 12] += pow( f1->Eval(x) - y  ,2) / f1->GetNDF();
      }
      
      
      targ->Draw("AP");
	
      TLegend * leg = new TLegend(0.11,0.71,0.35,0.91);
      leg->AddEntry(f1, "y = arctan{  (R/L) / w ) }", "l");
      leg->AddEntry(targ, "data", "p");
      leg->Draw();

      std::string OutObj = "ArgFdT_";
      std::string OutFileName = OutFilePrefix+OutObj+CompoName[comp]+OutFileExtension;

      Canv0->Print( OutFileName.c_str(), "eps");     
      
    }

    
    // stampa errori ricalcolati
    std::cout << "\n\n Stampa errori ricalcolati \n\n ";
    for (int i = 0; i < 16 ; i++) std::cout << sqrt( Chi2[i] ) << "\n" <<  std::endl;
  
return;}



void ComponentNameMessage( int i )
{
  
  using namespace gVar;
  
  std::cout << "\n\n" << CompoName[i] << "\n\n";
  
  return;
}



// NOTE: x[0] è la frequenza [Hz]. Sul TGraphErrors è in scala log10

double ModImpedenzaC ( double * x, double * par ) 
{ 
  double w = 2 * M_PI * exp10( x[0] );
  return sqrt (  par[0]*par[0] + 1.0 / ( par[1]*par[1]*w*w ) );  
}


double ArgImpedenzaC ( double * x, double * par )
{ 
  double w = 2 * M_PI * exp10( x[0] );
  return - atan (  1.0 / ( par[0]*w ) );  
}


double ModImpedenzaL ( double * x, double * par )
{ 
  double w = 2 * M_PI * exp10( x[0] );
  return sqrt( 	par[0]*par[0] + w*w*par[1]*par[1]  );
}


double ArgImpedenzaL ( double * x, double * par )
{ 
  double w = 2 * M_PI * exp10( x[0] );
  return atan (  w*par[0] );  
}



////////////////////////////////////////////////////////////
/// Funzioni di trasferimento


double ModFdtC ( double * x, double * par )
{ 
  double w = 2 * M_PI * exp10( x[0] );
  return 1.0 / sqrt( 1 + par[0] * par[0] * w * w );
}


double ArgFdtC ( double * x, double * par )
{ 
  double w = 2 * M_PI * exp10( x[0] );
  return atan (  - w * par[0] );  
}


double ModFdtL ( double * x, double * par )
{ 
  double w = 2 * M_PI * exp10( x[0] );
  return w * par[1] / sqrt( par[0] * par[0] + w * w * par[1] * par[1] );
}


double ArgFdtL ( double * x, double * par )
{ 
  double w = 2 * M_PI * exp10( x[0] );
  return atan (  par[0]/w );  
}