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
     Test program for the SerialGrp parallel processing class
*/

#include <stdlib.h>     
#include <math.h>
#include <iostream>
#include <string>


#include "gigatraj/gigatraj.hh"
#include "gigatraj/SerialGrp.hh"
#include "gigatraj/Parcel.hh"

#include "test_utils.hh"

using namespace gigatraj;
using std::cerr;
using std::endl;

int main(int argc, char* argv[]) 
{
    ProcessGrp *grp_a, *grp_b, *grp_c, *grp_d;
    real rv1, rv2;
    int  iv1, iv2;
    double dv1, dv2;
    int status;
    int i;
    int imroot ;
    int grpsize;
    int me;
    ProcessGrp::ProcessRole ptype;
    int pscpy;
    Parcel* p1;
    Parcel* p2;
    real lon, lat, z;
    string str;
    

    /* make a group of all the processors */
    grp_a = new SerialGrp();
    
    // =========================  method size
    grpsize = grp_a->size();
    if ( grpsize <= 0 ) {
       cerr << "[" << me << "] Base group size: " << grpsize  << endl;
       exit(1);
    }
    
    // =========================  method is_root
    // =========================  method id
    // =========================  method root_id    
    imroot = grp_a->is_root(); 
    me = grp_a->id();
    if ( imroot && me != grp_a->root_id() ) {
       cerr << "[" << me << "] Root ID does not match: " << me << " vs. " << grp_a->root_id() << endl;
       exit(1);
    } 
    if ( ! imroot && me == grp_a->root_id() ) {
       cerr << "[" << me << "] Root ID matches when it shouldn't: " << me << endl;
       exit(1);
    } 
    
    
    // =========================  method setType
    // =========================  method sync
    // =========================  method send_reals
    // =========================  method send_doubles
    // =========================  method send_ints
    // =========================  method send_string
    // =========================  method receive_reals
    // =========================  method receive_doubles
    // =========================  method receive_ints
    // =========================  method receive_string

    // test transmission of numeric values from one
    // processor to another
    if ( imroot ) {
       // these values will be transmitted between processes
       rv1 = 345.6;
       dv1 = 23425.43;
       iv1 = 42;
       str = "Just testing";
       
       grp_a->setType( me, ProcessGrp::PGrpRole_Coordinator );
    
       //cerr << "I am root (" << me << ")" << endl;

       grp_a->sync();
       
       for ( int i = 0; i < grp_a->size(); i++ ) {
          if ( me != i ) {
             
             grp_a->send_reals( i, 1, &rv1 );
             grp_a->send_doubles( i, 1, &dv1 );
             grp_a->send_ints( i, 1, &iv1 );
             grp_a->send_string( i, str );
             
          }
       }
    } else {
       rv1 = -1.0;
       dv1 = -1.0;
       iv1 = -1;
       str = "nuthin";
       grp_a->setType( me, ProcessGrp::PGrpRole_Tracer );

       //cerr << "I am NOT root (" << me << ")" << endl;
       
       grp_a->sync();
       
       grp_a->receive_reals( 0, 1, &rv1 );
       grp_a->receive_doubles( 0, 1, &dv1 );
       grp_a->receive_ints( 0, 1, &iv1 );
       grp_a->receive_string( 0, &str );
       
       
    }    
    //cerr << "rv1=" << rv1 << ", dv1=" << dv1 << ", iv1=" << iv1 << endl;
    if ( mismatch(rv1, 345.6) ) {
       cerr << "[" << me << "] Bad rv1 transmission: 345.6 != " << rv1 << endl;
       exit(1);
    } 
    if ( mismatch(dv1, 23425.43 ) ) {
       cerr << "[" << me << "] Bad dv1 transmission:  != " << dv1 << endl;
       exit(1);
    }     
    if ( mismatch(iv1, 42 ) ) {
       cerr << "[" << me << "] Bad iv1 transmission:  != " << iv1 << endl;
       exit(1);
    }
    if ( str.compare("Just testing") != 0 )  {
       cerr << "[" << me << "] Bad str transmission:  != " << iv1 << endl;
       exit(1);
    }

    // =========================  method type
    grp_a->sync();
    ptype = grp_a->type();
    if ( imroot && ptype != ProcessGrp::PGrpRole_Coordinator ) {
       cerr << "[" << me << "] Root process Type is wrong" << endl;
       exit(1);
    } 
    if ( ! imroot && ptype != ProcessGrp::PGrpRole_Tracer ) {
       cerr << "[" << me << "] Non-Root process Type is wrong" << endl;
       exit(1);
    } 
    

    // =========================  method subgroup
    // =========================  method belongs
    // test creating a subgroup
    grp_b = grp_a->subgroup(2);
    if ( grp_b->belongs() ) {
    
       if ( me >= 2 ) { 
          cerr << "[" << me << "] is in subgroup b when it should NOT be!" << endl;
          exit(1);
       } 

       // test communicating through this subgroup
       if ( grp_b->is_root() ) {
          iv1 = 56;
          for ( int i=0; i < grp_b->size(); i++ ) {
              if ( i != grp_b->id() ) {
                 grp_b->send_ints( i, 1, &iv1 );
              }
          }    
       
       } else {
          
          grp_b->receive_ints( 0, 1, &iv1 );

       }
       if ( mismatch(iv1, 56 ) ) {
          cerr << "[" << me << "] Bad iv1 transmission in subgroup b:  != " << iv1 << endl;
          exit(1);
       } 
       
       
    } else {
    
       if ( me < 2 ) { 
          cerr << "[" << me << "] is NOT in subgroup b when it should be!" << endl;
          exit(1);
       } 
    
    }

    delete grp_b;


    grp_a->sync();
    
    // subgroup again, this time asking for too many processes
    grp_b = grp_a->subgroup( grp_a->size() + 1 );
    if ( grp_a->size() != grp_b->size() ) {
       cerr << "[" << me << "] too-big subgroup b created with wrong size!" << endl;
       exit(1);
    }
    delete grp_b;

    // subgroup again, again time asking for too many processes, but with strictness this time
    status = 1;
    try {
       grp_b = grp_a->subgroup( grp_a->size() + 1, PG_STRICT );
    } catch ( ProcessGrp::badgroupsize err) {
       status = 0;
    }
    if ( status ) {
       cerr << "[" << me << "] too-big subgroup b, but created anyway!" << endl;
       exit(1);
    }


    

    // split the "a" group in two again, this time asking for the first group to be too big
    grp_a->split( grp_a->size()+1, &grp_b, &grp_c );
    if ( grp_c != NULLPTR ) {
       cerr << "[" << me << "] too-big split group c is non-NULL!" << endl;
       exit(1);    
    }
    delete grp_b;

    // =========================  method split
    status = 1;
    try {
       grp_a->split( grp_a->size() + 1, &grp_b, &grp_c, PG_KEEPROOT | PG_STRICT );
    } catch ( ProcessGrp::badgroupsize err) {
       status = 0;
    }
    if ( status ) {
       cerr << "[" << me << "] too-big split group b, but created anyway!" << endl;
       exit(1);
    }


    // =========================  method copy
    // copy 
    grp_b = grp_a->copy();
    if ( grp_b->size() != grp_a->size() ) {
       cerr << "[" << me << "] copy a->b failed!" << endl;
       exit(1);
    }


    // =========================  method sync #2
    grp_a->sync(0);


    // test dummy interprocess send/receive of parcels
    p1 = new Parcel;
    p1->setPos( 11.0, 22.0);
    p1->setZ(33.0);
    p2 = new Parcel;
    p2->setPos( 44.0, 55.0);
    p2->setZ(66.0);

    // =========================  method Parcel::send (deferred from test_Parcel)
    p1->send(grp_a,0);
    // =========================  method Parcel::receive (deferred from test_Parcel)
    p2->receive(grp_a,0);

    z = p1->getZ();
    p1->getPos( &lon, &lat );
    if ( mismatch( lon, 11.0 ) || mismatch( lat, 22.0 ) || mismatch(z, 33.0) ) {
       cerr << "Bad p1 copy: " << lat << " " << lon << " " << z << endl;
       exit(1);
    }
    z = p2->getZ();
    p2->getPos( &lon, &lat );
    if ( mismatch( lon, 44.0 ) || mismatch( lat, 55.0 ) || mismatch(z, 66.0) ) {
       cerr << "Bad p2 copy: " << lat << " " << lon << " " << z << endl;
       exit(1);
    }
    delete p1;
    delete p2;


    delete grp_a;


    // if we got this far, everything is OK
    exit(0);

}
    
