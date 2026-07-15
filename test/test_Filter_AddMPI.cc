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
     Test program for the FilterAdd class 
*/
     
#include <iostream>
#include <math.h>

#include <stdlib.h>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/MetGridSBRot.hh"
#include "gigatraj/Parcel.hh"
#include "gigatraj/Filter_Add.hh"
#include "gigatraj/PGenRnd.hh"
#include "gigatraj/Flock.hh"
#include "gigatraj/Swarm.hh"
#include "gigatraj/MPIGrp.hh"

#include "test_utils.hh"

using namespace gigatraj;
using std::cerr;
using std::endl;


int main(int argc, char* argv[]) 
{

    // random parcel generator
    PGenRnd pgen;
    // sample Parcel
    Parcel p;
    // various things to hold the generated parcels
    Parcel *p_array;
    std::vector<Parcel> *p_vector;
    std::vector<Parcel> *p_new;
    std::vector<Parcel>::iterator vector_i;
    std::list<Parcel> *p_list;
    std::list<Parcel>::iterator list_i;
    std::deque<Parcel> *p_deque;
    std::deque<Parcel>::iterator deque_i;
    Flock *p_flock;
    Flock::iterator flock_i;
    Swarm *p_swarm;
    Swarm::iterator swarm_i;
    MPIGrp *pgrp;
    // the number of parcels to generate
    int n;
    // the filter
    Filter_Add filter;
    // the flags to test
    ParcelFlag flags;
    // whether to negate
    bool negt;
    // old parcel count
    int count_old;
    // new parcel count
    int count_new;
    // tag value
    double tag;

    // create a process group (MPI, of course)
    // This will call MPI_Init().
    pgrp = new MPIGrp(argc, argv);
    
    // put the parcels all over the world, between 0 and 20 km
    pgen.setBox( -90.0, 0.0, 90.0, 359.999 );
    pgen.setZ(0, 20.0);
    
    flags = NoTrace;
    negt = false;

    // set the filter characteristics
    filter.flagset(flags);
    if ( filter.flagset() != flags ) {
       cerr << "Flagset does not match " << flags << " vs. " << filter.flagset() << endl;
       exit(1);    
    } 
    filter.negation(negt);
    if ( filter.negation() != negt ) {
       cerr << "Negation does not match " << negt << " vs. " << filter.negation() << endl;
       exit(1);    
    } 
    
    //n = 100;
    n = 50;
    
    // all of the parcels created below should have this tag value;
    p.tag(-1.0);
    
    p_flock = pgen.create_Flock( p, n, pgrp );
    p_swarm = pgen.create_Swarm( p, n, pgrp );
    
    // set flags on four Parcels

    ((*p_flock)[ 5]).setNoTrace();
    ((*p_flock)[10]).setNoTrace();
    ((*p_flock)[15]).setNoTrace();
    ((*p_flock)[20]).setNoTrace();
    count_old = 0;
    count_new = 0;
    for ( int i=0; i<n; i++ ) {
        p = p_flock->get( i, 0 );
        if ( ! p.queryNoTrace() ) {
           tag =  p.tag();
           if ( tag == -1.0 ) {
              count_old ++;
           }
           if ( tag == 1.0 ) {
              count_new ++;
           }
        }
    }
    if ( (count_old != (n - 4)) || (count_new != 0) ) {
       cerr << "flock initial setup is  " << count_old 
            << " and " << count_new << " instead of " 
            << n - 4 << " and " << 0 << endl;
       exit(1);           
    }
    
    p = (*p_swarm)[ 5];
    p.setNoTrace();
    p_swarm->set( 5, p);
    p = (*p_swarm)[10];
    p.setNoTrace();
    p_swarm->set(10, p);
    p = (*p_swarm)[15];
    p.setNoTrace();
    p_swarm->set(15, p);
    p = (*p_swarm)[20];
    p.setNoTrace();
    p_swarm->set(20, p);
    
    
    // Now generate a vector of three new parcels
    p.tag( 1.0);
    p.clearNoTrace();
    p_new = pgen.create_vector( p, 3 );
    
    if ( filter.source() != NULLPTR ) {
       cerr << "uninitialized filter filter source is non-null  " << endl;
       exit(1);
    }
    filter.source( p_new );
    if ( filter.source() != p_new ) {
       cerr << "initialized filter filter source is correct  " << endl;
       exit(1);
    }
    
    
    // now filter the points
    filter.apply( *p_flock );

    count_old = 0;
    count_new = 0;
    for ( int i=0; i<n; i++ ) {
        p = p_flock->get( i, 0 );
        if ( ! p.queryNoTrace() ) {
           tag =  p.tag();
           if ( tag == -1.0 ) {
              count_old ++;
           }
           if ( tag == 1.0 ) {
              count_new ++;
           }
        }
    }
    if ( (count_old != (n - 4)) || (count_new != 3) ) {
       cerr << "flock filter resulted in  " << count_old 
            << " and " << count_new << " instead of " 
            << n - 4 << " and " << 3 << endl;
       exit(1);           
    }

    filter.apply( *p_swarm );
    count_old = 0;
    count_new = 0;
    for ( int i=0; i<n; i++ ) {
        p = p_swarm->get( i, 0 );
        if ( ! p.queryNoTrace() ) {
           tag =  p.tag();
           if ( tag == -1.0 ) {
              count_old ++;
           }
           if ( tag == 1.0 ) {
              count_new ++;
           }
        }
    }
    if ( (count_old != (n - 4)) || (count_new != 3) ) {
       cerr << "swarm filter resulted in  " << count_old 
            << " and " << count_new << " instead of " 
            << n - 4 << " and " << 3 << endl;
       exit(1);           
    }
    
    
    delete p_new;
    delete p_swarm;
    delete p_flock;

    //------------------------------------------------------------------
    /* Shut down MPI */
    pgrp->shutdown();

    // if we got this far, everything is OK
    exit(0);

}    
