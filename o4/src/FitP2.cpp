/*	comando per compilare
c++ ../src/FitP2.cpp -o FitP2.o `root-config --cflags --glibs`
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
  OutputFile<<"\\label{"  <<label <<"}\n";
  OutputFile<<"\\end{center}\n";
  OutputFile<<"\\end{table}\n";
  return;
}

////////////////////////////// VARIABILI ///////////////////////////////////////
/////////////////////////////// GLOBALI ////////////////////////////////////////

std::string caption = "a";
std::string InFileName = "default.txt";
std::string OutFileName = "default.txt";

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// INIZIO ///////////////////////////////////////
///////////////////////////////// MAIN /////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char const *argv[]) {
  TApplication * Grafica = new TApplication("", NULL, NULL);
  std::ifstream InFile;
  std::ofstream OutFile;

  ////////////////////////////////// PARTE 1 ///////////////////////////////////
  /////////////////////////////// LEGGE MALUS //////////////////////////////////
  std::cout <<"\n----------------- Fit Parte 1: Malus --------------------\n";

  // ----------------- INPUT DA FILE
  InFileName = "P2_1.txt";
    // INPUT:   | Gamma | I | errGamma | errI | //

  // ---------------- GRAFICO
  TGraphErrors * g1 = new TGraphErrors(InFileName.c_str(),"%lg %lg %lg %lg");
  g1->GetXaxis()->SetTitle("Angolo Emettitore-Ricevitore [rad]");
  g1->GetYaxis()->SetTitle("Intensità [W/m^2]");

  // ---------------- FIT
  TF1 * f1 = new TF1("malus", "[0]*pow(cos(x),2)", 0, 2*M_PI);
  f1->SetParName(0,"I_0");
  f1->SetParameter(0,10);

  g1->Fit("malus","C");

  // -------------DRAW
  TCanvas * c1 = new TCanvas("c1", "Legge Malus", 0, 0, 700, 500);
  c1->cd();

  g1->Draw("AP");
  g1->SetMarkerColor(1);
  g1->SetMarkerSize(1);
  g1->SetMarkerStyle(21);

  std::cout
    <<"Intercetta: "  <<round_to_digits( f1->GetParameter(0) ,3 ) <<std::endl
    <<"Pendenza: "    <<round_to_digits( f1->GetParameter(1) ,3 ) <<std::endl
  ;

  ////////////////////////////////// PARTE 2 ///////////////////////////////////
  /////////////////////////// FILTRO POLARIZZATORE /////////////////////////////

  ////////////////////////////////// PARTE 3 ///////////////////////////////////
  ////////////////////////////// ANGOLO BREWSTER ///////////////////////////////
  std::cout <<"\n----------------- Fit Parte 3: BREWSTER --------------------\n";

  // ----------------- INPUT DA FILE
  InFileName = "P2_3.txt";
    // INPUT:   | Teta_i | I | errTeta_i | errI | //

  // ---------------- GRAFICO
  TGraphErrors * g3 = new TGraphErrors(InFileName.c_str(),"%lg %lg %lg %lg");
  g3->GetXaxis()->SetTitle("Angolo incidenza [rad]");
  g3->GetYaxis()->SetTitle("Intensità [W/m^2]");

  // ---------------- FIT
  // TF1 * f3 = new TF1("brewster", "malus", 0, 2*M_PI);
  // f1->SetParName(0,"I_0");
  // f1->SetParameter(0,10);
  //
  // g1->Fit("brewster","C");

  // -------------DRAW
  TCanvas * c3 = new TCanvas("c3", "Angolo Brewster", 0, 600, 700, 500);
  c3->cd();

  g3->Draw("AP");
  g3->SetMarkerColor(1);
  g3->SetMarkerSize(1);
  g3->SetMarkerStyle(21);

  //Eval
  std::cout
    <<"Ang.Brewster: "  <<std::endl
  ;
  
  //////////////////////////////////////////////////////////////////////////////
  ////////////////////////////// FINE //////////////////////////////////////////
  Grafica->Run();
  return 0;

}
