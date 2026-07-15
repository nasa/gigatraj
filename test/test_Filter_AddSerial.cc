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
    
    p_array = pgen.create_array( p, n );
    p_vector = pgen.create_vector( p, n );
    p_list = pgen.create_list( p, n );
    p_deque = pgen.create_deque( p, n );
    p_flock = pgen.create_Flock( p, n );
    p_swarm = pgen.create_Swarm( p, n );
    
    // set flags on four Parcels
    (p_array[ 5]).setNoTrace();
    (p_array[10]).setNoTrace();
    (p_array[15]).setNoTrace();
    (p_array[20]).setNoTrace();

    ((*p_vector)[ 5]).setNoTrace();
    ((*p_vector)[10]).setNoTrace();
    ((*p_vector)[15]).setNoTrace();
    ((*p_vector)[20]).setNoTrace();
    
    list_i = p_list->begin();
    list_i++;
    list_i++;
    list_i++;
    list_i++;
    list_i++;
    list_i->setNoTrace();
    list_i++;
    list_i++;
    list_i++;
    list_i++;
    list_i++;
    list_i->setNoTrace();
    list_i++;
    list_i++;
    list_i++;
    list_i++;
    list_i++;
    list_i->setNoTrace();
    list_i++;
    list_i++;
    list_i++;
    list_i++;
    list_i++;
    list_i->setNoTrace();
    
    deque_i = p_deque->begin();
    deque_i++;
    deque_i++;
    deque_i++;
    deque_i++;
    deque_i++;
    deque_i->setNoTrace();
    deque_i++;
    deque_i++;
    deque_i++;
    deque_i++;
    deque_i++;
    deque_i->setNoTrace();
    deque_i++;
    deque_i++;
    deque_i++;
    deque_i++;
    deque_i++;
    deque_i->setNoTrace();
    deque_i++;
    deque_i++;
    deque_i++;
    deque_i++;
    deque_i++;
    deque_i->setNoTrace();

    ((*p_flock)[ 5]).setNoTrace();
    ((*p_flock)[10]).setNoTrace();
    ((*p_flock)[15]).setNoTrace();
    ((*p_flock)[20]).setNoTrace();
    
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
    filter.apply( p_array, n );
    count_old = 0;
    count_new = 0;
    for ( int i=0; i<n; i++ ) {
        if ( ! (p_array[i]).queryNoTrace() ) {
           tag =  (p_array[i]).tag();
           if ( tag == -1.0 ) {
              count_old ++;
           }
           if ( tag == 1.0 ) {
              count_new ++;
           }
        }
    }
    if ( (count_old != (n - 4)) || (count_new != 3) ) {
       cerr << "array filter resulted in  " << count_old 
            << " and " << count_new << " instead of " 
            << n - 4 << " and " << 3 << endl;
       exit(1);           
    }
    
        
    
    filter.apply( *p_vector );
    count_old = 0;
    count_new = 0;
    for ( int i=0; i<n; i++ ) {
        if ( ! (*p_vector)[i].queryNoTrace() ) {
           tag =  (*p_vector)[i].tag();
           if ( tag == -1.0 ) {
              count_old ++;
           }
           if ( tag == 1.0 ) {
              count_new ++;
           }
        }
    }
    if ( (count_old != (n - 4)) || (count_new != 3) ) {
       cerr << "vector filter resulted in  " << count_old 
            << " and " << count_new << " instead of " 
            << n - 4 << " and " << 3 << endl;
       exit(1);           
    }
    
    filter.apply( *p_list );
    count_old = 0;
    count_new = 0;
    for ( list_i=p_list->begin(); list_i != p_list->end(); list_i++ ) {
        if ( ! list_i->queryNoTrace() ) {
           tag =  list_i->tag();
           if ( tag == -1.0 ) {
              count_old ++;
           }
           if ( tag == 1.0 ) {
              count_new ++;
           }
        }
    }
    if ( (count_old != (n - 4)) || (count_new != 3) ) {
       cerr << "list filter resulted in  " << count_old 
            << " and " << count_new << " instead of " 
            << n - 4 << " and " << 3 << endl;
       exit(1);           
    }
    
    filter.apply( *p_deque );
    count_old = 0;
    count_new = 0;
    for ( deque_i=p_deque->begin(); deque_i != p_deque->end(); deque_i++ ) {
        if ( ! deque_i->queryNoTrace() ) {
           tag =  deque_i->tag();
           if ( tag == -1.0 ) {
              count_old ++;
           }
           if ( tag == 1.0 ) {
              count_new ++;
           }
        }
    }
    if ( (count_old != (n - 4)) || (count_new != 3) ) {
       cerr << "deque filter resulted in  " << count_old 
            << " and " << count_new << " instead of " 
            << n - 4 << " and " << 3 << endl;
       exit(1);           
    }

    filter.apply( *p_flock );
    count_old = 0;
    count_new = 0;
    for ( int i=0; i<n; i++ ) {
        if ( ! (*p_flock)[i].queryNoTrace() ) {
           tag =  (*p_flock)[i].tag();
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
        if ( ! (*p_swarm)[i].queryNoTrace() ) {
           tag =  (*p_swarm)[i].tag();
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
    delete p_deque;
    delete p_list;
    delete p_vector;
    delete[] p_array;

    //------------------------------------------------------------------

    // if we got this far, everything is OK
    exit(0);

}    
