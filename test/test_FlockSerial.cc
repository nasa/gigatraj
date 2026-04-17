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


// Unfinished

#include <iostream>
#include <sstream>
#include "gigatraj/gigatraj.hh"
#include "gigatraj/Parcel.hh"
#include "gigatraj/SerialGrp.hh"
#include "gigatraj/Flock.hh"

#include "test_utils.hh"

using namespace gigatraj;
using std::cerr;
using std::endl;
using std::ostringstream;
using std::istringstream;
using std::operator<<;
using std::operator>>;

int main() 
{

    Parcel p;
    Parcel *pc;
    real lon;
    real lat;
    real z;
    int status;
    Flock *flk;
    Flock::iterator iter;
    SerialGrp *pgrp;
    int k;
    ParcelFlag checkflag;

    // create a process group (serial, of course)
    pgrp = new SerialGrp();

    // create a Flock of 100 parcels
    flk = new Flock( p, pgrp, 100, 0);

    if ( flk->size() != 100 ) {
       cerr << "Flock size not set properly: " << flk->size() << endl;
       exit(1);
    }
    
    // set all parcels in the flock
    //  Note: the proper way to set parcels by their index value
    //  is by the set() method.  Using the iterator here works
    //  only for serial processing.  This test is merely to
    //  check that the iterator works.
    k=0;
    for ( iter=flk->begin(); iter!=flk->end(); iter++ ) {
       //cerr << "iter " << k << endl;
       
       lon = k*10.0;
       lat = 80.0 - k*1.0;
       iter->setPos(lon,lat);
       
       // set a Flag on two of these
       if ( k == 30 || k == 60 ) {
           iter->setNoTrace();
       }
       
       k++;
    }
    // set the height of each parcel, this time
    // using the set() method
    for ( k=0; k<100; k++ ) {
        
        z = 300.0+k;
        
        p = flk->get(k);
        lon = -999.0;
        lat = -999.0;
        p.getPos(&lon,&lat);
        if ( mismatch( lat, 80.0 - k*1.0 ) || mismatch( lon, k*10.0 ) ) {
           cerr << "Bad lon/lat retrieval on " << k << ": ("
           << k*10.0 << ", " << 80-k*1.0 << ") != (" 
           << lon << ", " << lat << ")" << endl;
           exit(1);
        }   
        
        p.setZ(z);
        flk->set(k, p);

    }
    // check each parcel in the flock
    //  Note: the proper way to get parcels by their index value
    //  is by the get() method.  Using the iterator here works
    //  only for serial processing.  This test is merely to
    //  check that the iterator works.
    k=0;
    for ( iter=flk->begin(); iter!=flk->end(); iter++ ) {
       
       lon = -999.0;
       lat = -999.0;
       z = -999.0;
       iter->getPos(&lon,&lat);
       z = iter->getZ();
       checkflag = iter->flags();
       if ( mismatch( lat, 80.0 - k*1.0 ) || mismatch( lon, k*10.0 ) 
       || mismatch( z, 300.0+k) ) {
          cerr << "Bad lon,lat,z retrieval on " << k << ":"
          << "(" << k*10.0 << ", " << 80-k*1.0 << ", " << 300.0+k << ") != "
          << "(" << lon << ", " << lat << ", " << z << ")" << endl;
          exit(1);
       } 
       
       if ( k == 30 || k == 60 ) {
           if ( checkflag != NoTrace ) {
              cerr << "Parcel flagset for " << k << " is " << checkflag 
              << " instead of " << NoTrace << endl;
              exit(1);
           }
       } else {
           if ( checkflag != 0 ) {
              cerr << "Parcel flagset for " << k << " is " << checkflag 
              << " instead of " << 0 << endl;
              exit(1);
           }   
       }
       
       
       k++;
    }
    // test [] syntax
    for ( k=0; k<flk->size(); k++ ) {
       lon = -999.0;
       lat = -999.0;
       z = -999.0;
       p = (*flk)[k];
       p.getPos(&lon,&lat);
       z = p.getZ();
       if ( mismatch( lat, 80.0 - k*1.0 ) || mismatch( lon, k*10.0 ) 
       || mismatch( z, 300.0+k) ) {
          cerr << "Bad lon,lat,z retrieval on " << k << ":"
          << "(" << k*10.0 << ", " << 80-k*1.0 << ", " << 300.0+k << ") != "
          << "(" << lon << ", " << lat << ", " << z << ")" << endl;
          exit(1);
       }   
    
    }
    
    // add a new parcel to the flock
    flk->add(p);
    if ( flk->size() != 101 ) {
       cerr << "Flock size incremented improperly: " << flk->size() << endl;
       exit(1);
    }
    
    // check the count
    int cnt;
    cnt = flk->countFlags( NoTrace, false );
    if ( cnt != 2 ) {
       cerr << "Flock countFlags returned " << cnt << " instead of " << 2 << endl;
       exit(1);
    }


    delete flk;

    exit(0);
}
