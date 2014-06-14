#include "pulseShapeForFit.h"
#include <Math/Interpolator.h>
#include <TF1.h>

double PulseShapeX[]={
  -100.0,-1.59375,-1.40625,-1.21875,-1.03125,-0.84375,-0.65625,-0.46875,-0.28125,-0.09375,0.09375,0.28125,0.46875,0.65625,0.84375,1.03125,1.21875,1.40625,1.59375,1.78125,1.96875,2.15625,2.34375,2.53125,2.71875,2.90625,3.09375,3.28125,3.46875,3.65625,3.84375,4.03125,4.21875,4.40625,4.59375,4.78125,4.96875,5.15625,5.34375,5.53125,5.71875,5.90625,6.09375,6.28125,6.46875,6.65625,6.84375,7.03125,7.21875,7.40625,7.59375,7.78125,7.96875,8.15625,8.34375,8.53125,8.71875,8.90625,9.09375,9.28125,9.46875,9.65625,9.84375,10.0312,10.2188,10.4062,10.5938,10.7812,10.9688,11.1562,11.3438,11.5312,11.7188,11.9062,12.0938,12.2812,12.4688,12.6562,12.8438,13.0312,13.2188,13.4062,13.5938,13.7812,13.9688,14.1562,14.3438,14.5312,14.7188,14.9062,15.0938,15.2812,15.4688,15.6562,15.8438,16.0312,16.2188,16.4062,16.5938,16.7812,16.9688,17.1562,17.3438,17.5312,17.7188,17.9062,18.0938,18.2812,18.4688,18.6562,18.8438,19.0312,19.2188,19.4062,19.5938,19.7812,19.9688,20.1562,20.3438,20.5312,20.7188,20.9062,21.0938,21.2812,21.4688,21.6562,21.8438,22.0312,22.2188,22.4062,22.5938,22.7812,22.9688,23.1562,23.3438,23.5312,23.7188,23.9062,24.0938,24.2812,24.4688,24.6562,24.8438,25.0312,25.2188,25.4062,25.5938,25.7812,25.9688,26.1562,26.3438,26.5312,26.7188,26.9062,27.0938,27.2812,27.4688,27.6562,27.8438,28.0312,28.2188,28.4062,28.5938,28.7812,28.9688,29.1562,29.3438,29.5312,29.7188,29.9062,30.0938,30.2812,30.4688,30.6562,30.8438,31.0312,31.2188,31.4062,31.5938,31.7812,31.9688,32.1562,32.3438,32.5312,32.7188,32.9062,33.0938,33.2812,33.4688,33.6562,33.8438,34.0312,34.2188,34.4062,34.5938,34.7812,34.9688,35.1562,35.3438,35.5312,35.7188,35.9062,36.0938,36.2812,36.4688,36.6562,36.8438,37.0312,37.2188,37.4062,37.5938,37.7812,37.9688,38.1562,38.3438,38.5312,38.7188,38.9062,39.0938,39.2812,39.4688,39.6562,39.8438,40.0312,40.2188,40.4062,40.5938,40.7812,40.9688,41.1562,41.3438,41.5312,41.7188,41.9062,42.0938,42.2812,42.4688,42.6562,42.8438,43.0312,43.2188,43.4062,43.5938,43.7812,43.9688,44.1562,44.3438,44.5312,44.7188,44.9062,45.0938,45.2812,45.4688,45.6562,45.8438,46.0312,46.2188,46.4062,46.5938,46.7812,46.9688,47.1562,47.3438,47.5312,47.7188,47.9062,48.0938,48.2812,48.4688,48.6562,48.8438,49.0312,49.2188,49.4062,49.5938,49.7812,49.9688,50.1562,50.3438,50.5312,50.7188,50.9062,51.0938,51.2812,51.4688,51.6562,51.8438,52.0312,52.2188,52.4062,52.5938,52.7812,52.9688,53.1562,53.3438,53.5312,53.7188,53.9062,54.0938,54.2812,54.4688,54.6562,54.8438,55.0312,55.2188,55.4062,55.5938,55.7812,55.9688,56.1562,56.3438,56.5312,56.7188,56.9062,57.0938,57.2812,57.4688,57.6562,57.8438,58.0312,58.2188,58.4062,58.5938,58.7812,58.9688,59.1562,59.3438,59.5312,59.7188,120.0};

