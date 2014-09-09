#ifndef PULSESHAPEFORFIT_H
#define PULSESHAPEFORFIT_H
#include <TF1.h>

/**
   struct to return fit results

   The TF1 and be used to overlay the for on the pulse wave form 
   retrieved with PadeChannel::GetHist

   Some comments about the output: 
   
   status = 0 - normal outcome<br> 
   status = 4 - (about 14% of hits) <br> 
   Still OK.  Errors on amplitude and time may be unreliable
                
   aMaxError  - Not needed. It should be 0.5*noise for good pulses <br>
   tRiseError - Not needed. It does not include systematics that are significant <br>
   chi2       - Not needed. Use chi2Peak instead <br>
   ndof       - Not needed. Use ndofPeak instead
**/
struct PulseFit{
  double pedestal;   ///< PADE pedestal, typically ~100 ADC counts
  double noise;      ///< RMS of pedestal fluctuatio around average
  double aMaxValue;  ///< Amplitude of scaled pulse form
  double aMaxError;  ///< MINUIT fitting error on above
  double tRiseValue; ///< Position of rising edge of pulse
  double tRiseError; ///< MINUIT fitting error on above
  double chi2;       ///< calculated over full fit region
  double ndof;       ///< calculated over full fit region
  double chi2Peak;   ///< calcaulted in region of peak
  double ndofPeak;   ///< calcaulted in region of peak
  int status;
  TF1 func;
};

std::ostream& operator<<(std::ostream& s, const PulseFit& f);

/// function to fit beam and laser pulse shapes
/** Pulse shape functions use an averaged emperical pulse shape that is
    pedestal suptracted, shifted and scaled by the fit proceedure.
**/
double funcPulseA(double *x, double *par);
double funcPulseB(double *x, double *par);
double funcPulseC(double *x, double *par);
double funcPulseD(double *x, double *par);

double funcPulseLaserA(double *x, double *par);
double funcPulseLaserB(double *x, double *par);
	
#endif
