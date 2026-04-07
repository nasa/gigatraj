
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

#include "gigatraj/ThetaOTF.hh"


using namespace gigatraj;

ThetaOTF::ThetaOTF() 
{
   
   quant = "air_potential_temperature";
   uu = "Kelvin";
   pname = "air_pressure";
   tname = "air_temperature";

}

ThetaOTF::ThetaOTF( const std::string& theta, const std::string& temperature, const std::string& pressure)
{
   quant = theta;
   uu = "Kelvin";
   pname = pressure;
   tname = temperature;
}

ThetaOTF::~ThetaOTF()
{
}

ThetaOTF::ThetaOTF(const ThetaOTF& src) : MetOnTheFly(src)
{
    pname = src.pname;
    tname = src.tname;
}

ThetaOTF& ThetaOTF::operator=(const ThetaOTF& src)
{
    this->assign( src ) ;
    
    return *this;
}

void ThetaOTF::assign( const ThetaOTF& src)
{
    MetOnTheFly::assign( src );
    pname = src.pname;
    tname = src.tname;
}

real ThetaOTF::calc( real t, real p) const
{
   real val;

   // sanity-check the input values   
   if ( t < 100.0 || t > 1000.0 || p <= 0.0 || p > 150000.0 ) {
      throw (badinputquant());
   }

   val = dothet(t,p);
   
   return val;
}

GridField3D* ThetaOTF::calc( const GridField3D& t, const GridField3D& p, int flags) const
{
   // scale and offset to convert temperature (whatever units are being used) to (MKS) Kelvin
   real t_scale;
   real t_offset;
   // scale and offset to convert pressure to Pascals
   real p_scale;
   real p_offset;
   // iterator over input gridpoints
   GridField3D::const_iterator tPnt;
   // iterator over input gridpoints
   GridField3D::const_iterator pPnt;
    // iterator over output gridpoints
   GridField3D::iterator destPnt;
    // the output potential temperature field
   GridField3D *result;
   // temporary variables for intermediate results
   real tt, pp;
    // bad-or-missing-data fill values for holding inputs
   real tbad, pbad;
   
   // the input fields must be grid-compatible
   if ( ! t.compatible(p) ) {
      throw (badgrid());
   }

   // the input fields must be the right physical quantities
   if ( t.quantity() != tname || p.quantity() != pname ) {
      throw (badinputquant());
   }
   
   
   // get the conversion factors from the inputs
   t_scale = t.mksScale;
   t_offset = t.mksOffset;
   p_scale = p.mksScale;
   p_offset = p.mksOffset;
   
   // use the first input quantity as a base for the result
   result = t.duplicate();
   // change the quantity and units to potential temperature
   result->set_quantity(quant);
   result->set_units(uu);
   
   tbad = t.fillval();
   pbad = p.fillval();
   
   /* loop over every point in the two input objects, and set the value
      of the corresponding point in the output object.
      Note that all three grids have the same dimensions (as per the compatibility
      check above), so we need only the one end-check for the loop.
   */   
   for ( tPnt  = t.begin(), pPnt  = p.begin(), destPnt  = result->begin();
         tPnt != t.end();
         tPnt++, pPnt++, destPnt++ ) {
         
         
         tt = *tPnt;
         pp = *pPnt;
         if ( tt != tbad && pp != pbad && pp > 0.0 ) {
            tt = tt*t_scale + t_offset;
            pp = pp*p_scale + p_offset;
         
            *destPnt = dothet( tt, pp );
         } else {
            *destPnt = tbad;
         }
   }
   
   return result;

}


