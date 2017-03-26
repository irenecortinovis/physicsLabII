//
// Author: Alfredo Guarnieri   1/12/16


#include "./stat1.h"

#include <iostream>	// std::cout, std::cin, std::stoi, std::stod
#include <string>	// std::string
#include <fstream>	// std::fstream
#include <vector>	// std::vector::iterator
#include <cstring>	// atod(), atof()
#include <stdlib.h>	// strtod()
#include <cmath>	// pow(double, double);




////////////////////////////////////////////////////////////////////////////////
/// Costruttore: 
/// "path", "file", columns, sep_char, lines_skip

stat1::stat1 ( std::string dpath, std::string dfile, int dcols, char dsep, int dskip):
path( dpath ),
file( dfile ),
columns( dcols )
{  

	//////////////////////////////////////////////////
	/// matrice dei dati

	pData = new dataMat( dcols );
		


	//////////////////////////////////////////////////
	/// colonne dei dati
	
	//for ( int i = 0; i < dcols; i++ ) pData->push_back( new std::vector<double> );




	//////////////////////////////////////////////////
	/// strings stuffs

	int skip_lines  = dskip;	// linee da saltare
	int buffer_size = 256;	// dimensione buffer memorizzazione temporanea
	char buffer_c[ buffer_size ];

	std::string buffer (buffer_size, '0');
	std::string pathFile = path+file;

	std::ifstream import ( pathFile.c_str(), std::ios::in);
	if (import.good() != true ) std::cout << "Errore lettura file"<< std::endl; //import.open();

	// while ( import >> a >> b  )

	//////////////////////////////////////////////////
	/// ciclo importazione

	for ( ; skip_lines > 0; skip_lines-- ) import.ignore(buffer_size,'\n');
	
	while ( !import.eof() ) {

		for ( int i = 0; i < dcols; i++ ){
			clear_buffer   ( buffer_c, buffer_size );			// azzera il buffer
		
		if  ( i == dcols - 1 ) import.getline ( buffer_c, buffer_size, '\n' );
		else import.getline ( buffer_c, buffer_size, dsep );

    	buffer.assign(buffer_c);							// converto in string per facilità

	    if ( buffer.size() > 1 ) pData->at(i).push_back( strtod( buffer.c_str(), NULL ) );
	    
	}
}

  // Chiudo lo stream di importazione
  if ( import.is_open() == true ) import.close();

}




void stat1:: clear_buffer( char ptr[], int size ){

	for (int i = 0; i < size; i++) ptr[i] = '0';
	return;
}






 




/*

// argc conta anche la stringa contenente: argv[0] = "./eseguibile"
//
int array[argc-1];

// provato che funziona con interi positivi e negativi, tronca numeri con virgola 1234.23
for ( int i = 1; i < argc ; i++ ) {
  array[i] = std::stoi(argv[i]);
  std::cout << array[i] << std::endl;
}
*/



// lettura di un percorso da linea di comando
//
/*
std::string path = argv[1];
std::cout << path << std::endl;
std::cout << path+path << std::endl;

std::cout << path.length() << std::endl;
std::cout << path.size() << std::endl;
std::cout << path.capacity() << std::endl;

std::cout << path[1] << std::endl;
std::cout << path[0] << std::endl;
std::cout << path.back() << std::endl;
std::cout << path.front() << std::endl;
*/


/*
// apre uno stream associato al file
//
std::string folder = argv[1];
std::string file = argv[2];

std::fstream streamFile ( folder+file, std::fstream::in | std::fstream::out );
streamFile.close();
*/
