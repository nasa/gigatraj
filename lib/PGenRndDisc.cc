
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


#include "gigatraj/PGenRndDisc.hh"

using namespace gigatraj;

PGenRndDisc::PGenRndDisc()
{
   lon0 = 0.0;
   lat0 = 0.0;
   rad = 50.0;
   thk = 1e-10;
      
   rnd.seed();
}

PGenRndDisc::PGenRndDisc( const real lon, const real lat, const real level, const real r, const real thickness )
{
   lon0 = lon;
   lat0 = lat;
   rad = r;
   thk = thickness;
   z0 = level;

   rnd.seed();
}

// destructor
PGenRndDisc::~PGenRndDisc()
{

}

// copy constructor
PGenRndDisc::PGenRndDisc(const PGenRndDisc& src) : ParcelGenerator(src)
{
   lon0 = src.lon0;
   lat0 = src.lat0;
   rad = src.rad;
   thk = src.thk;

}

PGenRndDisc& PGenRndDisc::operator=(const PGenRndDisc& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    this->assign( src ) ;
    
    return *this;
}

void PGenRndDisc::assign( const PGenRndDisc& src)
{

    ParcelGenerator::assign(src);

    lon0 = src.lon0;
    lat0 = src.lat0;
    rad = src.rad;
    thk = src.thk;
    
}



const void PGenRndDisc::center( real &lon, real &lat, real &level )   
{
   lon = lon0;
   lat = lat0;
   level = z0;
}


void PGenRndDisc::set_center( const real lon, const real lat, const real level ) 
{
   lon0 = lon;
   lat0 = lat;
   z0 = level;
}


const void PGenRndDisc::size( real &r, real &thickness )
{
   r = rad;
   thickness = thk;
}


void PGenRndDisc::set_size( const real r, const real thickness )
{
   rad = r;
   thk = thickness;
}


void PGenRndDisc::seed( const unsigned int *saw ) 
{
    rnd.seed(saw);
}

template< template<class U, class = std::allocator<U> > class Seq>
void PGenRndDisc :: init( Seq<Parcel>* seq
                    )                          
{
     PlanetNav *nav;
     real lon, lat, z;
     
     if ( seq->size() <= 0 ) {
        throw  (ParcelGenerator :: badparcelcount());
     }

     try {                                       
         typename Seq<Parcel>::iterator it;     
         it = seq->begin();
         while ( it != seq->end() ) {
             real lon, lat, lev, r, ang;
             
             nav = it->getNav();
             
             r = rad * SQRT( rnd.uniform(0.0,1.0) );
             ang =  360.0*rnd.uniform(0.0, 360.0);
             nav->displace( lon0,lat0, r,ang, lon,lat);
             
             z = z0 + thk/2.0*rnd.uniform(-1.0,1.0);

             it->setPos( lon, lat );
             it->setZ( z );
          
             it++;
         }
    } catch (...) {
         throw (ParcelGenerator :: badgeneration());
    }
                                
     

};

