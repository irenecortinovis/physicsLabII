/*	comando per compilare
c++ ../src/RC_impulsata.cpp -o RC_impulsata.o `root-config --cflags --glibs`
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
#include <TStyle.h>

int main()
{
	TApplication * Grafica = new TApplication("", 0, NULL);
	gStyle->SetOptFit(1111);


//---------------------------Definizione Fit

	TF1 * fit1 = new TF1("RC Vc carica", "[0]*(1-exp(-x/[1]))",0,700);
	//[0]= V0 [1]=tau
	fit1->SetParName(0,"V_0");
	fit1->SetParName(1,"Tau");

	TF1 * fit2 = new TF1("RC Vc scarica", "[0]*exp(-x/[1])",0,700);
	//[0]= V0 [1]=tau
	fit2->SetParName(0,"V_0");
	fit2->SetParName(1,"Tau");

	//change these to current (divide by R)
	TF1 * fit3 = new TF1("RC Vr carica", "[0]*exp(-x/[1])",0,700);
	//[0]= V0 [1]=tau
	fit3->SetParName(0,"V_0");
	fit3->SetParName(1,"Tau");

	TF1 * fit4 = new TF1("RC Vr scarica", "-[0]*exp(-x/[1])",0,700);
	//[0]= V0 [1]=tau
	fit4->SetParName(0,"V_0");
	fit4->SetParName(1,"Tau");


//-----------------Definizione grafici

	TGraphErrors * Gerr1 = new TGraphErrors("../data/RCVCcharge.txt");
	Gerr1->SetTitle("V ai capi del condensatore durante carica");
	Gerr1->GetXaxis()->SetTitle("Tempo [ms]");
	Gerr1->GetYaxis()->SetTitle("Tensione [V]");

	TGraphErrors * Gerr2 = new TGraphErrors("../data/RCVCdischarge.txt");
	Gerr2->SetTitle("V ai capi del condensatore durante scarica");
	Gerr2->GetXaxis()->SetTitle("Tempo [ms]");
	Gerr2->GetYaxis()->SetTitle("Tensione [V]");

	TGraphErrors * Gerr3 = new TGraphErrors("../data/RCVRcharge.txt");
	Gerr3->SetTitle("V ai capi del resistore durante carica");
	Gerr3->GetXaxis()->SetTitle("Tempo [ms]");
	Gerr3->GetYaxis()->SetTitle("Tensione [V]");

	TGraphErrors * Gerr4 = new TGraphErrors("../data/RCVRdischarge.txt");
	Gerr4->SetTitle("V ai capi del resistore durante scarica");
	Gerr4->GetXaxis()->SetTitle("Tempo [ms]");
	Gerr4->GetYaxis()->SetTitle("Tensione [V]");

	
//-------------------------------------Disegna e fitta grafici

	TCanvas * c1 = new TCanvas("c1", "RC corrente impulsata",0,0,900,500);
	c1->Divide(2,2); 
	
	//------Carica VC
	c1->cd(1);
	Gerr1->Draw("AP");
	Gerr1->SetMarkerColor(1);
	Gerr1->SetMarkerSize(1);
	Gerr1->SetMarkerStyle(21);

	fit1->SetParameter(0,20);
	fit1->SetParameter(1,200);

	Gerr1->Fit(fit1,"C");

	//------Scarica VC
	c1->cd(2);
	Gerr2->Draw("AP");
	Gerr2->SetMarkerColor(1);
	Gerr2->SetMarkerSize(1);
	Gerr2->SetMarkerStyle(21);

	fit2->SetParameter(0,20);
	fit2->SetParameter(1,200);

	Gerr2->Fit(fit2,"C");

	//------Carica VR
	c1->cd(3);
	Gerr3->Draw("AP");
	Gerr3->SetMarkerColor(1);
	Gerr3->SetMarkerSize(1);
	Gerr3->SetMarkerStyle(21);

	fit3->SetParameter(0,20);
	fit3->SetParameter(1,200);

	Gerr3->Fit(fit3,"C");

	//------Scarica VR
	c1->cd(4);
	Gerr4->Draw("AP");
	Gerr4->SetMarkerColor(1);
	Gerr4->SetMarkerSize(1);
	Gerr4->SetMarkerStyle(21);

	fit4->SetParameter(0,20);
	fit4->SetParameter(1,200);

	Gerr4->Fit(fit4,"C");

	

//--------------------------------------Esporta immagini
	//c1->Print("sottosmorzato.eps","eps");



	Grafica->Run();

	return 0;
}
