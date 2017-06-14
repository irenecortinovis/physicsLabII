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

//----------------- Funzioni errori ----------------//

//N=1 per tutte le misure quindi non lo metto
double Lambda_P1_2(double sinAngolo, const double d){
  return d/1.*sinAngolo;
}
double LambdaErr_P1_2(double sinAngolo, double err_sinAngolo, const double d, const double err_d){
  return sqrt( pow (err_d*sinAngolo , 2 ) + pow ( d*cos(asin(sinAngolo))*err_sinAngolo,2) );
}

//----------------funzioni stampa tabelle latex
// E' IMPORTANTE CHE GLI SI PASSI UNA VARIABILE STRING SENNO' FA SBATTI DI COMPILAZIONE
void LatexTableBegin(std::ofstream& OutputFile, std::string& caption){
  //File deve essere open
  OutputFile<<"\\begin{table}[H]\n";
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
  //return sqrt( err_b*err_b/b + b/pow( (N-a), 2 )*( err_a*err_a + err_N*err_N) ) / (2*sqrt(N-a) );
  return sqrt( err_b*err_b/(4*b*(N-a)) + (err_N*err_N + err_a*err_a)*b/(4*pow((N-a), 3)));
}
