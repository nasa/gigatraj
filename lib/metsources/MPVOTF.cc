
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

#include "gigatraj/MPVOTF.hh"

using namespace gigatraj;


MPVOTF::MPVOTF() 
{
   
   quant = "modified_potential_vorticity";
   uu = "K m^2/kg/s";
   ename = "ertel_potential_vorticity";
   hname = "air_potential_temperature";

}
MPVOTF::MPVOTF( const std::string& epv, const std::string& theta )
{
   
   quant = "modified potential vorticity";
   uu = "K m^2/kg/s";
   ename = epv;
   hname = theta;

}

MPVOTF::~MPVOTF()
{
}

MPVOTF::MPVOTF(const MPVOTF& src) : MetOnTheFly(src)
{
   ename = src.ename;
   hname = src.hname;
}

MPVOTF& MPVOTF::operator=(const MPVOTF& src)
{
    this->assign( src ) ;
    
    return *this;
}

void MPVOTF::assign( const MPVOTF& src)
{
     MetOnTheFly::assign( src );
     ename = src.ename;
     hname = src.hname;
}

GridField3D* MPVOTF::calc( const GridField3D& epv, const GridField3D& theta, int flags) const
{

   // the output MPV field
   GridField3D *result;
   // iterator over the first input's gridpoints
   GridField3D::const_iterator inPnt1;
   // iterator over the second input's gridpoints
   GridField3D::const_iterator inPnt2;
   // iterator over output's gridpoints
   GridField3D::iterator outPnt;
   // temporary variables for input quantities
   real ee, hh;
   // bad-or-missing-data fill values
   real epvbad, thetabad;
   
   // the two input fields must be grid-compatible
   if ( ! epv.compatible(theta) ) {
      throw (badgrid());
   }
   // and they must be the right quantities
   if ( epv.quantity() != ename || theta.quantity() != hname ) {
      throw (badinputquant());
   }

   // create the output by copying the input grid 
   result = epv.duplicate();                    
   // and change its metadata                     
   result->set_quantity(quant);               
   // note: keep same units as epv field
   
   epvbad = epv.fillval();
   thetabad = theta.fillval();
   
   // iterate over all gridpoints                                                 
   for ( inPnt1 = epv.begin(), inPnt2 = theta.begin(), outPnt = result->begin() ; 
         inPnt1 != epv.end();                                                    
         inPnt1++, inPnt2++, outPnt++ ) {                                         
                                                                                  
         // grab the two input values                                             
         ee = *inPnt1;                                                          
         hh = *inPnt2;                                                          
                                                                                  
         if ( ee != epvbad && hh != thetabad && hh > 0.0 ) { 
            // transform to MKS units                                                      
            ee = ee * epv.mksScale + epv.mksOffset;                      
            hh = hh * theta.mksScale + theta.mksOffset;                      
            // calculate MPV
            ee = ee * POW( (hh/420.0), (-9./2.) );
            // scale it and store it in the profile
            *outPnt = ( ee - result->mksOffset )/result->mksScale;  
         } else {
            *outPnt = epvbad;
         }   

   }                                                                              
   
   if ( flags & OTF_MKS ) {
      result->transform("K m^2/kg/s");
   }                                                                            
                                                                                  
   return result;                                                                

}

