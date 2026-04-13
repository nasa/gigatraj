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
     Test program for the Filter_Null class 
*/
     
#include <iostream>
#include <math.h>

#include <stdlib.h>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/MetGridSBRot.hh"
#include "gigatraj/Parcel.hh"
#include "gigatraj/Filter_Null.hh"
#include "gigatraj/PGenRnd.hh"
#include "gigatraj/Flock.hh"
#include "gigatraj/Swarm.hh"

#include "test_utils.hh"

using namespace gigatraj;
using std::cerr;
using std::endl;


int main() 
{

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
    Filter_Null filter;
    // the useless parameter
    int useless;

    
    // put the parcels all over the world, between 0 and 20 km
    pgen.setBox( -90.0, 0.0, 90.0, 359.999 );
    pgen.setZ(0, 20.0);
    
    useless = 123;

    // set the filter characteristics
    filter.useless(useless); 
    
    //n = 100;
    n = 5;
    
    // check that the useless parameter took
    if ( filter.useless() != useless ) {
       cerr << "Useless parameter does not match " << useless << " vs. " << filter.useless() << endl;
       exit(1);
    }
        
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
    
    delete p_swarm;
    delete p_flock;
    delete p_deque;
    delete p_list;
    delete p_vector;
    delete[] p_array;

    //------------------------------------------------------------------

    // if we got this far, everything is OK
    exit(0);

}    