double PulseShapeY[]={
  0.0,0.0,0.000340905,0.00416903,0.0188591,0.0540343,0.120216,0.211643,0.319325,0.438917,0.565307,0.678123,0.774648,0.853604,0.917034,0.961746,0.988478,1.00175,1.00328,0.996401,0.982376,0.960598,0.933633,0.906427,0.876712,0.842186,0.80444,0.767743,0.729802,0.693321,0.652123,0.610939,0.570848,0.531327,0.493778,0.455752,0.41823,0.379785,0.343437,0.308885,0.276318,0.244931,0.21127,0.181186,0.152319,0.124507,0.0970509,0.0713865,0.0465666,0.0232995,0.00169822,-0.0190118,-0.0383741,-0.0575415,-0.07647,-0.0928749,-0.108921,-0.122877,-0.137493,-0.150936,-0.162448,-0.173478,-0.183942,-0.194041,-0.202699,-0.210915,-0.218462,-0.225555,-0.231788,-0.23681,-0.242883,-0.246953,-0.251139,-0.253966,-0.257424,-0.260764,-0.262843,-0.265014,-0.266073,-0.267625,-0.268352,-0.269586,-0.26966,-0.269682,-0.269118,-0.268598,-0.268516,-0.267674,-0.266596,-0.265142,-0.263401,-0.262182,-0.260167,-0.259138,-0.256472,-0.254264,-0.25223,-0.250577,-0.248215,-0.244661,-0.242364,-0.239785,-0.237653,-0.234758,-0.232217,-0.228892,-0.22565,-0.22331,-0.220976,-0.217693,-0.21426,-0.211006,-0.208441,-0.205559,-0.202891,-0.199441,-0.196758,-0.19343,-0.191008,-0.188348,-0.185209,-0.181814,-0.178858,-0.176257,-0.174057,-0.170931,-0.167695,-0.164981,-0.162478,-0.159944,-0.157208,-0.153947,-0.15163,-0.148922,-0.146591,-0.144276,-0.141221,-0.138867,-0.136506,-0.134048,-0.132015,-0.129569,-0.126415,-0.124332,-0.122195,-0.120258,-0.117937,-0.115615,-0.113143,-0.111283,-0.10933,-0.107563,-0.10484,-0.103088,-0.100973,-0.0991377,-0.0974394,-0.0954976,-0.0930423,-0.0912969,-0.0896049,-0.0883058,-0.0865524,-0.084371,-0.0826134,-0.0811334,-0.0799129,-0.0780751,-0.0761576,-0.0743783,-0.0733021,-0.0719741,-0.0707207,-0.0689514,-0.06692,-0.0657139,-0.0647728,-0.0636742,-0.062223,-0.0604053,-0.0591177,-0.058262,-0.0575238,-0.0560429,-0.0548002,-0.0534665,-0.0523395,-0.0513658,-0.050675,-0.0494789,-0.0476675,-0.0471164,-0.0462185,-0.0454703,-0.0445383,-0.042872,-0.0422344,-0.0414422,-0.0409904,-0.0399998,-0.0388039,-0.0380844,-0.0373905,-0.0365654,-0.0359743,-0.0351698,-0.0340795,-0.0333043,-0.0330561,-0.0322158,-0.0316515,-0.030709,-0.0298681,-0.0291885,-0.0290977,-0.0284244,-0.0275358,-0.0267061,-0.0261757,-0.0259042,-0.0255018,-0.0247829,-0.0239126,-0.0234716,-0.0230201,-0.0226471,-0.0222074,-0.0212443,-0.0206868,-0.0207469,-0.0205114,-0.0201613,-0.0195055,-0.0189771,-0.0183809,-0.0180907,-0.0180057,-0.0175903,-0.0168272,-0.0163545,-0.0164818,-0.0160499,-0.0156923,-0.01485,-0.0146478,-0.0143508,-0.0143088,-0.0139645,-0.0135595,-0.0129927,-0.0128587,-0.0127315,-0.0128118,-0.0120652,-0.0116984,-0.0113028,-0.0115245,-0.011311,-0.0108975,-0.0103816,-0.0101538,-0.0102596,-0.0101391,-0.00974901,-0.00922877,-0.00902822,-0.00887931,-0.0087756,-0.00891741,-0.00832845,-0.00792569,-0.00767892,-0.00766573,-0.00755212,-0.00756785,-0.00702448,-0.00687294,-0.00680349,-0.00683589,-0.00697256,-0.00630557,-0.00596187,-0.00621483,-0.00604657,-0.00593991,-0.00580416,-0.00521133,-0.00528717,-0.00521502,-0.00532682,-0.00508467,-0.00472202,-0.00440608,-0.0045109,-0.00452309,-0.00455987,-0.00423618,-0.00399851,-0.00359223,-0.004082,-0.00407724,-0.00361534,-0.00330791,-0.00324841,-0.00339795,-0.00338157,-0.0033128,-0.00316157,-0.00294039,-0.00274167,-0.00280329,-0.00298259,-0.0028353,-0.00269819,-0.00237636,-0.00258278,-0.00274308,-0.00253581,-0.00207717,-0.00200835,-0.00232801,-0.00252991,-0.00250679,-0.00181778,-0.00177016,-0.00176989,-0.00201327,-0.00191049,-0.00198259,0.0};


const int NPointsPulseShape=sizeof(PulseShapeX)/sizeof(double);

//ROOT::Math::Interpolator pulseItp(NPointsPulseShape);  // cspline

double funcPulse(double *x, double *par){
  static bool first=true;
  static ROOT::Math::Interpolator *pulseItp;
  if (first) {
    pulseItp=new ROOT::Math::Interpolator(NPointsPulseShape, 
					  ROOT::Math::Interpolation::kLINEAR);
    pulseItp->SetData(NPointsPulseShape,PulseShapeX,PulseShapeY);
    first=false;
  }
  return par[0] + par[1] * pulseItp->Eval( x[0] - par[2] );
}

	
