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

#include "gigatraj/DensOTF.hh"

using namespace gigatraj;


DensOTF::DensOTF() 
{
    dens_name = "air_density";
    press_name = "air_pressure";
    temp_name = "air_temperature";
    theta_name = "air_potential_temperature";
}

DensOTF::DensOTF(const std::string& dens, const std::string& press, const std::string& temp, const std::string& theta)
{
     dens_name = dens;
     press_name = press;
     temp_name = temp;
     theta_name = theta;
}

DensOTF::~DensOTF() 
{
}

DensOTF::DensOTF(const DensOTF& src) : MetOnTheFly(src)
{
     dens_name = src.dens_name;
     press_name = src.press_name;
     temp_name = src.temp_name;
     theta_name = src.theta_name;
}

DensOTF& DensOTF::operator=(const DensOTF& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }

    this->assign( src ) ;
    
    return *this;
}

void DensOTF::assign( const DensOTF& src)
{
     MetOnTheFly::assign( src );
     dens_name = src.dens_name;
     press_name = src.press_name;
     temp_name = src.temp_name;
     theta_name = src.theta_name;
}

GridField3D* DensOTF::calc( const GridField3D& input, int flags) const
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
    // a vector of vertical coordinate values from the input grid
    std::vector<real> vert;
    /* identifies which combination of input physical quantity and
       vertical coordinate quantity we are using for the calculation
    */
    int calctype = 0;

    // which two quantities are we using?
    if ( input.quantity() == temp_name && input.vertical() == press_name ) {
       calctype = 1;
    } else if ( input.quantity() == temp_name && input.vertical() == theta_name ) {
       calctype = 2;
    } else if ( input.quantity() == theta_name && input.vertical() == press_name ) {
       calctype = 3;
    } else if ( input.quantity() == press_name && input.vertical() == theta_name ) {
       calctype = 4;
    } else {
       // unusable input quantity
       throw (badinputquant());
    }

    // create the output by copying the input grid
    result = input.duplicate();
    // and change its metadata
    result->set_quantity(dens_name);
    result->set_units("kg/m^3");  
    
    badval = input.fillval();
    
    // get the vertical coordinates and
    // transform them to MKS units.
    vert = input.levels();
    for ( int k=0; k<vert.size(); k++ ) {
        vert[k] = vert[k] * input.mksVScale + input.mksVOffset;
    }   
    
    // iterate over each profile in the horizontal grid
    for ( inProf = input.profileBegin(), outProf = result->profileBegin();
          inProf != input.profileEnd();
          inProf++, outProf++ ) {
    
          // get the data profile
          profile = *inProf;
          
          // calculate the density values for this profile
          for ( int k=0; k<profile->size(); k++ ) {

               zval = vert[k];
               value = (*profile)[k];
               
               if ( value != badval ) {
                  // transform to MKS units
                  value = value * input.mksScale + input.mksOffset;
                  switch (calctype) {
                  case 1:  // input quantity is temperature on pressure aurfaces
                        (*profile)[k] = zval / 287.04 / value;
                        break;
                  case 2:  // input quantity is temperature on theta surfaces 
                        (*profile)[k] = 100000.0 / 287.04 / value * POW( value/zval, 7./2. );
                        break;
                  case 3:  // input quantity is theta on pressure surfaces 
                        (*profile)[k] = zval/value/287.04 * POW( 100000.0/zval, 2./7. );
                        break;
                  case 4:  // input quantity is pressure on theta surfaces 
                        (*profile)[k] = value/zval/287.04 * POW( 100000.0/value, 2./7. );
                        break;
                  }      
               }  else {
                  (*profile)[k] = badval;
               }       
          }

          // store the density into the output grid
          outProf.assign(*profile); 
          
          delete profile;

    }      
       
    
    return result;   

}


