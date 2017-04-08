#include <iostream>
#include <fstream>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TGraphErrors.h>
#include <TLine.h>

//c++ P2_2_maxPower.cpp `root-config --cflags --glibs` -o P2_2_maxPower.o



int main(){

	TApplication* Grafica = new TApplication("Grafica", 0, NULL);

	//create vectors for tgrapherrors
	int n=5;
	double rLoad[n];
	double power[n];
	//double errrLoad[n];
	//double errpower[n];

	//fill in vectors
	double a, b;	
	std::ifstream inFile;
	inFile.open("../data/P2_2_maxPower.txt", std::ios::in);

	int i = 0;
	while(!inFile.eof())
	{
	    inFile >> a >> b;
	    rLoad[i] = a;
	    power[i] = b;
	    //errrLoad[i] = 0.001;
	    //errpower[i] = c;
	    i++;
	}

	inFile.close();


	//plot and fit
	TCanvas* c1 = new TCanvas("c1","powermax");
	TGraph* gr = new TGraphErrors(n,rLoad,power);
	gr->SetTitle("Potenza massima dissipata al variare di R Load");
	gr->GetXaxis()->SetTitle("R Load [Ohm]");
	gr->GetYaxis()->SetTitle("Potenza [W]");
	gr->SetMarkerColor(2);

	//vertical line
	TLine *line = new TLine(150,0.028,150,0.098);
  	line->SetLineColor(kBlue);


	gr->Draw("A*");
	line->Draw("SAME");
	c1->Print("C1_P2_maxPower.eps", "eps");



	Grafica->Run("");

	return 0;
}