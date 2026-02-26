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

/*!
     Test program for the MetSBRot (Solid-Body Rotation met source) class
*/
     
#include <iostream>
#include <math.h>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/MetSBRot.hh"

#include "test_utils.hh"

using namespace gigatraj;
using std::cerr;
using std::endl;


int main() 
{

    MetSBRot *metsrc;
    real lon;
    real lat;
    real u,v,w;
    real val;
    int status;
    string time1, time2;
    double time0;
    int testInt;
    float testFloat;
    double testDouble;
    std::string testString;
    int testInt0;
    float testFloat0;
    double testDouble0;
    std::string testString0;
    
    /* create an object with the default values (40 m/s, no tilt) */
    metsrc = new MetSBRot;
    
    if ( metsrc->id() != string("MetSBRot") )
    {
       cerr << "Bad ID" << endl;
       exit(1);
    }

    // Check configuration settings
    if ( ! metsrc->getOption("test", testInt ) ) {
       cerr << "Could not getOption for Int 'test' : " << endl;
       exit(1);         
    }
    if ( ! metsrc->getOption("test", testFloat ) ) {
       cerr << "Could not getOption for Float 'test' : " << endl;
       exit(1);         
    }
    if ( ! metsrc->getOption("test", testDouble ) ) {
       cerr << "Could not getOption for Double 'test' : " << endl;
       exit(1);         
    }
    if ( ! metsrc->getOption("test", testString ) ) {
       cerr << "Could not getOption for String 'test' : " << endl;
       exit(1);         
    }

    testInt0 = -987;
    testFloat0 = -654.3;
    testDouble0 = -210.9;
    testString0 = "Hello World"; 

    metsrc->setOption("test", testInt0 );
    metsrc->setOption("test", testFloat0 );
    metsrc->setOption("test", testDouble0 );
    metsrc->setOption("test", testString0 );

    if ( ! metsrc->getOption("test", testInt ) ) {
       cerr << "Could not getOption for Int 'test' : " << endl;
       exit(1);         
    }
    if ( testInt != testInt0 ) {
       cerr << "Bad Int option set : " << testInt << " vs " << testInt0   << endl;
       exit(1);    
    }
    if ( ! metsrc->getOption("test", testFloat ) ) {
       cerr << "Could not getOption for Float 'test' : " << endl;
       exit(1);         
    }
    if ( testFloat != testFloat0 ) {
       cerr << "Bad Float option set : " << testFloat << " vs " << testFloat0   << endl;
       exit(1);    
    }
    if ( ! metsrc->getOption("test", testDouble ) ) {
       cerr << "Could not getOption for Double 'test' : " << endl;
       exit(1);         
    }
    if ( testDouble != testDouble0 ) {
       cerr << "Bad Double option set : " << testDouble << " vs " << testDouble0   << endl;
       exit(1);    
    }
    if ( ! metsrc->getOption("test", testString ) ) {
       cerr << "Could not getOption for String 'test' : " << endl;
       exit(1);         
    }
    if ( testString != testString0 ) {
       cerr << "Bad String option set : " << testString << " vs " << testString0   << endl;
       exit(1);    
    }
    
    
    // check the date conversion routines
    time1 = "3.1";
    time0 = metsrc->cal2Time( time1 );
    if ( mismatch( time0, 3.1 ) ) {
       cerr << "Bad time conversion : " << time0 << " vs " << time1   << endl;
       exit(1);  
    }
    time2 = metsrc->time2Cal( time0 );
    if ( time1 != time2 ) {
       cerr << "Bad reverse-time conversion : " << time2 << " vs " << time1   << endl;
       exit(1);  
    }
      
    
    /* sample the wind at the equator */
    metsrc->get_uvw( 0.0, 0.0, 0.0, 0.0, &u, &v, &w);  
    if ( mismatch(u, 40.0) || mismatch(v,0.0) || mismatch(w, 0.0) ) {
       cerr << "Bad equ. wind val 0-1 : (" << u << ", " << v  << ", " << w << ")" << endl;
       exit(1);  
    } 
    /* sample the wind at 45 N */
    metsrc->get_uvw( 0.0, 0.0, 45.0, 0.0, &u, &v, &w);  
    if ( mismatch(u, 28.28427125) || mismatch(v,0.0) || mismatch(w, 0.0) ) {
       cerr << "Bad equ. wind val 0-2 : (" << u << ", " << v  << ", " << w << ")" << endl;
       exit(1);  
    } 
    /* sample the wind at 45 S */
    metsrc->get_uvw( 0.0, 0.0, -45.0, 0.0, &u, &v, &w);  
    if ( mismatch(u, 28.28427125) || mismatch(v,0.0) || mismatch(w, 0.0) ) {
       cerr << "Bad equ. wind val 0-3 : (" << u << ", " << v  << ", " << w << ")" << endl;
       exit(1);  
    } 

    delete metsrc;


    /* create a new object, tilted */
    metsrc = new MetSBRot(53.0, 11.0, 38.0, -4.5);
    /* sample the wind at the equator */
    metsrc->get_uvw( 0.0, 0.0, 0.0, 0.0, &u, &v, &w);  
    if ( mismatch(u, 41.7646) || mismatch(v,-32.0305) || mismatch(w, 0.0) ) {
       cerr << "Bad equ. wind val 1-1 : (" << u << ", " << v  << ", " << w << ")" << endl;
       exit(1);  
    } 
    /* sample the wind at 45 N */
    metsrc->get_uvw( 0.0, 0.0, 45.0, 0.0, &u, &v, &w);  
    if ( mismatch(u,25.1295 ) || mismatch(v,-32.0305) || mismatch(w, 0.0) ) {
       cerr << "Bad equ. wind val 1-2 : (" << u << ", " << v  << ", " << w << ")" << endl;
       exit(1);  
    } 
    /* sample the wind at 45 S */
    metsrc->get_uvw( 0.0, 0.0, -45.0, 0.0, &u, &v, &w);  
    if ( mismatch(u,33.9345 ) || mismatch(v, -32.0305) || mismatch(w, 0.0) ) {
       cerr << "Bad equ. wind val 1-3 : (" << u << ", " << v  << ", " << w << ")" << endl;
       exit(1);  
    } 
    /* sample the wind at 90 N */
    metsrc->get_uvw( 0.0, 0.0, 90.0, 0.0, &u, &v, &w);  
    if ( mismatch(u,-6.22611 ) || mismatch(v,-32.0306) || mismatch(w, 0.0) ) {
       cerr << "Bad equ. wind val 1-4 : (" << u << ", " << v  << ", " << w << ")" << endl;
       exit(1);  
    } 
    /* sample the wind at 90 S */
    metsrc->get_uvw( 0.0, 0.0, -90.0, 0.0, &u, &v, &w);  
    if ( mismatch(u, 6.22611) || mismatch(v,-32.0306) || mismatch(w, 0.0) ) {
       cerr << "Bad equ. wind val 1-5 : (" << u << ", " << v  << ", " << w << ")" << endl;
       exit(1);  
    } 
    
    /* re-sample the wind at 45 N */
    metsrc->get_uvw( 0.0, 0.0, 45.0, 0.0, &u, &v, &w);  
    /* check against convenience routines */
    val = metsrc->get_u( 0.0, 0.0, 45.0, 0.0 );
    if ( mismatch(u, val) ) {
       cerr << "Bad 45-deg U-wind val: " << u << " vs. " << val << endl;
       exit(1);  
    } 
    val = metsrc->get_v( 0.0, 0.0, 45.0, 0.0 );
    if ( mismatch(v, val) ) {
       cerr << "Bad 45-deg V-wind val: " << v << " vs. " << val << endl;
       exit(1);  
    } 
    val = metsrc->get_w( 0.0, 0.0, 45.0, 0.0 );
    if ( mismatch(w, val) ) {
       cerr << "Bad 45-deg W-wind val: " << w << " vs. " << val << endl;
       exit(1);  
    } 
    val = metsrc->getData( "u", 0.0, 0.0, 45.0, 0.0 );
    if ( mismatch(u, val) ) {
       cerr << "Bad! 45-deg U-wind value: " << u << " vs. " << val << endl;
       exit(1);  
    } 
    
    real trop = 11.3892; //from local GSFC ACD IDL wmo_trop() routine
    val = metsrc->getData( "tropz", 0.0, 0.0, 45.0, 0.0 );
    if ( mismatch( trop, val, 0.01) ) {
       cerr << "Bad! 45-deg trop value: " << trop << " vs. " << val << endl;
       exit(1);  
    } 
    val = metsrc->getData( "tropz", 0.0, 180.0, -45.0, 0.0 );
    if ( mismatch( trop, val, 0.01) ) {
       cerr << "Bad! -45-deg trop value: " << trop << " vs. " << val << endl;
       exit(1);  
    } 
    
    // if we got this far, everything is OK
    exit(0);
    
}
