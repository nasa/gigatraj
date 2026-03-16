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
#include "gigatraj/PGenGrid.hh"

#include "test_utils.hh"


using namespace gigatraj;
using std::cout;
using std::cerr;
using std::endl;
using std::ostringstream;
using std::istringstream;
using std::operator<<;
using std::operator>>;

static Earth e;

int main() 
{

    Parcel p;
    real lon;
    real wlon;
    real lat;
    real z;
    real xlon;
    real xlat;
    real xz;
    real time;
    int status;
    int i;
    int np;
    static PGenGrid gen;
    const real lat0 = 23.45;
    const real lon0 = -45.678;
    const real z0 = 456.5;
    const double time0 = 1000.0;
    
    p.setPos(lon0,lat0);
    p.setZ(z0);
    p.setTime(time0);

    // array of parcels
    Parcel *aflock = gen.create_array(p, &np
                                 ,   0.0, 355.0, 5.0 
                                 , -90.0,  90.0, 5.0
                                 , 320.0, 450.0,10.0
                                 );
    i = 0; 
    for ( real z = 320.0; z<= 450.0; z += 10.0 ) {
    for ( real lat = -90.0; lat <= 90.0; lat += 5.0 ) {
    for ( real lon =  0.0; lon <= 355.0; lon += 5.0 ) {
        wlon = e.wrap( lon );
        aflock[i].getPos( &xlon, &xlat );
        xz = aflock[i].getZ();
        time = aflock[i].getTime();
        if ( mismatch( xlat, lat ) || mismatch( xlon, wlon ) ||  mismatch( xz, z) ) {
           cerr << "Bad lon,lat,z retrieval on array[" << i << ": (" << wlon << "," << lat << ", " << z << ") != (" 
                << xlon << ", " << xlat << ", " << xz << ")" << endl;
           exit(1);
        } 
        
        if ( mismatch( time, time0 ) ) {
           cerr << "Bad time retrieval on array: (" << lon << "," << lat << z << ") :" << time0 << " != (" << time << ")" << endl;
           exit(1);          
        }  

        i++;
    }
    }
    }
    if ( np != i ) {
       cerr << "Wrong number of array elements: " << np << " :" << i << endl;
       exit(1);    
    }
    delete []aflock;


    // vector container of parcels
    std::vector<Parcel>* vflock = gen.create_vector(p 
                                 ,   0.0, 355.0, 5.0 
                                 , -90.0,  90.0, 5.0
                                 , 320.0, 450.0,10.0
                                 );
 
    std::vector<Parcel>::iterator vit = vflock->begin();
    
    
    for ( real z = 320.0; z<= 450.0; z += 10.0 ) {
    for ( real lat = -90.0; lat <= 90.0; lat += 5.0 ) {
    for ( real lon =  0.0; lon <= 355.0; lon += 5.0 ) {
        wlon = e.wrap( lon );
        vit->getPos( &xlon, &xlat );
        xz = vit->getZ();
        time = vit->getTime();
        if ( mismatch( xlat, lat ) || mismatch( xlon, wlon ) ||  mismatch( xz, z) ) {
           cerr << "Bad lon,lat,z retrieval on vector: (" << wlon << "," << lat << ", " << z << ") != (" 
                << xlon << ", " << xlat << ", " << xz << ")" << endl;
           exit(1);
        } 
        
        if ( mismatch( time, time0 ) ) {
           cerr << "Bad time retrieval on vector: (" << wlon << "," << lat << z << ") :" << time0 << " != (" << time << ")" << endl;
           exit(1);          
        }  

        ++vit;
    }
    }
    }

    
    // list container of parcels
    std::list<Parcel>* lflock = gen.create_list(p 
                                 ,   0.0, 355.0, 5.0 
                                 , -90.0,  90.0, 5.0
                                 , 320.0, 450.0,10.0
                                 );
 
    std::list<Parcel>::iterator lit = lflock->begin();
    
    for ( real z = 320.0; z<= 450.0; z += 10.0 ) {
    for ( real lat = -90.0; lat <= 90.0; lat += 5.0 ) {
    for ( real lon =  0.0; lon <= 355.0; lon += 5.0 ) {
        wlon = e.wrap( lon );
        lit->getPos( &xlon, &xlat );
        xz = lit->getZ();
        time = lit->getTime();
        if ( mismatch( xlat, lat ) || mismatch( xlon, wlon ) ||  mismatch( xz, z) ) {
           cerr << "Bad lon,lat,z retrieval on list: (" << wlon << "," << lat << ", " << z << ") != (" 
                << xlon << ", " << xlat << ", " << xz << ")" << endl;
           exit(1);
        } 
        
        if ( mismatch( time, time0 ) ) {
           cerr << "Bad time retrieval on list: (" << wlon << "," << lat << z << ") :" << time0 << " != (" << time << ")" << endl;
           exit(1);          
        }  

        ++lit;
    }
    }
    }


    
    // deque container of parcels
    std::deque<Parcel>* dflock = gen.create_deque(p 
                                 ,   0.0, 355.0, 5.0 
                                 , -90.0,  90.0, 5.0
                                 , 320.0, 450.0,10.0
                                 );
 
    std::deque<Parcel>::iterator dit = dflock->begin();
    
    for ( real z = 320.0; z<= 450.0; z += 10.0 ) {
    for ( real lat = -90.0; lat <= 90.0; lat += 5.0 ) {
    for ( real lon =  0.0; lon <= 355.0; lon += 5.0 ) {
        wlon = e.wrap( lon );
        dit->getPos( &xlon, &xlat );
        xz = dit->getZ();
        time = dit->getTime();
        if ( mismatch( xlat, lat ) || mismatch( xlon, wlon ) ||  mismatch( xz, z) ) {
           cerr << "Bad lon,lat,z retrieval on deque: (" << wlon << "," << lat << ", " << z << ") != (" 
                << xlon << ", " << xlat << ", " << xz << ")" << endl;
           exit(1);
        } 
        
        if ( mismatch( time, time0 ) ) {
           cerr << "Bad time retrieval on deque: (" << wlon << "," << lat << z << ") :" << time0 << " != (" << time << ")" << endl;
           exit(1);          
        }  

        ++dit;
    }
    }
    }

    // Flock container of parcels
    Flock* flock = gen.create_Flock(p 
                                 ,   0.0, 355.0, 5.0 
                                 , -90.0,  90.0, 5.0
                                 , 320.0, 450.0,10.0
                                 );
 
    Flock::iterator fit = flock->begin();
    for ( real z = 320.0; z<= 450.0; z += 10.0 ) {
    for ( real lat = -90.0; lat <= 90.0; lat += 5.0 ) {
    for ( real lon =  0.0; lon <= 355.0; lon += 5.0 ) {
        wlon = e.wrap( lon );
        fit->getPos( &xlon, &xlat );
        xz = fit->getZ();
        time = fit->getTime();
        if ( mismatch( xlat, lat ) || mismatch( xlon, wlon ) ||  mismatch( xz, z) ) {
           cerr << "Bad lon,lat,z retrieval on Flockl: (" << wlon << "," << lat << ", " << z << ") != (" 
                << xlon << ", " << xlat << ", " << xz << ")" << endl;
           exit(1);
        } 
        
        if ( mismatch( time, time0 ) ) {
           cerr << "Bad time retrieval on Flock: (" << wlon << "," << lat << z << ") :" << time0 << " != (" << time << ")" << endl;
           exit(1);          
        }  

        ++fit;
    }
    }
    }    
    delete flock;


    // Swarm container of parcels
    Swarm* swarm = gen.create_Swarm(p 
                                 ,   0.0, 355.0, 5.0 
                                 , -90.0,  90.0, 5.0
                                 , 320.0, 450.0,10.0
                                 );
 
    Swarm::iterator sit = swarm->begin();
    for ( real z = 320.0; z<= 450.0; z += 10.0 ) {
    for ( real lat = -90.0; lat <= 90.0; lat += 5.0 ) {
    for ( real lon =  0.0; lon <= 355.0; lon += 5.0 ) {
        wlon = e.wrap( lon );
        sit->getPos( &xlon, &xlat );
        xz = sit->getZ();
        time = sit->getTime();
        if ( mismatch( xlat, lat ) || mismatch( xlon, wlon ) ||  mismatch( xz, z) ) {
           cerr << "Bad lon,lat,z retrieval on Swarml: (" << wlon << "," << lat << ", " << z << ") != (" 
                << xlon << ", " << xlat << ", " << xz << ")" << endl;
           exit(1);
        } 
        
        if ( mismatch( time, time0 ) ) {
           cerr << "Bad time retrieval on Swarm: (" << wlon << "," << lat << z << ") :" << time0 << " != (" << time << ")" << endl;
           exit(1);          
        }  

        ++sit;
    }
    }
    }    
    delete swarm;


    
    exit(0);
    
}
