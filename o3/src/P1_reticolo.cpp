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

struct misuraP1{
  string colore;
  double lambda;
  double lambda_err;
};


void O3_P1_reticolo()
{
	TApplication * Grafica = new TApplication("", NULL, NULL);

//----------------- Definizione canvas -----------------

    gStyle->SetOptFit(1111);
    gStyle->SetOptStat(0);

	TCanvas * c1 = new TCanvas("c1", "Reticolo",0,0,900,500);
	TCanvas * c2 = new TCanvas("c2", "Reticolo",0,0,900,500);
	c2->Divide(1,3);


//----------------- Definizione grafici -----------------

	//parte 1 determinazione di d

	TGraphErrors * Gerr1 = new TGraphErrors("./o3/data/P1_1_d.txt", "%lg %lg %lg %lg");
	Gerr1->SetTitle("Determinazione del passo del reticolo d");
	Gerr1->GetXaxis()->SetTitle("N");
	Gerr1->GetYaxis()->SetTitle("sen(theta)");

	//parte 1 spettro di emissione delle lampade

	TH1F* histoP1B = new TH1F("Reticolo: spettro di emissione lampada B","Reticolo: Spettro di emissione lampadaB",300,400,700);
	TH1F* histoP1C = new TH1F("Reticolo: Spettro di emissione lampada C","Reticolo: Spettro di emissione lampada C",300,400,700);
	TH1F* histoP1D = new TH1F("Reticolo: Spettro di emissione lampada D","Reticolo: Spettro di emissione lampada D",300,400,700);




//----------------- Determinazione di d -----------------

	std::cout << "\n\n-------------- Determinazione del passo del reticolo d ------------\n" << std::endl;

	const double lambdaNa = 589; //[nm]

	TF1 * fit1 = new TF1("reticolo_d", "[1]/[0]*x",0,3);
	//[0]= d
	fit1->SetParName(0,"d [nm]");
	fit1->SetParName(1, "lambda Na yellow [nm]");
	fit1->SetParameter(0,1./600.*1000000); //[nm]
	fit1->FixParameter(1,lambdaNa); //[nm]

	c1->cd();
	Gerr1->Draw("AP");
	Gerr1->SetMarkerColor(1);
	Gerr1->SetMarkerSize(1);
	Gerr1->SetMarkerStyle(21);

	Gerr1->Fit(fit1,"C");
	const double d_reticolo = fit1->GetParameter(0);
	const double d_reticolo_err = fit1->GetParError(0);
	std::cout << "\nRISULTATO" << std::endl;
	std::cout << "Passo del reticolo: " << round(d_reticolo) <<
	" +/- " << round(d_reticolo_err) << " nm" << std::endl;




	//----------------- Preparazione file -----------------

	std::ifstream InFile;
	std::ofstream OutFile;
	int counter = 0;





	//---------------------------Determinazione di lambda per diversi materiali


  double tempSin, tempErr;
  std::string tempColore;
  std:string caption = "a";

/////////////////////LAMPADA B

  InFile.open("./o3/data/P1_2_lampadaB.txt", std::ios::in);
  if(InFile.good()==0)
    std::cout << "Errore! Impossibile aprire il file." << std::endl;


  std::vector<misuraP1> LampadaB_P1;

  counter = 0;
  while (InFile >>tempColore >> tempSin >>tempErr){
    LampadaB_P1.push_back(misuraP1() );
    LampadaB_P1[counter].colore = tempColore;
    LampadaB_P1[counter].lambda = Lambda_P1_2(tempSin, d_reticolo);
	histoP1B->Fill(LampadaB_P1[counter].lambda);
    LampadaB_P1[counter].lambda_err =  LambdaErr_P1_2(tempSin,tempErr,d_reticolo,d_reticolo_err);
    counter++;
  }

  InFile.close();

  //disegno istogramma
  c2->cd(1);
  histoP1B->Draw();


  //Output tabella latex -------------------
  OutFile.open("./o3/build/O3_P1_2_LambdaB.tex", std::ios::out);

  caption = "Spettro Lampada B";
  LatexTableBegin(OutFile, caption);

  OutFile
  <<"Colore & Lungh.Onda [nm] & Errore \\\\ \n"
  <<"\\hline\n";

  for (size_t i = 0; i < counter; i++) {
    OutFile
    <<LampadaB_P1[i].colore <<" & "
    <<round_to_digits(LampadaB_P1[i].lambda, 2) <<" & "
    <<round_to_digits(LampadaB_P1[i].lambda_err, 1) <<" \\\\ \n";
  }
  // se si vuole inserire una label
  caption = "label";
  LatexTableEnd(OutFile, caption);

  OutFile.close();

  std::cout << "\nCreato file O3_P1_2_LambdaB.tex in build" << std::endl;


////////////////////////////LAMPADA C
  InFile.open("./o3/data/P1_2_lampadaC.txt", std::ios::in);
  if(InFile.good()==0)
    std::cout << "Errore! Impossibile aprire il file." << std::endl;


  std::vector<misuraP1> LampadaC_P1;

  counter = 0;
  while (InFile >>tempColore >> tempSin >>tempErr){
    LampadaC_P1.push_back(misuraP1());
    LampadaC_P1[counter].colore = tempColore;
    LampadaC_P1[counter].lambda = Lambda_P1_2(tempSin, d_reticolo);
    histoP1C->Fill(LampadaC_P1[counter].lambda);
    LampadaC_P1[counter].lambda_err =  LambdaErr_P1_2(tempSin,tempErr,d_reticolo,d_reticolo_err);
    counter++;
  }

  InFile.close();

  //disegno istogramma
  c2->cd(2);
  histoP1C->Draw();

  //Output tabella latex -------------------
  OutFile.open("./o3/build/O3_P1_2_LambdaC.tex", std::ios::out);
  caption = "Spettro lampada C";

  LatexTableBegin(OutFile, caption);

  OutFile
  <<"Colore & Lungh.Onda [nm] & Errore [nm]\\\\ \n"
  <<"\\hline\n";

  for (size_t i = 0; i < counter; i++) {
    OutFile
    <<LampadaC_P1[i].colore <<" & "
    <<round_to_digits(LampadaC_P1[i].lambda, 2) <<" & "
    <<round_to_digits(LampadaC_P1[i].lambda_err, 1) <<" \\\\ \n";
  }
  // se si vuole inserire una label
  caption = "label";
  LatexTableEnd(OutFile, caption);

  OutFile.close();

  std::cout << "Creato file O3_P1_2_LambdaC.tex in build" << std::endl;


  /////////////////LAMPADA D (input: |Colore | Media(DX-SX)sinAngolo | ErrMedia sinAngolo |)
  InFile.open("./o3/data/P1_2_lampadaD.txt", std::ios::in);
  if(InFile.good()==0)
    std::cout << "Errore! Impossibile aprire il file." << std::endl;

  std::vector<misuraP1> LampadaD_P1;

  counter = 0;
  while (InFile >>tempColore >> tempSin >>tempErr){
    LampadaD_P1.push_back(misuraP1() );
    LampadaD_P1[counter].colore = tempColore;
    LampadaD_P1[counter].lambda = Lambda_P1_2(tempSin, d_reticolo);
    histoP1D->Fill(LampadaD_P1[counter].lambda);
    LampadaD_P1[counter].lambda_err =  LambdaErr_P1_2(tempSin,tempErr,d_reticolo,d_reticolo_err);
    counter++;
  }

  InFile.close();

  //disegno istogramma
  c2->cd(3);
  histoP1D->Draw();

  //Output tabella latex -------------------
  OutFile.open("./o3/build/O3_P1_2_LambdaD.tex", std::ios::out);
  caption = "Spettro Lampada D";

  LatexTableBegin(OutFile, caption);

  OutFile
    <<"Colore & Lungh.Onda [nm] & Errore [nm]\\\\ \n"
    <<"\\hline\n";

  for (size_t i = 0; i < counter; i++) {
    OutFile
    <<LampadaD_P1[i].colore <<" & "
    <<round_to_digits(LampadaD_P1[i].lambda, 2) <<" & "
    <<round_to_digits(LampadaD_P1[i].lambda_err, 1) <<" \\\\ \n";
  }
  caption = "label";
  LatexTableEnd(OutFile, caption);

  OutFile.close();

  std::cout << "Creato file O3_P1_2_LambdaD.tex in build" << std::endl;





//--------------------------------------Esporta immagini
	c1->Print("./o3/O3_P1_1_d.eps","eps");
	c2->Print("./o3/O3_P1_2_spectrum.eps","eps");

//-------------------------------------


	//Grafica->Run();

	return;
}
