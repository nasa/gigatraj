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

#include "gigatraj/PressOTF.hh"

using namespace gigatraj;

static const real stdZdata[] = {  0000.00, 1000.00, 2000.00, 3000.00, 4000.00,
      5000.00, 6000.00, 7000.00, 8000.00, 9000.00, 10000.0, 11000.0, 12000.0,
      13000.0, 14000.0, 15000.0, 16000.0, 17000.0, 18000.0, 19000.0, 20000.0,
      21000.0, 22000.0, 23000.0, 24000.0, 25000.0, 26000.0, 27000.0, 28000.0,
      29000.0, 30000.0, 31000.0, 32000.0, 33000.0, 34000.0, 35000.0, 36000.0,
      37000.0, 38000.0, 39000.0, 40000.0, 41000.0, 42000.0, 43000.0, 44000.0,
      45000.0, 46000.0, 47000.0, 48000.0, 49000.0, 50000.0, 51000.0, 52000.0,
      53000.0, 54000.0, 55000.0, 56000.0, 57000.0, 58000.0, 59000.0, 60000.0,
      61000.0, 62000.0, 63000.0, 64000.0, 65000.0, 66000.0, 67000.0, 68000.0,
      69000.0};

static const real stdLogPdata[] = { 6.92092, 6.80100, 6.67828, 6.55263, 6.42390,
   6.29194, 6.15658, 6.01764, 5.87493, 5.72823, 5.57732, 5.42195, 5.26426,
   5.10658, 4.94889, 4.79120, 4.63351, 4.47582, 4.31813, 4.16045, 4.00276,
   3.84543, 3.68883, 3.53294, 3.37775, 3.22328, 3.06949, 2.91640, 2.76398,
   2.61225, 2.46118, 2.31078, 2.16104, 2.01254, 1.86582, 1.72084, 1.57757,
   1.43596, 1.29597, 1.15757, 1.02073, 0.885401, 0.751557, 0.619167, 0.488196,
   0.358618, 0.230399, 0.103517, -0.0227100, -0.148936, -0.275163, -0.401389,
   -0.528273, -0.656490, -0.786069, -0.917040, -1.04943, -1.18327, -1.31860,
   -1.45545, -1.59385, -1.73383, -1.87544, -2.01872, -2.16369, -2.31041,
   -2.45892, -2.60925, -2.76146, -2.91559 };

const int PressOTF::stdN = 70;
const real *PressOTF::stdZ = stdZdata; 
const real *PressOTF::stdLogP = stdLogPdata; 


PressOTF::PressOTF() 
{
    quant = "air_pressure";
    uu = "hPa";
    temp_name = "air_temperature";
    theta_name = "air_potential_temperature";
    thick_name = "air_layer_pressure_thickness";
    dens_name = "air_density";
    alt_name = "pressure_altitude";
}

PressOTF::PressOTF(const std::string& press, const std::string& temp, const std::string& theta, const std::string& thick, const std::string& dens, const std::string& alt)
{
     quant = press;
     uu = "hPa";
     temp_name = temp;
     theta_name = theta;
     thick_name = thick;
     dens_name = dens;
     alt_name = alt;
}

PressOTF::~PressOTF() 
{
}

PressOTF::PressOTF(const PressOTF& src) : MetOnTheFly(src)
{
     temp_name = src.temp_name;
     theta_name = src.theta_name;
     thick_name = src.thick_name;
     dens_name = src.dens_name;
     alt_name = src.alt_name;
}

PressOTF& PressOTF::operator=(const PressOTF& src)
{
    this->assign( src ) ;
    
    return *this;
}

void PressOTF::assign( const PressOTF& src)
{
     MetOnTheFly::assign( src );

     temp_name = src.temp_name;
     theta_name = src.theta_name;
     thick_name = src.thick_name;
     dens_name = src.dens_name;
     alt_name = src.alt_name;
}

real PressOTF::calp( real alt ) const
{
    real result;
    real logp;
    
    if ( alt > 0 ) {
       
       for (int i=1; i < stdN; i++ ) {
           if ( stdZ[i-1] <= alt && alt <= stdZ[i] ) {
              result = (stdLogP[i] - stdLogP[i-1])/(stdZ[i] - stdZ[i-1])*(alt -stdZ[i-1]) + stdLogP[i-1];
              result = EXP(result);
              return result;
           }
       }
       throw (badcalculation());
       
    } else {
       throw (badcalculation());
    } 
    
    return result;

}

