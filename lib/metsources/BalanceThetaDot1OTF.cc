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
#include "gigatraj/BalanceThetaDot1OTF.hh"

#include "gigatraj/MetGridData.hh"
#include "gigatraj/DensOTF.hh"

using namespace gigatraj;



BalanceThetaDot1OTF::BalanceThetaDot1OTF() 
{
    /* In the default contructor, we set the names of the physical
       quantities to their default (UCAR CF conventions, where they exist)
    */
    quant = "tendency_of_air_potential_temperature";
    uu = "K/s";
    press_name = "air_pressure";
    temp_name = "air_temperature";
    theta_name = "air_potential_temperature";
    dens_name = "air_density";

}

BalanceThetaDot1OTF::BalanceThetaDot1OTF(const std::string& dthdt, const std::string& temp, const std::string& press, const std::string& theta, const std::string& dens)
{
     /* in this version of the constructor, the user
        specifies all the quantity names */
     quant = dthdt;
     temp_name = temp;
     press_name = press;
     theta_name = theta;
     dens_name = dens;

}

BalanceThetaDot1OTF::~BalanceThetaDot1OTF() 
{
   // nothing to do here
}

BalanceThetaDot1OTF::BalanceThetaDot1OTF(const BalanceThetaDot1OTF& src) : MetOnTheFly(src)
{
     temp_name = src.temp_name;
     press_name = src.press_name;
     theta_name = src.theta_name;
     dens_name = src.dens_name;
}

BalanceThetaDot1OTF& BalanceThetaDot1OTF::operator=(const BalanceThetaDot1OTF& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }

    this->assign( src ) ;
    
    return *this;
}

void BalanceThetaDot1OTF::assign( const BalanceThetaDot1OTF& src)
{
     MetOnTheFly::assign( src );
     temp_name = src.temp_name;
     press_name = src.press_name;
     theta_name = src.theta_name;
     dens_name = src.dens_name;
}

