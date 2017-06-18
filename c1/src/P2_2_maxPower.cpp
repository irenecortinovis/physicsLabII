#include <iostream>
#include <fstream>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TGraphErrors.h>
#include <TLine.h>
#include <TStyle.h>
#include <TLegend.h>

//c++ P2_2_maxPower.cpp `root-config --cflags --glibs` -o P2_2_maxPower.o



int main(){

	TApplication* Grafica = new TApplication("Grafica", 0, NULL);
	gStyle->SetOptFit(1111);

	//create vectors for tgrapherrors
	int n=5;
	double rLoad[n];
	double power[n];
	double errLoad[n];
	double errpower[n];

	//fill in vectors
	double a, b, c;	
	std::ifstream inFile;
	inFile.open("../data/P2_2_maxPower.txt", std::ios::in);

	int i = 0;
	while(!inFile.eof())
	{
	    inFile >> a >> b >> c;
	    std::cout << a << "\t" << b << "\t" << c << std::endl;
	    rLoad[i] = a;
	    power[i] = b;
	    errLoad[i] = 0.01*a;
	    errpower[i] = c;
	    std::cout << rLoad[i] << "\t" << power[i] << "\t" << errLoad[i] << "\t" << errpower[i] << std::endl;
	    i++;
	    
	}

	inFile.close();


	//plot and fit
	TCanvas* c1 = new TCanvas("c1","powermax");
	TGraphErrors* gr = new TGraphErrors(n,rLoad,power,errLoad,errpower);
	gr->SetTitle("Potenza massima dissipata al variare di R Load");
	gr->GetXaxis()->SetTitle("R Load [kOhm]");
	gr->GetYaxis()->SetTitle("Potenza [mW]");
	gr->SetMarkerColor(4);
    gr->SetMarkerStyle(21);

	//vertical line
	TLine *line = new TLine(150,0.028,150,0.098);
  	line->SetLineColor(kBlue);

  	TF1 *f1 = new TF1("fitC1","[0]^2*x/(149+[1]+x)^2",rLoad[0],rLoad[n-1]);
  	f1->SetParName(0,"fem [V]");
  	f1->SetParName(1,"Rg [V]");
  	f1->SetParameter(0,7.5);
  	f1->SetParameter(1,0);
	
	gr->Fit(f1);

		gr->Draw("A*");
	line->Draw("SAME");

	TLegend* legend1 = new TLegend(0.1,0.75,0.5,0.9);
	legend1->AddEntry(f1,"V^2*x/(149+x)^2","l");
	legend1->AddEntry(gr,"data","p");
	legend1->AddEntry(line,"Massimo teorico: R_L = 149 kOhm","l");
	legend1->Draw();

	
	c1->Print("C1_P2_maxPower.eps", "eps");



	Grafica->Run("");

	delete[] rLoad;
	delete[] power;
	delete[] errLoad;
	delete[] errpower;
	return 0;
}