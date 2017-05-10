/*	comando per compilare
c++ ../src/fit.cpp -o fit.o `root-config --cflags --glibs`
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

	TCanvas * c1 = new TCanvas("c1", "Reticolo d",0,0,900,500);
	
	TCanvas * c2 = new TCanvas("c2", "Reticolo lambda",950,0,900,500);
	c2->Divide(2,1);
	
	TCanvas * c3 = new TCanvas("c3", "Prisma",1950,0,900,500);


//-----------------Definizione grafici

//parte 1 determinazione di d

	TGraphErrors * Gerr1 = new TGraphErrors("../data/P1_1_d.txt", "%lg %lg %lg %lg");
	Gerr1->GetXaxis()->SetTitle("N");
	Gerr1->GetYaxis()->SetTitle("sen(theta)");

//parte 1 determinazione di lambda (diversi materiali)

	//materiale 1
	TGraphErrors * Gerr2 = new TGraphErrors("../data/P1_2_lambda_materiale1.txt", "%lg %lg %lg %lg");
	Gerr2->GetXaxis()->SetTitle("N");
	Gerr2->GetYaxis()->SetTitle("sen(theta)");

	//materiale 2
	TGraphErrors * Gerr3 = new TGraphErrors("../data/P1_2_lambda_materiale2.txt", "%lg %lg %lg %lg");
	Gerr3->GetXaxis()->SetTitle("N");
	Gerr3->GetYaxis()->SetTitle("sen(theta)");

//parte 2 determinazione di a e b in cauchy

	TGraphErrors * Gerr4 = new TGraphErrors("../data/P2_1_cauchy.txt", "%lg %lg %lg %lg");
	Gerr4->GetXaxis()->SetTitle("n(lambda)");
	Gerr4->GetYaxis()->SetTitle("lambda");

	
//---------------------------Determinazione di d

	//double lambdaNa = 589; //[nm]
	TF1 * fit1 = new TF1("reticolo_d", "[1]/[0]*x",0,3);
	//[0]= d
	fit1->SetParName(0,"d [nm]");
	fit1->SetParName(1, "lambda Na yellow [nm]");
	fit1->SetParameter(0,1./600.*1000000); //[nm]
	fit1->FixParameter(1,589); //[nm]

	c1->cd();
	Gerr1->Draw("AP");
	Gerr1->SetMarkerColor(1);
	Gerr1->SetMarkerSize(1);
	Gerr1->SetMarkerStyle(21);

	Gerr1->Fit(fit1,"C");
	double d_reticolo = fit1->GetParameter(0);


//---------------------------Determinazione di lambda per diversi materiali

	//----------materiale 1

	TF1 * fit2 = new TF1("reticolo lambda materiale 1", "[0]/[1]*x",0,3);
	//[0]= lambda
	fit2->SetParName(0,"lambda [nm]");
	fit2->SetParName(1, "d reticolo [nm]");
	fit2->SetParameter(0,500); //[nm]
	fit2->FixParameter(1,d_reticolo); //[nm]

	c2->cd(1);
	Gerr2->Draw("AP");
	Gerr2->SetMarkerColor(1);
	Gerr2->SetMarkerSize(1);
	Gerr2->SetMarkerStyle(21);

	Gerr2->Fit(fit2,"C");
	double lambda_materiale1 = fit2->GetParameter(0);


	//----------materiale 2

	TF1 * fit3 = new TF1("reticolo lambda materiale 2", "[0]/[1]*x",0,10);
	//[0]= lambda
	fit3->SetParName(0,"lambda [nm]");
	fit3->SetParName(1, "d reticolo [nm]");
	fit3->SetParameter(0,500); //[nm]
	fit3->FixParameter(1,d_reticolo); //[nm]

	c2->cd(2);
	Gerr3->Draw("AP");
	Gerr3->SetMarkerColor(1);
	Gerr3->SetMarkerSize(1);
	Gerr3->SetMarkerStyle(21);

	Gerr3->Fit(fit3,"C");
	double lambda_materiale2 = fit3->GetParameter(0);


//---------------------------Determinazione di a e b nella formula di Cauchy
	
	TF1 * fit4 = new TF1("prisma a b cauchy", "[0]+[1]/(x*x)",0,2); //[nm]
	//[0]=a, [1]=b
	fit4->SetParName(0,"a");
	fit4->SetParName(1, "b [nm^2]");
	fit4->SetParameter(0,1); //
	fit4->SetParameter(1,10000); //[nm^2]

	
	c3->cd();
	Gerr4->Draw("AP");
	Gerr4->SetMarkerColor(1);
	Gerr4->SetMarkerSize(1);
	Gerr4->SetMarkerStyle(21);

	Gerr4->Fit(fit4,"C");
	double a_cauchy = fit4->GetParameter(0);
	double b_cauchy = fit4->GetParameter(1);
	




//-------------------------------------Disegna e fitta grafici
	
	
	Grafica->Run();

	return 0;
}
