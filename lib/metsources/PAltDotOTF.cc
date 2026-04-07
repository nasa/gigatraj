/******************************************************************************* 
***  Written by: 
***     L. R. Lait (NASA Ames Research Center, Code SG) 
***     Code 614 
***     NASA Goddard Space Flight Center 
***     Greenbelt, MD 20771 
*** 
***  Copyright (c) 2023 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.  All Rights Reserved. 
*** 
*** Disclaimer:
*** No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS." 
*** Waiver and Indemnity:  RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT. 
***  (Please see the NOSA_19110.pdf file for more information.) 
*** 
********************************************************************************/
            
#include "config.h"

#include "gigatraj/PAltDotOTF.hh"


using namespace gigatraj;

static const real stdZdata[] = {  0.00, 1.00, 2.00, 3.00, 4.00, 5.00, 6.00, 7.00, 8.00,
      9.00, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0,
      21.0, 22.0, 23.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0, 32.0,
      33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0, 40.0, 41.0, 42.0, 43.0, 44.0,
      45.0, 46.0, 47.0, 48.0, 49.0, 50.0, 51.0, 52.0, 53.0, 54.0, 55.0, 56.0,
      57.0, 58.0, 59.0, 60.0, 61.0, 62.0, 63.0, 64.0, 65.0, 66.0, 67.0, 68.0,
      69.0};

static const real stdlogDzDpdata[] = { 
     -4.79626,    -4.68790,    -4.58836,    -4.48643,    -4.38200,    -4.27493,
     -4.16509,    -4.05234,    -3.93651,    -3.81744,    -3.69492,    -3.56036,
     -3.40889,    -3.25120,    -3.09351,    -2.93582,    -2.77813,    -2.62045,
     -2.46276,    -2.30507,    -2.14607,    -1.98517,    -1.82406,    -1.66368,
     -1.50403,    -1.34511,    -1.18688,    -1.02939,   -0.872589,   -0.716475,
    -0.561058,   -0.406319,   -0.250038,  -0.0911927,   0.0673624,    0.224037,
     0.378872,    0.531911,    0.683210,    0.832789,    0.980690,     1.12696,
      1.27163,     1.41473,     1.55630,     1.69636,     1.83497,     1.96922,
      2.09768,     2.22390,     2.35013,     2.47347,     2.59231,     2.71012,
      2.82918,     2.94952,     3.07118,     3.19416,     3.31852,     3.44427,
      3.57145,     3.70010,     3.83024,     3.96191,     4.09514,     4.23000,
      4.36648,     4.50465,     4.64457,     4.76667 };



const int PAltDotOTF::stdN = 70;
const real *PAltDotOTF::stdZ = stdZdata; 
const real *PAltDotOTF::stdLogDzDp = stdlogDzDpdata; 


PAltDotOTF::PAltDotOTF() 
{
    quant = "tendency_of_pressure_altitude";
    uu = "Pa/s";
    omega_name = "lagrangian_tendency_of_air_pressure";
    press_name = "air_pressure";
    palt_name = "pressure_altitude";

}

PAltDotOTF::PAltDotOTF(const std::string& w, const std::string& omega, const std::string& press, const std::string& palt)
{
     quant = w;
     omega_name = omega;
     press_name = press;
     palt_name = palt;

}

PAltDotOTF::~PAltDotOTF() 
{
}

PAltDotOTF::PAltDotOTF(const PAltDotOTF& src) : MetOnTheFly(src)
{
     omega_name = src.omega_name;
     press_name = src.press_name;
     palt_name = src.palt_name;
}

PAltDotOTF& PAltDotOTF::operator=(const PAltDotOTF& src)
{
    this->assign( src ) ;
    
    return *this;
}

void PAltDotOTF::assign( const PAltDotOTF& src)
{
     MetOnTheFly::assign( src );
     omega_name = src.omega_name;
     press_name = src.press_name;
     palt_name = src.palt_name;
}

real PAltDotOTF::get_dzdp( real alt ) const
{
    real result;
    
    if ( alt > 0 ) {
       
       for (int i=1; i < stdN; i++ ) {
           if ( (stdZ[i-1] <= alt && alt <= stdZ[i]) || (stdZ[i-1] >= alt && alt >= stdZ[i]) ) {
              // log-linearly interpolate the standard dp/dz
              result = (stdLogDzDp[i] - stdLogDzDp[i-1])/(stdZ[i] - stdZ[i-1])*(alt -stdZ[i-1]) + stdLogDzDp[i-1];
              result = - EXP(result)*100.0; // (km/mb -> km/Pa)
              return result;
           }
       }
       //std::cerr << "a: " << alt << std::endl;
       throw (badcalculation());
       
    } else {
       //std::cerr << "b: " << alt << std::endl;
       throw (badcalculation());
    } 
    
    return result;


}

