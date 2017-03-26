
#include <iostream>
#include <iomanip>
#include <cmath>		// pow(), tgamma()

#include "../root/TH1D.h"
#include "../root/TMath.h"

#include "./stat1.h"
#include "./funzioni.h"




int main( int argc, char * argv[]){



/*	///////////////////////////////////////////////////////////
	/// Istanzia finestre grafiche, canvas e pads

	TApplication* x1 = new TApplication( "x1", &argc, argv);
	TCanvas     * Canv0 = new TCanvas     ( "Canv0", "Canv0", 950, 950 );
*/	//	Canv0->Divide(2,1);



	///////////////////////////////////////////////////////////
	/// Carico dati nell'istogramma

	//  std::string path  = "/home/hyppo/Unimib/2_corsi/Lab2/modulo_1/dati_lezione_11/";
	std::string path  = "/mnt/hgfs/windows-home/LabInfoII/";
	std::string file1 = "data_funzione.txt";
	std::string file2 = "data_sidebands.txt";
	std::string file3 = "prova_3_col.txt";

	class stat1 dati1 ( path, file1 , 1, ' ', 0 );
	class stat1 dati2 ( path, file2 , 1, ' ', 0 );
	class stat1 dati3 ( path, file3 , 3, ' ', 0 );

		Stat::calc( & dati1.pData->at(0) );
		Stat::calc( & dati2.pData->at(0) );

	

/*	///////////////////////////////////////////////////////////
	/// Istogramma

	Canv0->Divide(2,2);

	TH1F* hist1 = new TH1F ( "hist1", file1.c_str(), size1 / 100 , min1, max1+1 );
		for ( dati1.ClearIterator() ; dati1.it < dati1.to ; dati1.it++ ) hist1->Fill( *dati1.it );
	 	Canv0->cd(1); hist1->Draw();

	TH1F* hist2 = new TH1F ( "hist2", file2.c_str(), size2 / 100 , min2, max2+1 );
		for ( dati2.ClearIterator() ; dati2.it < dati2.to ; dati2.it++ ) hist2->Fill( *dati2.it );
	 	Canv0->cd(2); hist2->Draw();

	// potrei anche procedere così ...
*/	// double mean = TMath::Mean< std::vector<double>::iterator > (dati1.from, dati1.to);





//	x1->Run();

return 0;}







