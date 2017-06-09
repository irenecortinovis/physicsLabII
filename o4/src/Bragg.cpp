#include <iostream>
#include <fstream>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TGraphErrors.h>

//c++ Bragg.cpp `root-config --cflags --glibs` -o Bragg.o



int main(){

	TApplication* Grafica = new TApplication("Grafica", 0, NULL);

	//plot
	TCanvas* c1 = new TCanvas("c1","A Simple Graph with error bars");
	TGraphErrors* gr = new TGraphErrors("../data/Bragg.txt");
	gr->SetTitle("Diffrazione di Bragg");
	gr->GetXaxis()->SetTitle("Angolo [gradi]");
	gr->GetYaxis()->SetTitle("Intensità [mA]");


	gr->Draw("APL");
	c1->Print("Bragg.eps", "eps");

	Grafica->Run("");

	return 0;
}