GridField3D* PAltDotOTF::calc( const GridField3D& input1, int flags ) const
{
    // the output temperature tendency field
    GridField3D *result;
    // bad-or-missing-data fill value
    real badval1;
    // temporary variable for holding inputs
    real value1;
    // vertical coordinate value
    real zval;
    // iterator over input gridpoint profiles
    GridField3D::const_profileIterator inProf1;
    // iterator over output gridpoint profiles
    GridField3D::profileIterator outProf;
    // vector of vertical profile data extracted from the input grids
    std::vector<real> *profile1;
    // vector of vertical profile data for the output grid
    std::vector<real> *oprofile;
    // a vector of vertical coordinate values from the input grid
    std::vector<real> vert;
    // temporary variables for intermediate results
    real omega, dzdp, alt, dzdt;
    /* identifies which combination of input physical quantity and
       vertical coordinate quantity we are using for the calculation
    */
    int type;
    // vertical distance coord name, as obained from the input vert coord
    std::string vq;
    // vertical distance coord MKS factors
    real mksScale, mksOffset;
   

    // the input field must be the right physical quantity
    if ( input1.quantity() == omega_name && input1.vertical() == press_name ) {
       // omega on pressure surfaces
       type = 1;
       vq = "m";
       mksScale = 1.0;
       mksOffset = 0.0;
    } else if ( input1.quantity() == omega_name && input1.vertical() == palt_name  ) {
       // omega on pressure altitude surfaces
       type = 2;
       // grab the units from the input vert coord
       vq =  input1.vunits();
       mksScale = input1.mksVScale;
       mksOffset = input1.mksVOffset;
    } else {
       throw (badinputquant());
    }   

    
    // use the first input quantity as a base for the result
    result = input1.duplicate();
    // set the metadata
    result->set_quantity(quant);
    result->set_units( vq + "/s", mksScale, mksOffset); 

    badval1 = input1.fillval();
       
    vert = input1.levels();

    // iterate over all horizontal gridpoints
    for ( inProf1 = input1.profileBegin(), outProf = result->profileBegin();
          inProf1 != input1.profileEnd();
          inProf1++, outProf++ ) {
    
          // get the data profiles
          profile1 = *inProf1;
          oprofile = *outProf;
          
          // for each pressure value in this profile...
          for ( int k=0; k<profile1->size(); k++ ) {
              
          
              value1 = (*profile1)[k];
              alt = vert[k]*input1.mksVScale + input1.mksVOffset;
              if ( type == 1 ) {
                 // alt is pressure, so convert it to pressure altitude in kilometers
                 alt = palt.calc( alt );
              }
              // interpolate the dp/dz derivative to this altitude, in m/s
              dzdp = get_dzdp( alt )*1000.0;
              if ( value1 != badval1 ) {
                 
                 // get the dp/dt value, in MKS units
                 omega = value1*input1.mksScale + input1.mksOffset;
                 
                 dzdt = omega * dzdp;                 
                 
                 // convert to desried coords
                 (*oprofile)[k] = dzdt/mksScale - mksOffset/mksScale;

              } else {
                 (*oprofile)[k] = badval1;
              }

          }
          outProf.assign(*oprofile); 
          
          delete oprofile;
          delete profile1;
          
    }      
    
    if ( flags & OTF_MKS ) {
       result->transform("m/s");
    }
    
    return result;   

}

