#include <iostream>
#include <fstream>
#include <vector>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TGraphErrors.h>

//c++ P2_1_voltDividerHigh.cpp `root-config --cflags --glibs` -o P2_1_voltDividerHigh.o

//bisogna dividere il file txt in 3 parti? secondo me no

int main(){

	TApplication* Grafica = new TApplication("Grafica", 0, NULL);

	//create vectors for tgrapherrors
	int n=0;

	std::vector<double> voltageIn;
	std::vector<double> voltageOut;

	std::vector<double> errvoltageIn;
	std::vector<double> errvoltageOut;


	//fill in voltage vectors for high R1 and R2 resistance
	double a, b;
	std::ifstream inFile;
	inFile.open("../data/P2_1_voltDividerHigh.txt", std::ios::in);

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


	//draw canvas
	TCanvas* c1 = new TCanvas("Voltage divider","Voltage divider");
	
	//plot
	TGraphErrors* gr = new TGraphErrors(n, &voltageIn.at(0), &voltageOut.at(0), &errvoltageIn.at(0), &errvoltageOut.at(0));
	gr->SetTitle("R1 and R2 high resistance");
	gr->GetXaxis()->SetTitle("Voltage in [V]");
	gr->GetYaxis()->SetTitle("Voltage out [V]");
	gr->Draw("AP");


	//TF1
	TF1* f1 = new TF1("f1", "0.5*x", voltageIn.at(0), voltageIn.at(n-1));

	//chi square
	double chi2 = 0;
	double x, y;
	int dof = n-1;

	for (int i = 0; i<n; i++)
	{
	  gr->GetPoint(i, x, y);
	  std::cout << x << "\t" << y << std::endl;
	  chi2 +=  pow(y - f1->Eval(x) ,2) / (gr->GetErrorY(i));
	  //chi2 +=  pow(y - f1->Eval(x) ,2) / (f1->Eval(x));
	}

	std::cout << "chi2 / dof : "<< chi2 / dof << std::endl;

	Grafica->Run("");

	return 0;
}