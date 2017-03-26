#ifndef funzioni_h
#define funzioni_h

/* NOTE
 *
 * alcune funzioni ricorrenti
 *
 *
 */

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

#include <TMath.h>
#include <TF1.h>







///////////////////////////////////////////////////////////
///
///
/// Statistiche per dati raggruppati (conteggi)
///
///
///
///////////////////////////////////////////////////////////


namespace BinnedStat{


double fsize ( std::vector<double> * dati ){ return dati->size(); }

double fintegral ( std::vector<double> * dati ){ 
	double sum = 0;
	for (std::vector<double>::iterator it = dati->begin() ; it != dati->end(); ++it) sum += (*it);
	return sum; }

double fmom1 ( std::vector<double> * dati ){
	
	double sum = 0;
	double integral = 0;
	double freqass;
	int i  = 0;			//  assumo che primo evento = 0

	for (std::vector<double>::iterator it = dati->begin() ; it != dati->end(); ++it) {
		freqass = *it;
		sum += freqass * i;		// freq * evento=i
		integral += freqass;
		i++;
	}
	
	return sum / integral  ;}

double fmom2 ( std::vector<double> * dati ){
	
	double sum = 0;
	double integral = 0;
	double freqass;
	int i  = 0;			//  assumo che primo evento = 0

	for (std::vector<double>::iterator it = dati->begin() ; it != dati->end(); ++it) {
		freqass = *it;
		sum += freqass * i * i;		// freq * evento=i
		integral += freqass;
		i++;
	}
	
	return sum / integral  ;}

void calc ( std::vector<double> * dati ){

	double varianza   = fmom2( dati ) - fmom1( dati )*fmom1( dati );
	double varianza_c = (double) varianza * fsize( dati ) / ( fsize( dati ) - 1 );

	double std   = sqrt( varianza );
	double std_c = sqrt( varianza_c );

	std::cout << " Summ. stats for binned data: [*ptr]" << dati << "\n\n"
	<< "--------------------------"       << "\n\n"
	<< "size    : " <<  fsize( dati )     << "\n"
	<< "integral: " <<  fintegral( dati ) << "\n"
	<< "mean    : " <<  fmom1( dati )     << "\n"
	<< "mom2    : " <<  fmom2( dati )     << "\n"
	<< "varia   : " <<  varianza          << "\n"
	<< "std     : " <<  std               << "\n"
	<< "varic   : " <<  varianza_c        << "\n"
	<< "stdc    : " <<  std_c             << "\n"
	<< "--------------------------"       << "\n\n"
	;

return;
}

};



/* Test main

int main (void){

std::vector<double> x (10,1);
	BinnedStat::calc ( &x );

std::vector<double> y (10,1);
	BinnedStat::calc ( &y );

x.insert (x.begin()+y.size(), y.begin(), y.end() );

BinnedStat::calc ( &x );

return 0;}

*/









///////////////////////////////////////////////////////////
///
///
/// Statistiche per dati non raggruppati
///
///
///
///////////////////////////////////////////////////////////


namespace Stat{


double fsize( std::vector<double> * dati ){ return dati->size(); }



double fmax ( std::vector<double> * dati ){ 

std::vector<double>::iterator pto = dati->begin();

pto = TMath::LocMax< std::vector<double>::iterator >( pto, dati->end() );

return dati->at(pto - dati->begin() );
}



double fmin ( std::vector<double> * dati ){ 

std::vector<double>::iterator pto = dati->begin();

pto = TMath::LocMin< std::vector<double>::iterator >( pto, dati->end() );

return dati->at(pto - dati->begin() );
}



void  fsortinc( std::vector<double> * dati ){

std::vector<double>::iterator start = dati->begin();
std::vector<double>::iterator end   = dati->end();

std::sort< std::vector<double>::iterator > ( start, end+1 );

return;}



double fmom1 ( std::vector<double> * dati ){
	
	double sum = 0;
	int i  = 0;

	for (std::vector<double>::iterator it = dati->begin() ; it != dati->end(); ++it) {
		sum += *it;
		i++;
	}
	
	return sum / i  ;}

double fmom2 ( std::vector<double> * dati ){
	
	double sum = 0;
	int i  = 0;

	for (std::vector<double>::iterator it = dati->begin() ; it != dati->end(); ++it) {
		sum += (*it) * (*it);
		i++;
	}
	
	return sum / i  ;}

void calc ( std::vector<double> * dati ){

	double varianza   = fmom2( dati ) - fmom1( dati )*fmom1( dati );
	double varianza_c = (double) varianza * fsize( dati ) / ( fsize( dati ) - 1 );

	double std   = sqrt( varianza );
	double std_c = sqrt( varianza_c );

	std::cout << " Summ. stats for unbinned data: [*ptr]" << dati << "\n\n"
	<< "--------------------------"       << "\n\n"
	<< "size    : " <<  fsize( dati )     << "\n"
	<< "max     : " <<  fmax ( dati )     << "\n"
	<< "min     : " <<  fmin ( dati )     << "\n"
	<< "mean    : " <<  fmom1( dati )     << "\n"
	<< "mom2    : " <<  fmom2( dati )     << "\n"
	<< "varia   : " <<  varianza          << "\n"
	<< "std     : " <<  std               << "\n"
	<< "varic   : " <<  varianza_c        << "\n"
	<< "stdc    : " <<  std_c             << "\n"
	<< "--------------------------"       << "\n\n"
	;

return;
}

};










