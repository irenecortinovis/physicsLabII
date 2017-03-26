#ifndef IFunzioni_h
#define IFunzioni_f


/* NOTE
 *
 * 
 *
 *
 */



#include <cmath>
#include <algorithm>

#include "../root/TMath.h"

#include "Math/IFunction.h"
#include "Math/IParamFunction.h"





class RegressionLinear: public ROOT::Math::IParametricFunctionMultiDim
{
private:
  
const double* pars;
const int dim;				// number of independent variables, constant excluded
const bool constant;		// if TRUE, include constant, default = kTRUE

public:

  RegressionLinear( int N, bool include_const = kTRUE ): dim( N ), constant( include_const ){};

  double DoEvalPar(const double* x, const double* p) const
  {
	 double value = 0;
	 	if ( constant ) value = p[0];		// assume first is the costant
		for ( int i = 1; i <= dim; i++ ) value += x[i]*p[i];
		
	 return value  ;
  }

  unsigned int NDim() const
  {
	 return dim;
  }

  ROOT::Math::IParametricFunctionMultiDim* Clone() const
  {
	 return new RegressionLinear();
  }

  const double* Parameters() const 
  {
	 return pars;
  }

  void SetParameters(const double* p)
  {
	 pars = p;
  }

  unsigned int NPar() const
  {
	 if ( constant ) return dim + 1 ;	// constant included
	 	else return dim;		
  }
  
};






#endif