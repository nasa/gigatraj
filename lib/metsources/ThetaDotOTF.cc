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

#include "gigatraj/ThetaDotOTF.hh"


using namespace gigatraj;

ThetaDotOTF::ThetaDotOTF() 
{
    quant = "tendency_of_air_potential_temperature";
    uu = "K/s";
    dtdt_name = "tendency_of_air_temperature";
    press_name = "air_pressure";
    temp_name = "air_temperature";
    theta_name = "air_potential_temperature";
    omega_name = "lagrangian_tendency_of_air_pressure";

}

ThetaDotOTF::ThetaDotOTF(const std::string& dthdt,const std::string& dtdt, const std::string& temp, const std::string& press, const std::string& theta, const std::string& omega)
{
     quant = dthdt;
     dtdt_name = dtdt;
     temp_name = temp;
     press_name = press;
     theta_name = theta;
     omega_name = omega;

}

ThetaDotOTF::~ThetaDotOTF() 
{
}

ThetaDotOTF::ThetaDotOTF(const ThetaDotOTF& src) : MetOnTheFly(src)
{
     dtdt_name = src.dtdt_name;
     temp_name = src.temp_name;
     press_name = src.press_name;
     theta_name = src.theta_name;
     omega_name = src.omega_name;
}

ThetaDotOTF& ThetaDotOTF::operator=(const ThetaDotOTF& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }

    this->assign( src ) ;
    
    return *this;
}

void ThetaDotOTF::assign( const ThetaDotOTF& src)
{
     MetOnTheFly::assign( src );
     dtdt_name = src.dtdt_name;
     temp_name = src.temp_name;
     press_name = src.press_name;
     theta_name = src.theta_name;
     omega_name = src.omega_name;
}

GridField3D* ThetaDotOTF::calc( const GridField3D& input1, const GridField3D& input2, int flags) const
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
    real dtdt, temp, theta, press;
    /* identifies which combination of input physical quantity and
       vertical coordinate quantity we are using for the calculation
    */
    int type;
   
    // the two input fields must be grid-compatible
    if ( ! input1.compatible(input2) ) {
       throw (badgrid());
    }

    // and they must be allowed physical quantities
    if ( input1.quantity() == dtdt_name && input2.quantity() == temp_name && input1.vertical() == press_name ) {
       // dt/dt, t on pressure surfaces
       type = 1;
    } else if ( input1.quantity() == dtdt_name && input2.quantity() == theta_name && input1.vertical() == press_name  ) {
       // dt/dt, theta on pressure surfaces
       type = 2;
    } else if ( input1.quantity() == dtdt_name && input2.quantity() == temp_name && input1.vertical() == theta_name ) {
       // dt/dt, t on theta surfaces
       type = 3;
    } else if ( input1.quantity() == dtdt_name && input2.quantity() == press_name  && input1.vertical() == theta_name ) {
       // dt/dt, pressure on theta surfaces
       type = 4;
    } else {
       throw (badinputquant());
    }   

    
    // use the first input quantity as a base for the result
    result = input1.duplicate();
    // set the metadata
    result->set_quantity(quant);
    result->set_units("K/s"); 

    badval1 = input1.fillval();
    badval2 = input2.fillval();
       
    vert = input1.levels();

    // iterate over all horizontal gridpoints
    for ( inProf1 = input1.profileBegin(), inProf2 = input2.profileBegin(), outProf = result->profileBegin();
          inProf1 != input1.profileEnd();
          inProf1++, inProf2++, outProf++ ) {
    
          // get the data profiles
          profile1 = *inProf1;
          profile2 = *inProf2;
          oprofile = *outProf;
          
          // for each pressure value in this profile...
          for ( int k=0; k<profile1->size(); k++ ) {
              
          
              value1 = (*profile1)[k];
              value2 = (*profile2)[k];
              
              if ( value1 != badval1 && value2 != badval2 ) {
                 
                 dtdt = value1*input1.mksScale + input1.mksOffset;
                 
                 switch (type) {
                 case 1:
                       temp = value2*input2.mksScale + input2.mksOffset;
                       press = vert[k] * input1.mksVScale + input1.mksVOffset;
                       theta = temp * POW( 100000.0/press, 2./7. );
                     break;
                 case 2:
                       theta = value2*input2.mksScale + input2.mksOffset;
                       press = vert[k] * input1.mksVScale + input1.mksVOffset;
                       temp = theta * POW( 100000.0/press, -2./7. );
                     break;
                 case 3:
                       temp = value2*input2.mksScale + input2.mksOffset;
                       theta = vert[k] * input1.mksVScale + input1.mksVOffset;
                       press = 100000.0 * POW( theta/temp, -7./2. );
                     break;
                 case 4:
                       press = value2*input2.mksScale + input2.mksOffset;
                       theta = vert[k] * input1.mksVScale + input1.mksVOffset;
                       temp = theta * POW( 100000.0/press, -2./7. );
                     break;
                 }
                 
                 (*oprofile)[k] = theta * dtdt / temp;

              } else {
                 (*oprofile)[k] = badval1;
              }

          }
          outProf.assign(*oprofile); 
          
          delete oprofile;
          delete profile2;
          delete profile1;
          
          //i++;

    }      
       
    
    return result;   

}