GridField3D* BalanceThetaDot1OTF::calc( const GridField3D& thetadot, const GridField3D& input2, int flags) const
{
    // what we are calculating
    GridField3D *result;
    // identifies what combination of inut physical quantities we have
    int type;
    // input density field
    const GridField3D *density;
    // OTF density generator
    DensOTF getdens;
    // scale and offset to take the input thetadot quantity into MKS units
    real thd_scale, thd_offset;
    // scale and offset to take the density into MKS units
    real den_scale, den_offset;
    // bad-or-missing-data flag value for thetadot and density
    real thd_bad, den_bad;
    // the set of vertical (theta) levels being used by the GridField3D objects
    std::vector<real> vert;
    // an array of grid point areas
    const GridFieldSfc *areas;
    // a specific surface of thetadot values
    const GridFieldSfc *thd_sfc;
    // a specific surface of density values
    const GridFieldSfc *den_sfc;
    // the surface of the resulting balanced thetadot field
    GridFieldSfc *result_sfc;
    // area-weighted averaged used to calcuate the thetadot offset
    real weighted_total, total_weights;
    // the thetadot offset that will bring the input thetadot field into mass balance
    real adjust;
    // iterates over the thetadot grid points
    GridFieldSfc::const_iterator thd_it;
    // iterates over the density grid points
    GridFieldSfc::const_iterator den_it;
    // iterates over the area grid points
    GridFieldSfc::const_iterator area_it;
    // gridpoint values of thetadot and density
    real thd_val, den_val;
    // iterates over the output grid surface grid points
    GridFieldSfc::iterator iout;
    
   
    // the two input fields must be grid-compatible
    if ( ! thetadot.compatible(input2) ) {
       throw (badgrid());
    }
    
    /*! \todo strictly speaking, we should be testing the horizontal grid to make sure that
        it covers the globe, since the balancing operation is valid only
        globally (if at all).
    */    
    
    // and they must be allowed physical quantities
    // Note that the thetadot quantity is always the input unbalanced theta-dot field.
    if ( thetadot.quantity() == quant && input2.quantity() == temp_name && thetadot.vertical() == theta_name ) {
       // dth/dt, t on theta surfaces
       type = 1;
    } else if ( thetadot.quantity() == quant && input2.quantity() == press_name && thetadot.vertical() == theta_name  ) {
       // dth/dt, pressure on theta surfaces
       type = 2;
    } else if ( thetadot.quantity() == quant && input2.quantity() == dens_name && thetadot.vertical() == theta_name  ) {
       // dth/dt, density on theta surfaces
       type = 3;
    } else {
       throw (badinputquant());
    }   

    // We will need density.
    // Either it was supplied (type 3 above)
    // or we need to calculate it on the fly.
    if ( type == 3 ) {
       density = &input2;
    } else {
       density = getdens.calc( input2 );
    }
    
    /// Create a new GridField object to hold the
    /// results.  To ensure it is grid-compatible
    /// with the inputs, we copy the thetadot object.
    result = thetadot.duplicate();
    /// Note that the quantity name and units do not have to be changed,
    /// since the result is the same physical quantity

    /// We do the calculation in MKS units,
    /// so we may as well grab the MKS conversion factos
    /// for the input quantities now
    thd_offset = thetadot.mksOffset;
    thd_scale = thetadot.mksScale;
    den_offset = density->mksOffset;
    den_scale = density->mksScale;
    /// get their bad-or-missing-data flag values as well
    thd_bad = thetadot.fillval();
    den_bad = density->fillval();
    
    // get the vector of vertical (theta) levels
    vert = thetadot.levels();
    
    // get the grid point areas (normalized to the unit sphere)
    areas = thetadot.areas();

    // iterate over each theta surface
    for ( int k=0; k<vert.size(); k++ ) {
    
        // only balance mass flux for this range of theta values
        if ( vert[k] >= 50.0 && vert[k] <= 500.0 ) {

           // get the kth surface of theta-dot and density
           thd_sfc = thetadot.extractSurface(k);
           den_sfc = density->extractSurface(k);
           
           // create this to hold the results
           result_sfc = result->extractSurface(k);
           
           weighted_total = 0.0;
           total_weights = 0.0;
           
           // iterate over each surface gridpoint
           for ( thd_it=thd_sfc->begin(), den_it=den_sfc->begin(), area_it=areas->begin();
                 thd_it != thd_sfc->end();
                 thd_it++, den_it++, area_it++ ) {
                 
                 // get the two values that we need
                 thd_val = *thd_it;
                 den_val = *den_it;
                 if ( (thd_val != thd_bad) && (den_val != den_bad) ) {
                    // scale the two input quantities to MKS units
                    thd_val = thd_val * thd_scale + thd_offset;
                    den_val = den_val * den_scale + den_offset;
                    // make this grid point's contribution to the weighted average
                    weighted_total = weighted_total + thd_val*den_val*(*area_it);
                    total_weights = total_weights + den_val*(*area_it);
                 }
           }
           
           // get the area weighted global average of 
           /// \todo check that total_weights is not 0.0!
           adjust = weighted_total / total_weights;
           
           // apply the adjustment to this surface
           
           // again, iterate over each gridpoint on the surface
           for ( thd_it=thd_sfc->begin(), iout=result_sfc->begin();
                 thd_it != thd_sfc->end();
                 thd_it++, iout++ ) {
                 
                 *iout = thd_bad;
                 thd_val = *thd_it;
                 if ( (thd_val != thd_bad) ) {
                    // convert the UN-balanced theta value to MKS units
                    thd_val = thd_val*thd_scale + thd_offset;
                    // apply the offset
                    thd_val = thd_val - adjust;
                    // convert the result back to the original thetadot units
                    // and store it
                    *iout = ( thd_val - thd_offset)/thd_scale;
                 }   
           }
           
           // replace the surface in the result GridField3D object
           result->replaceLevel( *result_sfc, k );

           delete result_sfc;
           delete thd_sfc;
           delete den_sfc;
        }   
    }           
    
    
    // If we had to create this before, we
    // need to destroy it now.
    if ( type != 3 ) {
       delete density;
    } 
    
    delete areas;
    
    if ( flags & OTF_MKS ) {
       result->transform("K/s");
    }
    
      
    return result;   

}