Parcel * PGenRndDisc :: create_array(Parcel parcel, int n 
                                ) 
{
    Parcel* pa;
    PlanetNav *nav;
     
    
    if ( n <= 0 ) {
       throw (ParcelGenerator :: badparcelcount());
    };  
     
    nav = parcel.getNav();
    
    try {

       pa = new Parcel[n];
       // initialize the parcel value
       for (int i=0; i<n; i++ ) {
          real lon, lat, z, r, ang;

          r = rad * SQRT( rnd.uniform(0.0,1.0) );
          ang =  360.0*rnd.uniform(0.0, 360.0);
          nav->displace( lon0,lat0, r,ang, lon,lat);
          
          z = z0 + thk/2.0*rnd.uniform(-1.0,1.0);             

          pa[i] = parcel;
          pa[i].setPos( lon, lat );
          pa[i].setZ( z );
       }
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     

    return pa;


}; 

std::vector<Parcel>* PGenRndDisc :: create_vector(Parcel parcel, int n 
                                             )
{
    std::vector<Parcel>*bunch;
     
    
    if ( n <= 0 ) {
       throw (ParcelGenerator :: badparcelcount());
    };  
    
    try {
       bunch = new std::vector<Parcel>;
       bunch->reserve(n);
    
       for (int i=0; i<n; i++ ) {
          bunch->push_back(parcel);
       }
       
       PGenRndDisc::init(bunch);

    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     
    
    return bunch;

};

std::list<Parcel>* PGenRndDisc :: create_list(Parcel parcel, int n 
                                         )
{
    std::list<Parcel>*bunch;
     
    
    if ( n <= 0 ) {
       throw (ParcelGenerator :: badparcelcount());
    };  
    
    try {
       bunch = new std::list<Parcel>;
    
       for (int i=0; i<n; i++ ) {
          bunch->push_back(parcel);
       }

       PGenRndDisc::init(bunch);

    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     
    
    return bunch;

};

std::deque<Parcel>* PGenRndDisc :: create_deque(Parcel parcel, int n 
                                           )
{
    std::deque<Parcel>*bunch;
     
    
    if ( n <= 0 ) {
       throw (ParcelGenerator :: badparcelcount());
    };  
    
    try {
       bunch = new std::deque<Parcel>;
    
       for (int i=0; i<n; i++ ) {
          bunch->push_back(parcel);
       }

       PGenRndDisc::init(bunch);

    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     
    
    return bunch;

};


Flock* PGenRndDisc :: create_Flock(const Parcel& p, int n
                   , ProcessGrp* pgrp, int r
                   )                        
{
     // the parcel container
     Flock *flock;
     Flock::iterator ip;         
     real lon,lat,z, rx, ang;
     int status = 0;
     PlanetNav *nav;
     

     if ( n <= 0 ) {
        throw (ParcelGenerator :: badparcelcount());
     };  

     try {
     
        // now create a Flock os that many parels
        flock = new Flock( p, pgrp, n, r);

        // sync all the processors before we start loading
        if ( pgrp != NULLPTR ) {
           pgrp->sync();
        }

        for ( ip=flock->begin(); ip != flock->end(); ip++ ) {
            try {
              // generate a random position

              nav = ip->getNav();
    
              rx = rad * SQRT( rnd.uniform(0.0,1.0) );
              ang =  360.0*rnd.uniform(0.0, 360.0);
              nav->displace( lon0,lat0, rx,ang, lon,lat);
               
              z = z0 + thk/2.0*rnd.uniform(-1.0,1.0);

               // load the parcel location into the parcel
              ip->setPos( lon, lat );
              ip->setZ( z );

            } catch (std::ios::failure) {
               throw (ParcelGenerator :: badgeneration());
            }   
        }   
     
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     


    return flock;

};

Swarm* PGenRndDisc :: create_Swarm(const Parcel& p, int n
                   , ProcessGrp* pgrp, int r
                   )                        
{
     // the parcel container
     Swarm *swarm;
     Swarm::iterator ip;         
     real lon,lat,z, rx, ang;
     int status = 0;
     PlanetNav *nav;
     

     if ( n <= 0 ) {
        throw (ParcelGenerator :: badparcelcount());
     };  

     try {
     
        // now create a Swarm os that many parels
        swarm = new Swarm( p, pgrp, n, r);

        // sync all the processors before we start loading
        if ( pgrp != NULLPTR ) {
           pgrp->sync();
        }

        for ( ip=swarm->begin(); ip != swarm->end(); ip++ ) {
            try {
              // generate a random position

              nav = ip->getNav();
    
              rx = rad * SQRT( rnd.uniform(0.0,1.0) );
              ang =  360.0*rnd.uniform(0.0, 360.0);
              nav->displace( lon0,lat0, rx,ang, lon,lat);
               
              z = z0 + thk/2.0*rnd.uniform(-1.0,1.0);

               // load the parcel location into the parcel
              ip->setPos( lon, lat );
              ip->setZ( z );

            } catch (std::ios::failure) {
               throw (ParcelGenerator :: badgeneration());
            }   
        }   
     
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     


    return swarm;

};
