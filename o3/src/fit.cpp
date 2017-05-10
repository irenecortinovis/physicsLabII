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

//----------------- Funzioni errori ----------------//
//----------- Parte 2_2 ------------//

//Errore di indice di rifrazione N
double errN(double alfa, double alfa_err, double delta, double delta_err){
	double alfa_derivative = pow( cos(alfa + 0.5*delta) , 2) / pow( sin(0.5*alfa) , 2 );
	double delta_derivative = pow( cos(0.5 * (alfa+delta) ) , 2 );
	return sqrt( alfa_err*alfa_err*alfa_derivative + delta_err*delta_err*delta_derivative ) / ( 2*sin(0.5*alfa) );
}

//Errore della lunghezza d'onda Lambda
double errLambda(double N, double err_N, const double b, const double err_b, const double a, const double err_a ){
	return sqrt( err_b*err_b/b + b/pow( (N-a), 2 )*( err_a*err_a + err_N*err_N) ) / (2*sqrt(N-a) );
}




int main(int argc, char const *argv[]) {

	TApplication * Grafica = new TApplication("", NULL, NULL);


//----------------- Definizione canvas -----------------

	TCanvas * c1 = new TCanvas("c1", "Reticolo d",0,0,900,500);
	TCanvas * c3 = new TCanvas("c3", "Prisma",1950,0,900,500);


//----------------- Definizione grafici -----------------

//parte 1 determinazione di d

	TGraphErrors * Gerr1 = new TGraphErrors("../data/P1_1_d.txt", "%lg %lg %lg %lg");
	Gerr1->SetTitle("Determinazione del passo del reticolo d");
	Gerr1->GetXaxis()->SetTitle("N");
	Gerr1->GetYaxis()->SetTitle("sen(theta)");

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
	const double d_reticolo = fit1->GetParameter(0);
	const double d_reticolo_err = fit1->GetParError(0);
	std::cout << "\nRISULTATO" << std::endl;
	std::cout << "Passo del reticolo: " << round(d_reticolo) <<
	" +/- " << round(d_reticolo_err) << " nm" << std::endl;


//---------------------------Determinazione di lambda per diversi materiali
// to do: medie anziché grafici



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
		lampadaC[counter].n_err = errN(alpha, alpha_err,
									   lampadaC[counter].delta, lampadaC[counter].delta_err);
		lampadaC[counter].lambda = sqrt(b_cauchy/(lampadaC[counter].n-a_cauchy));
		lampadaC[counter].lambda_err = errLambda(lampadaC[counter].n, lampadaC[counter].n_err,
												 b_cauchy, b_cauchy_err, a_cauchy, a_cauchy_err);
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
		lampadaD.push_back(misura());
		lampadaD[counter].colore = colore;
		lampadaD[counter].delta = x;
		lampadaD[counter].delta_err = y;
		lampadaD[counter].n = sin(0.5*(alpha + x))/sin(0.5*alpha);
		lampadaD[counter].n_err = errN(alpha, alpha_err,
								   lampadaD[counter].delta, lampadaD[counter].delta_err);
		lampadaD[counter].lambda = sqrt(b_cauchy/(lampadaD[counter].n-a_cauchy));
		lampadaD[counter].lambda_err = errLambda(lampadaD[counter].n, lampadaD[counter].n_err,
												 b_cauchy, b_cauchy_err, a_cauchy, a_cauchy_err);
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
