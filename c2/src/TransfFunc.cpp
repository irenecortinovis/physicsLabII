#include <iostream>
#include <fstream>
#include <iomanip>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TGraphErrors.h>
#include <TGraphPolar.h>
#include <TGraphPolargram.h>

/*
compile:
c++ TransfFunc.cpp `root-config --cflags --glibs` -o TransfFunc.o

run:
transfer function Va-b/Vb deltaPhi
./TransfFunc.o 1

transfer function Vb/Va deltaPhi'
./TransfFunc.o 2
*/


int main(int argc, char** argv){

	if(argc==1)
	{
		std::cout << "error: missing argument: run with argument 1 or 2" << std::endl;
		return 1;
	}

	std::string arg1(argv[1]);

	TApplication* Grafica = new TApplication("Grafica", 0, NULL);

	//create vectors for tgraphpolar
	int n=13;
	double vba_vb[n];
	double errv[n];
	double deltaphi[n];
	double errdeltaphi[n];

	//fill in vectors
	double a, b, c, d;	
	std::ifstream inFile;
	std::string filename = "../data/TransfFunct" + arg1 + ".txt";

	inFile.open(filename.c_str(), std::ios::in);

	int i = 0;
	while(!inFile.eof())
	{
	    inFile >> a >> b >> c >> d;
	    vba_vb[i] = a;
	    errv[i] = b;
	    deltaphi[i] = c;
	    errdeltaphi[i] = d;
	    i++;
	}

	inFile.close();

	//plot
	TCanvas* c1 = new TCanvas("c1","Transfer function");
	TGraphPolar * gr = new TGraphPolar(n, deltaphi, vba_vb, errdeltaphi, errv);
	if(arg1 == 1)
		gr->SetTitle("Transfer function Va-b/Vb deltaPhi");
	else if(arg1 == 2)
		gr->SetTitle("Transfer function Vb/Va deltaPhi'");
	gr->SetMarkerStyle(20);
	gr->SetMarkerSize(2.);
	gr->SetMarkerColor(4);
	gr->SetLineColor(2);
	gr->SetLineWidth(3);
    gr->Draw("PEL");
	

	c1->Update();
   	gr->GetPolargram()->SetToRadian();
	c1->Print("R1.eps", "eps");


	Grafica->Run("");

	return 0;
}