GridField3D* PressOTF::calc( const GridField3D& input, const real start, int flags) const
{
    // the output density field
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
    // vertical profile of input data 
    std::vector<real> *inData;
    // vertical profile of output data
    std::vector<real> *outData;
    // a vector of vertical coordinate values from the input grid
    std::vector<real> vert;
    // the length of the vert vector
    int nz;
    // direction in which the vertical coordinate increases wrt index
    int dir;
   
    if ( input.vertical() == quant ) {
       // input quantity is irrelevant, as its vertical coordinate is pressure
       
       // duplicate the input field, to create the needed output object
       // Use the vertical coordinate values as data values.
       // (Note that his also takes care of any metadata settings.)
       result = input.generateVertical();
       
    } else if ( input.quantity() == temp_name && input.vertical() == theta_name ) {
       // input quantity is temperature on theta surfaces 
       
       // duplicate the input field, to create the needed output object
       result = input.duplicate();
       // and set the metadata
       result->set_quantity(quant);
       result->set_units("hPa", 100.0);  // the 100 takes us from HPa to Pa(MKS)
       badval = input.fillval();
       
       // get the vertical (theta) coordinates
       vert = input.levels();
       // and transform them to MKS units.
       for ( int k=0; k<vert.size(); k++ ) {
           vert[k] = vert[k] * input.mksVScale + input.mksVOffset;
       }   
       
       // iterate over each profile on the horizontal grid
       for ( inProf = input.profileBegin(), outProf = result->profileBegin();
             inProf != input.profileEnd();
             inProf++, outProf++ ) {
       
             // get the data profile
             profile = *inProf;
             
             // calculate the pressure values for this profile
             for ( int k=0; k<profile->size(); k++ ) {
                 value = (*profile)[k];
                 if ( value != badval ) {
                    // transform to MKS units
                    value = value * input.mksScale + input.mksOffset;
                    
                    (*profile)[k] = 1000.0 * POW( zval / value, -7./2. );
                 }
             }
             outProf.assign(*profile); 

             delete profile;

       } 
       
    } else if ( input.vertical() == alt_name ) {
       // input quantity is on altitude aurfaces
       
       
       // duplicate the input field, to create the needed output object
       result = input.duplicate();
       // and set the metadata
       result->set_quantity(quant);
       result->set_units("hPa", 100.0);  // the 100 takes us from hPa to Pa(MKS)
       badval = input.fillval();

       // get the vertical (altitude) coordinates
       vert = input.levels();
       for ( int k=0; k<vert.size(); k++ ) {
           // and transform pressure altitude to MKS units,
           // then convert to pressure
           vert[k] = calp( vert[k] * input.mksVScale + input.mksVOffset );
       }
       
       // iterate over each profile on the horizontal grid
       for ( inProf = input.profileBegin(), outProf = result->profileBegin();
             inProf != input.profileEnd();
             inProf++, outProf++ ) {
       
             // get the data profile
             profile = *inProf;
             
             // load the pressure values for this profile
             for ( int k=0; k<profile->size(); k++ ) {                    
                 (*profile)[k] = vert[k];
             }
             outProf.assign(*profile); 

             delete profile;

       }      
    
    } else if ( input.quantity() == alt_name ) {
       // input quantity is pressure altitude on who-cares surfaces
       
       // duplicate the input field, to create the needed output object
       result = input.duplicate();
       // and set the metadata
       result->set_quantity(quant);
       result->set_units("hPa", 100.0);  // the 100 takes us from hPa to Pa(MKS)
       badval = input.fillval();
       
       // iterate over each profile on the horizontal grid
       for ( inProf = input.profileBegin(), outProf = result->profileBegin();
             inProf != input.profileEnd();
             inProf++, outProf++ ) {
       
             // get the data profile
             profile = *inProf;
             
             // calculate the pressure values for this profile
             for ( int k=0; k<profile->size(); k++ ) {
                 value = (*profile)[k];
                 if ( value != badval ) {
                    // transform to MKS units
                    value = value * input.mksScale + input.mksOffset;
                    
                    (*profile)[k] = calp( value );
                 }
             }
             outProf.assign(*profile); 

             delete profile;

       }      
       
    } else if ( input.quantity() == thick_name ) {
       // input quantity is pressure thicknesses on arbitrary surfaces
       
       
       // duplicate the input field, to create the needed output object
       result = input.duplicate();
       // and set the metadata
       result->set_quantity(quant);
       result->set_units(input.units());  // superfluous, but illustrative
       badval = input.fillval();

       // get the vertical coordinate
       // and its size
       // (note: these are used only to get the direction)
       vert = input.levels();
       nz = vert.size();
       
       // the input field must have a number of levels
       if (nz < 3) {
          throw (badinputquant());
       }   
       // determine the direction
       if ( (vert[1] - vert[0]) > (vert[nz-1] - vert[nz-2]) ) {
          dir = -1;
       } else {
          dir = 1;
       }
             
       // iterate over all horizontal gridpoints
       for ( inProf = input.profileBegin(), outProf = result->profileBegin() ;
             inProf != input.profileEnd();
             inProf++, outProf++ ) {
             
           // get the vertical profile of data
           inData = *inProf;  
           
           // get the output vertical profile
           outData = *outProf;

           if ( dir > 0 ) {
              // greater pressures (lower altitudes) are first in the array
          
              // start at the uppermost index...
              (*outData)[nz-1] = start + (*inData)[nz-1]/2.0;
              // ...and work our way downward
              for ( int k=nz-2; k>=0; k-- ) {
                  if ( (*outData)[k+1] != badval && (*inData)[k] != badval ) {
                     (*outData)[k] = (*outData)[k+1] + ((*inData)[k+1] + (*inData)[k])/2.0;
                  } else {
                     (*outData)[k] = badval;
                  }      
              }
           } else {
              // greater pressures (lower altitudes) are last in the array

              // start at the lowermost index...
              (*outData)[0] = start + (*inData)[0]/2.0;
              // ... and work our way up
              for ( int k=1; k<nz; k++ ) {
                  if ( (*outData)[k-1] != badval && (*inData)[k] != badval ) {
                     (*outData)[k] = (*outData)[k-1] + ((*inData)[k-1] + (*inData)[k])/2.0;
                  } else {
                     (*outData)[k] = badval;
                  }      
              }
           }
           
           // load this profile into the output
           outProf.assign(*outData);
           
           delete outData;
           delete inData;
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


GridField3D* PressOTF::calc( const GridField3D& input1, const GridField3D& input2, int flags ) const
{
    // the output density field
    GridField3D *result;
    // bad-or-missing-data fill values
    real badval1;
    real badval2;
    // iterator over first input gridpoints
    GridField3D::const_iterator inPnt1;
    // iterator over second input gridpoints
    GridField3D::const_iterator inPnt2;
    // iterator over output gridpoint profiles
    GridField3D::iterator outPnt;
    // a vector of vertical coordinate values from the input grid
    std::vector<real> vert;
    // pointers to the first and second inputs, providing a layer of
    // indirection that permits us to be lenient about the order
    // in which the input fields are given.
    const GridField3D* in1;
    const GridField3D* in2;
    /* identifies which combination of input physical quantity and
       vertical coordinate quantity we are using for the calculation
    */
    int type;
    // temporary variables for holding results
    real val1, val2;

    // the two input fields must be grid-compatible
    if ( ! input1.compatible(input2) ) {
       throw (badgrid());
    }

    /* which two quantities are we using for inputs?
       Note that because the calling routine may give us the two input fields
       in either order, we use pointers here to establish a fixed
       order.
    */
    if ( input1.quantity() == temp_name && input2.quantity() == theta_name ) {
       // use  theta = T * (p0/p)^(2./7.)
       //   in1 = T, in2 = theta
       type = 1;
       in1 = &input1;
       in2 = &input2;
    } else if ( input1.quantity() == theta_name && input2.quantity() == temp_name ) {
       // use  theta = T * (p0/p)^(2./7.)
       //   in1 = T, in2 = theta
       type = 1;
       in1 = &input2;
       in2 = &input1;
    } else if ( input1.quantity() == temp_name && input2.quantity() == dens_name ) {
       // use  p = rho * R * T
       //   in1 = T, in2 = rho
       type = 2;
       in1 = &input1;
       in2 = &input2;
    } else if ( input1.quantity() == dens_name && input2.quantity() == temp_name ) {
       // use  p = rho * R * T
       //   in1 = T, in2 = rho
       type = 2;
       in1 = &input2;
       in2 = &input1;
    } else {
       throw (badinputquant());
    }   

    // use the first input quantity as a base for the result
    result = in1->duplicate();
    // set the metadata
    result->set_quantity(quant);
    result->set_units("hPa", 100.0); // the 100 takes us from hPa(mb) to Pa

    badval1 = in1->fillval();
    badval2 = in2->fillval();

    // iterate over all horizontal gridpoints
    for ( inPnt1 = in1->begin(), inPnt2 = in2->begin(), outPnt = result->begin() ;
          inPnt1 != in1->end();
          inPnt1++, inPnt2++, outPnt++ ) {
          
         val1 = *inPnt1;                                                                         
         val2 = *inPnt2;                                                                         
        
         if ( val1 != badval1 && val2 != badval2 ) {                                              
                                                                                                  
            // transform to MKS units                                                             
            val1 = val1 * in1->mksScale + in1->mksOffset;                                         
            val2 = val2 * in2->mksScale + in2->mksOffset;                                         
                                                                                                  
            switch (type) {                                                                       
            case 1:  // p = p0 * (T/theta)^(-7./2.)                                               
               *outPnt = 1000.0 * POW(val2/val1, -7./2.);                                   
                     break;                                                                       
            case 2:  // p = rho * R * T                                                           
               *outPnt = val2 * 287.04 * val1 / 100.0;  // the 100 is to get to mb units)   
                     break;                                                                       
            }                                                                                     
         } else {                                                                                 
            *outPnt = badval1;                                                              
         }                                                                                        
        
    }
       
    if ( flags & OTF_MKS ) {
        result->transform("Pa");
    }     
       
    
    return result;   


}



GridFieldSfc* PressOTF::calc( const GridFieldSfc& input1, const GridFieldSfc& input2, int flags ) const
{
    // the output density field
    GridFieldSfc *result;
    // bad-or-missing-data fill values
    real badval1;
    real badval2;
    // iterator over first input gridpoints
    GridFieldSfc::const_iterator inPnt1;
    // iterator over second input gridpoints
    GridFieldSfc::const_iterator inPnt2;
    // iterator over output gridpoint profiles
    GridFieldSfc::iterator outPnt;
    // pointers to the first and second inputs, providing a layer of
    // indirection that permits us to be lenient about the order
    // in which the input fields are given.
    const GridFieldSfc* in1;
    const GridFieldSfc* in2;
    /* identifies which combination of input physical quantity and
       vertical coordinate quantity we are using for the calculation
    */
    int type;
    // temporary variables for holding results
    real val1, val2;

    // the two input fields must be grid-compatible
    if ( ! input1.compatible(input2) ) {
       throw (badgrid());
    }

    /* which two quantities are we using for inputs?
       Note that because the calling routine may give us the two input fields
       in either order, we use pointers here to establish a fixed
       order.
    */
    if ( input1.quantity() == temp_name && input2.quantity() == theta_name ) {
       // use  theta = T * (p0/p)^(2./7.)
       //   in1 = T, in2 = theta
       type = 1;
       in1 = &input1;
       in2 = &input2;
    } else if ( input1.quantity() == theta_name && input2.quantity() == temp_name ) {
       // use  theta = T * (p0/p)^(2./7.)
       //   in1 = T, in2 = theta
       type = 1;
       in1 = &input2;
       in2 = &input1;
    } else if ( input1.quantity() == temp_name && input2.quantity() == dens_name ) {
       // use  p = rho * R * T
       //   in1 = T, in2 = rho
       type = 2;
       in1 = &input1;
       in2 = &input2;
    } else if ( input1.quantity() == dens_name && input2.quantity() == temp_name ) {
       // use  p = rho * R * T
       //   in1 = T, in2 = rho
       type = 2;
       in1 = &input2;
       in2 = &input1;
    } else {
       throw (badinputquant());
    }   

    // use the first input quantity as a base for the result
    result = in1->duplicate();
    // set the metadata
    result->set_quantity(quant);
    result->set_units("hPa", 100.0); // the 100 takes us from hPa(mb) to Pa

    badval1 = in1->fillval();
    badval2 = in2->fillval();

    // iterate over all horizontal gridpoints
    for ( inPnt1 = in1->begin(), inPnt2 = in2->begin(), outPnt = result->begin() ;
        inPnt1 != in1->end();
        inPnt1++, inPnt2++, outPnt++ ) {
        
        val1 = *inPnt1;
        val2 = *inPnt2;
        
        if ( val1 != badval1 && val2 != badval2 ) {
        
           // transform to MKS units
           val1 = val1 * in1->mksScale + in1->mksOffset;
           val2 = val2 * in2->mksScale + in2->mksOffset;
        
           switch (type) {
           case 1:  // p = p0 * (T/theta)^(-7./2.) 
              *outPnt = 1000.0 * POW(val2/val1, -7./2.);   
                    break;
           case 2:  // p = rho * R * T
              *outPnt = val2 * 287.04 * val1 / 100.0;  // the 100 is to get to mb units)
                    break;
           }
        }  else {
            *outPnt = badval1;
        }                    
    }
       
    if ( flags & OTF_MKS ) {
        result->transform("Pa");
    }     
       
    
    return result;   


}