GridField3D* ThetaOTF::calc( const GridField3D& t, int flags ) const
{
   // scale and offset to convert temperature (whatever units are being used) to (MKS) Kelvin
   real t_scale;
   real t_offset;
   // scale and offset to convert pressure to Pascals
   real p_scale;
   real p_offset;
   // the output potential temperature field
   GridField3D *result;
   // the vertical coordinate values (pressure)
   std::vector<real> p;
   // temporary variables for intermediate results
   real tt, pp;
   // bad-or-missing-data fill values for holding inputs
   real tbad, pbad;
   // iterator over input gridpoints
   GridField3D::const_iterator tPnt;
   // iterator over output gridpoints
   GridField3D::iterator destPnt;
   
   // the input field must be temperature on pressure surfaces
   if ( t.quantity() != tname || t.vertical() != pname ) {
      throw (badinputquant());
   }
   

   // convert temperature (whatever units are being used) to (MKS) Kelvin
   t_scale = t.mksScale;
   t_offset = t.mksOffset;
   // convert pressure to millibars
   p_scale = t.mksVScale;
   p_offset = t.mksVOffset;


   // use the input quantity as a base for the result
   result = t.duplicate();
   // change the quantity and units to potential temperature
   result->set_quantity(quant);
   result->set_units(uu);
   
   tbad = t.fillval();
   
   p = t.levels();

   /* loop over every point in the two input objects, and set the value
      of the corresponding point in the output object.
      Note that both grids have the same dimensions, so we need 
      only the one end-check for the loop.
   */
   
   for ( int k=0; k < p.size(); k++ ) {

       pp = p[k]*p_scale + p_offset;

       for ( tPnt  = t.begin(k), destPnt  = result->begin(k);
           tPnt != t.end(k);
           tPnt++, destPnt++ ) {
         
         
           tt = *tPnt;
           if ( tt != tbad ) {
           
              tt = tt*t_scale + t_offset;
         
              *destPnt = dothet( tt, pp );
           } else {
              *destPnt = tbad;
           }

       }

   }

   
   return result;


}


GridFieldSfc* ThetaOTF::calc( const GridFieldSfc& t, const GridFieldSfc& p, int flags) const
{
   // scale and offset to convert temperature (whatever units are being used) to (MKS) Kelvin
   real t_scale;
   real t_offset;
   // scale and offset to convert pressure to Pascals
   real p_scale;
   real p_offset;
    // the output potential temperature field
   GridFieldSfc *result;
   // temporary variables for intermediate results
   real tt, pp;
    // bad-or-missing-data fill values for holding inputs
   real tbad, pbad;
    // iterator over input gridpoints
   GridFieldSfc::const_iterator tPnt;
    // iterator over input gridpoints
   GridFieldSfc::const_iterator pPnt;
    // iterator over output gridpoints
   GridFieldSfc::iterator destPnt;
   
    // the two input fields must be grid-compatible
   if ( ! t.compatible(p) ) {
      throw (badgrid());
   }

   // and they must be the right physical quantities
   if ( t.quantity() != tname || p.quantity() != pname ) {
      throw (badinputquant());
   }
   
   
   // convert temperature (whatever units are being used) to (MKS) Kelvin
   t_scale = t.mksScale;
   t_offset = t.mksOffset;
   // convert pressure to millibars
   p_scale = p.mksScale;
   p_offset = p.mksOffset;
   

   // use the first input quantity as a base for the result
   result = t.duplicate();
   // change the quantity and units to potential temperature
   result->set_quantity(quant);
   result->set_units(uu);
   
   tbad = t.fillval();
   pbad = p.fillval();

   // loop over every gridpoint
   for ( tPnt  = t.begin(), pPnt  = p.begin(), destPnt  = result->begin();
         tPnt != t.end();
         tPnt++, pPnt++, destPnt++ ) {
         
         tt = *tPnt;
         pp = *pPnt;
         if ( tt != tbad && pp != pbad && pp > 0.0 ) {

            tt = tt*t_scale + t_offset;
            pp = pp*p_scale + p_offset;
         
            *destPnt = dothet( tt, pp );
         } else {
            *destPnt = tbad;
         }

   }

   
   return result;

}

