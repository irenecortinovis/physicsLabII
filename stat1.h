//
// Author: Alfredo Guarnieri   1/12/16



#ifndef stat1_h
#define stat1_h

#include <string>	// std::string
#include <vector>	// std::vector::iterator




////////////////////////////////////////////////////////////////////////////////
/** \class stat1
Importa N colonne di dati da un file di testo. Calcola le principali statistiche descrittive.
Depone le colonne in container vector<.> e fornisce l'iteratore it di inizio e fine per accedere ai dati.

<h2> stat1 </h2>

**/


typedef std::vector< std::vector<double> > dataMat;


class stat1{
  
public:
  
	stat1( std::string, std::string, int, char, int );
	//~stat1() { delete pData; };

	std::string path;
	std::string file;
	int columns;
	dataMat* pData;

private:
	
	void clear_buffer( char*, int );
};



#endif