GridField3D* PAltDotOTF::calc( const GridField3D& input1, const GridField3D& input2, int flags) const
{
    // the output temperature tendency field
    GridField3D *result;
    // bad-or-missing-data fill values
    real badval1, badval2;
    // temporary variables for holding inputs
    real value1, value2;
    // vertical coordinate value
    real zval;
    // iterator over input gridpoint profiles
    GridField3D::const_profileIterator inProf1, inProf2;
    // iterator over output gridpoint profiles
    GridField3D::profileIterator outProf;
    // vectors of vertical profile data extracted from the input grids
    std::vector<real> *profile1, *profile2;
    // vector of vertical profile data for the output grid
    std::vector<real> *oprofile;
    // a vector of vertical coordinate values from the input grid
    std::vector<real> vert;
    // temporary variables for intermediate results
    real omega, dzdp, alt, dzdt;
    /* identifies which combination of input physical quantity and
       vertical coordinate quantity we are using for the calculation
    */
    int type;
    // index
    int idx;
    // vertical distance coord name, as obained from the input vert coord
    std::string vq;
    // vertical distance coord MKS factors
    real mksScale, mksOffset;
   
   
    // the two input fields must be grid-compatible
    if ( ! input1.compatible(input2) ) {
       throw (badgrid());
    }


    // the input fields must be the right physical quantities
    if ( input1.quantity() == omega_name && input2.quantity() == press_name ) {
       // omega and pressure 
       type = 1;
       vq = "m";
       mksScale = 1.0;
       mksOffset = 0.0;
    } else if ( input1.quantity() == omega_name && input2.quantity() == palt_name  ) {
       // omega and pressure altitude 
       type = 2;
       vq =  input2.units();
       mksScale = input2.mksScale;
       mksOffset = input2.mksOffset;
    } else {
       throw (badinputquant());
    }   

    
    // use the first input quantity as a base for the result
    result = input1.duplicate();
    // set the metadata
    result->set_quantity(quant);
    result->set_units( vq + "/s", mksScale, mksOffset); 

    badval1 = input1.fillval();
    badval2 = input2.fillval();
       
    vert = input1.levels();

    // iterate over all horizontal gridpoints
    for ( idx = 0, inProf1 = input1.profileBegin(), inProf2 = input2.profileBegin(), outProf = result->profileBegin();
          inProf1 != input1.profileEnd();
          inProf1++, inProf2++, outProf++, idx++ ) {
    
          // get the data profiles
          profile1 = *inProf1;
          profile2 = *inProf2;
          oprofile = *outProf;
          
          // for each pressure value in this profile...
          for ( int k=0; k<profile1->size(); k++ ) {
              
              value1 = (*profile1)[k];

              value2 = (*profile2)[k];
          
              if ( value1 != badval1 && value2 != badval2 ) {
                 
                 omega = value1*input1.mksScale + input1.mksOffset;

                 alt = value2*input2.mksScale + input2.mksOffset;

                 if ( type == 1 ) {
                    // alt is pressure, so convert it to meters
                    alt = palt.calc( alt )*1000.0;
                 }
                 // interpolate the dp/dz derivative to this altitude
                 dzdp = get_dzdp( alt/1000.0 )*1000.0;
              
                 dzdt = omega * dzdp;                 
                 
                 // convert to desried coords
                 (*oprofile)[k] = dzdt/mksScale - mksOffset/mksScale;

              } else {
                 (*oprofile)[k] = badval1;
              }

          }
          outProf.assign(*oprofile); 
          
          delete oprofile;
          delete profile1;
          delete profile2;
          
    }      
    
    if ( flags & OTF_MKS ) {
       result->transform("m/s");
    }
 

    return result;   
}


GridFieldSfc* PAltDotOTF::calc( const GridFieldSfc& input1, const GridFieldSfc& input2, int flags) const
{
    // the output temperature tendency field
    GridFieldSfc *result;
    // bad-or-missing-data fill values
    real badval1, badval2;
    // temporary variables for holding inputs
    real value1, value2;
    // vertical coordinate value
    real zval;
    // iterator over input gridpoint profiles
    GridFieldSfc::const_iterator inPnt1, inPnt2;
    // iterator over output gridpoint profiles
    GridFieldSfc::iterator outPnt;
    // temporary variables for intermediate results
    real omega, dzdp, alt, dzdt;
    /* identifies which combination of input physical quantity and
       second coordinate quantity we are using for the calculation
    */
    int type;
    // index
    int idx;
    // vertical distance coord name, as obained from the input vert coord
    std::string vq;
    // vertical distance coord MKS factors
    real mksScale, mksOffset;
   
   
    // the two input fields must be grid-compatible
    if ( ! input1.compatible(input2) ) {
       throw (badgrid());
    }


    // the input fields must be the right physical quantities
    if ( input1.quantity() == omega_name && input2.quantity() == press_name ) {
       // omega and pressure 
       type = 1;
       vq = "m";
       mksScale = 1.0;
       mksOffset = 0.0;
    } else if ( input1.quantity() == omega_name && input2.quantity() == palt_name  ) {
       // omega and pressure altitude 
       type = 2;
       vq =  input2.units();
       mksScale = input2.mksScale;
       mksOffset = input2.mksOffset;
    } else {
       throw (badinputquant());
    }   

    
    // use the first input quantity as a base for the result
    result = input1.duplicate();
    // set the metadata
    result->set_quantity(quant);
    result->set_units( vq + "/s", mksScale, mksOffset); 

    badval1 = input1.fillval();
    badval2 = input2.fillval();
       
    // iterate over all horizontal gridpoints
    for ( idx = 0, inPnt1 = input1.begin(), inPnt2 = input2.begin(), outPnt = result->begin();
          inPnt1 != input1.end();
          inPnt1++, inPnt2++, outPnt++, idx++ ) {
    
          // get the data pointd
          value1 = *inPnt1;
          value2 = *inPnt2;
          
          
          if ( value1 != badval1 && value2 != badval2 ) {
             
             omega = value1*input1.mksScale + input1.mksOffset;

             alt = value2*input2.mksScale + input2.mksOffset;

             if ( type == 1 ) {
                // alt is pressure, so convert it to meters
                alt = palt.calc( alt )*1000.0;
             }
             // interpolate the dp/dz derivative to this altitude
             dzdp = get_dzdp( alt/1000.0 )*1000.0;
          
             dzdt = omega * dzdp;                 
             
             // convert to desried coords
             *outPnt =  dzdt/mksScale - mksOffset/mksScale;

          } else {
             *outPnt = badval1;
          }
          
    }      
    
    if ( flags & OTF_MKS ) {
       result->transform("m/s");
    }
 

    return result;   
}

