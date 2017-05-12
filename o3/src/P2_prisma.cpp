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




//struct per ogni misura in P2_2
struct misuraP2
{
  string colore;
  double delta;
  double delta_err;
  double n;
  double n_err;
  double lambda;
  double lambda_err;
};


void O3_P2_prisma()
{
	TApplication * Grafica = new TApplication("", NULL, NULL);


//----------------- Definizione canvas -----------------

    gStyle->SetOptFit(1111);
    //gStyle->SetOptStat(0);

	TCanvas * c3 = new TCanvas("c3", "Prisma",1950,0,900,500);
	TCanvas * c4 = new TCanvas("c4", "Prisma",0,0,900,500);
	c4->Divide(1,2);


//----------------- Definizione grafici -----------------

//parte 2 determinazione di a e b in cauchy

	TGraphErrors * Gerr4 = new TGraphErrors("./o3/data/P2_1_cauchy.txt", "%lg %lg %lg %lg");
	Gerr4->SetTitle("Determinazione di coefficienti A e B nella formula di Cauchy");
	Gerr4->GetXaxis()->SetTitle("n(lambda)");
	Gerr4->GetYaxis()->SetTitle("lambda");

//parte 2 spettro di emissione delle lampade

	TH1F* histoP2C = new TH1F("Prisma: Spettro di emissione lampada C","Prisma: Spettro di emissione lampada C",300,400,700);
	TH1F* histoP2D = new TH1F("Prisma: Spettro di emissione lampada D","Prisma: Spettro di emissione lampada D",300,400,700);


//----------------- Preparazione file -----------------

	std::ifstream InFile;
	std::ofstream OutFile;
	int counter = 0;


//---------------------------Determinazione di a e b nella formula di Cauchy

	std::cout << "\n\n-------------- Determinazione dei coefficienti A e B" <<
				 "nella formula di Cauchy ------------\n" << std::endl;


	TF1 * fit4 = new TF1("prisma a b cauchy", "[0]+[1]/(x*x)",0,2); //[nm]
	//[0]=a, [1]=b
	fit4->SetParName(0,"A");
	fit4->SetParName(1, "B [nm^2]");
	fit4->SetParameter(0,1); //
	fit4->SetParameter(1,10000); //[nm^2]


	c3->cd();
	Gerr4->Draw("AP");
	Gerr4->SetMarkerColor(1);
	Gerr4->SetMarkerSize(1);
	Gerr4->SetMarkerStyle(21);

	Gerr4->Fit(fit4,"C");
	const double a_cauchy = fit4->GetParameter(0);
	const double b_cauchy = fit4->GetParameter(1);
	const double a_cauchy_err = fit4->GetParError(0);
	const double b_cauchy_err = fit4->GetParError(1);
	std::cout << "\nRISULTATO" << std::endl;
	std::cout << "A: " << round_to_digits(a_cauchy,4) <<
	" +/- " << round_to_digits(a_cauchy_err,1) << std::endl;
	std::cout << "B: " << round_to_digits(b_cauchy,2) <<
	" +/- " << round_to_digits(b_cauchy_err,1) << " nm^2" << std::endl;





//--------------------------Determinazione di lambda dalla formula di Cauchy

	/*P2_2_lambda_lampadaN.txt hanno colore, delta, err_delta in radianti in tre colonne
	e si devono trovare n, err_n, da cui lambda, err_lambda*/

	const double alpha = 62*2*M_PI/360;	//62 gradi in radianti
	const double alpha_err = 1*2*M_PI/360; //1 grado in radianti

	//vettore di struct per le misure delle lampade C e D
	vector<misuraP2> lampadaC_P2;
	vector<misuraP2> lampadaD_P2;

	//variabili per la lettura dei file
	double x, y;
	string colore;
	counter = 0;

	//--------------- lampada C --------------------

	std::cout << "\n\n-------------- Determinazione di lambda per lampada C ------------\n" << std::endl;


	InFile.open("./o3/data/P2_2_lambda_lampadaC.txt", std::ios::in);

	//riempio vettore di struct lampadaC_P2
	while(InFile >> colore >> x >> y)
	{
		lampadaC_P2.push_back(misuraP2());
		lampadaC_P2[counter].colore = colore;
		lampadaC_P2[counter].delta = x;
		lampadaC_P2[counter].delta_err = y;
		lampadaC_P2[counter].n = sin(0.5*(alpha + x))/sin(0.5*alpha);
		lampadaC_P2[counter].n_err = errN(alpha, alpha_err,
									   lampadaC_P2[counter].delta, lampadaC_P2[counter].delta_err);
		lampadaC_P2[counter].lambda = sqrt(b_cauchy/(lampadaC_P2[counter].n-a_cauchy));
		histoP2C->Fill(lampadaC_P2[counter].lambda);
		lampadaC_P2[counter].lambda_err = errLambda(lampadaC_P2[counter].n, lampadaC_P2[counter].n_err,
												 b_cauchy, b_cauchy_err, a_cauchy, a_cauchy_err);
		counter++;
	}

	InFile.close();

	//disegno istogramma
	c4->cd(1);
	histoP2C->Draw();

	//Output tabella latex -------------------
	OutFile.open("./o3/build/O3_P2_2_LambdaC.tex", std::ios::out);

  	std:string caption;
	caption = "Spettro Lampada C";
	LatexTableBegin(OutFile, caption);

	OutFile
		<<"Colore & Lungh.Onda [nm] & Errore [nm]\\\\ \n"
		<<"\\hline\n";

	for (size_t i = 0; i < counter; i++)
	{
		OutFile
			<<lampadaC_P2[i].colore <<" & "
			<<round_to_digits(lampadaC_P2[i].lambda, 2) <<" & "
			<<round_to_digits(lampadaC_P2[i].lambda_err, 1) <<" \\\\ \n";
	}

	// se si vuole inserire una label
	caption = "label";
	LatexTableEnd(OutFile, caption);

	OutFile.close();

	std::cout << "Creato file O3_P2_2_LambdaC.tex in build" << std::endl;




	//--------------- lampada D --------------------

	std::cout << "\n-------------- Determinazione di lambda per lampada D ------------\n" << std::endl;


	InFile.open("./o3/data/P2_2_lambda_lampadaD.txt", std::ios::in);

	//reset counter
	counter = 0;

	//riempio vettore di struct lampadaD_P2
	while(InFile >> colore >> x >> y)
	{
		lampadaD_P2.push_back(misuraP2());
		lampadaD_P2[counter].colore = colore;
		lampadaD_P2[counter].delta = x;
		lampadaD_P2[counter].delta_err = y;
		lampadaD_P2[counter].n = sin(0.5*(alpha + x))/sin(0.5*alpha);
		lampadaD_P2[counter].n_err = errN(alpha, alpha_err,
								   lampadaD_P2[counter].delta, lampadaD_P2[counter].delta_err);
		lampadaD_P2[counter].lambda = sqrt(b_cauchy/(lampadaD_P2[counter].n-a_cauchy));
		histoP2D->Fill(lampadaD_P2[counter].lambda);
		lampadaD_P2[counter].lambda_err = errLambda(lampadaD_P2[counter].n, lampadaD_P2[counter].n_err,
												 b_cauchy, b_cauchy_err, a_cauchy, a_cauchy_err);
		counter++;
	}

	InFile.close();

	//disegno istogramma
	c4->cd(2);
	histoP2D->Draw();

	//Output tabella latex -------------------
	OutFile.open("./o3/build/O3_P2_2_LambdaD.tex", std::ios::out);

	caption = "Spettro Lampada D";
	LatexTableBegin(OutFile, caption);

	OutFile
		<<"Colore & Lungh.Onda [nm] & Errore [nm] \\\\ \n"
		<<"\\hline\n";

	for (size_t i = 0; i < counter; i++)
	{
		OutFile
			<<lampadaD_P2[i].colore <<" & "
			<<round_to_digits(lampadaD_P2[i].lambda, 2) <<" & "
			<<round_to_digits(lampadaD_P2[i].lambda_err, 1) <<" \\\\ \n";
	}

	// se si vuole inserire una label
	caption = "label";
	LatexTableEnd(OutFile, caption);

	OutFile.close();

	std::cout << "Creato file O3_P2_2_LambdaD.tex in build" << std::endl;



//--------------------------------------Esporta immagini
	c3->Print("./o3/O3_P2_1_AB.eps","eps");
	c4->Print("./o3/O3_P2_2_spectrum.eps","eps");


//-------------------------------------


	//Grafica->Run();

	return;
}
