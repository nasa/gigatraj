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

#include "gigatraj/PAltOTF.hh"

using namespace gigatraj;

static const real stdZdata[] = {  0.00, 1.00, 2.00, 3.00, 4.00, 5.00, 6.00, 7.00, 8.00,
      9.00, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0,
      21.0, 22.0, 23.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0, 32.0,
      33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0, 40.0, 41.0, 42.0, 43.0, 44.0,
      45.0, 46.0, 47.0, 48.0, 49.0, 50.0, 51.0, 52.0, 53.0, 54.0, 55.0, 56.0,
      57.0, 58.0, 59.0, 60.0, 61.0, 62.0, 63.0, 64.0, 65.0, 66.0, 67.0, 68.0,
      69.0};

static const real stdLogPdata[] = {  11.5261, 11.4062, 11.2835, 11.1578, 11.0291,
    10.8971, 10.7617, 10.6228, 10.4801, 10.3334, 10.1825, 10.0271, 9.86943, 9.71175,
    9.55406, 9.39637, 9.23868, 9.08099, 8.92330, 8.76562, 8.60793, 8.45060, 8.29400,
    8.13811, 7.98292, 7.82845, 7.67466, 7.52157, 7.36915, 7.21742, 7.06635, 6.91595,
    6.76621, 6.61771, 6.47099, 6.32601, 6.18274, 6.04113, 5.90114, 5.76274, 5.62590,
    5.49057, 5.35673, 5.22434, 5.09337, 4.96379, 4.83557, 4.70869, 4.58246, 4.45623,
    4.33001, 4.20378, 4.07690, 3.94868, 3.81910, 3.68813, 3.55574, 3.42190, 3.28657,
    3.14972, 3.01132, 2.87134, 2.72973, 2.58645, 2.44148, 2.29476, 2.14625, 1.99592,
    1.84371, 1.68958 };


const int PAltOTF::stdN = 70;
const real *PAltOTF::stdZ = stdZdata; 
const real *PAltOTF::stdLogP = stdLogPdata; 

PAltOTF::PAltOTF()
{
    quant = "pressure_altitude";
    uu = "km";
    press_name = "air_pressure";
     
}

PAltOTF::PAltOTF(const std::string& palt, const std::string& press)
{
     quant = palt;
     uu = "km";
     press_name = press;
     
}

PAltOTF::~PAltOTF() 
{
}

PAltOTF::PAltOTF(const PAltOTF& src) : MetOnTheFly(src)
{
    press_name = src.press_name;
}

PAltOTF& PAltOTF::operator=(const PAltOTF& src)
{
    this->assign( src ) ;
    
    return *this;
}

void PAltOTF::assign( const PAltOTF& src)
{
    MetOnTheFly::assign( src );
    
    press_name = src.press_name;
}

real PAltOTF::calc( real p ) const
{
    real result;
    real logp;
    
    if ( p > 0 ) {
       logp = LOG(p);
       
       for (int i=1; i < stdN; i++ ) {
           if ( (stdLogP[i-1] <= logp && logp <= stdLogP[i]) || (stdLogP[i-1] >= logp && logp >= stdLogP[i]) ) {
              result = (stdZ[i] - stdZ[i-1])/(stdLogP[i] - stdLogP[i-1])*(logp -stdLogP[i-1]) + stdZ[i-1];
              return result;
           }
       }
       throw (badcalculation());
       
    } else {
       throw (badcalculation());
    } 
    
    return result;
}

real PAltOTF::clac( real z ) const
{
    real result;
    real logp;
    
    
    if ( z > -1 && z < 100.0 ) {
       
       for (int i=1; i < stdN; i++ ) {
           if ( (stdZ[i-1] <= z && z <= stdZ[i]) || (stdZ[i-1] >= z && z >= stdZ[i]) ) {
              logp = (stdLogP[i] - stdLogP[i-1])/(stdZ[i] - stdZ[i-1])*(z -stdZ[i-1]) + stdLogP[i-1];
              result = EXP(logp);
              return result;
           }
       }
       throw (badcalculation());
       
    } else {
       throw (badcalculation());
    } 
    
    return result;
}

