//////          Created: 4/12/2014 S. Bein:
//////                    Modified code from J.Adams and A.Santra, added stack
/////                     display features and formatted to sync with TB Gui 
//////         4/20/2014 BH: Update to start using tools in TBReco library  */
#include "WC.h"

int tdc2WC(int tdcNum){
  if (tdcNum<5) return 1;
  else if (tdcNum <9) return 2;
  else if (tdcNum <13) return 3;
  else return 4;
}
