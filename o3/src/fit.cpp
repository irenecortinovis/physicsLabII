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


//http://stackoverflow.com/questions/13094224/a-c-routine-to-round-a-float-to-n-significant-digits
double round_to_digits(double value, int digits)
{
    if (value == 0.0) // otherwise it will return 'nan' due to the log10() of zero
        return 0.0;

    double factor = pow(10.0, digits - ceil(log10(fabs(value))));
    return round(value * factor) / factor;
}


/////////////////////////////////////////
/////////////////PARTE 1/////////////////
/////////////////////////////////////////
struct misuraP1{
  string colore;
  double lambda;
  double lambda_err;
};

//----------------- Funzioni errori ----------------//

//N=1 per tutte le misure quindi non lo metto
double Lambda_P1_2(double sinAngolo, const double d){
  return d/1.*sinAngolo;
}
double LambdaErr_P1_2(double sinAngolo, double err_sinAngolo, const double d, const double err_d){
  return sqrt( pow (err_d/1.*sinAngolo , 2 ) + pow ( d/(sinAngolo*sinAngolo)*err_sinAngolo,2) );
}

//----------------funzioni stampa tabelle latex
// E' IMPORTANTE CHE GLI SI PASSI UNA VARIABILE STRING SENNO' FA SBATTI DI COMPILAZIONE
void LatexTableBegin(std::ofstream& OutputFile, std::string& caption){
  //File deve essere open
  OutputFile<<"\\begin{table}[htbp]\n";
  OutputFile<<"\\begin{center}\n";
  OutputFile<<"\\caption{"  <<caption <<"}\n";
  OutputFile<<"\\begin{tabular}{|c|c|c|}\n";
  OutputFile<<"\\hline\n";
  return;
}

// E' IMPORTANTE CHE GLI SI PASSI UNA VARIABILE STRING SENNO' FA SBATTI DI COMPILAZIONE
void LatexTableEnd(std::ofstream& OutputFile, std::string& label){
  //File deve essere open
  OutputFile<<"\\hline\n";
  OutputFile<<"\\end{tabular}\n";
  OutputFile<<"\\end{center}\n";
  OutputFile<<"\\label{"  <<label <<"}\n";
  OutputFile<<"\\end{table}\n";
  return;
}

/////////////////////////////////////////
/////////////////PARTE 2/////////////////
/////////////////////////////////////////
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

    gStyle->SetOptFit(1111);
    //gStyle->SetOptStat(0);

	TCanvas * c1 = new TCanvas("c1", "Reticolo",0,0,900,500);
	TCanvas * c2 = new TCanvas("c2", "Reticolo",0,0,900,500);
	c2->Divide(1,3);
	TCanvas * c3 = new TCanvas("c3", "Prisma",1950,0,900,500);
	TCanvas * c4 = new TCanvas("c4", "Prisma",0,0,900,500);
	c4->Divide(1,2);

	TCanvas * c5 = new TCanvas("c5", "Lampada C",0,0,900,500);
	c5->Divide(1,2);

	TCanvas * c6 = new TCanvas("c6", "Lampada D",0,0,900,500);
	c6->Divide(1,2);


//----------------- Definizione grafici -----------------

//parte 1 determinazione di d

	TGraphErrors * Gerr1 = new TGraphErrors("../data/P1_1_d.txt", "%lg %lg %lg %lg");
	Gerr1->SetTitle("Determinazione del passo del reticolo d");
	Gerr1->GetXaxis()->SetTitle("N");
	Gerr1->GetYaxis()->SetTitle("sen(theta)");

//parte 2 spettro di emissione delle lampade

	TH1F* histoP1B = new TH1F("Reticolo: spettro di emissione lampada B","Reticolo: Spettro di emissione lampadaB",300,400,700);
	TH1F* histoP1C = new TH1F("Reticolo: Spettro di emissione lampada C","Reticolo: Spettro di emissione lampada C",300,400,700);
	TH1F* histoP1D = new TH1F("Reticolo: Spettro di emissione lampada D","Reticolo: Spettro di emissione lampada D",300,400,700);


//parte 2 determinazione di a e b in cauchy

	TGraphErrors * Gerr4 = new TGraphErrors("../data/P2_1_cauchy.txt", "%lg %lg %lg %lg");
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


  double tempSin, tempErr;
  std::string tempColore;
  std:string caption = "a";

/////////////////////LAMPADA B

  InFile.open("../data/P1_2_lampadaB.txt", std::ios::in);

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
  OutFile.open("../build/O3_P1_2_LambdaB.tex", std::ios::out);

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
  InFile.open("../data/P1_2_lampadaC.txt", std::ios::in);

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
  OutFile.open("../build/O3_P1_2_LambdaC.tex", std::ios::out);
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
  InFile.open("../data/P1_2_lampadaD.txt", std::ios::in);

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
  OutFile.open("../build/O3_P1_2_LambdaD.tex", std::ios::out);
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







//-------------------------------------
//------------ PARTE 2 ----------------
//-------------------------------------


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


	InFile.open("../data/P2_2_lambda_lampadaC.txt", std::ios::in);

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
	OutFile.open("../build/O3_P2_2_LambdaC.tex", std::ios::out);

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


	InFile.open("../data/P2_2_lambda_lampadaD.txt", std::ios::in);

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
	OutFile.open("../build/O3_P2_2_LambdaD.tex", std::ios::out);

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





	//-------------------------------------
	//----------  CONCLUSIONI -------------
	//-------------------------------------

	//lampada C
	c5->cd(1);
	histoP1C->Draw();
	c5->cd(2);
	histoP2C->Draw();

	//lampada D
	c6->cd(1);
	histoP1D->Draw();
	c6->cd(2);
	histoP2D->Draw();


//-------------------------------------

	Grafica->Run();

	return 0;
}
