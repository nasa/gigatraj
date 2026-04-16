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

#include <stdlib.h>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/Parcel.hh"
#include "gigatraj/NetcdfOut.hh"
#include "gigatraj/NetcdfIn.hh"

#include "test_utils.hh"

using namespace gigatraj;
using std::cerr;
using std::cout;
using std::endl;


int main() 
{

    Parcel p, q;
    real lon;
    real lat;
    real lat0;
    real lon0;
    real z;
    real z0;
    real baseZ;
    double time;
    double time0;
    double basetime;
    int status;
    NetcdfOut *out;
    static const int SZ = 500;
    char junk[SZ];
    std::string sss;
    std::string ccc;
    char cc;
    int i;
    int np;
    Parcel *pa;
    std::vector<Parcel> pv;
    std::vector<Parcel> ps;
    Flock flk;
    Swarm swm;
    Parcel *pp;
    std::string outfile;
    NetcdfIn *in;
    int ip, it;
    int ms,ms0;
    Flock* pf;
    Swarm* pw;
    string cmd;
    int dr;
    std::string fmt;
    double tagval;
    double dNaN;
    double tag0;
    
    dNaN = ACOS(2.0); // NaN
    
    basetime = 17.34;
    baseZ = 12.3457869;
    
    // set a horizontal position
    p.setPos( 30.2345768, -43.4568709 );
    p.setTime(basetime);
    p.setZ(baseZ);
    
    // netcdf file name
    outfile = "test_netcdfout_data_01.nc4";
    
    out = new NetcdfOut();
    
    //out->debug( 100 );
    
    out->filename( outfile );
    ccc = out->filename();
    if ( ccc != outfile ) {
       cerr << "NetcdfOut failed to set filename: " << ccc << " instead of " << outfile << endl;
       exit(1);  
    
    }
    
    sss = "test traj file";
    out->contents( sss ) ;
    ccc = out->contents();
    if ( ccc != sss ) {
       cerr << "NetcdfOut failed to set contents: " << ccc << " instead of " << sss << endl;
       exit(1);  
    
    }
    
    sss = "someone@somewhere";
    out->contact( sss ) ;
    ccc = out->contact();
    if ( ccc != sss ) {
       cerr << "NetcdfOut failed to set contact: " << ccc << " instead of " << sss << endl;
       exit(1);      
    }
    
    ms0 = 20;
    out->maxSequence( ms0 );
    ms = out->maxSequence();
    if ( ms != ms0 ) {
       cerr << "NetcdfOut MaxSequence not set: " << ms << " instead of " << ms0 << endl;
    }
    
    out->init( &p, 10 );
    np = out->N();
    if ( np != 10 ) {
       cerr << "NetcdfOut failed to set number of parcels: " << np << " instead of " << 10 << endl;
       exit(1);              
    }
    
    p.setTime( 55.5);
    for ( int i=0; i<np; i++ ) {
        ps.push_back( p );     
    }

    fmt = out->format();
    if ( fmt != "%t %o %a %v" ) {
       cerr << "Default format string returned is " << fmt << endl;
       exit(1);
    }
    
    
    out->open();
    for ( it=0; it<11; it++ ) {
        time = it*0.15;
        p.setTime( time );
        for ( ip=0; ip<np; ip++ ) {
            lat = 45.0 + (ip - np/2)*0.5 + time/50.0;
            lon = COS( lat/180*PI );
            z = baseZ + time/100.0;
            p.setPos( lon, lat );
            p.setZ( z );
            
            out->apply( p );    
        }
    }
    
    out->close();

    // now set to resuming, re-open, and try to add to the file
    out->resuming( true );
    out->open();
    for ( it=11; it<12; it++ ) {
        time = it*0.15;
        p.setTime( time );
        for ( ip=0; ip<np; ip++ ) {
            lat = 45.0 + (ip - np/2)*0.5 + time/50.0;
            lon = COS( lat/180*PI );
            z = baseZ + time/100.0;
            p.setPos( lon, lat );
            p.setZ( z );
            
            out->apply( p );    
        }
    }
    
    out->close();

    out->resuming( false );

    // try reading in what we just wrote
    // (remember: by default twe will take the first time step)
    in = new NetcdfIn();
    in->open( outfile );
    in->apply(ps);
    in->close();
    delete in;

    it = 0;
    for ( int i=0; i<np; i++ ) {
        time0 = it*0.15; 
        lat0 = 45.0 + (i - np/2)*0.5 + time0/50.0;
        lon0 = COS( lat0/180*PI );
        z0 = baseZ + time0/100.0;
        
        ps[i].getPos( &lon, &lat );
        z = ps[i].getZ();
        time = ps[i].getTime();
        
        if ( mismatch(lon0, lon) || mismatch(lat0,lat) 
          || mismatch( z0, z) || mismatch( time0, time) ) {
           cerr << "NetcdfOut failed to set parcel[" << i << "] position "
           << " lon " << lon << " instead of " << lon0 << ", "
           << " lat " << lat << " instead of " << lat0 
           << " z " << z << " instead of " << z0 
           << " time " << time << " instead of " << time0
           << endl; 
           exit(1);              
        
        }
    }
    
    // try reading it again, this time from the end
    // (remember: here we will take the last time step)
    in = new NetcdfIn();
    in->at_end( true );
    in->open( outfile );
    in->apply(ps);
    in->close();
    delete in;

    it = 11;
    for ( int i=0; i<np; i++ ) {
        time0 = it*0.15; 
        lat0 = 45.0 + (i - np/2)*0.5 + time0/50.0;
        lon0 = COS( lat0/180*PI );
        z0 = baseZ + time0/100.0;
        
        ps[i].getPos( &lon, &lat );
        z = ps[i].getZ();
        time = ps[i].getTime();
        
        if ( mismatch(lon0, lon) || mismatch(lat0,lat) 
          || mismatch( z0, z) || mismatch( time0, time) ) {
           cerr << "NetcdfOut failed to set parcel[" << i << "] position "
           << " lon " << lon << " instead of " << lon0 << ", "
           << " lat " << lat << " instead of " << lat0 
           << " z " << z << " instead of " << z0 
           << " time " << time << " instead of " << time0
           << endl; 
           exit(1);              
        
        }
    }
    




    /// write an array of parcels
    np = 14;
    out->maxSequence(10);
    out->init( &p, np );
    out->format("%T %g %f %s"); // testing formats, too
    if ( out->format() != "%T %g %f %s" ) {
       cerr << "Format was not set: " << out->format() << endl;
    }
    if ( (! out->writeFlags()) || ( ! out->writeStatus()) || ( ! out->writeTag())
      || ( ! out->writeTimestamp() ) ) {
       cerr << "format set did not set the appropriate flags" << endl;
       cerr << "     flags: " << out->writeFlags() << endl;
       cerr << "    status: " << out->writeStatus() << endl;
       cerr << "       tag: " << out->writeTag() << endl;
       cerr << " timestamp: " << out->writeTimestamp() << endl;
       exit(1);
    }
    out->open();
    pa = new Parcel[np];
    tag0 = 47.0;
    for ( it=0; it < 7; it++ ) {
        time = it*0.15;
        for ( ip=0; ip<np; ip++ ) {
            lat = 45.0 + (ip - np/2)*0.5 + time/50.0;
            lon = COS( lat/180*PI );
            z = baseZ + time/100.0;
            pa[ip].setPos( lon, lat );
            pa[ip].setZ( z );
            pa[ip].setTime( time );
            pa[ip].tag( tag0 );
        }
        out->apply( pa, np );    
    }            
    out->close();
      
    // try reading it again
    in = new NetcdfIn();
    in->open( outfile );
    ps.clear();
    for ( int i=0; i < np; i++ ) {
        ps.push_back(p);
    }
    in->apply(ps);
    in->close();
    delete in;
    it = 0;
    for ( int i=0; i<np; i++ ) {
        time0 = it*0.15;
        lat0 = 45.0 + (i - np/2)*0.5 + time0/50.0;
        lon0 = COS( lat0/180*PI );
        z0 = baseZ + time0/100.0;
        
        ps[i].getPos( &lon, &lat );
        z = ps[i].getZ();
        time = ps[i].getTime();
        tagval = ps[i].tag();
        
        if ( mismatch(lon0, lon) || mismatch(lat0,lat) 
          || mismatch( z0, z) || mismatch( time0, time) ) {
           cerr << "NetcdfOut failed to set array parcel[" << i << "] position "
           << " lon " << lon << " instead of " << lon0 << ", "
           << " lat " << lat << " instead of " << lat0 
           << " z " << z << " instead of " << z0 
           << " time " << time << " instead of " << time0
           << endl; 
           exit(1);              
        
        }
        if ( mismatch( tagval, tag0 ) ) {
           cerr << "NetcdfOut failed to set vector parcel[" << i << "] tag to "
                << tag0 << ": "
                << tagval << endl;
           exit(1);
        }
    }
    out->format(""); // reset to the default format
    fmt = out->format();
    if ( fmt != "%t %o %a %v" ) {
       cerr << "reset format string returned is " << fmt << endl;
       exit(1);
    }
    
    
    // write a vector of parcels
    np = 15;
    out->maxSequence(10);
    out->init( &p, np );
    out->open();
    for ( it=0; it < 7; it++ ) {
        time = it*0.15;
        pv.clear();
        for ( ip=0; ip<np; ip++ ) {
            lat = 45.0 + (ip - np/2)*0.5 + time/50.0;
            lon = COS( lat/180*PI );
            z = baseZ + time/100.0;
            p.setPos( lon, lat );
            p.setZ( z );
            p.setTime( time );
            p.clearNoTrace();
            if ( (it > 4) && (ip > 5) ) {
               p.setNoTrace();
            }
            pv.push_back(p);
        }
        out->apply( pv );    
    }            
    out->close();
      
    // try reading what we just wrote
    in = new NetcdfIn();
    in->open( outfile );
    ps.clear();
    for ( int i=0; i < np; i++ ) {
        ps.push_back(p);
    }
    in->apply(ps);
    in->close();
    delete in;
    it = 0;
    for ( int i=0; i<np; i++ ) {
        time0 = it*0.15;
        lat0 = 45.0 + (i - np/2)*0.5 + time0/50.0;
        lon0 = COS( lat0/180*PI );
        z0 = baseZ + time0/100.0;
        
        ps[i].getPos( &lon, &lat );
        z = ps[i].getZ();
        time = ps[i].getTime();
        
        if ( mismatch(lon0, lon) || mismatch(lat0,lat) 
          || mismatch( z0, z) || mismatch( time0, time) ) {
           cerr << "NetcdfOut failed to set vector parcel[" << i << "] position "
           << " lon " << lon << " instead of " << lon0 << ", "
           << " lat " << lat << " instead of " << lat0 
           << " z " << z << " instead of " << z0 
           << " time " << time << " instead of " << time0
           << endl; 
           exit(1);              
        
        }
        
    }
    it = 6;
    for ( int i=0; i<np; i++ ) {
        time0 = it*0.15;
        lat0 = 45.0 + (i - np/2)*0.5 + time0/50.0;
        lon0 = COS( lat0/180*PI );
        z0 = baseZ + time0/100.0;
        
        ps[i].getPos( &lon, &lat );
        z = ps[i].getZ();
        time = ps[i].getTime();
        
        if ( (it > 4) && (ip > 5) ) {
           if ( ! ps[i].queryNoTrace() ) {
              cerr << "NetcdfOut failed to disable dead parcel[" << i << "] " << endl;
              exit(1);
           }
        } else {   
           if ( mismatch(lon0, lon) || mismatch(lat0,lat) 
             || mismatch( z0, z) || mismatch( time0, time) ) {
              cerr << "NetcdfOut failed to set vector parcel[" << i << "] position "
              << " lon " << lon << " instead of " << lon0 << ", "
              << ", lat " << lat << " instead of " << lat0 
              << ", z " << z << " instead of " << z0 
              << ", time " << time << " instead of " << time0
              << endl; 
              exit(1);              
        
           }
        }
    }
 
 
 
    
    // write a Flock of Parcels
    np = 16;
    out->init( &p, np );
    out->open();
    pf = new Flock(p, np);
    for ( it=0; it < 7; it++ ) {
        time = it*0.15;
        for ( ip=0; ip<np; ip++ ) {
            lat = 45.0 + (ip - np/2)*0.5 + time/50.0;
            lon = COS( lat/180*PI );
            z = baseZ + time/100.0;
            p.clearNoTrace();
            p.setPos( lon, lat );
            p.setZ( z );
            p.setTime( time );
            if ( it >= 3 && ip == 5 ) {
               p.setNoTrace();
            }
            
            pf->set( ip, p);
            
        }
        out->apply( *pf );    
    }            
    dr = out->direction();
    if ( dr != 1 ) {
       cerr << "Flock Trajectory direction should be forward but is " << dr << endl;
       exit(1);
    }            
    out->close();
    delete pf;
      
    // try reading it again, at the start
    in = new NetcdfIn();
    in->open( outfile );
    ps.clear();
    for ( int i=0; i < np; i++ ) {
        ps.push_back(p);
    }
    in->apply(ps);
    in->close();
    delete in;
    it = 0;
    for ( int i=0; i<np; i++ ) {
        time0 = it*0.15;
        lat0 = 45.0 + (i - np/2)*0.5 + time0/50.0;
        lon0 = COS( lat0/180*PI );
        z0 = baseZ + time0/100.0;
        
        ps[i].getPos( &lon, &lat );
        z = ps[i].getZ();
        time = ps[i].getTime();
        
        if ( mismatch(lon0, lon) || mismatch(lat0,lat) 
          || mismatch( z0, z) || mismatch( time0, time) ) {
           cerr << "NetcdfOut failed to set Flock parcel[" << i << "] position "
           << " lon " << lon << " instead of " << lon0 << ", "
           << ", lat " << lat << " instead of " << lat0 
           << ", z " << z << " instead of " << z0 
           << ", time " << time << " instead of " << time0
           << endl; 
           exit(1);              
        
        }
    }
    // try reading it again, at the end
    in = new NetcdfIn();
    in->at_end( true );
    in->open( outfile );
    ps.clear();
    for ( int i=0; i < np; i++ ) {
        ps.push_back(p);
    }
    in->apply(ps);
    in->close();
    delete in;
    it = 6;
    for ( int i=0; i<np; i++ ) {
        time0 = it*0.15;
        lat0 = 45.0 + (i - np/2)*0.5 + time0/50.0;
        lon0 = COS( lat0/180*PI );
        z0 = baseZ + time0/100.0;
        
        ps[i].getPos( &lon, &lat );
        z = ps[i].getZ();
        time = ps[i].getTime();
        
        if ( i != 5 ) {
           if ( mismatch(lon0, lon) || mismatch(lat0,lat) 
             || mismatch( z0, z) || mismatch( time0, time) ) {
              cerr << "NetcdfOut failed to set Flock parcel[" << i << "] position "
              << " lon " << lon << " instead of " << lon0 << ", "
              << ", lat " << lat << " instead of " << lat0 
              << ", z " << z << " instead of " << z0 
              << ", time " << time << " instead of " << time0
              << endl; 
              exit(1);              
        
           }
        } else {
           if ( ! ps[i].queryNoTrace()  ) {
              cerr << "trace is set on untraced parcel " << ip << " at time " << time0 << endl;
              exit(1);
           }   
        }
    }

    // write a Swarm of Parcels
    np = 16;
    out->init( &p, np );
    out->setTimeTransform( 24.0, -1000.0 );
    out->bad( -9999.99 );
    out->open();
    pw = new Swarm(p, np);
    for ( it=0; it < 7; it++ ) {
        time = - it*0.15;
        for ( ip=0; ip<np; ip++ ) {
            lat = 45.0 + (ip - np/2)*0.5 + time/50.0;
            lon = COS( lat/180*PI );
            z = baseZ + time/100.0;
            p.clearNoTrace();
            p.setPos( lon, lat );
            p.setZ( z );
            p.setTime( time );
            pw->set( ip, p);
        }
        out->apply( *pw );    
    }
    dr = out->direction();
    if ( dr != -1 ) {
       cerr << "Trajectory direction should be backward but is " << dr << endl;
       exit(1);
    }                
    out->close();
    delete pw;
    if ( mismatch( out->bad(), -9999.99 ) ) {
       cerr << "bad-value should be -9999.99 but it " << out->bad() << endl;
       exit(1);   
    }
      
    // try reading it again
    in = new NetcdfIn();
    in->setTimeTransform( 24.0, -1000.0 );
    in->open( outfile );
    ps.clear();
    for ( int i=0; i < np; i++ ) {
        ps.push_back(p);
    }
    in->apply(ps);
    in->close();
    delete in;
    it = 0;
    for ( int i=0; i<np; i++ ) {
        time0 = - it*0.15;
        lat0 = 45.0 + (i - np/2)*0.5 + time0/50.0;
        lon0 = COS( lat0/180*PI );
        z0 = baseZ + time0/100.0;
        
        ps[i].getPos( &lon, &lat );
        z = ps[i].getZ();
        time = ps[i].getTime();
        
        if ( mismatch(lon0, lon) || mismatch(lat0,lat) 
          || mismatch( z0, z) || mismatch( time0, time) ) {
           cerr << "NetcdfOut failed to set Swarm parcel[" << i << "] position "
           << " lon " << lon << " instead of " << lon0 << ", "
           << ", lat " << lat << " instead of " << lat0 
           << ", z " << z << " instead of " << z0 
           << ", time " << time << " instead of " << time0
           << endl; 
           exit(1);              
        
        }
    }
    
    
    // clean up
    
    delete out;

    // delete the netcdf file
    cmd = "/bin/rm -f " + outfile;
    int junkx = system(cmd.c_str());

    exit(0);
}
