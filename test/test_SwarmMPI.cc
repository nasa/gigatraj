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
#include <math.h>

#include "mpi.h"

#include "gigatraj/gigatraj.hh"
#include "gigatraj/Parcel.hh"
#include "gigatraj/MPIGrp.hh"
#include "gigatraj/Swarm.hh"

#include "test_utils.hh"

using namespace gigatraj;
using std::cerr;
using std::endl;
using std::ostringstream;
using std::istringstream;
using std::operator<<;
using std::operator>>;

int main(int argc, char* argv[]) 
{

    Parcel p;
    Parcel *pc;
    real lon;
    real lat;
    real z;
    real xlon, xlat, xz;
    real lon0, lat0, z0;
    int status;
    Swarm *swm, *swm2;
    Swarm::iterator iter;
    MPIGrp *pgrp;
    int k;
    int n;
    int my_id;
    int nprocs;
    ParcelFlag checkflag;
    ParcelStatus checkstatus;
    int cnt;
    
    n = 104;
    
    /* start up MPI */
    //MPI_Init(&argc, &argv);

    // create a process group (MPI, of course)
    // This will call MPI_Init().
    pgrp = new MPIGrp(argc, argv);
    
    my_id = pgrp->id();
    nprocs = pgrp->size();

    // create a Swarm of n parcels
    swm = new Swarm( p, pgrp, n, 0);
    pgrp->sync();


    if ( swm->size() != n ) {
       cerr << "Swarm size not set properly: " << swm->size() << endl;
       pgrp->shutdown();
       exit(1);
    }
    pgrp->sync();

    //swm->dump();
    pgrp->sync();

    // set the height of each parcel, 
    // using the set() method as is appropriate for multi-processing
    for ( k=0; k<n; k++ ) {
        
        if ( my_id == 0 ) {
           z = 300.0+k;
           lon = k*10.0;
           lat = 80.0 - k*1.0;
        } else {
           z = -999.0;
           lon = 0.0;
           lat = 0.0;
        }   
        p.setZ(z);
        p.setPos(lon,lat);

        // set a Flag on two of these
        if ( k == 30 || k == 60 ) {
           p.setNoTrace();
           p.setHitBdy();
        }

        swm->set(k, p);
     
        p.clearNoTrace();
        p.clearHitBdy();
        
    }
    pgrp->sync();


    
    // test get 
    for ( k=0; k<swm->size(); k++ ) {
       lon = -999.0;
       lat = -999.0;
       z = -999.0;
       p = swm->get(k);
       p.getPos(&lon,&lat);
       z = p.getZ();
       checkflag = p.flags();
       checkstatus = p.status();
       if ( mismatch( lat, 80.0 - k*1.0 ) || mismatch( lon, k*10.0 ) 
       || mismatch( z, 300.0+k) ) {
          cerr << "A: Bad lon,lat,z retrieval on " << k << ":"
          << "(" << k*10.0 << ", " << 80-k*1.0 << ", " << 300.0+k << ") != "
          << "(" << lon << ", " << lat << ", " << z << ")" << endl;
          pgrp->shutdown();
          exit(1);
       }   
    
       if ( k == 30 || k == 60 ) {
           if ( checkflag != NoTrace ) {
              cerr << "Parcel flagset for " << k << " is " << checkflag 
              << " instead of " << NoTrace << endl;
              exit(1);
           }
           if ( checkstatus != HitBdy ) {
              cerr << "Parcel status for " << k << " is " << checkstatus 
              << " instead of " << HitBdy << endl;
              exit(1);
           }
       } else {
           if ( checkflag != 0 ) {
              cerr << "Parcel flagset for " << k << " is " << checkflag 
              << " instead of " << 0 << endl;
              exit(1);
           }   
           if ( checkstatus != 0 ) {
              cerr << "Parcel status for " << k << " is " << checkstatus 
              << " instead of " << 0 << endl;
              exit(1);
           }   
       }
    }
    pgrp->sync();
    


    // test get with [] syntax
    for ( k=0; k<swm->size(); k++ ) {
       lon = -999.0;
       lat = -999.0;
       z = -999.0;
       p = (*swm)[k];
       p.getPos(&lon,&lat);
       z = p.getZ();
       if ( mismatch( lat, 80.0 - k*1.0 ) || mismatch( lon, k*10.0 ) 
       || mismatch( z, 300.0+k) ) {
          cerr << "B: Bad lon,lat,z retrieval on " << k << ":"
          << "(" << k*10.0 << ", " << 80-k*1.0 << ", " << 300.0+k << ") != "
          << "(" << lon << ", " << lat << ", " << z << ")" << endl;
          pgrp->shutdown();
          exit(1);
       }   
    
    }
    pgrp->sync();
    
    // test parallelized get 
    for ( k=0; k<swm->size(); k++ ) {
       lon = -999.0;
       lat = -999.0;
       z = -999.0;
       pc = swm->parcel(k, 1);
       if ( pc != NULLPTR ) {
          pc->getPos(&lon,&lat);
          z = pc->getZ();
          if ( my_id == 0 ) {
             if ( mismatch( lat, 80.0 - k*1.0 ) || mismatch( lon, k*10.0 ) 
             || mismatch( z, 300.0+k) ) {
                cerr << "C: Bad lon,lat,z retrieval on " << k << ":"
                << "(" << k*10.0 << ", " << 80-k*1.0 << ", " << 300.0+k << ") != "
                << "(" << lon << ", " << lat << ", " << z << ")" << endl;
                pgrp->shutdown();
                exit(1);
             }   
          }
       }   
    
    }

    pgrp->sync();

    // now test getting and setting parcels with an iterator
    for ( iter=swm->begin(); iter!=swm->end(); iter++ ) {
       
       lon0 = -999.0;
       lat0 = -999.0;
       z0 = -999.0;
       iter->getPos(&lon0,&lat0);
       z0 = iter->getZ();
       
       k = iter.index();
       lon = (k*10.0)/4.0;
       lat = (80.0 - k*1.0)/3.0;
       z = (300.0+k)/2.0;
       
       iter->setPos(lon,lat);
       iter->setZ(z);

       // check that we just set what we thought we did
       iter->getPos(&xlon,&xlat);
       xz = iter->getZ();       
       if ( mismatch( lat, xlat) || mismatch( lon, xlon ) || mismatch( z, xz ) ) {
          cerr << "pow! D: Bad lon,lat,z retrieval on " << k << ":"
          << "(" << xlon << ", " << xlat << ", " << xz << ") != "
          << "(" << lon << ", " << lat << ", " << z << ")" << ", was "
          << "(" << lon0 << ", " << lat0 << ", " << z0 << ")" << endl;
          pgrp->shutdown();
          exit(1);
       } 
       
       
       // now try to access the parcel by dereferencing the iterator this way
       xlon = -999.0;
       xlat = -999.0;
       xz = -999.0;
       (*iter).getPos(&xlon,&xlat);
       xz = (*iter).getZ();       
       if ( mismatch( lat, xlat) || mismatch( lon, xlon ) || mismatch( z, xz ) ) {
          cerr << "pow! E: Bad lon,lat,z retrieval on " << k << ":"
          << "(" << xlon << ", " << xlat << ", " << xz << ") != "
          << "(" << lon << ", " << lat << ", " << z << ")" << endl;
          pgrp->shutdown();
          exit(1);
       } 
        
    }
    pgrp->sync();

    // did our changes stick?
    for ( iter=swm->begin(); iter!=swm->end(); iter++ ) {
       
       lon = -999.0;
       lat = -999.0;
       z = -999.0;
       iter->getPos(&lon,&lat);
       z = iter->getZ();
       
       k = iter.index();
       
       xlon = (k*10.0)/4.0;
       xlat = (80.0 - k*1.0)/3.0;
       xz = (300.0+k)/2.0;
       
       if ( mismatch( lat, xlat) || mismatch( lon, xlon ) || mismatch( z, xz ) ) {
          cerr << "uhoh F: Bad lon,lat,z retrieval on " << k << ":"
          << "(" << xlon << ", " << xlat << ", " << xz << ") != "
          << "(" << lon << ", " << lat << ", " << z << ")" << endl;
          pgrp->shutdown();
          exit(1);
       }   
       
    }
    pgrp->sync();

    // (verify that the parcels were indeed set correctly above)
    for ( k=0; k<swm->size(); k++ ) {
    
       xlon = (k*10.0)/4.0;
       xlat = (80.0 - k*1.0)/3.0; 
       xz = (300.0+k)/2.0;
    
       lon = -999.0;
       lat = -999.0;
       z = -999.0;
       //p = swm->get(k);
       pc = swm->parcel(k,0);
       pc->getPos(&lon,&lat);
       z = pc->getZ();
       if ( mismatch( lat, xlat) || mismatch( lon, xlon ) || mismatch( z, xz ) ) {
          cerr << "G: Bad lon,lat,z retrieval on " << k << ":"
          << "(" << xlon << ", " << xlat << ", " << xz << ") != "
          << "(" << lon << ", " << lat << ", " << z << ")" << endl;
          pgrp->shutdown();
          exit(1);
       }   
    
    }

    pgrp->sync();

    
    // check the flags count
    cnt = swm->countFlags( NoTrace, false );
    if ( cnt != 2 ) {
       cerr << "Swarm countFlags returned " << cnt << " instead of " << 2 << endl;
       exit(1);
    }
    // check the status count
    cnt = swm->countStatus( HitBdy, false );
    if ( cnt != 2 ) {
       cerr << "Swarm countStatus returned " << cnt << " instead of " << 2 << endl;
       exit(1);
    }

    pgrp->sync();


    delete swm;

    pgrp->sync();

    // create a Swarm of n parcels, met met processing
    swm = new Swarm( p, pgrp, n, nprocs/3);

    if ( swm->size() != n ) {
       cerr << "Swarm-met size not set properly: " << swm->size() << endl;
       pgrp->shutdown();
       exit(1);
    }
    
    // set the height of each parcel, 
    // using the set() method as is appropriate for multi-processing
    for ( k=0; k<n; k++ ) {
        
        if ( my_id == 0 ) {
           z = 300.0+k;
           lon = k*10.0;
           lat = 80.0 - k*1.0;
        } else {
           z = -999.0;
           lon = 0.0;
           lat = 0.0;
        }   
        p.setZ(z);
        p.setPos(lon,lat);

        // set a Flag on two of these
        if ( k == 30 || k == 60 ) {
           p.setNoTrace();
           p.setHitBdy();
        }

        swm->set(k, p);
        
        p.clearNoTrace();
        p.clearHitBdy();
        
    }

   pgrp->sync();    

    // (verify that the parcels were indeed set correctly above)
    for ( k=0; k<swm->size(); k++ ) {
       lon = -999.0;
       lat = -999.0;
       z = -999.0;
       pc = swm->parcel(k);
       if ( pc != NULLPTR ) {
          pc->getPos(&lon,&lat);
          z = pc->getZ();
          if ( mismatch( lat, (80.0 - k*1.0) ) || mismatch( lon, (k*10.0) ) 
          || mismatch( z, (300.0+k) ) ) {
             cerr << "H: Bad lon,lat,z retrieval on " << k << ":"
             << "(" << (k*10.0) << ", " << (80-k*1.0) << ", " << (300.0+k) << ") != "
             << "(" << lon << ", " << lat << ", " << z << ")" << endl;
             pgrp->shutdown();
             exit(1);
          }   
       }
    }

    pgrp->sync();    

    // now test getting and setting parcels with an iterator
    for ( iter=swm->begin(); iter!=swm->end(); iter++ ) {
       
       lon = -999.0;
       lat = -999.0;
       z = -999.0;
       iter->getPos(&lon,&lat);
       z = iter->getZ();
       
       z = z/2.0;
       lon = lon/4.0;
       lat = lat/3.0;

       iter->setPos(lon,lat);
       iter->setZ(z);
       
    }

    pgrp->sync();

    // (verify that the parcels were indeed set correctly above)
    for ( k=0; k<swm->size(); k++ ) {
       lon = -999.0;
       lat = -999.0;
       z = -999.0;
       pc = swm->parcel(k);
       if ( pc != NULLPTR ) {
          pc->getPos(&lon,&lat);
          z = pc->getZ();
          if ( mismatch( lat, (80.0 - k*1.0)/3.0 ) || mismatch( lon, (k*10.0)/4.0 ) 
          || mismatch( z, (300.0+k)/2.0 ) ) {
             cerr << "I: Bad lon,lat,z retrieval on " << k << ":"
             << "(" << (k*10.0)/4.0 << ", " << (80-k*1.0)/3.0 << ", " << (300.0+k)/2.0 << ") != "
             << "(" << lon << ", " << lat << ", " << z << ")" << endl;
             pgrp->shutdown();
             exit(1);
          }   
       }   
    
    }

    pgrp->sync();    

    //swm->dump();

    // check the count
    cnt = swm->countFlags( NoTrace, false );
    if ( cnt != 2 ) {
       cerr << "Swarm countFlags returned " << cnt << " instead of " << 2 << endl;
       exit(1);
    }
    // check the status count
    cnt = swm->countStatus( HitBdy, false );
    if ( cnt != 2 ) {
       cerr << "Swarm countStatus returned " << cnt << " instead of " << 2 << endl;
       exit(1);
    }

    delete swm;

    pgrp->sync();    


    /* Shut down MPI */
    //MPI_Finalize();
    pgrp->shutdown();

    // if we got this far, everything is OK
    return 0;

}
    
