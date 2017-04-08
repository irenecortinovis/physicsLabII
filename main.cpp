//
//
// c++ stat1.cc main.cpp -oL2 -Wall -std=c++98 `root-config --cflags --glibs`

/* NOTE:



*/


#include <iostream>
#include <iomanip>
#include <cmath>

#include <TApplication.h>
#include <TCanvas.h>


#include "./c1/src/P3_1_diod.cpp"
#include "./c2/src/P1_1_trueRMS.cpp"

int main( int argc, char * argv[]){



  ///////////////////////////////////////////////////////////
  /// Istanzia finestre grafiche, canvas e pads

  TApplication* x1 = new TApplication( "x1", &argc, argv);
  TCanvas     * Canv0 = new TCanvas     ( "Canv0", "Canv0", 950, 950 );
// 	 	Canv0->Divide(2,1);
		
		
  ///////////////////////////////////////////////////////////
  /// Laboratorio

//   P3_1_diod( Canv0 );	// C1
  P1_1_trueRMS( Canv0 );	// C2

  
  x1->Run();


return 0;}
