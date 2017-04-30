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
	fit1->SetParName(0,"V_0 [V]");
	fit1->SetParName(1,"Tau [micro s]");

	TF1 * fit2 = new TF1("RC Vc scarica", "[0]*exp(-x/[1])",0,700);
	//[0]= V0 [1]=tau
	fit2->SetParName(0,"V_0 [V]");
	fit2->SetParName(1,"Tau [micro s]");

	//change these to current (divide by R)
	TF1 * fit3 = new TF1("RC Vr carica", "[0]*exp(-x/[1])",0,700);
	//[0]= V0 [1]=tau
	fit3->SetParName(0,"V_0 [V]");
	fit3->SetParName(1,"Tau [micro s]");

	TF1 * fit4 = new TF1("RC Vr scarica", "-[0]*exp(-x/[1])",0,700);
	//[0]= V0 [1]=tau
	fit4->SetParName(0,"V_0 [V]");
	fit4->SetParName(1,"Tau [micro s]");


//-----------------Definizione grafici

	TGraphErrors * Gerr1 = new TGraphErrors("../data/RCVCcharge.txt");
	Gerr1->SetTitle("V ai capi del condensatore durante carica");
	Gerr1->GetXaxis()->SetTitle("Tempo [micro s]");
	Gerr1->GetYaxis()->SetTitle("Tensione [V]");

	TGraphErrors * Gerr2 = new TGraphErrors("../data/RCVCdischarge.txt");
	Gerr2->SetTitle("V ai capi del condensatore durante scarica");
	Gerr2->GetXaxis()->SetTitle("Tempo [micro s]");
	Gerr2->GetYaxis()->SetTitle("Tensione [V]");

	TGraphErrors * Gerr3 = new TGraphErrors("../data/RCVRcharge.txt");
	Gerr3->SetTitle("V ai capi del resistore durante carica");
	Gerr3->GetXaxis()->SetTitle("Tempo [micro s]");
	Gerr3->GetYaxis()->SetTitle("Tensione [V]");

	TGraphErrors * Gerr4 = new TGraphErrors("../data/RCVRdischarge.txt");
	Gerr4->SetTitle("V ai capi del resistore durante scarica");
	Gerr4->GetXaxis()->SetTitle("Tempo [micro s]");
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

	fit1->SetParameter(0,10);
	fit1->SetParameter(1,200);

	Gerr1->Fit(fit1,"C");

	//------Scarica VC
	c1->cd(2);
	Gerr2->Draw("AP");
	Gerr2->SetMarkerColor(1);
	Gerr2->SetMarkerSize(1);
	Gerr2->SetMarkerStyle(21);

	fit2->SetParameter(0,10);
	fit2->SetParameter(1,200);

	Gerr2->Fit(fit2,"C");

	//------Carica VR
	c1->cd(3);
	Gerr3->Draw("AP");
	Gerr3->SetMarkerColor(1);
	Gerr3->SetMarkerSize(1);
	Gerr3->SetMarkerStyle(21);

	fit3->SetParameter(0,10);
	fit3->SetParameter(1,200);

	Gerr3->Fit(fit3,"C");

	//------Scarica VR
	c1->cd(4);
	Gerr4->Draw("AP");
	Gerr4->SetMarkerColor(1);
	Gerr4->SetMarkerSize(1);
	Gerr4->SetMarkerStyle(21);

	fit4->SetParameter(0,10);
	fit4->SetParameter(1,200);

	Gerr4->Fit(fit4,"C");

	

//--------------------------------------Esporta immagini
	//c1->Print("C3_P2_RC_impulsata.eps","eps"


//--------------------------------------Calcolo di tau e di C
	//tau: mean (weighted) value and error [s]
	double tau1 = (fit1->GetParameter(1))/1000000;
	double wtauerr1 = 1/pow((fit1->GetParError(1))/1000000, 2);
	double tau2 = (fit2->GetParameter(1))/1000000;
	double wtauerr2 = 1/pow((fit2->GetParError(1))/1000000, 2);
	double tau3 = (fit3->GetParameter(1))/1000000;
	double wtauerr3 = 1/pow((fit3->GetParError(1))/1000000, 2);
	double tau4 = (fit4->GetParameter(1))/1000000;
	double wtauerr4 = 1/pow((fit4->GetParError(1))/1000000, 2);

	double tau_mean = (tau1*wtauerr1+tau2*wtauerr2+tau3*wtauerr3+tau4*wtauerr4)
						/ (wtauerr1+wtauerr2+wtauerr3+wtauerr4); 
	double tau_err = 1/sqrt(wtauerr1+wtauerr2+wtauerr3+wtauerr4);

	std::cout << "Weighted average for tau [s] = " << tau_mean <<
				 " +/- " << tau_err << std::endl;

	//resistance: value and error
	const double resistance = 14870; //Ohm
	double resistance_err = 1 + 14870/100*0.9; //Ohm, as in Fluke manual

	//capacitance: value and error (propagation)
	double capacitance = tau_mean/resistance;
	double capacitance_err = sqrt(pow(tau_err/resistance,2)+
							pow(tau_mean/(resistance*resistance)*resistance_err,2));

	std::cout << "Capacitance [farad] = " << capacitance << " +/- " << capacitance_err << std::endl;



	Grafica->Run();

	return 0;
}
