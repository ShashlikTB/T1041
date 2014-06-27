#ifndef PULSESHAPEFORFIT_H
#define PULSESHAPEFORFIT_H
#include <TF1.h>
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

double funcPulse(double *x, double *par);


	
#endif
