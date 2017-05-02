/*
comando per compilare da c3/build
c++ ../src/testtf1.cpp -o testtf1.o `root-config --cflags --glibs`
*/

//librerie c++
#include <iostream>
#include <cmath>
#include <string>
#include <ctime>
#include <cstdlib>
#include <fstream>


//root
#include <TCanvas.h>
#include <TH1F.h>
#include <TF1.h>
#include <TGraph.h>
#include <TApplication.h>
#include <TGraphErrors.h>

//librerie funzioni
// #include "path/to/libreria/funzioni.h"
using namespace std;
int main() {

//-------------------------------------- codice

	TApplication * Grafica = new TApplication("", NULL, NULL);

	
	TCanvas * c1 = new TCanvas("c1", "Sottosmorzato",0,0,900,500);
	TCanvas * c2 = new TCanvas("c2", "Critico",950,0,900,500);
	TCanvas * c3 = new TCanvas("c3", "Sovrasmorzato",950,0,900,500);

//---------------------------Definizione Fit

	TF1 * fit1 = new TF1("sottosmorzato", "[0]*exp(-[1]*x)*(-[1]*cos([2]*x)-[2]*sin([2]*x))",0,0.05);
	//[0]= RCV0 [1]=gamma [2]=omega
	fit1->SetParName(0,"RCV_0");
	fit1->SetParameter(0,0.006);
	
	fit1->SetParName(1,"gamma");
	fit1->SetParameter(1,300);
	
	fit1->SetParName(2,"omega");
	fit1->SetParameter(2,1000);

	TF1 * fit2 = new TF1("critico","[0]*[1]*[1]*x*exp(-[1]*x)",0,0.01); 
	//[0]=Q0R [1]=gamma
	fit2->SetParName(0,"RQ_0");
	fit2->SetParameter(0,0.006);

	fit2->SetParName(1,"gamma");
	fit2->SetParameter(1,300);


	TF1 * fit3 = new TF1("sovrasmorzato","[0]/[1]*(exp(-x*([2]-[1])) - exp(-x*([2]+[1])) )",0,0.05);
	//[0]=1/2*Q0R*(omega0)^2 [1]= beta [2]=gamma
	fit3->SetParName(0,"1/2*RQ_0*(omega_0)^2");
	fit3->SetParameter(0,100000);
	
	fit3->SetParName(1,"beta");
	fit3->SetParameter(1,8130);
	
	fit3->SetParName(2,"gamma");
	fit3->SetParameter(2,8330);

//-------------------------------------Disegna e fitta grafici
	//------Sottosmorzato
	c1->cd();
	fit1->Draw();

	//------Critico
	c2->cd();
	fit2->Draw();

	//------Sovrasmorzato
	c3->cd();
	fit3->Draw();

	Grafica->Run();

	return 0;
}
