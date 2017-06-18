#include <iostream>
#include <fstream>
#include <vector>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TGraphErrors.h>
#include <TStyle.h>
#include <TLegend.h>

//c++ P2_1_voltDividerHigh.cpp `root-config --cflags --glibs` -o P2_1_voltDividerHigh.o

int main(){

	TApplication* Grafica = new TApplication("Grafica", 0, NULL);
	gStyle->SetOptFit(1111);


	//create vectors for tgrapherrors
	int n=3;

	std::vector<double> voltageIn1;
	std::vector<double> voltageOut1;
	std::vector<double> voltageIn2;
	std::vector<double> voltageOut2;
	std::vector<double> voltageIn3;
	std::vector<double> voltageOut3;

	std::vector<double> errvoltageIn;
	std::vector<double> errvoltageOut;


	//fill in error vectors
	for(int i=0; i<n; i++)
	{
		    errvoltageIn.push_back(0.01);
		    errvoltageOut.push_back(0.001);
	}

	//fill in voltage vectors for high R1 and R2 resistance from the three sets
	double a, b;
	std::ifstream inFile;

	//set 1
	inFile.open("../data/P2_1_voltDividerHigh1.txt", std::ios::in);
	while(!inFile.eof())
	{
	    inFile >> a >> b;
	    voltageIn1.push_back(a);
	    voltageOut1.push_back(b);
	}
	inFile.close();

	//set 2
	inFile.open("../data/P2_1_voltDividerHigh2.txt", std::ios::in);
	while(!inFile.eof())
	{
	    inFile >> a >> b;
	    voltageIn2.push_back(a);
	    voltageOut2.push_back(b);
	}
	inFile.close();

	//set 3
	inFile.open("../data/P2_1_voltDividerHigh3.txt", std::ios::in);
	while(!inFile.eof())
	{
	    inFile >> a >> b;
	    voltageIn3.push_back(a);
	    voltageOut3.push_back(b);
	}
	inFile.close();


	//draw canvas
	TCanvas* c1 = new TCanvas("Voltage divider1","Voltage divider1");
	TCanvas* c2 = new TCanvas("Voltage divider2","Voltage divider2");
	TCanvas* c3 = new TCanvas("Voltage divider3","Voltage divider3");
	
	
	//tgrapherrors
	TGraphErrors* gr1 = new TGraphErrors(n, &voltageIn1.at(0), &voltageOut1.at(0), &errvoltageIn.at(0), &errvoltageOut.at(0));
	gr1->SetTitle("R1 e R2 resistenza alta, RL = 0.677 kOhm");
	gr1->GetXaxis()->SetTitle("V in [V]");
	gr1->GetYaxis()->SetTitle("V out [V]");
	gr1->SetMarkerColor(4);
	gr1->SetMarkerStyle(21);

	TGraphErrors* gr2 = new TGraphErrors(n, &voltageIn2.at(0), &voltageOut2.at(0), &errvoltageIn.at(0), &errvoltageOut.at(0));
	gr2->SetTitle("R1 e R2 resistenza alta, RL = 15.97 kOhm");
	gr2->GetXaxis()->SetTitle("V in [V]");
	gr2->GetYaxis()->SetTitle("V out [V]");
	gr2->SetMarkerColor(4);
	gr2->SetMarkerStyle(21);

	TGraphErrors* gr3 = new TGraphErrors(n, &voltageIn3.at(0), &voltageOut3.at(0), &errvoltageIn.at(0), &errvoltageOut.at(0));
	gr3->SetTitle("R1 e R2 resistenza alta, RL = 100.1 kOhm");
	gr3->GetXaxis()->SetTitle("V in [V]");
	gr3->GetYaxis()->SetTitle("V out [V]");
	gr3->SetMarkerColor(4);
	gr3->SetMarkerStyle(21);


	//TF1s
	TF1* f1 = new TF1("f1", "[0]*x", 5, 14);
	f1->SetParameter(0,0.5);
	f1->SetParName(0, "[0] (pendenza)");
	TF1* f2 = new TF1("f2", "[0]*x", 2.7, 7);
	f2->SetParameter(0,0.5);
	f2->SetParName(0, "[0] (pendenza)");
	TF1* f3 = new TF1("f3", "[0]*x", 2.7, 7);
	f3->SetParameter(0,0.5);
	f3->SetParName(0, "[0] (pendenza)");

	c1->cd();
	gr1->Draw("A*");
	gr1->Fit(f1);

	TLegend* legend1 = new TLegend(0.1,0.75,0.5,0.9);
	legend1->AddEntry(f1,"V_out = [0]*V_in","l");
	legend1->AddEntry(gr1,"data","p");
	legend1->Draw();


	c2->cd();
	gr2->Draw("A*");
	gr2->Fit(f2);

	TLegend* legend2 = new TLegend(0.1,0.75,0.5,0.9);
	legend2->AddEntry(f2,"V_out = [0]*V_in","l");
	legend2->AddEntry(gr2,"data","p");
	legend2->Draw();


	c3->cd();
	gr3->Draw("A*");
	gr3->Fit(f3);

	TLegend* legend3 = new TLegend(0.1,0.75,0.5,0.9);
	legend3->AddEntry(f3,"V_out = [0]*V_in","l");
	legend3->AddEntry(gr3,"data","p");
	legend3->Draw();


	

	//c1->Print("C1_P2_partResHigh.eps", "eps");

	Grafica->Run("");

	return 0;
}