/*	comando per compilare
c++ ../src/RL_impulsata.cpp -o RL_impulsata.o `root-config --cflags --glibs`
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

	TF1 * fit1 = new TF1("RL VL carica", "2*[0]*exp(-x/[1])",0,700);
	//[0]= V0 [1]=ta
	fit1->SetParName(0,"V_0 [V]");
	fit1->SetParName(1,"Tau [micro s]");

	TF1 * fit2 = new TF1("RL VL scarica", "-2*[0]*exp(-x/[1])",0,700);
	//[0]= V0 [1]=tau
	fit2->SetParName(0,"V_0 [V]");
	fit2->SetParName(1,"Tau [micro s]");

	TF1 * fit3 = new TF1("RL VL carica", "-[0]+2*[0]*(1-exp(-x/[1]))",0,700);
	//[0]= V0 [1]=tau
	fit3->SetParName(0,"V_0 [V]");
	fit3->SetParName(1,"Tau [micro s]");

	TF1 * fit4 = new TF1("RL VL scarica", "-[0]+2*[0]*exp(-x/[1])",0,700);
	//[0]= V0 [1]=tau
	fit4->SetParName(0,"V_0 [V]");
	fit4->SetParName(1,"Tau [micro s]");


//-----------------Definizione grafici

	TGraphErrors * Gerr1 = new TGraphErrors("../data/RLVLcharge.txt");
	Gerr1->SetTitle("V ai capi dell'induttore durante carica");
	Gerr1->GetXaxis()->SetTitle("Tempo [ms]");
	Gerr1->GetYaxis()->SetTitle("Tensione [V]");

	TGraphErrors * Gerr2 = new TGraphErrors("../data/RLVLdischarge.txt");
	Gerr2->SetTitle("V ai capi dell'induttore durante scarica");
	Gerr2->GetXaxis()->SetTitle("Tempo [ms]");
	Gerr2->GetYaxis()->SetTitle("Tensione [V]");

	TGraphErrors * Gerr3 = new TGraphErrors("../data/RLVRcharge.txt");
	Gerr3->SetTitle("V ai capi del resistore durante carica");
	Gerr3->GetXaxis()->SetTitle("Tempo [ms]");
	Gerr3->GetYaxis()->SetTitle("Tensione [V]");

	TGraphErrors * Gerr4 = new TGraphErrors("../data/RLVRdischarge.txt");
	Gerr4->SetTitle("V ai capi del resistore durante scarica");
	Gerr4->GetXaxis()->SetTitle("Tempo [ms]");
	Gerr4->GetYaxis()->SetTitle("Tensione [V]");

	
//-------------------------------------Disegna e fitta grafici

	TCanvas * c1 = new TCanvas("c1", "RL corrente impulsata",0,0,900,500);
	c1->Divide(2,2); 
	
	//------Carica VC
	c1->cd(1);
	Gerr1->Draw("AP");
	Gerr1->SetMarkerColor(1);
	Gerr1->SetMarkerSize(1);
	Gerr1->SetMarkerStyle(21);

	fit1->SetParameter(0,10);
	fit1->SetParameter(1,50);

	Gerr1->Fit(fit1,"C");

	//------Scarica VC
	c1->cd(2);
	Gerr2->Draw("AP");
	Gerr2->SetMarkerColor(1);
	Gerr2->SetMarkerSize(1);
	Gerr2->SetMarkerStyle(21);

	fit2->SetParameter(0,10);
	fit2->SetParameter(1,50);

	Gerr2->Fit(fit2,"C");

	//------Carica VR
	c1->cd(3);
	Gerr3->Draw("AP");
	Gerr3->SetMarkerColor(1);
	Gerr3->SetMarkerSize(1);
	Gerr3->SetMarkerStyle(21);

	fit3->SetParameter(0,10);
	fit3->SetParameter(1,50);

	Gerr3->Fit(fit3,"C");

	//------Scarica VR
	c1->cd(4);
	Gerr4->Draw("AP");
	Gerr4->SetMarkerColor(1);
	Gerr4->SetMarkerSize(1);
	Gerr4->SetMarkerStyle(21);

	fit4->SetParameter(0,10);
	fit4->SetParameter(1,50);

	Gerr4->Fit(fit4,"C");

	

//--------------------------------------Esporta immagini
	//c1->Print("RL_impulsata.eps","eps");



	Grafica->Run();

	return 0;
}
