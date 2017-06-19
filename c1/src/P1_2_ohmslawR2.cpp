#include <iostream>
#include <fstream>
#include <iomanip>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TGraphErrors.h>
#include <TStyle.h>
#include <TLegend.h>

//c++ P1_2_ohmslawR2.cpp `root-config --cflags --glibs` -o P1_2_ohmslawR2.o



int main(){

	TApplication* Grafica = new TApplication("Grafica", 0, NULL);
	gStyle->SetOptFit(1111);

	//create vectors for tgrapherrors
	int n=20;
	double voltage[n];
	double current[n];
	double errvoltage[n];
	double errcurrent[n];

	//fill in vectors
	double a, b, c;	
	std::ifstream inFile;
	inFile.open("../data/P1_2_ohmslawR2.txt", std::ios::in);

	int i = 0;
	while(!inFile.eof())
	{
	    inFile >> a >> b >> c;
	    voltage[i] = a;
	    current[i] = b;
	    errvoltage[i] = 0.001;
	    errcurrent[i] = c;
	    i++;
	}

	inFile.close();

	//plot and fit
	TCanvas* c1 = new TCanvas("c1","A Simple Graph with error bars");
	TGraphErrors* gr = new TGraphErrors(n,voltage,current,errvoltage,errcurrent);
	gr->SetTitle("Legge di Ohm - Resistore 2");
	gr->GetXaxis()->SetTitle("Voltage [V]");
	gr->GetYaxis()->SetTitle("Current [A]");


	TF1 *fitC1 = new TF1("fitC1","x/[0]",voltage[0],voltage[n-1]);
	fitC1->SetParName(0, "R [Ohm]");
	fitC1->SetParameter(0, 260);
	gr->Fit("fitC1");

	//find value of resistance
	double resistance;
	resistance = fitC1->GetParameter(0);
	//find error for resistance
	double errresistance = fitC1->GetParError(0);

	gr->Draw("A*");
	c1->Print("R2.eps", "eps");

	TLegend* legend1 = new TLegend(0.1,0.75,0.5,0.9);
	legend1->AddEntry(fitC1,"I = 1/R * V(I)","l");
	legend1->AddEntry(gr,"data","p");
	legend1->Draw();


	std::cout << std::fixed;
	std::cout << "resistance from fit = " << std::setprecision(2) << resistance << " ohm +/- " << std::setprecision(2) << errresistance << " ohm" << std::endl;
	std::cout << "measured resistance = 266 +/- 1 ohm" << std::endl;



	Grafica->Run("");

	return 0;
}