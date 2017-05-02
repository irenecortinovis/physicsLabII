//  Circuiti 4, Fit delle funzioni di trasferimento
//    codice per compilare :
//      c++ path/to/P2_FitFDT.cpp -o c4fit.o `root-config --cflags --glibs`

//***********NOTA***********
// questo è solo il pre compilatore. non compila. a me da errore "Modulo_R"
// does not name a type quando faccio i SetParName perchè lui li vuole fatti nel main


#include <TF1.h>
#include <cmath>

//------------FDT Resistenza

  //Modulo: wRC / sqrt( (wC)^2*(R+r_g+r_L)^2 + (w^2LC - 1)^2 )
    TF1 * Modulo_R = new TF1("Modulo_R"," x*[0] / sqrt (x*x*[1] + pow( (x*x*[2] - 1) , 2 ) ) " , 0, 1000 );
    //x = w ; [0]= RC ; [1] = C^2(R+r_g+r_L)^2 ; [2] = LC
    Modulo_R->SetParName(0, "RC");
    Modulo_R->SetParName(1, "[C*(SommeR)]^2");
    Modulo_R->SetParName(2, "LC");

  //Fase: - arctan( (w^2LC -1)/( wC*(R+r_g+r_l) ) )
    TF1 * Fase_R = new TF1("Fase_R", "-atan( (x*x*[0]-1) / (x*[1]) )" , 0, 1000 );
    //x = w ; [0] = LC  ; [1] = C(R+r_g+r_L)
    Fase_R->SetParName(0, "LC");
    Fase_R->SetParName(1, "C*(SommeR)");


//------------FDT Capacità

  //Modulo: 1/ sqrt( (wC)^2*(R+r_g+r_L)^2 + (w^2LC-1)^2 )
    TF1 * Modulo_C = new TF1( "Modulo_C", " 1 / sqrt( x*x*[0] + pow( (x*x*[1] - 1) , 2 ) )" , 0, 1000 );
    //x = w ; [0] = C^2(R+r_g+r_L)^2 ; [1] = LC
    Modulo_C->SetParName(0, "[C*(SommeR)]^2");
    Modulo_C->SetParName(1, "LC");

  //Fase: arctan ( wC(R+r_g+r_l)/(w^2LC-1) )
    TF1 * Fase_C = new TF1("Fase_C", "atan( (x*[0]) /(x*x*[1]-1))" , 0, 1000 );
    //x = w ; [0] = C(R+r_g+r_L) ; [1] = LC
    Fase_C->SetParName(0, "C*(SommeR)");
    Fase_C->SetParName(1, "LC");

//------------FDT Induttanza

  //Modulo: sqrt( (wCr_L)^2) + (w^2CL)^2 )/sqrt( (wC)^2*(R+r_g+r_L)^2 + (w^2LC-1)^2 )
    TF1 * Modulo_L = new TF1( "Modulo_L" , " sqrt ( pow( x*x*[0], 2) + pow( x*x*[2], 2) )/ sqrt( x*x*[1] + pow( (x*x*[2] - 1) , 2 ) ) " , 0, 1000 );
    //x = w ; [0]= Cr_L  ; [1] = C^2(R+r_g+r_L)^2 ; [2] = LC  ;
    Modulo_R->SetParName(0, "C*r_L");
    Modulo_R->SetParName(1, "[C*(SommeR)]^2");
    Modulo_R->SetParName(2, "LC");

  //Fase: arctan(wL/r_L) - arctan( (w^2LC -1)/(wC(R+r_g+r_L)) )
    TF1 * Fase_L = new TF1("Fase_L" , " atan(x*[0]) - atan( (x*x*[2] - 1)/ (x*[1]) )" , 0, 1000 );
    //x = w ; [0]= L/r_L ; [1] = C(R+r_g+r_L) ; [2] = LC
    Fase_L->SetParName(0, "L/r_L");
    Fase_L->SetParName(1, "C*(SommeR)");
    Fase_L->SetParName(2, "LC");
