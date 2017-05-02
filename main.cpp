//
//
// c++ stat1.cc main.cpp -o L2.o -Wall -std=c++98 `root-config --cflags --glibs`

/* NOTE:



*/


#include <iostream>
#include <iomanip>
#include <cmath>

#include <TApplication.h>
#include <TCanvas.h>


#include "./c1/src/P3_1_diod.cpp"
//#include "./c2/src/P1_1_trueRMS.cpp"
#include "./c3/src/P1_impedenze_fdt.cpp"
#include "./c3/src/P2_RL_impulsata.cpp"
#include "./c3/src/P2_RC_impulsata.cpp"


int main( int argc, char * argv[]){



  ///////////////////////////////////////////////////////////
  /// Istanzia finestre grafiche, canvas e pads

  //TApplication* x1 = new TApplication( "x1", &argc, argv);
  //TCanvas     * Canv0 = new TCanvas     ( "Canv0", "Canv0", 950, 950 );
// 	 	Canv0->Divide(2,1);
		
		
 ///////////////////////////////////////////////////////////
 /// Laboratorio

//------------ C1 ---------------
//   P3_1_diod( Canv0 );

//------------ C2 ---------------
//   P1_1_trueRMS( Canv0 );

//------------ C3 ---------------
//   P1_impedenze_fdt( Canv0 );
	 C3_P2_RL_impulsata();
//	 C3_P2_RC_impulsata();

//------------ C4 ---------------



  
//  x1->Run();


return 0;}
