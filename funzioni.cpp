/* NOTE
 *
 * alcune funzioni ricorrenti
 * c++ funzioni.cpp -otest_tf1 -std=c++98 -Wall `root-config --cflags --glibs` 2> log.txt;
 *
 */

#include <iostream>
#include <cmath>
#include <vector>




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

	std::cout << " Summ. stats for binne data: [*ptr]" << dati << "\n\n"
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






int main (void){

std::vector<double> x (10,1);
	BinnedStat::calc ( &x );

std::vector<double> y (10,1);
	BinnedStat::calc ( &y );

x.insert (x.begin()+y.size(), y.begin(), y.end() );

BinnedStat::calc ( &x );

return 0;}




