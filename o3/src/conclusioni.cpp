/*	comando per compilare
c++ ../src/fit.cpp -o fit.o `root-config --cflags --glibs`
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
#include <TStyle.h>


using namespace std;




void O3_conclusioni()
{

	TApplication * Grafica = new TApplication("", NULL, NULL);

//----------------- Definizione canvas -----------------
    gStyle->SetOptStat(0);

	TCanvas * c6 = new TCanvas("c6", "Lampada D",0,0,900,500);
	c6->Divide(1,2);

//parte 1 spettro di emissione delle lampade
	TH1F* histoP1D = O3_P1_reticolo();
//parte 2 spettro di emissione delle lampade
	TH1F* histoP2D = O3_P2_prisma();

	//lampada D
	c6->cd(1);
	histoP1D->Draw();
	c6->cd(2);
	histoP2D->Draw();

	//--------------------------------------Esporta immagini
	c6->Print("./o3/O3_C_lampadaD_spectrum.eps","eps");

	//Grafica->Run();

	return;
}
