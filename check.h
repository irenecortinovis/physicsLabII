#ifndef check_h
#define check_h

/* NOTE
 *
 * helper functions
 * print and quick checks for objects' consistency
 *
 */

#include <TH1D.h>
#include <TF1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TMath.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TMatrixT.h>
#include <TMatrixDBase.h>
#include <TMatrixD.h>




void check_TGraph ( TGraph * ptr ){

	ptr->Print();

	std::cout
	<< " GetN: " << ptr->GetN() << std::endl
	;

	return;};




// void check_TGraphErrors ( TGraphErrors * ptr ){
//
// 	ptr->Print();
//
// 	std::cout
// 	<< " GetN: " << ptr->GetN() << std::endl
// 	;
//
// 	return;};




void check_hist( TH1F * ptr ){

	std::cout << "\n Verifica dati istrogramma : \n" << std::endl;
	ptr->Print();

	std::cout << "\n"
	<< " Underflow : "   << ptr->GetBinContent( 0 ) << "\n"
	<< " Overflow  : "   << ptr->GetBinContent( ptr->GetNbinsX()+1) << "\n"
	<< " GetNbinsX : "   << ptr->GetNbinsX() << "\n"
	<< " Integral() : "  << ptr->Integral()  << "\n"
	<< std::endl;

	return; };



/*
void check_fit_result( TFitResultPtr ptr ){

	std::cout << "\n Verifica risultati fit : \n" << std::endl;

	ptr->Print("V");

	// 	std::cout << "\n"
	// 	<< " Chi2 : "   							<< ptr->Chi2( ) << "\n"
	// 	<< " Parameter Error by Index  : "  << ptr->Error( 1 ) << "\n"
	// 	<< " Parameter Value by Index  : "  << ptr->Value( 1 ) << "\n"
	// 	<< " Correlation by indexes : "   	<< ptr->Correlation( 1, 2 ) << "\n"
	// 	<< std::endl;


return;};
*/


#endif