///////////////////////////////////////////////////////////
///
///
/// Probability density functions
///
///
///
///////////////////////////////////////////////////////////




/// PDF:		densità gamma(n/2,2)=chi2 unidimensionale
/// parametrizzazione:	forma p[0], scale p[1], dove media = forma / scala
///

double chi2( double * at, double * dof ){

  float scale = 2  ;
  int shape   = dof[0] / 2;
  double x = at[0];

return (double) pow(scale, -shape) * pow( x, shape-1 ) * exp( -x/scale ) / tgamma(shape);}




/// PDF:		densità gaussiana unidimensionale
/// parametrizzazione:	media p[0], varianza p[1], norma[2]
///

double gaussiana( double * at, double * param ){

  double mean  = param[0];
  double var   = param[1];
  double norm  = param[2];

  double x = at[0];

return (double) norm * pow( 2*M_PI*var , -0.5) *  exp( -(x - mean)*(x - mean) / (2*var) ) ;}



/// PDF:		funzione di probabilità di Poisson
/// parametrizzazione:	media = varianza p[0], norma p[1]
/// int x = 0, 1, 2, ....

double poisson( double * at, double * param ){

  double mean  = param[0];
  double norm  = param[1];

  int x = at[0];

return (double) norm * exp(-mean) * pow( mean, x ) * TMath::Gamma( x+1 )  ;}



/// PDF:		funzione di probabilità di Poisson
/// parametrizzazione:	media = varianza p[0], norma p[1]
/// int x = 0, 1, 2, ....

double poissonf( double * at, double * param ){

  double mean  = param[0];
  double norm  = param[1];

  int x = at[0];

return norm * TMath::Poisson( x, mean )  ;}



/// Funzione:	Grafico della funzione Chi2
/// Intput:	gradi di libertà
/// Output:

void Draw_Chi2( int* dof ){

  /// Disegna Chi2 usando la funzione chi2
  /// kBlue
  TF1 * tf1 = new TF1 ( "chi2_n", chi2, 0., 0.75*dof[0]*dof[0], 1 );
    tf1->SetParameter ( 0,  dof[0]  );
    tf1->SetParName   ( 0, "dof"    );
    tf1->SetLineColor ( kBlue       );

    tf1->Draw();


//   /// Disegna Chi2 usando una pdf di root
//   /// kTellow
//   TF1 * tf2 = new TF1 ( "chi2_n", "ROOT::Math::gamma_pdf( x, [0], [1], [2] )", 0., 0.75*dof[0]*dof[0] );
//     tf2->SetParameter( 0,  dof[0]/2  );
//     tf2->SetParameter( 1,  2         );
//     tf2->SetParameter( 2,  0         );
//     tf2->SetParName  ( 0, "shape"    );
//     tf2->SetParName  ( 1, "scale"    );
//     tf2->SetLineColor( kYellow       );
//
//     tf2->Draw();
//     // tf2->Draw("SAME"); // se nello stesso pad

  return;}






  
///////////////////////////////////////////////////////////
///
///
/// Modelli per rumori di fondo / estrazione segnali
///
///
///
///////////////////////////////////////////////////////////





double fondo_espo ( double * x, double * par ){

  double scale		= par[0];	// Integrale
  double mean 		= par[1];	// Media

  return scale * exp( - x[0] / mean   ) / mean;
}




double fondo_gauss ( double * x, double * par )  {

  // parametri della parabola
  double a = par[0];
  double b = par[1];
  double c = par[2];

  // parametri della gaussiana
  double mean 		= par[3];
  double variance = par[4];
  double norm 		= par[5];


  return norm * exp( -pow( x[0]-mean, 2 ) / (2*variance) ) + a*pow(x[0]-b,2) + c;
}




double gauss_mix ( double * x, double * par )  {

  // parametri gaussiane
  double mean1		= par[0];
  double norm1 	= par[1];
  double mean2		= par[2];
  double norm2 	= par[3];
  double variance1 = par[4];
  double variance2 = par[5];
  double flat = par[6];

  double y1 = 0; double y2 = 0; double y = flat;

  y1 = norm1 * exp( -pow( x[0]-mean1, 2 ) / (2*variance1) ) ;
  y2 = norm2 * exp( -pow( x[0]-mean2, 2 ) / (2*variance2) ) ;

  if ( x[0]> 130 && x[0] < 141 ) y = y1;
	 else if ( x[0]> 160 && x[0] < 180 ) y = y2;

  return y; }



#endif