GridField3D* PAltOTF::calc( const GridField3D& input, int flags) const
{
    // the output Palt field
    GridField3D *result;
    // bad-or-missing-data fill value
    real badval;
    // temporary variable for holding results
    real value;
    // vertical coordinate value
    real zval;
    // iterator over input gridpoint profiles
    GridField3D::const_profileIterator inProf;
    // iterator over output gridpoint profiles
    GridField3D::profileIterator outProf;
    // a vector of vertical profile data extracted from the input grid
    std::vector<real> *profile;
    // a vector of vertical coordinate values from the input grid
    std::vector<real> vert;
   
    if ( input.quantity() == quant ) {
       // input quantity is pressure altitude
       
       // duplicate the input field, to create the needed output object
       // (Note that this also takes care of any metadata settings.)
       result = input.duplicate();
       
    } else if ( input.vertical() == quant ) {
       // input quantity's vertical coordinate is pressure altitude
       
       // duplicate the input field, to create the needed output object
       // Use the vertical coordinate values as data values.
       // (Note that this also takes care of any metadata settings.)
       result = input.generateVertical();
       
    } else if ( input.vertical() == press_name ) {
       // input quantity is on pressure aurfaces
       
       
       // duplicate the input field, to create the needed output object
       result = input.duplicate();
       // and set the metadata
       result->set_quantity(quant);
       result->set_units("km", 1000.0);  // the 1000 takes us from km to m(MKS)
       badval = input.fillval();

       // get the vertical (pressure) coordinates
       vert = input.levels();
       for ( int k=0; k<vert.size(); k++ ) {
           // and transform pressure to MKS units,
           // then convert to pressure altitude
           if ( vert[k] != badval ) {
              vert[k] = calc( vert[k] * input.mksVScale + input.mksVOffset );
           }
       }
       
       // iterate over each profile on the horizontal grid
       for ( inProf = input.profileBegin(), outProf = result->profileBegin();
             inProf != input.profileEnd();
             inProf++, outProf++ ) {
       
             // get the data profile
             profile = *inProf;
             
             // load the altitude values for this profile
             for ( int k=0; k<profile->size(); k++ ) {                    
                 (*profile)[k] = vert[k];
             }
             outProf.assign(*profile); 

             delete profile;
 
       }      
    
    } else if ( input.quantity() == press_name ) {
       // input quantity is pressure on who-cares surfaces
       
       // duplicate the input field, to create the needed output object
       result = input.duplicate();
       // and set the metadata
       result->set_quantity(quant);
       result->set_units("km", 1000.0);  // the 1000 takes us from km to m(MKS)
       badval = input.fillval();

       // iterate over each profile on the horizontal grid
       int ij = 0;
       for ( inProf = input.profileBegin(), outProf = result->profileBegin();
             inProf != input.profileEnd();
             inProf++, outProf++ ) {
       
             // get the data profile
             profile = *inProf;
             
             // calculate the pressure values for this profile
             for ( int k=0; k<profile->size(); k++ ) {
                 value = (*profile)[k];
                 if ( value != badval ) {
                    if ( value <= 0.0 ) {
                       std::cerr << "Bad pressure " << value 
                                 << " at " << ij << " , " << k 
                                 << " w/ bad=" << badval 
                                 << std::endl;
                    }                 
                    // transform to MKS units
                    value = value * input.mksScale + input.mksOffset;
                    // convert Pa to km
                    (*profile)[k] = calc( value );
                 }
             }
             outProf.assign(*profile); 

             delete profile;

             ij++;
       }      
       
    } else {
       // unusable input quantity
       throw (badinputquant());
    }
    
    if ( flags & OTF_MKS ) {
       result->transform("m");
    }
    
    return result;   

}



GridFieldSfc* PAltOTF::calc( const GridFieldSfc& input, int flags) const
{
    // the output Palt field
    GridFieldSfc *result;
    // bad-or-missing-data fill value
    real badval;
    // temporary variable for holding results
    real value;
    // vertical coordinate value
    real zval;
    // iterator over input gridpoints
    GridFieldSfc::const_iterator inPnt;
    // iterator over output gridpoints
    GridFieldSfc::iterator outPnt;

   
    if ( input.quantity() == quant ) {
       // input quantity is pressure altitude
       
       // duplicate the input field, to create the needed output object
       // (Note tha this also takes care of any metadata settings.)
       result = input.duplicate();
       
    } else if ( input.quantity() == press_name ) {
       // input quantity is pressure 
       
       // duplicate the input field, to create the needed output object
       result = input.duplicate();
       // and set the metadata
       result->set_quantity(quant);
       result->set_units("km", 1000.0);  // the 1000 takes us from km to m(MKS)
       badval = input.fillval();
       
       // iterate over each profile on the horizontal grid
       for ( inPnt = input.begin(), outPnt = result->begin();
             inPnt != input.end();
             inPnt++, outPnt++ ) {
       
             value = *inPnt;
             if ( value != badval ) {
                // transform to MKS units
                value = value * input.mksScale + input.mksOffset;
                    
             }
             *outPnt = calc( value );

       }      
       
    } else {
       // unusable input quantity
       throw (badinputquant());
    }

    if ( flags & OTF_MKS ) {
       result->transform("m");
    }    
    
    return result;   

}

