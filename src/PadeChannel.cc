#include "PadeChannel.h"
#include "calConstants.h"
#include "Mapper.h"


void PadeChannel::Reset(){
  _ts=0;
  _transfer_size=0;
  _board_id=0;
  _hw_counter=0;
  _ch_number=0;
  _eventnum=0;
  _max=0;
  for (int i=0; i<N_PADE_SAMPLES; i++) _wform[i]=0;
}

void PadeChannel::Dump() const{
  cout << "Header ==> timestamp: " <<  _ts << " size: " 
       << _transfer_size << " board: " << _board_id << " xfer#: " 
       << _hw_counter << " ch#: " <<  _ch_number << " event#: " 
       << _eventnum << endl << "samples=> " << (hex);
  for (int i=0; i<N_PADE_SAMPLES; i++) cout << _wform[i] << " ";
  cout << (dec) << endl;
}


void PadeChannel::Fill(ULong64_t ts, UShort_t transfer_size, 
		      UShort_t board_id, UInt_t hw_counter, 
		      UInt_t  ch_number,  UInt_t eventnum, Int_t *wform){
  _ts = ts;
  _transfer_size = transfer_size;
  _board_id = board_id;
  _hw_counter = hw_counter;
  _ch_number = ch_number;
  _eventnum = eventnum;
  _max=0;

  // HACK, needs improvement
  // find in-time window
  int idx=_board_id-112;   // BAD practice!
  if (idx>1) idx-=2;   
  int tmin=PADE_SAMPLE_TIMES[idx]-PADE_SAMPLE_RANGE;
  int tmax=PADE_SAMPLE_TIMES[idx]+PADE_SAMPLE_RANGE;
  
  // another evil hack - this handles the start of testbeam2 data where the first
  // 32 wave form samples are not valid wave data
  if (_ts<START_PORCH16) { // shift wform array by 16 counts, widen peak search window
    for (int i=0; i<N_PADE_DATA-16; i++) wform[i]=wform[i+16];
    for (int i=N_PADE_DATA-16; i<N_PADE_DATA; i++) wform[i]=wform[N_PADE_DATA-17];
    tmin=30;
    tmax=60;
  }

  /// todo add ped calculation here
  for (int i=0; i<N_PADE_SAMPLES; i++) {
    int dataIdx=i+N_PADE_PORCH;
    _wform[i]=wform[dataIdx];
    // max/min from start of data (not samples)
    if (i<=tmin || i>tmax) continue; 
    if (_wform[i]>_max) {
      _max=_wform[i];
      _peak=i;  // sample number for peak
    }
  }
  /// todo add pulse shape fit here 
  _flag = 0;
}

void PadeChannel::GetHist(TH1F *h){
  TString ti;
  ti.Form("Event %d : Board %d, channel %d",_eventnum, GetBoardID(),GetChannelNum());
  h->Reset();
  h->SetTitle(ti);
  h->SetBins(N_PADE_SAMPLES,-0.5,N_PADE_SAMPLES-0.5);
  for (int i=0; i<N_PADE_SAMPLES; i++){
    h->SetBinContent(i+1,_wform[i]);
  }
  h->SetMinimum(0);
}

void PadeChannel::GetXYZ(double &x, double &y, double &z){
  Mapper *mapper=Mapper::Instance();
  mapper->ChannelXYZ(GetChannelID(),x,y,z);
}

// trivial pedistal estimation
void PadeChannel::GetPedestal(double &ped, double &stdev){
  const int nsamples=10;
  double sum=0;
  double sum2=0;
  for (int i=0;i<PADE_PED_SAMPLES;i++) {sum+=_wform[i]; sum2+=_wform[i]*_wform[i];}
  ped=sum/PADE_PED_SAMPLES;
  double var =  1.0/(nsamples-1) * (sum2-sum*sum/PADE_PED_SAMPLES);
  stdev = TMath::Sqrt(var);
}


double PadeChannel::GetPedestal(){
double ped = 0;
double stdev = 0;
GetPedestal(ped, stdev);
return ped;
}

