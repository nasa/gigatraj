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
/*!
     Test program for the Filter_Trop class using the MetGridSBRot as a met data source 
*/
     
#include <iostream>
#include <math.h>

#include <stdlib.h>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/MetGridSBRot.hh"
#include "gigatraj/Parcel.hh"
#include "gigatraj/Filter_Trop.hh"
#include "gigatraj/PGenRnd.hh"
#include "gigatraj/Flock.hh"
#include "gigatraj/Swarm.hh"

#include "test_utils.hh"

using namespace gigatraj;
using std::cerr;
using std::endl;

bool pcheck( int i, Parcel& p, real thresh, MetGridSBRot *metsrc, std::string tag )
{
   double ptime;
   real plon;
   real plat;
   real pz;
   real ptrop;
   bool result;
   
   result = false;
   
   ptime = p.getTime();
   p.getPos( &plon, &plat );
   pz = p.getZ();
   ptrop = metsrc->getData("tropz", ptime, plon, plat, pz );
   if ( (pz - ptrop) < (- thresh) ) {
      if ( p.queryNoTrace() ) {
         // good
         result = true;
      } else {
         // bad:
         result = -1;
         cerr << tag << " parcel " << i << ": z=" << pz << ", trop = " << ptrop
              << " (at " << plon << ", " << plat << ") should have been filtered but was not " << endl;
              
      }       
   } else {
      if ( p.queryNoTrace() ) {
         // bad:
         cerr << tag << " parcel " << i << ": z=" << pz << ", trop = " << ptrop
              << " (at " << plon << ", " << plat << ") should not have been filtered but was " << endl;
              
      } else {
         // good
         result = true;
      }       
    
   } 

   return result;
}

int main() 
{

    // the met data source
    MetGridSBRot *metsrc;
    // random parcel generator
    PGenRnd pgen;
    // sample Parcel
    Parcel p;
    // various things to hold the generated parcels
    Parcel *p_array;
    std::vector<Parcel> *p_vector;
    std::vector<Parcel>::iterator vector_i;
    std::list<Parcel> *p_list;
    std::list<Parcel>::iterator list_i;
    std::deque<Parcel> *p_deque;
    std::deque<Parcel>::iterator deque_i;
    Flock *p_flock;
    Flock::iterator flock_i;
    Swarm *p_swarm;
    Swarm::iterator swarm_i;
    // the number of parcels to generate
    int n;
    // the filter
    Filter_Trop filter;
    // theshold distance
    real thresh;
    // a test Parcel
    Parcel pp;
    // test parcel altitude
    real pz;
    // test parcel longitude
    real plon;
    // test parcel latitude
    real plat;
    // test parcel time
    real ptime;
    // tropopause at test parcel
    real ptrop;

    // gridded SBRot source
    metsrc = new MetGridSBRot;
    
    // put the parcels all over the world, between 0 and 20 km
    pgen.setBox( -90.0, 0.0, 90.0, 359.999 );
    pgen.setZ(0, 20.0);
    
    thresh = 0.2;

    // set the filter characteristics
    filter.quantity("tropz"); // this is our tropopause quantity
    filter.field("alt"); // this is our tropopause quantity
    filter.direction(-1); // filter everything below the trop
    filter.threshold( thresh ); // filter parcels lower than this distance below the trop
    filter.negation(false); // no negation
    
    n = 100;
    
    // make sure that the sample parcel is set to
    // be traced
    p.clearNoTrace();
    // and set the met data source
    p.setMet( *metsrc );
    
    p_array = pgen.create_array( p, n );
    p_vector = pgen.create_vector( p, n );
    p_list = pgen.create_list( p, n );
    p_deque = pgen.create_deque( p, n );
    p_flock = pgen.create_Flock( p, n );
    p_swarm = pgen.create_Swarm( p, n );
    
    // now filter the points
    filter.apply( p_array, n );
    filter.apply( *p_vector );
    filter.apply( *p_list );
    filter.apply( *p_deque );
    filter.apply( *p_flock );
    filter.apply( *p_swarm );

    // now go through each point and check the results
    int i;
    for ( i=0
        , vector_i = p_vector->begin()
        , list_i   = p_list->begin()
        , deque_i  = p_deque->begin()
        , flock_i  = p_flock->begin()
        , swarm_i  = p_swarm->begin()
        ; i < n
        ; i++
        , vector_i++ 
        , list_i++
        , deque_i++
        , flock_i++
        , swarm_i++
        ) {
       
        if ( ! pcheck( i, p_array[i], thresh, metsrc, "Array" ) ) {
           exit(1);
        }   
        
        if ( ! pcheck( i, *vector_i, thresh, metsrc, "Vector" ) ) {
           exit(1);
        }   
        if ( ! pcheck( i, *list_i, thresh, metsrc, "List" ) ) {
           exit(1);
        }   
        if ( ! pcheck( i, *deque_i, thresh, metsrc, "Deque" ) ) {
           exit(1);
        }   
        if ( ! pcheck( i, *flock_i, thresh, metsrc, "Flock" ) ) {
           exit(1);
        }   
        if ( ! pcheck( i, *swarm_i, thresh, metsrc, "Swarm" ) ) {
           exit(1);
        }   
        
    }
    
    delete p_swarm;
    delete p_flock;
    delete p_deque;
    delete p_list;
    delete p_vector;
    delete[] p_array;

    delete metsrc;

    //------------------------------------------------------------------

    // if we got this far, everything is OK
    exit(0);

}    