GridField3D* MPVOTF::calc( const GridField3D& epv, int flags ) const
{
   // the output MPV field
   GridField3D *result;
   // the set of vertical coordinates
   std::vector<real> theta;
   // a vector of vertical profile data extracted from the input grid
   std::vector<real> *profile;
   // iterator over input gridpoint profiles  
   GridField3D::const_profileIterator inProf; 
   // iterator over output gridpoint profiles 
   GridField3D::profileIterator outProf;      
   // temporary variables for input quantities
   real ee, hh;
   // bad-or-missing-data fill value
   real epvbad;
   
   // the input physical quantity and its vertical coordinate quantity
   // must be correct. 
   if ( epv.quantity() != ename || epv.vertical() != hname ) {
      throw (badinputquant());
   }
   

   // create the output by copying the input grid 
   result = epv.duplicate();                    
   // and change its metadata                     
   result->set_quantity(quant);               
   // note: keep same units as epv field

   // get the vertical coordinates and                       
   // transform them to MKS units.                           
   theta = epv.levels();                                     
   for ( int k=0; k<theta.size(); k++ ) {                    
       theta[k] = theta[k] * epv.mksVScale + epv.mksVOffset; 
   }                                                         
   
   
   epvbad = epv.fillval();
   
   // iterate over each profile in the horizontal grid                                       
   for ( inProf = epv.profileBegin(), outProf = result->profileBegin();                    
         inProf != epv.profileEnd();                                                       
         inProf++, outProf++ ) {                                                             
                                                                                             
         // get the data profile                                                             
         profile = *inProf;                                                                  
                                                                                             
         // calculate the density values for this profile                                    
         for ( int k=0; k<profile->size(); k++ ) {                                           

              hh = theta[k]; 
              ee = (*profile)[k];                                                               
                                                                                             
              if ( ee != epvbad && hh > 0.0 ) {                
                  // transform to MKS units                                                      
                  ee = (*profile)[k] * epv.mksScale + epv.mksOffset;                      
                  // calculate MPV
                  ee = ee * POW( (hh/420.0), (-9./2.) );
                  // scale it and store it in the profile
                  (*profile)[k] = ( ee - result->mksOffset )/result->mksScale;  
              } else {                                                      
                 (*profile)[k] = epvbad;
              }                                    
         }                                                                                   

         // store the density into the output grid                                           
         outProf.assign(*profile);                                                           
                                                                                             
         delete profile;                                                                     

   }                                                                                         
   
   if ( flags & OTF_MKS ) {
      result->transform("K m^2/kg/s");
   }                                                                            
                                                                                  
   
   return result;


}


GridFieldSfc* MPVOTF::calc( const GridFieldSfc& epv, const GridFieldSfc& theta, int flags) const
{

   // the output MPV field
   GridFieldSfc *result;
   // temporary variables for input quantities
   real hh, ee;
   // bad-or-missing-data fill values
   real epvbad, thetabad;
   // iterator over the first input's gridpoints
   GridFieldSfc::const_iterator inPnt1;
   // iterator over the second input's gridpoints
   GridFieldSfc::const_iterator inPnt2;
   // iterator over output's gridpoints
   GridFieldSfc::iterator outPnt;
   
   // the two input fields must be grid-compatible
   if ( ! epv.compatible(theta) ) {
      throw (badgrid());
   }
   // and they must be the right quantities
   if ( epv.quantity() != ename || theta.quantity() != hname ) {
      throw (badinputquant());
   }
   
   // create the output by copying the input grid 
   result = epv.duplicate();                    
   // and change its metadata                     
   result->set_quantity(quant);               
   // note: keep same units as epv field
   
   epvbad = epv.fillval();
   thetabad = theta.fillval();

   // iterate over all gridpoints
   for ( inPnt1 = epv.begin(), inPnt2 = theta.begin(), outPnt = result->begin() ;
         inPnt1 != epv.end();
         inPnt1++, inPnt2++, outPnt++ ) 
   {
         
         // grab the two input values
         ee = *inPnt1;
         hh = *inPnt2;
       
         if ( ee != epvbad && hh != thetabad && hh > 0.0  ) {
            // transform to MKS units                                                      
            ee = ee * epv.mksScale + epv.mksOffset;                      
            hh = hh * theta.mksScale + theta.mksOffset;                      
            // calculate MPV
            ee = ee * POW( (hh/420.0), (-9./2.) );
            // scale it and store it in the profile
            *outPnt = ( ee - result->mksOffset )/result->mksScale;  
         } else {
            *outPnt = epvbad;
         }   

   }      
                                                                                  
   if ( flags & OTF_MKS ) {
      result->transform("K m^2/kg/s");
   }                                                                            
   
   return result;   
   
}

