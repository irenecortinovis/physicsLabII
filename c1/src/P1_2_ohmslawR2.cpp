#include <iostream>
#include <fstream>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TGraphErrors.h>
#include <TFitResultPtr.h>
#include <TFrame.h>

//c++ P1_2_ohmslawR2.cpp `root-config --cflags --glibs` -o P1_2_ohmslawR2.o



int main(){

	TApplication* Grafica = new TApplication("Grafica", 0, NULL);

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
	gr->SetTitle("TGraphErrors c1 test");
	gr->GetXaxis()->SetTitle("Voltage [V]");
	gr->GetYaxis()->SetTitle("Current [A]");


	TF1 *fitC1 = new TF1("fitC1","[0]*x",voltage[0],voltage[n-1]);
	gr->Fit("fitC1");

	//find value of resistance
	double resistance;
	resistance = 1/fitC1->GetParameter(0);
	//find error for resistance
	double errresistance;
	//?? 

	gr->Draw("AP");


	std::cout << "resistance from fit = " << resistance << " ohm" << std::endl;
	std::cout << "measured resistance = 266 +/- 1 ohm" << std::endl;



	Grafica->Run("");

	return 0;
}