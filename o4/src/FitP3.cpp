/*	comando per compilare
c++ ../src/FitP3.cpp -o FitP3.o `root-config --cflags --glibs`
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
double Lambda = 0.285; // Unità: Metri

///////////////////////////////// INIZIO ///////////////////////////////////////
///////////////////////////////// MAIN /////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char const *argv[]) {
  TApplication * Grafica = new TApplication("", NULL, NULL);
  std::ifstream InFile;
  std::ofstream OutFile;

  ////////////////////////////////// PARTE 1 ///////////////////////////////////
  ///////////////////////////// ONDE STAZIONARIE ///////////////////////////////
  std::cout <<"\n------------- Fit Parte 1: Onde Stazionarie ------------------\n";

  // ----------------- INPUT DA FILE
  InFileName = "P3_1.txt";
    // INPUT:   | d | N | errd | errN | //

  // ---------------- GRAFICO
  TGraphErrors * g1 = new TGraphErrors(InFileName.c_str(),"%lg %lg %lg %lg");
  g1->GetXaxis()->SetTitle("Distanza [m]");
  g1->GetYaxis()->SetTitle("Ordine di massimo N []");
  // ---------------- FIT
  TF1 * f1 = new TF1("stazio", "2/[0]*x", 0, 1 );
    //condizione di massimo: 2x = N*Lambda
  f1->SetParName(0,"Lambda");
  f1->SetParameter(1, Lambda);  //Unità: Metri

  g1->Fit("stazio","C");

  // -------------DRAW
  TCanvas * c1 = new TCanvas("c1", "Onde Stazionarie", 0, 0, 700, 500);
  c1->cd();

  g1->Draw("AP");
  g1->SetMarkerColor(1);
  g1->SetMarkerSize(1);
  g1->SetMarkerStyle(21);

  std::cout
    <<"Lunghezza Onda: "    <<round_to_digits( f1->GetParameter(0) ,3 ) <<std::endl
  ;

  ////////////////////////////////// PARTE 2 ///////////////////////////////////
  ///////////////////////////// DOPPIA FENDITURA ///////////////////////////////
  std::cout <<"\n--------------- Fit Parte 2: Doppia Fenditura ----------------\n";

  // ----------------- INPUT DA FILE
  InFileName = "P3_2.txt";
    // INPUT:   | sinTeta | N | errsinTeta | errN = 0 | //

  // ---------------- GRAFICO
  TGraphErrors * g2 = new TGraphErrors(InFileName.c_str(),"%lg %lg %lg %lg");
  g2->GetXaxis()->SetTitle("sinTeta []");
  g2->GetYaxis()->SetTitle("Ordine di massimo N []");

  // ---------------- FIT
  TF1 * f2 = new TF1("fenditura", "[0]/[1]*x", -1, 1 );
    //Fit su condizione di massimo: N*Lambda = d*sinTeta
  f2->SetParName(0,"d");  //Max Ampiezza
  f2->FixParameter(0,10); //Unità
  f2->SetParName(1,"Lambda");
  f2->SetParameter(1, Lambda);  //Unità: Metri

  g2->Fit("fenditura","C");

  // -------------DRAW
  TCanvas * c2 = new TCanvas("c2", "Doppia Fenditura", 800, 0, 700, 500);
  c2->cd();

  g2->Draw("AP");
  g2->SetMarkerColor(1);
  g2->SetMarkerSize(1);
  g2->SetMarkerStyle(21);

  std::cout
    <<"Lunghezza Onda: "    <<round_to_digits( f2->GetParameter(1) ,3 ) <<std::endl
  ;


  ////////////////////////////////// PARTE 3 ///////////////////////////////////
  ////////////////////////////// SPECCHIO LLOYD ////////////////////////////////
  std::cout <<"\n--------------- Fit Parte 3: Specchio Lloyd ------------------\n";

  // ----------------- INPUT DA FILE
  InFileName = "P3_3.txt";
    // INPUT:   | d | N | errd | errN = 0 | //

  // ---------------- GRAFICO
  TGraphErrors * g3 = new TGraphErrors(InFileName.c_str(),"%lg %lg %lg %lg");
  g3->GetXaxis()->SetTitle("d [m]");
  g3->GetYaxis()->SetTitle("Ordine di massimo N []");

  // ---------------- FIT
  TF1 * f3 = new TF1("lloyd", " ( [0] - sqrt( 4*x*x+[0]*[0] ) ) / [1] ", 0, 1 );
    //Fit su condizione di massimo: N*Lambda = L - sqrt( 4d^2 + L^2 )
  f3->SetParName(0,"L");  //Max Ampiezza
  f3->FixParameter(0,1); //Unità: Metri
  f3->SetParName(1,"Lambda");
  f3->SetParameter(1, Lambda);  //Unità: Metri

  g3->Fit("lloyd","C");

  // -------------DRAW
  TCanvas * c3 = new TCanvas("c3", "Lloyd", 1600, 0, 700, 500);
  c3->cd();

  g3->Draw("AP");
  g3->SetMarkerColor(1);
  g3->SetMarkerSize(1);
  g3->SetMarkerStyle(21);

  std::cout
    <<"Lunghezza Onda: "    <<round_to_digits( f3->GetParameter(1) ,3 ) <<std::endl
  ;


  ////////////////////////////////// PARTE 4 ///////////////////////////////////
  /////////////////////////////// FABRY-PERROT /////////////////////////////////
  std::cout <<"\n------------------ Fit Parte 4: Fabry-Perrot -----------------\n";

  // ----------------- INPUT DA FILE
  InFileName = "P3_4.txt";
    // INPUT:   | d | N | errd | errN = 0 | //

  // ---------------- GRAFICO
  TGraphErrors * g4 = new TGraphErrors(InFileName.c_str(),"%lg %lg %lg %lg");
  g4->GetXaxis()->SetTitle("d [m]");
  g4->GetYaxis()->SetTitle("Ordine di massimo N []");

  // ---------------- FIT
  TF1 * f4 = new TF1("fp", " 2*[0]/[1]*[2]*x ", 0, 1 );
    //Fit su condizione di massimo: N*Lambda = 2*M*cos(Teta)*d (M = numero riflessioni)
  f4->SetParName(0,"Nr.Riflessioni");  //Numero Riflessioni
  f4->FixParameter(0,1);
  f4->SetParName(1,"Lambda");
  f4->SetParameter(1, Lambda);  //Unità: Metri
  f4->SetParName(1, "Cos(Teta)"); //Angolo incidenza radiazione
  f4->FixParameter(2,1);

  g4->Fit("fp","C");

  // -------------DRAW
  TCanvas * c4 = new TCanvas("c4", "Lloyd", 0, 600, 700, 500);
  c4->cd();

  g4->Draw("AP");
  g4->SetMarkerColor(1);
  g4->SetMarkerSize(1);
  g4->SetMarkerStyle(21);

  std::cout
    <<"Lunghezza Onda: "    <<round_to_digits( f4->GetParameter(1) ,3 ) <<std::endl
  ;


  ////////////////////////////////// PARTE 5 ///////////////////////////////////
  ///////////////////////////////// MiCHELSON //////////////////////////////////
  std::cout <<"\n------------------ Fit Parte 5: Michelson --------------------\n";
    //-- Detta L distanza Emettitore-Specchio A ; M distanza Ricevitore-Specchio B
    //-- Supposto specchio C al centro del sistema
    //-- DeltaR = L + M = x

  // ----------------- INPUT DA FILE
  InFileName = "P3_5.txt";
    // INPUT:   | DeltaR | N | errDeltaR | errN = 0 | //

  // ---------------- GRAFICO
  TGraphErrors * g5 = new TGraphErrors(InFileName.c_str(),"%lg %lg %lg %lg");
  g5->GetXaxis()->SetTitle("d [m]");
  g5->GetYaxis()->SetTitle("Ordine di massimo N []");

  // ---------------- FIT
  TF1 * f5 = new TF1("michelson", "[0] + x/[1]", 0, 1 );
    //Fit su condizione di massimo: N*Lambda = R+L = x (M = numero riflessioni)
  f5->SetParName(0,"Intercetta");
  f5->SetParameter(0,0);
  f5->SetParName(1,"Lambda");
  f5->SetParameter(1, Lambda);  //Unità: Metri

  g5->Fit("fp","C");

  // -------------DRAW
  TCanvas * c5 = new TCanvas("c5", "michelson", 800, 600, 700, 500);
  c5->cd();

  g5->Draw("AP");
  g5->SetMarkerColor(1);
  g5->SetMarkerSize(1);
  g5->SetMarkerStyle(21);

  std::cout
    <<"Lunghezza Onda: "    <<round_to_digits( f5->GetParameter(1) ,3 ) <<std::endl
  ;


  ////////////////////////////////// PARTE 6 ///////////////////////////////////
  /////////////////////////////////// BRAGG ////////////////////////////////////
  std::cout <<"\n-------------------- Fit Parte 6: Bragg ----------------------\n";
  std::cout <<"\n------------------ Fit Parte 5: Michelson --------------------\n";
    //-- Detta L distanza Emettitore-Specchio A ; M distanza Ricevitore-Specchio B
    //-- Supposto specchio C al centro del sistema
    //-- DeltaR = L + M = x

  // ----------------- INPUT DA FILE
  InFileName = "P3_6.txt";
    // INPUT:   | sinTeta | N | errsinTeta | errN = 0 | //

  // ---------------- GRAFICO
  TGraphErrors * g6 = new TGraphErrors(InFileName.c_str(),"%lg %lg %lg %lg");
  g6->GetXaxis()->SetTitle("sin(Teta) []");
  g6->GetYaxis()->SetTitle("Ordine di massimo N []");

  // ---------------- FIT
  TF1 * f6 = new TF1("bragg", "2*[0]/[1]*x", -1, 1 );
    //Fit su condizione di massimo: N*Lambda = R+L = x (M = numero riflessioni)
  f6->SetParName(0,"d");
  f6->FixParameter(0,5);  //// d fisso o varia? //////////////////////////////// !!!!
  f6->SetParName(1,"Lambda");
  f6->SetParameter(1, Lambda);  //Unità: Metri

  g6->Fit("fp","C");

  // -------------DRAW
  TCanvas * c6 = new TCanvas("c6", "bragg", 1600, 600, 700, 500);
  c6->cd();

  g6->Draw("AP");
  g6->SetMarkerColor(1);
  g6->SetMarkerSize(1);
  g6->SetMarkerStyle(21);

  std::cout
    <<"Lunghezza Onda: " <<round_to_digits( f6->GetParameter(1) ,3 )
    <<std::endl;

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////// OUTPUT RISULTATI ////////////////////////////////

  std::vector<double> v_lambda;
  std::vector<double> v_lambda_err;
  //-- Fill lambda
    v_lambda.push_back( f1->GetParameter(0) );
    v_lambda.push_back( f2->GetParameter(1) );
    v_lambda.push_back( f3->GetParameter(1) );
    v_lambda.push_back( f4->GetParameter(1) );
    v_lambda.push_back( f5->GetParameter(1) );
    v_lambda.push_back( f6->GetParameter(1) );

  //-- Fill lambda_err
    v_lambda_err.push_back( f1->GetParError(0) );
    v_lambda_err.push_back( f2->GetParError(1) );
    v_lambda_err.push_back( f3->GetParError(1) );
    v_lambda_err.push_back( f4->GetParError(1) );
    v_lambda_err.push_back( f5->GetParError(1) );
    v_lambda_err.push_back( f6->GetParError(1) );

  //---------------- Media Pesata
  double lambdaMean = 0; double lambdaMean_err = 0;
  for (size_t i = 0; i < v_lambda.size(); i++)
  {
    lambdaMean += v_lambda[i] / ( v_lambda_err[i] * v_lambda_err[i] );
    lambdaMean_err += 1 / ( v_lambda_err[i] * v_lambda_err[i] );
  }
  lambdaMean /= lambdaMean_err;

  //-- err media pesata = sqrt( 1/somma_i(1/err_i) )
  lambdaMean_err = 1 / lambdaMean_err;
  lambdaMean_err = sqrt(lambdaMean_err);

  //---------- Print Finale
  std::cout
    << "Lungh.Onda (media pesata): "  <<lambdaMean
    <<" +/- "                         <<lambdaMean_err <<std::endl
    << "Lungh.Onda (nominale)"        <<Lambda      <<std::endl
  ;

  //////////////////////////////////////////////////////////////////////////////
  ////////////////////////////// FINE //////////////////////////////////////////
  Grafica->Run();
  return 0;
}