Int_t PadeChannel::GetChannelIndex(){
  Mapper *mapper=Mapper::Instance();
  return mapper->ChannelID2ChannelIndex(GetChannelID());
}

PulseFit PadeChannel::FitPulse(PadeChannel *pc, bool laserShape){ 
  cout << "hello!!"<<endl;
  static bool first=true;
  static TF1 *func;
  if (first){
    func = new TF1("func", funcPulse, 0.0, 120.0, 3);
    func->SetNpx(N_PADE_SAMPLES);
    first=false;
  }
  PulseFit result;
  result.aMaxValue  = 0.;
  result.aMaxError  = 0.;
  result.tRiseValue = 0.;
  result.tRiseError = 0.;
  result.status     = -1;
  if (!pc) return result;
  pc->GetPedestal(result.pedestal,result.noise);
  UShort_t* a=pc->GetWform();
  for(int i=10; i<80; i++){
    if(fabs(a[i] - result.pedestal) >= fabs(result.aMaxValue)){
      result.aMaxValue  = a[i] - result.pedestal;
      result.tRiseValue = i - 1.0;
    }
  }
  func->SetParameters( result.pedestal, result.aMaxValue, result.tRiseValue );
  // Limit range of timing within first 80 samples. We want to avoid
  // arbitrary amplitude with very late timing (out of range) in pure
  // noise events
  func->SetParLimits(0,  1.e+0, 1.e+4);
  func->SetParLimits(1, -1.e+4, 1.e+4);
  func->SetParLimits(2,  5.e+0, 8.e+1);

  // One can enable option "E" for the fitter
  // It will result in many messages about finding a new minimum for low amplitude pulses
  // Some extra CPU time will be spent but not significant to notice
  // The difference will be for events without visible signal that we don't care anyways

  TH1F h;
  pc->GetHist(&h);
  result.status = h.Fit("func", "BQW");

  result.aMaxValue  = func->GetParameter(1);
  result.aMaxError  = func->GetParError(1);
  result.tRiseValue = func->GetParameter(2);
  result.tRiseError = func->GetParError(2);
  result.chi2       = func->GetChisquare();
  result.ndof       = func->GetNDF();

  // recalculated chi2 using samples around the peak only
  func->SetParameters( result.pedestal, result.aMaxValue, result.tRiseValue );
  func->FixParameter(0, result.pedestal);
  func->FixParameter(1, result.aMaxValue);
  func->FixParameter(2, result.tRiseValue);
  h.Fit("func", "BQW", "", result.tRiseValue - 5.0, result.tRiseValue + 15.0 );
  result.chi2Peak       = func->GetChisquare();
  result.ndofPeak       = func->GetNDF();
  result.func = *func;

  // Calculate noise using all available samples before the
  // signal. Take it seriously if the number of such samples is more
  // than 5. Scale errors and chisquare.
  double sum0 = 0.;
  double sum1 = 0.;
  double sum2 = 0.;
  for(int i=0; i<(result.tRiseValue - 2.0); i++){
    sum0 += 1.0;
    sum1 += a[i];
    sum2 += a[i] * a[i];
  }
  if( sum0 > 5.5 ){
    double avg = sum1 / sum0;
    double rms = sqrt( sum2 / sum0 - avg * avg );
    result.noise = rms;
    result.aMaxError  *= rms;
    result.tRiseError *= rms;
    result.chi2       /= rms * rms;
    result.chi2Peak   /= rms * rms;
  }


  // Some comments about output: 
  //
  //    status = 0 - normal outcome 
  //    status = 4 - (about 14% of hits). 
  //                  Still OK. 
  //                  Errors on amplitude and time may be
  //                  unreliable
  //              
  //    aMaxError  - Not needed. It should be 0.5*noise for good pulses
  //    tRiseError - Not needed. It does not include systematics that is significant
  //    chi2       - Not needed. Use ch2Peak instead
  //    ndof       - Not needed. Use ndofPeak instead

  return result;
}


float PadeChannel::GetMaxCalib(){
  int idx=GetChannelIndex();
  return _max*CalConstants[idx];
}
