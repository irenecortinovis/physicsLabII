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

using namespace std;

//struct per ogni misura in P2_2
struct misura
{
  string colore;
  double delta;
  double delta_err;
  double n;
  double n_err;
  double lambda;
  double lambda_err;
};


//http://stackoverflow.com/questions/13094224/a-c-routine-to-round-a-float-to-n-significant-digits
double round_to_digits(double value, int digits)
{
    if (value == 0.0) // otherwise it will return 'nan' due to the log10() of zero
        return 0.0;

    double factor = pow(10.0, digits - ceil(log10(fabs(value))));
    return round(value * factor) / factor;   
}





int main(int argc, char const *argv[]) {

	TApplication * Grafica = new TApplication("", NULL, NULL);


//----------------- Definizione canvas -----------------

	TCanvas * c1 = new TCanvas("c1", "Reticolo d",0,0,900,500);
	
	//TCanvas * c2 = new TCanvas("c2", "Reticolo lambda",950,0,900,500);
	//c2->Divide(2,1);
	
	TCanvas * c3 = new TCanvas("c3", "Prisma",1950,0,900,500);



//----------------- Definizione grafici -----------------

//parte 1 determinazione di d

	TGraphErrors * Gerr1 = new TGraphErrors("../data/P1_1_d.txt", "%lg %lg %lg %lg");
	Gerr1->SetTitle("Determinazione del passo del reticolo d");
	Gerr1->GetXaxis()->SetTitle("N");
	Gerr1->GetYaxis()->SetTitle("sen(theta)");

/*
//parte 1 determinazione di lambda (diversi materiali)

	//materiale 1
	TGraphErrors * Gerr2 = new TGraphErrors("../data/P1_2_lambda_lampadaD.txt", "%lg %lg %lg %lg");
	Gerr2->GetXaxis()->SetTitle("N");
	Gerr2->GetYaxis()->SetTitle("sen(theta)");

	//materiale 2
	TGraphErrors * Gerr3 = new TGraphErrors("../data/P1_2_lambda_lampadaC.txt", "%lg %lg %lg %lg");
	Gerr3->GetXaxis()->SetTitle("N");
	Gerr3->GetYaxis()->SetTitle("sen(theta)");
*/
//parte 2 determinazione di a e b in cauchy

	TGraphErrors * Gerr4 = new TGraphErrors("../data/P2_1_cauchy.txt", "%lg %lg %lg %lg");
	Gerr4->SetTitle("Determinazione di coefficienti A e B nella formula di Cauchy");
	Gerr4->GetXaxis()->SetTitle("n(lambda)");
	Gerr4->GetYaxis()->SetTitle("lambda");





//-------------------------------------
//------------ PARTE 1 ----------------
//-------------------------------------

	
//---------------------------Determinazione di d

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
	double d_reticolo = fit1->GetParameter(0);
	double d_reticolo_err = fit1->GetParError(0);
	std::cout << "\nRISULTATO" << std::endl;
	std::cout << "Passo del reticolo: " << round(d_reticolo) <<
	" +/- " << round(d_reticolo_err) << " nm" << std::endl;


//---------------------------Determinazione di lambda per diversi materiali
// to do: medie anziché grafici
/*
	//----------lampada D

	std::cout << "\n\n-------------- Determinazione di lambda per lampada D ------------\n" << std::endl;


	TF1 * fit2 = new TF1("reticolo lambda lampada D", "[0]/[1]*x",0,3);
	//[0]= lambda
	fit2->SetParName(0,"lambda [nm]");
	fit2->SetParName(1, "d reticolo [nm]");
	fit2->SetParameter(0,500); //[nm]
	fit2->FixParameter(1,d_reticolo); //[nm]

	c2->cd(1);
	Gerr2->Draw("AP");
	Gerr2->SetMarkerColor(1);
	Gerr2->SetMarkerSize(1);
	Gerr2->SetMarkerStyle(21);

	Gerr2->Fit(fit2,"C");
	double lambda_materiale1 = fit2->GetParameter(0);


	//----------lampada C

	std::cout << "\n\n-------------- Determinazione di lambda per lampada C ------------\n" << std::endl;


	TF1 * fit3 = new TF1("reticolo lambda lampada D", "[0]/[1]*x",0,10);
	//[0]= lambda
	fit3->SetParName(0,"lambda [nm]");
	fit3->SetParName(1, "d reticolo [nm]");
	fit3->SetParameter(0,500); //[nm]
	fit3->FixParameter(1,d_reticolo); //[nm]

	c2->cd(2);
	Gerr3->Draw("AP");
	Gerr3->SetMarkerColor(1);
	Gerr3->SetMarkerSize(1);
	Gerr3->SetMarkerStyle(21);

	Gerr3->Fit(fit3,"C");
	double lambda_materiale2 = fit3->GetParameter(0);

*/


//-------------------------------------
//------------ PARTE 2 ----------------
//-------------------------------------


//---------------------------Determinazione di a e b nella formula di Cauchy

	std::cout << "\n\n-------------- Determinazione dei coefficienti A e B" <<
				 "nella formula di Cauchy ------------\n" << std::endl;

	
	TF1 * fit4 = new TF1("prisma a b cauchy", "[0]+[1]/(x*x)",0,2); //[nm]
	//[0]=a, [1]=b
	fit4->SetParName(0,"a");
	fit4->SetParName(1, "b [nm^2]");
	fit4->SetParameter(0,1); //
	fit4->SetParameter(1,10000); //[nm^2]

	
	c3->cd();
	Gerr4->Draw("AP");
	Gerr4->SetMarkerColor(1);
	Gerr4->SetMarkerSize(1);
	Gerr4->SetMarkerStyle(21);

	Gerr4->Fit(fit4,"C");
	double a_cauchy = fit4->GetParameter(0);
	double b_cauchy = fit4->GetParameter(1);
	double a_cauchy_err = fit4->GetParError(0);
	double b_cauchy_err = fit4->GetParError(1);
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
	vector<misura> lampadaC;
	vector<misura> lampadaD;

	//variabili per la lettura dei file
	double x, y;
	string colore;
	int counter = 0;

	//--------------- lampada C --------------------

	std::cout << "\n\n-------------- Determinazione di lambda per lampada C ------------\n" << std::endl;


	std::ifstream infileC;
	infileC.open("../data/P2_2_lambda_lampadaC.txt", std::ios::in);

	//riempio vettore di struct lampadaC
	while(infileC >> colore >> x >> y)
	{
		lampadaC.push_back(misura());
		lampadaC[counter].colore = colore;
		lampadaC[counter].delta = x;
		lampadaC[counter].delta_err = y;
		lampadaC[counter].n = sin(0.5*(alpha + x))/sin(0.5*alpha);
		lampadaC[counter].n_err = y*0.5*cos(0.5*(alpha + x))/sin(0.5*alpha);
		lampadaC[counter].lambda = sqrt(b_cauchy/(lampadaC[counter].n-a_cauchy));
		lampadaC[counter].lambda_err = lampadaC[counter].n_err*0.5*sqrt(b_cauchy/(pow((lampadaC[counter].n-a_cauchy),3)));
		//print
		std::cout << "Colore:\t" << lampadaC[counter].colore << 
					 "\nlambda:\t" << round(lampadaC[counter].lambda) << 
					 " +/- " << round(lampadaC[counter].lambda_err) << "\n" << std::endl;
		counter++;
	}

	infileC.close();



	//--------------- lampada D --------------------

	std::cout << "\n-------------- Determinazione di lambda per lampada D ------------\n" << std::endl;


	std::ifstream infileD;
	infileD.open("../data/P2_2_lambda_lampadaD.txt", std::ios::in);

	//reset counter
	counter = 0;

	//riempio vettore di struct lampadaD
	while(infileD >> colore >> x >> y)
	{
		lampadaC.push_back(misura());
		lampadaC[counter].colore = colore;
		lampadaC[counter].delta = x;
		lampadaC[counter].delta_err = y;
		lampadaC[counter].n = sin(0.5*(alpha + x))/sin(0.5*alpha);
		lampadaC[counter].n_err = y*0.5*cos(0.5*(alpha + x))/sin(0.5*alpha);
		lampadaC[counter].lambda = sqrt(b_cauchy/(lampadaC[counter].n-a_cauchy));
		lampadaC[counter].lambda_err = lampadaC[counter].n_err*0.5*sqrt(b_cauchy/(pow((lampadaC[counter].n-a_cauchy),3)));
		//print
		std::cout << "Colore:\t" << lampadaC[counter].colore << 
					 "\nlambda:\t" << round(lampadaC[counter].lambda) << 
					 " +/- " << round(lampadaC[counter].lambda_err) << "\n" << std::endl;
		counter++;
	}

	infileD.close();

//-------------------------------------
	
	Grafica->Run();

	return 0;
}
