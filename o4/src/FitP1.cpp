/*	comando per compilare
c++ ../src/FitP1.cpp -o FitP1.o `root-config --cflags --glibs`
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

////////////////////////////////// FUNZIONI ////////////////////////////////////
////////////////////////////////// GENERALI ////////////////////////////////////

//http://stackoverflow.com/questions/13094224/a-c-routine-to-round-a-float-to-n-significant-digits
double round_to_digits(double value, int digits)
{
    if (value == 0.0) // otherwise it will return 'nan' due to the log10() of zero
        return 0.0;

    double factor = pow(10.0, digits - ceil(log10(fabs(value))));
    return round(value * factor) / factor;
}

////////////////////////////////// FUNZIONI ////////////////////////////////////
/////////////////////////////////// LATEX //////////////////////////////////////

// E' IMPORTANTE CHE GLI SI PASSI UNA VARIABILE STRING SENNO' FA SBATTI DI COMPILAZIONE
void LatexTableBegin(std::ofstream& OutputFile, std::string& caption)
{
  //File deve essere open
  OutputFile<<"\\begin{table}[htbp]\n";
  OutputFile<<"\\begin{center}\n";
  OutputFile<<"\\caption{"  <<caption <<"}\n";
  OutputFile<<"\\begin{tabular}{|c|c|c|}\n";
  OutputFile<<"\\hline\n";
  return;
}

// E' IMPORTANTE CHE GLI SI PASSI UNA VARIABILE STRING SENNO' FA SBATTI DI COMPILAZIONE
void LatexTableEnd(std::ofstream& OutputFile, std::string& label)
{
  //File deve essere open
  OutputFile<<"\\hline\n";
  OutputFile<<"\\end{tabular}\n";
  OutputFile<<"\\label{"  <<label <<"}\n";
  OutputFile<<"\\end{center}\n";
  OutputFile<<"\\end{table}\n";
  return;
}

////////////////////////////// VARIABILI ///////////////////////////////////////
/////////////////////////////// GLOBALI ////////////////////////////////////////

std::string caption = "a";
std::string InFileName = "default.txt";
std::string OutFileName = "default.tex";


///////////////////////////////// INIZIO ///////////////////////////////////////
///////////////////////////////// MAIN /////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char const *argv[])
{

  TApplication * Grafica = new TApplication("", NULL, NULL);
  std::ifstream InFile;
  std::ofstream OutFile;

  ////////////////////////////////// PARTE 1 ///////////////////////////////////
  ////////////////////////////// LEGGE CARTESIO ////////////////////////////////
  std::cout <<"\n----------------- Fit Parte 1: Cartesio --------------------\n";

  // ----------------- INPUT DA FILE
  InFileName = "P1_1.txt";
    // INPUT:   | Teta_i | Teta_r | errTeta_i | errTeta_r | //

  // ---------------- GRAFICO
  TGraphErrors * g1 = new TGraphErrors(InFileName.c_str(),"%lg %lg %lg %lg");
  g1->GetXaxis()->SetTitle("Angolo incidenza [rad]");
  g1->GetYaxis()->SetTitle("Angolo riflessione [rad]");

  // ---------------- FIT
  TF1 * f1 = new TF1("cartesio", "[0] + [1]*x", 0, 2*M_PI);
  f1->SetParName(0,"Intercetta");
  f1->SetParameter(0,0);
  f1->SetParName(1,"Pendenza");
  f1->SetParameter(1,1);

  g1->Fit("cartesio","C");

  // -------------DRAW
  TCanvas * c1 = new TCanvas("c1", "Legge Cartesio", 0, 0, 700, 500);
  c1->cd();

	g1->Draw("AP");
	g1->SetMarkerColor(1);
	g1->SetMarkerSize(1);
	g1->SetMarkerStyle(21);

  std::cout
    <<"Intercetta: "  <<round_to_digits( f1->GetParameter(0) ) <<std::endl
    <<"Pendenza: "    <<round_to_digits( f1->GetParameter(1) ) <<std::endl
  ;

  ////////////////////////////////// PARTE 2 ///////////////////////////////////
  //////////////////////////// INDICE RIFRAZIONE ///////////////////////////////
  std::cout <<"\n---------------- Fit Parte 2: Rifrazione ------------------\n";

  // ----------------- INPUT DA FILE
  InFileName = "P1_2.txt";
    // INPUT:   | sin(T_2) | Pi/2-Alfa | errsin(T_2) | errsin(Pi/2-Alfa)) | //

  // ---------------- GRAFICO
  TGraphErrors * g2 = new TGraphErrors(InFileName.c_str(),"%lg %lg %lg %lg");
  g2->GetXaxis()->SetTitle("sin( Angolo_incidenza ) []");
  g2->GetYaxis()->SetTitle("sin(Pi/2 - Alfa) []");

  // ---------------- FIT
  TF1 * f2 = new TF1("cartesio", "[0] + [1]*x", 0, 2*M_PI);
  f2->SetParName(0,"Intercetta");
  f2->SetParameter(0,0);
  f2->SetParName(1,"n");
  f2->SetParameter(1,1);

  g1->Fit("cartesio","C");

  // -------------DRAW
  TCanvas * c2 = new TCanvas("c2", "Indice di Rifrazione", 800, 0, 700, 500);
  c2->cd();

  g2->Draw("AP");
  g2->SetMarkerColor(1);
  g2->SetMarkerSize(1);
  g2->SetMarkerStyle(21);

  std::cout
    <<"Intercetta: "  <<round_to_digits( f2->GetParameter(0) ) <<std::endl
    <<"n: "           <<round_to_digits( f2->GetParameter(1) ) <<std::endl
  ;

  //////////////////////////////////////////////////////////////////////////////
  ////////////////////////////// FINE //////////////////////////////////////////
  Grafica->Run();
  return 0;

}
