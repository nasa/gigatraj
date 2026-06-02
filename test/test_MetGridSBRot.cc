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
     Test program for the MetGridSBRot (Solid-Body Rotation met source) class
*/
     
#include <iostream>
#include <math.h>

#include <stdlib.h>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/MetSBRot.hh"
#include "gigatraj/MetGridSBRot.hh"

#include "test_utils.hh"

using namespace gigatraj;
using std::cerr;
using std::endl;


int main() 
{

    MetSBRot *metsrc0;
    MetGridSBRot *metsrc;
    real lon;
    real lat;
    real u,v,w;
    real u0, v0, w0;
    real val;
    real val0;
    int status;
    string time1, time2;
    double time0;
    double tt;
    std::vector<real> thetas;
    real thet;
    int i;
    std::string fname;
    int testInt;
    float testFloat;
    double testDouble;
    std::string testString;
    int testInt0;
    float testFloat0;
    double testDouble0;
    std::string testString0;
    std::string datdir;
    std::string cfgfile;
   
    datdir = datadir("srcdir"); 
    
    /* create an object with the default values (40 m/s, no tilt) */

    // original SBRot source
    metsrc0 = new MetSBRot(20.0, 30.0);

    // *** GRIDDED** SBRot source
    metsrc = new MetGridSBRot;
    
    //metsrc->dbug = 1;
    
    if ( metsrc->id() != string("MetGridSBRot") )
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

    cfgfile = "test_MetGridSBRot.cfg";
    if ( datdir != "" ) {
       cfgfile = datdir + "/" + cfgfile;
    }
    metsrc->configFile( cfgfile  );
    if ( metsrc->configFile() != cfgfile ) {
       cerr << "Could not set configFile() " << cfgfile << endl;
       exit(1);             
    }
    metsrc->readConfig();
    
    //metsrc->setOption("test", testInt0 );
    //metsrc->setOption("test", testFloat0 );
    //metsrc->setOption("test", testDouble0 );
    //metsrc->setOption("test", testString0 );

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
    metsrc0->get_uvw( 0.0, 0.0, 0.0, 15.0, &u0, &v0, &w0);
    metsrc->get_uvw( 0.0, 0.0, 0.0, 15.0, &u, &v, &w);  
    //if ( mismatch(u, 40.0) || mismatch(v,0.0) || mismatch(w, 0.0) ) {
    if ( mismatch(u0, u) || mismatch(v0, v) || mismatch(w0, w) ) {
       cerr << "Bad equ. wind val 0-1 : (" << u << ", " << v  << ", " << w << ")" << endl;
       cerr << "                   vs : (" << u0 << ", " << v0  << ", " << w0 << ")" << endl;
       exit(1);  
    } 
    /* sample the wind at 45 N */
    metsrc0->get_uvw( 0.0, 0.0, 45.0, 15.0, &u0, &v0, &w0);
    metsrc->get_uvw( 0.0, 0.0, 45.0, 15.0, &u, &v, &w);  
    if ( mismatch(u, u0) || mismatch(v,v0) || mismatch(w, w0) ) {
       cerr << "Bad equ. wind val 0-2 : (" << u << ", " << v  << ", " << w << ")" << endl;
       exit(1);  
    } 
    /* sample the wind at 45 S */
    metsrc0->get_uvw( 0.0, 0.0, -45.0, 0.0, &u0, &v0, &w0);  
    metsrc->get_uvw( 0.0, 0.0, -45.0, 0.0, &u, &v, &w);  
    if ( mismatch(u, u0) || mismatch(v,v0) || mismatch(w, w0) ) {
       cerr << "Bad equ. wind val 0-3 : (" << u << ", " << v  << ", " << w << ")" << endl;
       exit(1);  
    } 

    delete metsrc;
    delete metsrc0;

    /* create a new object, tilted */
    metsrc0 = new MetSBRot( 53.0, 11.0, 38.0, -4.5);
    metsrc = new MetGridSBRot(1.0, 1.0, 53.0, 11.0, 38.0, -4.5);

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

    u = metsrc0->getData( "somestuff", 0.0, 0.0, 45.0, 0.0 );
    val = metsrc->getData( "somestuff", 0.0, 0.0, 45.0, 0.0 );
    if ( mismatch(u, val) ) {
       cerr << "Bad 45-deg somestuff  value: " << u << " vs. " << val << endl;
       exit(1);  
    } 
    
    u = metsrc0->get_u( 0.0, 20.0, 45.0, 0.0 );
    val = metsrc->get_u( 0.0, 20.0, 45.0, 0.0 );
    if ( mismatch(u, val) ) {
       cerr << "Bad 45/20 U-wind val: " << u << " vs. " << val << endl;
       exit(1);  
    } 

    // now test 2D surfaces
    u = metsrc0->getData( "somestuff", 0.0, 0.0, 45.0, 11.4469 );
    val = metsrc->getData( "somestuff@trop", 0.0, 0.0, 45.0, 0.0 );
    if ( mismatch(u, val) ) {
       cerr << "Bad trop somestuff  value: " << u << " vs. " << val << endl;
       exit(1);  
    } 
    
    
    // tropopause test
    /* Note: in MetSBRot, the profiles are in alt, bute
       here the data are on pressure surfaces, which results
       in slightly different tropopause values:
          11.3892 km (MetSBRot) vs. 11.6264 (MetGridSBRot) 
         212.849 mb (MetSBRot) vs. 205.002 (MetGridSBRot)
    */     
    real tropz = 11.6264; //from local GSFC ACD IDL wmo_trop() routine
    val = metsrc->getData( "tropz", 0.0, 0.0, 45.0, 0.0 );
    if ( mismatch(tropz, val, 0.01) ) {
       cerr << "Bad trop z value: " << tropz << " vs. " << val << endl;
       exit(1);  
    } 

    real tropp = 205.002; //from local GSFC ACD IDL wmo_trop() routine
    val = metsrc->getData( "tropp", 0.0, 0.0, 45.0, 0.0 );
    if ( mismatch(tropp, val, 1.5) ) {
       cerr << "Bad trop p value: " << tropp << " vs. " << val << endl;
       exit(1);  
    } 
    

    //------------------------------------------------------------------


    //=============  time interpolation (testing caching)
    metsrc->set_period(3.5);
    //metsrc->report();
    u0 = metsrc->get_u( 1.0, 23.4, 45.1, 30.0 );
    //metsrc->report();
    u  = metsrc->get_u( 2.0, 23.4, 45.1, 30.0 );
    //metsrc->report();
    metsrc0->set_period(3.5);
    v0 = metsrc0->get_u( 1.0, 23.4, 45.1, 30.0 );
    v  = metsrc0->get_u( 2.0, 23.4, 45.1, 30.0 );
    //cerr << "u0=" << u0 << ", u=" << u << " raw = " << v0 << ", " << v << endl;
    for ( tt=1.0; tt<=2.0; tt+=0.1 ) {
        val = metsrc->get_u(tt, 23.4, 45.1, 30.0);
        // metsrc->report();
        val0 = (u-u0)/(2.0-1.0)*(tt-1.0) + u0;
        if ( mismatch(val0, val) ) {
           cerr << "bad time interp @ " << tt << ": " << val0 << " vs. " << val << endl;
           exit(1);  
        } 

        //cerr << "tt=" << tt << ": " << val0 << " vs " << val << endl;

    }

    //=============  time interpolation (testing caching)


    // Now use theta as the vertical coordinate
    // from original function
    v0 = metsrc0->get_u( 3.0*24.0*3600.0, 23.4, 45.1, 30.0 );
    // from native alt grid
    v  = metsrc->get_u( 3.0*24.0*3600.0, 23.4, 45.1, 30.0 );
    if ( mismatch(v0, v, 1.0e-3) ) {
       cerr << " native grid mismatch : " << v0 << " vs. " << v << endl;
       exit(1);  
    }
    // get the theta value at 30 km.
    thet  = metsrc->getData( "theta", 3.0*24.0*3600.0, 23.4, 45.1, 30.0 );
    //cerr << "thet=" << thet << endl; 
    
    thetas.reserve(76);
    for ( i=0; i<76; i++ ) {
        thetas.push_back( static_cast<real>(i) * 10.0+250.0 );
    }    
    // from theta grid
    metsrc->set_vertical( "theta", "K", &thetas );
    u  = metsrc->get_u( 3.0*24.0*3600.0, 23.4, 45.1, thet );
    if ( mismatch(v, u) ) {
       cerr << " theta grid mismatch : " << v0 << " vs. " << u << endl;
       exit(1);  
    } 


    //---- now test disk caching
    //cerr << "====================================================" << endl;
    //metsrc->dbug = 1;
    metsrc->setCacheDir("test_diskcache");
    v0  = metsrc->get_u( 4.0*24.0*3600.0, 23.4, 45.1, thet );
    u  = metsrc->get_u( 5.0*24.0*3600.0, 23.4, 45.1, thet );
    u  = metsrc->get_u( 6.0*24.0*3600.0, 23.4, 45.1, thet );
    u  = metsrc->get_u( 7.0*24.0*3600.0, 23.4, 45.1, thet );
    u  = metsrc->get_u( 4.0*24.0*3600.0, 23.4, 45.1, thet );
    if ( u != v0 ) {
       cerr << " disk cache 3D read failure " << v0 << " vs. " << u << endl;
       exit(1);
    }
    
    int junk = system( "/bin/rm -rf test_diskcache/" );
    
    delete metsrc;
    delete metsrc0;

    //------------------------------------------------------------------

    // if we got this far, everything is OK
    exit(0);
    
}