GridField3D* PAltOTF::clac( const GridField3D& input, int flags) const
{
    // the output P field
    GridField3D *result;
    // bad-or-missing-data fill value
    real badval;
    // temporary variable for holding results
    real value;
    // vertical coordinate value
    real zval;
    // iterator over input gridpoint profiles
    GridField3D::const_profileIterator inProf;
    // iterator over output gridpoint profiles
    GridField3D::profileIterator outProf;
    // a vector of vertical profile data extracted from the input grid
    std::vector<real> *profile;
    // a vector of vertical coordinate values from the input grid
    std::vector<real> vert;
   
    if ( input.quantity() == press_name ) {
       // input quantity is pressure 
       
       // duplicate the input field, to create the needed output object
       // (Note that this also takes care of any metadata settings.)
       result = input.duplicate();
       
    } else if ( input.vertical() == press_name ) {
       // input quantity's vertical coordinate is pressure 
       
       // duplicate the input field, to create the needed output object
       // Use the vertical coordinate values as data values.
       // (Note that this also takes care of any metadata settings.)
       result = input.generateVertical();
       
    } else if ( input.vertical() == quant ) {
       // input quantity is on pressure altitude aurfaces
       
       
       // duplicate the input field, to create the needed output object
       result = input.duplicate();
       // and set the metadata
       result->set_quantity(press_name);
       result->set_units("hPa", 100.0);  // the 100 takes us from hPa to Pa
       badval = input.fillval();

       // get the vertical (pressure altitude) coordinates
       vert = input.levels();
       for ( int k=0; k<vert.size(); k++ ) {
           // and transform pressure altitude to MKS units, then km, 
           // then convert to pressure in hPa 
           if ( vert[k] != badval ) {
              vert[k] = calc( (vert[k] * input.mksVScale + input.mksVOffset)/1000.0 )/100.0;
           }
       }
       
       // iterate over each profile on the horizontal grid
       for ( inProf = input.profileBegin(), outProf = result->profileBegin();
             inProf != input.profileEnd();
             inProf++, outProf++ ) {
       
             // get the data profile
             profile = *inProf;
             
             // load the altitude values for this profile
             for ( int k=0; k<profile->size(); k++ ) {                    
                 (*profile)[k] = vert[k];
             }
             outProf.assign(*profile); 

             delete profile;
 
       }      
    
    } else if ( input.quantity() == quant ) {
       // input quantity is pressure altitude on who-cares surfaces
       
       // duplicate the input field, to create the needed output object
       result = input.duplicate();
       // and set the metadata
       result->set_quantity(quant);
       result->set_units("hPa", 100.0);  // the 100 takes us from hPa to Pa(MKS)
       badval = input.fillval();

       // iterate over each profile on the horizontal grid
       int ij = 0;
       for ( inProf = input.profileBegin(), outProf = result->profileBegin();
             inProf != input.profileEnd();
             inProf++, outProf++ ) {
       
             // get the data profile
             profile = *inProf;
             
             // calculate the pressure values for this profile
             for ( int k=0; k<profile->size(); k++ ) {
                 value = (*profile)[k];
                 if ( value != badval ) {
                    // transform to MKS units, then to km
                    value = (value * input.mksScale + input.mksOffset)/1000.0;
                    // convert km to Pa, then to hPa
                    (*profile)[k] = calc( value )/100.0;
                 }
             }
             outProf.assign(*profile); 

             delete profile;

             ij++;
       }      
       
    } else {
       // unusable input quantity
       throw (badinputquant());
    }
    
    if ( flags & OTF_MKS ) {
       result->transform("Pa");
    }
    
    return result;   

}

GridFieldSfc* PAltOTF::clac( const GridFieldSfc& input, int flags) const
{
    // the output P field
    GridFieldSfc *result;
    // bad-or-missing-data fill value
    real badval;
    // temporary variable for holding results
    real value;
    // vertical coordinate value
    real zval;
    // iterator over input gridpoints
    GridFieldSfc::const_iterator inPnt;
    // iterator over output gridpoints
    GridFieldSfc::iterator outPnt;

   
    if ( input.quantity() == press_name ) {
       // input quantity is pressure 
       
       // duplicate the input field, to create the needed output object
       // (Note tha this also takes care of any metadata settings.)
       result = input.duplicate();
       
    } else if ( input.quantity() == quant ) {
       // input quantity is pressure altitude
       
       // duplicate the input field, to create the needed output object
       result = input.duplicate();
       // and set the metadata
       result->set_quantity(quant);
       result->set_units("hPa", 100.0);  // the 100 takes us from hPa to Pa(MKS)
       badval = input.fillval();
       
       // iterate over each profile on the horizontal grid
       for ( inPnt = input.begin(), outPnt = result->begin();
             inPnt != input.end();
             inPnt++, outPnt++ ) {
       
             value = *inPnt;
             if ( value != badval ) {
                // transform to MKS units, then to km
                value = ( value * input.mksScale + input.mksOffset)/1000.0;
                    
             }
             // convert to pressure in PA, then convert that to hPa
             *outPnt = clac( value )/100;

       }      
       
    } else {
       // unusable input quantity
       throw (badinputquant());
    }

    if ( flags & OTF_MKS ) {
       result->transform("m");
    }    
    
    return result;   

}
