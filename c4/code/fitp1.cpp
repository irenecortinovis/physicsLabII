/*	comando per compilare
			c++ plotGrafici.cpp -o prova.o `root-config --cflags --glibs`
			-o prova.o sottosmorzato.txt critico.txt sovrasmorzato.txt
*/

//librerie c++
#include <iostream>
#include <cmath>
#include <string>
#include <ctime>
#include <cstdlib>
#include <fstream>


//root
#include <TCanvas.h>
#include <TH1F.h>
#include <TF1.h>
#include <TGraph.h>
#include <TApplication.h>
#include <TGraphErrors.h>

//librerie funzioni
// #include "path/to/libreria/funzioni.h"
using namespace std;
int main(int argc, char const *argv[]) {
//-------------------------------------- preliminare
// 	string FileName = "default" ;
// 	if( argc>1 ){
// 		FileName = argv[1];
// 	}
// 	ifstream InFile;
// //-------------------------------------- apertura file
// 	InFile.open(FileName.c_str(), std::ios::in);
// //-------------------------------------- controlli
// 	if( !InFile.good() ){
// 		std::cout <<"Errore. File non trovato.\n";
// 		return 1;
// 	}

//-------------------------------------- codice

	TApplication * Grafica = new TApplication("", NULL, NULL);

	TCanvas * c1 = new TCanvas("c1", "Sottosmorzato",0,0,900,500);
	TCanvas * c2 = new TCanvas("c2", "Critico",950,0,900,500);
	TCanvas * c3 = new TCanvas("c3", "Sovrasmorzato",950,0,900,500);

//---------------------------Definizione Fit

	TF1 * fit1 = new TF1("sottosmorzato", "[0]*exp(-[1]*x)*([1]*cos([2]*x)+[2]*sin([2]*x))",0,100);
	//[0]= RCV0 [1]=gamma [2]=omega
	fit1->SetParName(0,"RCV_0");
	fit1->SetParName(1,"gamma");
	fit1->SetParName(2,"omega");

	TF1 * fit2 = new TF1("critico","[0]*[1]*[1]*x*exp(-[1]*x)",0,100); 
	//[0]=Q0R [1]=gamma
	fit2->SetParName(0,"RQ_0");
	fit2->SetParName(1,"gamma");


	TF1 * fit3 = new TF1("sovrasmorzato","[0]/[1]*(exp(-x*([2]-[1])) - exp(-x*([2]+[1])) )",0,100);
	//[0]=1/2*Q0R*(omega0)^2 [1]= beta [2]=gamma
	fit1->SetParName(0,"1/2*RQ_0*(omega_0)^2");
	fit1->SetParName(1,"beta");
	fit1->SetParName(2,"gamma");

//-----------------Definizione grafici

	TGraphErrors * Gerr1 = new TGraphErrors(argv[1], "%lg %lg %lg %lg");
	Gerr1->GetXaxis()->SetTitle("Tempo [ms]");
	Gerr1->GetYaxis()->SetTitle("Tensione [V]");

	TGraphErrors * Gerr2 = new TGraphErrors(argv[2], "%lg %lg %lg %lg");
	Gerr2->GetXaxis()->SetTitle("Tempo [ms]");
	Gerr2->GetYaxis()->SetTitle("Tensione [V]");

	TGraphErrors * Gerr3 = new TGraphErrors(argv[3], "%lg %lg %lg %lg");
	Gerr3->GetXaxis()->SetTitle("Tempo [ms]");
	Gerr3->GetYaxis()->SetTitle("Tensione [V]");

//-------------------------------------Disegna e fitta grafici
	
	//------Sottosmorzato
	c1->cd();
	Gerr1->Draw("AP");
	Gerr1->SetMarkerColor(1);
	Gerr1->SetMarkerSize(1);
	Gerr1->SetMarkerStyle(21);

	fit1->SetParameter(0,1);
	fit1->SetParameter(1,1);
	fit1->SetParameter(2,1);

	Gerr1->Fit(fit1,"C");

	//------Critico
	c2->cd();
	Gerr2->Draw("AP");
	Gerr2->SetMarkerColor(1);
	Gerr2->SetMarkerSize(1);
	Gerr2->SetMarkerStyle(21);

	fit2->SetParameter(0,1);
	fit2->SetParameter(1,1);

	Gerr2->Fit(fit2,"C");

	//------Sovrasmorzato
	c3->cd();
	Gerr3->Draw("AP");
	Gerr3->SetMarkerColor(1);
	Gerr3->SetMarkerSize(1);
	Gerr3->SetMarkerStyle(21);

	fit3->SetParameter(0,1);
	fit3->SetParameter(1,1);
	fit3->SetParameter(2,1);

	Gerr3->Fit(fit3,"C");

//--------------------------------------Esporta immagini
	c1->Print("sottosmorzato.eps","eps");
	c2->Print("critico.eps","eps");
	c3->Print("sovrasmorzato.eps","eps");





	// Gerr1->DrawPanel();


	Grafica->Run();

	return 0;
}