GridField3D* ThetaDotOTF::calc( const GridField3D& input1, const GridField3D& input2, const GridField3D& input3, int flags) const
{
    // the output temperature tendency field
    GridField3D *result;
    // bad-or-missing-data fill values for holding inputs
    real badval1, badval2, badval3;
    // temporary variables for holding inputs 
    real value, value1, value2, value3;
    // vertical coordinate value
    real zval;
    // iterator over input gridpoint profiles
    GridField3D::const_profileIterator inProf1, inProf2, inProf3;
    // iterator over output gridpoint profiles
    GridField3D::profileIterator outProf;
    // vectors of vertical profile data extracted from the input grids
    std::vector<real> *profile1, *profile2, *profile3;
    // vector of vertical profile data for the output grid
    std::vector<real> *oprofile;
    // a vector of vertical coordinate values from the input grid
    std::vector<real> vert;
    // temporary variables for intermediate results
    real dtdt, temp, theta, press, omega;
    /* identifies which combination of input physical quantity and
       vertical coordinate quantity we are using for the calculation
    */
    int type;

   
    // the two input fields must be grid-compatible
    if ( ! input1.compatible(input2)  || ! input1.compatible(input2) || ! input1.compatible(input3) ) {
       throw (badgrid());
    }

    // and they must be allowed physical quantities
    if ( input1.quantity() == dtdt_name && input2.quantity() == temp_name && input3.quantity() == press_name ) {
       // dt/dt, t, p
       type = 1;
    } else if ( input1.quantity() == dtdt_name && input2.quantity() == temp_name && input3.quantity() == theta_name ) {
       // dt/dt, t, theta
       type = 2;
    } else if ( input1.quantity() == dtdt_name && input2.quantity() == theta_name && input3.quantity() == press_name ) {
       // dt/dt, theta, p
       type = 3;
    } else if ( input1.quantity() == dtdt_name && input2.quantity() == temp_name && input3.quantity() == omega_name && input1.vertical() == press_name  ) {
       // dt/dt, t, omega on pressure surfaces
       type = 4;
    } else if ( input1.quantity() == dtdt_name && input2.quantity() == theta_name && input3.quantity() == omega_name && input1.vertical() == press_name  ) {
       // dt/dt, theta, omega on pressure surfaces
       type = 5;
    } else if ( input1.quantity() == dtdt_name && input2.quantity() == temp_name && input3.quantity() == omega_name  && input1.vertical() == theta_name ) {
       // dt/dt, t, omega  on theta surfaces
       type = 6;
    } else if ( input1.quantity() == dtdt_name && input2.quantity() == press_name && input3.quantity() == omega_name  && input1.vertical() == theta_name ) {
       // dt/dt, pressure, omega on theta surfaces
       type = 7;
    } else {
       throw (badinputquant());
    }   

    
    // use the first input quantity as a base for the result
    result = input1.duplicate();
    // set the metadata
    result->set_quantity(quant);
    result->set_units("K/s"); 

    badval1 = input1.fillval();
    badval2 = input2.fillval();
    badval3 = input3.fillval();
       
    vert = input1.levels();

    // iterate over all horizontal gridpoints
    for ( inProf1 = input1.profileBegin(), inProf2 = input2.profileBegin(), inProf3 = input3.profileBegin(), outProf = result->profileBegin();
          inProf1 != input1.profileEnd();
          inProf1++, inProf2++, inProf3++, outProf++ ) {
    
          // get the data profiles
          profile1 = *inProf1;
          profile2 = *inProf2;
          profile3 = *inProf3;
          oprofile = *outProf;
          
          // for each pressure value in this profile...
          for ( int k=0; k<profile1->size(); k++ ) {
              
          
              value1 = (*profile1)[k];
              value2 = (*profile2)[k];
              value3 = (*profile3)[k];
              
              if ( value1 != badval1 && value2 != badval2 && value3 != badval3 ) {
                 
                 dtdt = value1*input1.mksScale + input1.mksOffset;

                 switch (type) {
                 case 1:
                       temp  = value2*input2.mksScale + input2.mksOffset;
                       press = value3*input3.mksScale + input3.mksOffset;
                       theta = temp * POW( 100000.0/press, 2./7. );
                       value = theta * dtdt / temp;
                     break;
                 case 2:
                       temp  = value2*input2.mksScale + input2.mksOffset;
                       theta = value3*input3.mksScale + input3.mksOffset;
                       //press = 100000.0 * POW( theta/temp, -7./2. );
                       value = theta * dtdt / temp;
                     break;
                 case 3:
                       theta = value2*input2.mksScale + input2.mksOffset;
                       press = value3*input3.mksScale + input3.mksOffset;
                       temp = theta * POW( 100000.0/press, -2./7. );
                       value = theta * dtdt / temp;
                      break;
                 case 4:
                       temp  = value2*input2.mksScale + input2.mksOffset;
                       press = vert[k] * input1.mksVScale + input1.mksVOffset;
                       theta = temp * POW( 100000.0/press, 2./7. );
                       omega = value3*input3.mksScale + input3.mksOffset;
                       value = theta * (dtdt / temp - 2./7.*omega/press);
                      break;
                 case 5:
                       theta  = value2*input2.mksScale + input2.mksOffset;
                       press = vert[k] * input1.mksVScale + input1.mksVOffset;
                       temp = theta * POW( 100000.0/press, -2./7. );
                       omega = value3*input3.mksScale + input3.mksOffset;
                       value = theta * (dtdt / temp - 2./7.*omega/press);
                      break;
                 case 6:
                       temp  = value2*input2.mksScale + input2.mksOffset;
                       theta = vert[k] * input1.mksVScale + input1.mksVOffset;
                       press = 100000.0 * POW( theta/temp, -7./2. );
                       omega = value3*input3.mksScale + input3.mksOffset;
                       value = theta * ( dtdt / temp - 2./7.*omega/press);
                      break;
                 case 7:
                       press = value2*input2.mksScale + input2.mksOffset;
                       theta = vert[k] * input1.mksVScale + input1.mksVOffset;
                       temp = theta * POW( 100000.0/press, -2./7. );
                       omega = value3*input3.mksScale + input3.mksOffset;
                       value = theta * ( dtdt / temp - 2./7.*omega/press);
                     break;
                 }
                 

              } else {
                 value = badval1;
              }
              
              (*oprofile)[k] = value;

          }
          
          // load this profile into the output grid
          outProf.assign(*oprofile); 
          
          delete oprofile;
          delete profile3;
          delete profile2;
          delete profile1;

          //i++;
    }      
       
    return result;   

}

