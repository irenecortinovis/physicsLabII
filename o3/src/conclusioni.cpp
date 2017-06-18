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

    gStyle->SetOptFit(1111);
    gStyle->SetOptStat(0);

  TCanvas * c5 = new TCanvas("c5", "Lampada C",0,0,900,500);
  c5->Divide(1,2);

  TCanvas * c6 = new TCanvas("c6", "Lampada D",0,0,900,500);
  c6->Divide(1,2);


//----------------- Definizione grafici -----------------

	//parte 1 determinazione di d

	TGraphErrors * Gerr1 = new TGraphErrors("./o3/data/P1_1_d.txt", "%lg %lg %lg %lg");
	Gerr1->SetTitle("Determinazione del passo del reticolo d");
	Gerr1->GetXaxis()->SetTitle("N");
	Gerr1->GetYaxis()->SetTitle("sen(theta)");

	//parte 1 spettro di emissione delle lampade

	TH1F* histoP1C = new TH1F("Reticolo: Spettro di emissione lampada C","Reticolo: Spettro di emissione lampada C",300,400,700);
	TH1F* histoP1D = new TH1F("Reticolo: Spettro di emissione lampada D","Reticolo: Spettro di emissione lampada D",300,400,700);

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


//----------------- Determinazione di d -----------------

	std::cout << "\n\n-------------- Determinazione del passo del reticolo d ------------\n" << std::endl;

	const double lambdaNa = 589; //[nm]

	TF1 * fit1 = new TF1("reticolo_d", "[1]/[0]*x",0,3);
	//[0]= d
	fit1->SetParName(0,"d [nm]");
	fit1->SetParName(1, "lambda Na yellow [nm]");
	fit1->SetParameter(0,1./600.*1000000); //[nm]
	fit1->FixParameter(1,lambdaNa); //[nm]
	Gerr1->Fit(fit1,"C");
	const double d_reticolo = fit1->GetParameter(0);
	const double d_reticolo_err = fit1->GetParError(0);
	std::cout << "\nRISULTATO" << std::endl;
	std::cout << "Passo del reticolo: " << round(d_reticolo) <<
	" +/- " << round(d_reticolo_err) << " nm" << std::endl;


	//---------------------------Determinazione di lambda per lampade C e D


  double tempSin, tempErr;
  std::string tempColore;


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



  /////////////////LAMPADA D
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


  




//----------------- PARTE 2 -----------------



//---------------------------Determinazione di a e b nella formula di Cauchy

  std::cout << "\n\n-------------- Determinazione dei coefficienti A e B" <<
         "nella formula di Cauchy ------------\n" << std::endl;


  TF1 * fit4 = new TF1("prisma a b cauchy", "[0]+[1]/(x*x)",0,2); //[nm]
  //[0]=a, [1]=b
  fit4->SetParName(0,"A");
  fit4->SetParName(1, "B [nm^2]");
  fit4->SetParameter(0,1); //
  fit4->SetParameter(1,10000); //[nm^2]

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

  const double alpha = 62*2*M_PI/360; //62 gradi in radianti
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
  if(InFile.good()==0)
    std::cout << "Errore! Impossibile aprire il file." << std::endl;


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

 

  //--------------- lampada D --------------------

  std::cout << "\n-------------- Determinazione di lambda per lampada D ------------\n" << std::endl;


  InFile.open("./o3/data/P2_2_lambda_lampadaD.txt", std::ios::in);
  if(InFile.good()==0)
    std::cout << "Errore! Impossibile aprire il file." << std::endl;


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

  

//------------------- confronto finale -----------------
  

  //lampada D
  c6->cd(1);
  histoP1D->Draw();
  c6->cd(2);
  histoP2D->Draw();


  //lampada C
  c5->cd(1);
  histoP1C->Draw();
  c5->cd(2);
  histoP2C->Draw();

  //--------------------------------------Esporta immagini
  c5->Print("./o3/O3_C_lampadaC_spectrum.eps","eps");
  c6->Print("./o3/O3_C_lampadaD_spectrum.eps","eps");

  //Grafica->Run();

  return;
}
