#include <iostream>
#include <fstream>
#include <vector>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TGraphErrors.h>

//c++ P2_1_voltDividerHigh.cpp `root-config --cflags --glibs` -o P2_1_voltDividerHigh.o

int main(){

	TApplication* Grafica = new TApplication("Grafica", 0, NULL);

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
	TCanvas* c1 = new TCanvas("Voltage divider","Voltage divider");
	c1->Divide(3,1); // divides the canvas into three columns
	
	
	//tgrapherrors
	TGraphErrors* gr1 = new TGraphErrors(n, &voltageIn1.at(0), &voltageOut1.at(0), &errvoltageIn.at(0), &errvoltageOut.at(0));
	gr1->SetTitle("R1 e R2 resistenza alta");
	gr1->GetXaxis()->SetTitle("V in [V]");
	gr1->GetYaxis()->SetTitle("V out [V]");

	TGraphErrors* gr2 = new TGraphErrors(n, &voltageIn2.at(0), &voltageOut2.at(0), &errvoltageIn.at(0), &errvoltageOut.at(0));
	gr2->SetTitle("R1 e R2 resistenza alta");
	gr2->GetXaxis()->SetTitle("V in [V]");
	gr2->GetYaxis()->SetTitle("V out [V]");

	TGraphErrors* gr3 = new TGraphErrors(n, &voltageIn3.at(0), &voltageOut3.at(0), &errvoltageIn.at(0), &errvoltageOut.at(0));
	gr3->SetTitle("R1 e R2 resistenza alta");
	gr3->GetXaxis()->SetTitle("V in [V]");
	gr3->GetYaxis()->SetTitle("V out [V]");


	//TF1s
	TF1* f1 = new TF1("f1", "0.5*x", 5, 14);
	TF1* f2 = new TF1("f1", "0.5*x", 2.7, 7);
	TF1* f3 = new TF1("f1", "0.5*x", 2.7, 7);

	//PERCHE NON MI STAMPA F1 INSIEME AI GRAPHS?????
	c1->cd(1);
	gr1->Draw("A*");
	f1->Draw("SAME");

	c1->cd(2);
	gr2->Draw("A*");
	f2->Draw("SAME");

	c1->cd(3);
	gr3->Draw("A*");
	f3->Draw("SAME");

	c1->Print("C1_P2_partResHigh.eps", "eps");

	Grafica->Run("");

	return 0;
}