GridField3D* DensOTF::calc( const GridField3D& input1, const GridField3D& input2, int flags ) const
{
    // the output density field
    GridField3D *result;
    // bad-or-missing-data fill value from the first input grid
    real badval1;
    // bad-or-missing-data fill value from the second input grid
    real badval2;
    // iterator over the first input's gridpoints
    GridField3D::const_iterator inPnt1;
    // iterator over the second input's gridpoints
    GridField3D::const_iterator inPnt2;
    // iterator over output's gridpoints
    GridField3D::iterator outPnt;
    // pointers to the input grids, so that their given order does not have to matter 
    const GridField3D* in1;
    const GridField3D* in2;
    // temporary variables for holding results
    real val1, val2;
    /* identifies which combination of input physical quantity and
       vertical coordinate quantity we are using for the calculation
    */
    int calctype = 0;

    // the two input fields must be grid-compatible
    if ( ! input1.compatible(input2) ) {
       throw (badgrid());
    }

    /* which two quantities are we using for inputs?
       Note that because the calling routine may give us the two input fields
       in either order, we use pointers here to establish a fixed
       order.
    */
    if ( input1.quantity() == temp_name && input2.quantity() == press_name ) {
       calctype = 1;
       in1 = &input1;
       in2 = &input2;
    } else if ( input1.quantity() == press_name && input2.quantity() == temp_name ) {
       calctype = 1;
       in1 = &input2;
       in2 = &input1;
    } else if ( input1.quantity() == temp_name  && input2.quantity() == theta_name ) {
       calctype = 2;
       in1 = &input1;
       in2 = &input2;
    } else if ( input1.quantity() == theta_name  && input2.quantity() == temp_name ) {
       calctype = 2;
       in1 = &input2;
       in2 = &input1;
    } else if ( input1.quantity() == theta_name && input2.quantity() == press_name ) {
       calctype = 3;
       in1 = &input1;
       in2 = &input2;
    } else if ( input1.quantity() == press_name && input2.quantity() == theta_name ) {
       calctype = 3;
       in1 = &input2;
       in2 = &input1;
    } else {
       // unusable input quantity
       throw (badinputquant());
    }

    // create the output by copying an input grid
    result = input1.duplicate();
    // and change its metadata
    result->set_quantity(dens_name);
    result->set_units("kg/m^3");  
    
    badval1 = in1->fillval();
    badval2 = in2->fillval();

    // iterate over all gridpoints
    for ( inPnt1 = in1->begin(), inPnt2 = in2->begin(), outPnt = result->begin() ;
          inPnt1 != in1->end();
          inPnt1++, inPnt2++, outPnt++ ) {
          
          // grab the two input values
          val1 = *inPnt1;
          val2 = *inPnt2;
        
          if ( val1 != badval1 && val2 != badval2 ) {
          
             // transform to MKS units
             val1 = val1 * in1->mksScale + in1->mksOffset;
             val2 = val2 * in2->mksScale + in2->mksOffset;
               
             switch (calctype) {
             case 1:  // input quantity is temperature on pressure aurfaces
                   *outPnt = val2 / 287.04 / val1;
                   break;
             case 2:  // input quantity is temperature on theta surfaces 
                   *outPnt = 100000.0 / 287.04 / val1 * POW( val1/val2, 7./2. );
                   break;
             case 3:  // input quantity is theta on pressure surfaces 
                   *outPnt = val2/val1/287.04 * POW( 100000.0/val2, 2./7. );
                   break;
             }      
          }  else {
             *outPnt = badval1;
          }       

    }      
       
    
    return result;   

}



GridFieldSfc* DensOTF::calc( const GridFieldSfc& input1, const GridFieldSfc& input2, int flags ) const
{
    // the output density field
    GridFieldSfc *result;
    // bad-or-missing-data fill value from the first input grid
    real badval1;
    // bad-or-missing-data fill value from the second input grid
    real badval2;
    // iterator over the first input's gridpoints
    GridFieldSfc::const_iterator inPnt1;
    // iterator over the second input's gridpoints
    GridFieldSfc::const_iterator inPnt2;
    // iterator over output's gridpoints
    GridFieldSfc::iterator outPnt;
    // pointers to the input grids, so that their given order does not have to matter 
    const GridFieldSfc* in1;
    const GridFieldSfc* in2;
    // temporary variables for holding results
    real val1, val2;
    /* identifies which combination of input physical quantity and
       vertical coordinate quantity we are using for the calculation
    */
    int calctype = 0;

    // the two input fields must be grid-compatible
    if ( ! input1.compatible(input2) ) {
       throw (badgrid());
    }


    /* which two quantities are we using for inputs?
       Note that because the calling routine may give us the two input fields
       in either order, we use pointers here to establish a fixed
       order.
    */
    if ( input1.quantity() == temp_name && input2.quantity() == press_name ) {
       calctype = 1;
       in1 = &input1;
       in2 = &input2;
    } else if ( input1.quantity() == press_name && input2.quantity() == temp_name ) {
       calctype = 1;
       in1 = &input2;
       in2 = &input1;
    } else if ( input1.quantity() == temp_name  && input2.quantity() == theta_name ) {
       calctype = 2;
       in1 = &input1;
       in2 = &input2;
    } else if ( input1.quantity() == theta_name  && input2.quantity() == temp_name ) {
       calctype = 2;
       in1 = &input2;
       in2 = &input1;
    } else if ( input1.quantity() == theta_name && input2.quantity() == press_name ) {
       calctype = 3;
       in1 = &input1;
       in2 = &input2;
    } else if ( input1.quantity() == press_name && input2.quantity() == theta_name ) {
       calctype = 3;
       in1 = &input2;
       in2 = &input1;
    } else {
       // unusable input quantity
       throw (badinputquant());
    }

    // create the output by copying an input grid
    result = input1.duplicate();
    // and change its metadata
    result->set_quantity(dens_name);
    result->set_units("kg/m^3");  
    
    badval1 = in1->fillval();
    badval2 = in2->fillval();

    // iterate over all gridpoints
    for ( inPnt1 = in1->begin(), inPnt2 = in2->begin(), outPnt = result->begin() ;
          inPnt1 != in1->end();
          inPnt1++, inPnt2++, outPnt++ ) {
          
          // grab the two input values
          val1 = *inPnt1;
          val2 = *inPnt2;
        
          if ( val1 != badval1 && val2 != badval2 ) {
          
             // transform to MKS units
             val1 = val1 * in1->mksScale + in1->mksOffset;
             val2 = val2 * in2->mksScale + in2->mksOffset;
               
             switch (calctype) {
             case 1:  // input quantity is temperature on pressure aurfaces
                   *outPnt = val2 / 287.04 / val1;
                   break;
             case 2:  // input quantity is temperature on theta surfaces 
                   *outPnt = 100000.0 / 287.04 / val1 * POW( val1/val2, 7./2. );
                   break;
             case 3:  // input quantity is theta on pressure surfaces 
                   *outPnt = val2/val1/287.04 * POW( 100000.0/val2, 2./7. );
                   break;
             }      
          }  else {
             *outPnt = badval1;
          }       

    }      
       
    
    return result;   


}
