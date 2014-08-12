#ifndef PULSESHAPEFORFIT_H
#define PULSESHAPEFORFIT_H
#include <TF1.h>

/**
   struct to return fit results
   The TF1 and be used to overlay the for on the pulse wave form 
   retrieved with PadeChannel::GetHist
 **/
struct PulseFit{
  double pedestal;
  double noise;
  double aMaxValue;
  double aMaxError;
  double tRiseValue;
  double tRiseError;
  double chi2;
  double ndof;
  double chi2Peak;
  double ndofPeak;
  int status;
  TF1 func;
};

std::ostream& operator<<(std::ostream& s, const PulseFit& f);

/// function to fit beam pulse shapes
double funcPulse(double *x, double *par);
/// function to fit laser pulse shapes
double funcPulseLaser(double *x, double *par);



	
#endif