GridField3D* ThetaDotOTF::calc( const GridField3D& input1, const GridField3D& input2, const GridField3D& input3, const GridField3D& input4, int flags) const
{
    // the output temperature tendency field
    GridField3D *result;
    // bad-or-missing-data fill values for holding inputs
    real badval1, badval2, badval3, badval4;
    // temporary variables for holding inputs     
    real value, value1, value2, value3, value4;
    // iterator over input gridpoints
    GridField3D::const_iterator inPnt1, inPnt2, inPnt3, inPnt4;
    // iterator over output gridpoints
    GridField3D::iterator outPnt;
    // vectors of vertical profile data extracted from the input grids
    std::vector<real> *profile1, *profile2, *profile3, *profile4;
    // temporary variables for intermediate results
    real dtdt, temp, theta, press, omega;
    /* identifies which combination of input physical quantity and
       vertical coordinate quantity we are using for the calculation
    */
    int type;

   
    // the two input fields must be grid-compatible
    if ( ! input1.compatible(input2)  || ! input1.compatible(input2) || ! input1.compatible(input3) || ! input1.compatible(input4) ) {
       throw (badgrid());
    }

    // and they must be allowed physical quantities
    if ( input1.quantity() == dtdt_name && input2.quantity() == temp_name && input3.quantity() == press_name && input4.quantity() == omega_name ) {
       // dt/dt, t, pressure, omega
       type = 1;
    } else if ( input1.quantity() == dtdt_name && input2.quantity() == temp_name && input3.quantity() == theta_name  && input4.quantity() == omega_name ) {
       // dt/dt, t, theta, omega
       type = 2;
    } else if ( input1.quantity() == dtdt_name && input2.quantity() == theta_name && input3.quantity() == press_name  && input4.quantity() == omega_name ) {
       // dt/dt, t, theta, omega
       type = 3;
    } else {
       throw (badinputquant());
    }   

    
    // use the first input quantity as a base for the result
    result = input1.duplicate();
    // set the metadata
    result->set_quantity(quant);
    result->set_units("K/s"); 

    badval1 = input1.fillval();
    badval2 = input2.fillval();
    badval3 = input3.fillval();
    badval4 = input4.fillval();
       

    // iterate over all gridpoints
    // (note that we do not use profiles here, since we are not using
    // any vertical coordinate values)
    for ( inPnt1 = input1.begin(), inPnt2 = input2.begin(), inPnt3 = input3.begin(), inPnt4 = input4.begin(), outPnt = result->begin();
          inPnt1 != input1.end();
          inPnt1++, inPnt2++, inPnt3++, inPnt4++, outPnt++ ) {
    
          // get the data profile
          value1 = *inPnt1;
          value2 = *inPnt2;
          value3 = *inPnt3;
          value4 = *inPnt4;
          
          if ( value1 != badval1 && value2 != badval2 && value3 != badval3 && value4 != badval4 ) {

             dtdt  = value1*input1.mksScale + input1.mksOffset;
             omega = value4*input4.mksScale + input4.mksOffset;
             
             switch (type) {
             case 1:
                   temp  = value2*input2.mksScale + input2.mksOffset;
                   press = value3*input3.mksScale + input3.mksOffset;
                   theta = temp * POW( 100000.0/press, 2./7. );
                 break;
             case 2:
                   temp  = value2*input2.mksScale + input2.mksOffset;
                   theta = value3*input3.mksScale + input3.mksOffset;
                   press = 100000.0 * POW( theta/temp, -7./2. );
                 break;
             case 3:
                   theta = value2*input2.mksScale + input2.mksOffset;
                   press = value3*input3.mksScale + input3.mksOffset;
                   temp = theta * POW( 100000.0/press, -2./7. );
                  break;
             }
             
             value = theta * ( dtdt / temp - 2./7.*omega/press);

          } else {
             value = badval1;
          }
          
          *outPnt = value;

    }      
       
    
    return result;   

}
