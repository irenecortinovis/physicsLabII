#include <iostream>
#include <fstream>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TGraphErrors.h>
#include <TStyle.h>
#include <TLegend.h>

//c++ P2_1_voltDividerLow.cpp `root-config --cflags --glibs` -o P2_1_voltDividerLow.o



int main(){

	TApplication* Grafica = new TApplication("Grafica", 0, NULL);
	gStyle->SetOptFit(1111);

	//draw canvas
	TCanvas* c1 = new TCanvas("Voltage divider","Voltage divider");
	


	//create vectors for tgrapherrors
	int n=0;

	std::vector<double> voltageIn;
	std::vector<double> voltageOut;

	std::vector<double> errvoltageIn;
	std::vector<double> errvoltageOut;

	//fill in voltage vectors for low R1 and R2 resistance
	double a, b;
	std::ifstream inFile;
	inFile.open("../data/P2_1_voltDividerLow.txt", std::ios::in);

	while(!inFile.eof())
	{
	    inFile >> a >> b;
	    voltageIn.push_back(a);
	    voltageOut.push_back(b);
	    errvoltageIn.push_back(0.01);
		errvoltageOut.push_back(0.001);
	    n++;
	}
	inFile.close();

	//check number of lines
	std::cout << "n = " << n << std::endl;

	//plot gr
	TGraphErrors* gr = new TGraphErrors(n, &voltageIn.at(0), &voltageOut.at(0), &errvoltageIn.at(0), &errvoltageOut.at(0));
	gr->SetTitle("R1 e R2 resistenza bassa");
	gr->GetXaxis()->SetTitle("V in [V]");
	gr->GetYaxis()->SetTitle("V out [V]");
	gr->SetMarkerColor(4);
	gr->SetMarkerStyle(21);
	gr->Draw("AP");


	//TF1 fit
	TF1* fit1 = new TF1("fit1", "[0]*x", voltageIn.at(0), voltageIn.at(n-1));
	fit1->SetParameter(0, 0.5);
	fit1->SetParName(0, "[0] (pendenza)");


	gr->Fit(fit1);
	gr->Draw("AP");
	//c1->Print("C1_P2_partResLow.eps", "eps");

	TLegend* legend1 = new TLegend(0.1,0.75,0.5,0.9);
	legend1->AddEntry(fit1,"V_out = [0]*V_in","l");
	legend1->AddEntry(gr,"data","p");
	legend1->Draw();

	Grafica->Run("");

	return 0;
}