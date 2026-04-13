
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

#include "config.h"


#include "gigatraj/PGenRnd.hh"

using namespace gigatraj;

PGenRnd::PGenRnd( float LLlat, float LLlon, float URlat, float URlon )
{

     use_z = 0;
     
     setBox( LLlat, LLlon, URlat, URlon );
     
}

// destructor
PGenRnd::~PGenRnd()
{

}

// copy constructor
PGenRnd::PGenRnd(const PGenRnd& src) : ParcelGenerator(src)
{
   lon0 = src.lon0;
   lon1 = src.lon1;
   slat0 = src.slat0;
   slat1 = src.slat1;
   use_z= src.use_z;
   z0 = src.z0;
   z1 = src.z1;   
}

PGenRnd& PGenRnd::operator=(const PGenRnd& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    this->assign( src ) ;
    
    return *this;
}

void PGenRnd::assign( const PGenRnd& src)
{

    ParcelGenerator::assign(src);

    lon0 = src.lon0;
    lon1 = src.lon1;
    slat0 = src.slat0;
    slat1 = src.slat1;
    use_z= src.use_z;
    z0 = src.z0;
    z1 = src.z1;   
    
}




void PGenRnd::setBox( float LLlat, float LLlon, float URlat, float URlon )
{
     lon0 = LLlon;
     slat0 = SIN( LLlat*RCONV );
     lon1 = URlon;
     slat1 = SIN( URlat*RCONV );
     
     // make sure th epair is in the right order
     while ( lon1 <= lon0 ) {
         lon1 = lon1 + 360.0;
     }
     // they might be equal
     if ( lon0 == lon1 ) { 
        // make sure they are not equal
        lon1 = lon1 - 0.0001;
     }

}

void PGenRnd::setZ( float zlow, float zhigh )
{
     use_z = 1;
     z0 = zlow;
     z1 = zhigh;
     
}

void PGenRnd::seed( const unsigned int *saw ) 
{
    rnd.seed(saw);
}


template< template<class U, class = std::allocator<U> > class Seq>
void PGenRnd :: init( Seq<Parcel>* seq
                    )                          
{
          
     seed();
     
     if ( seq->size() <= 0 ) {
        throw  (ParcelGenerator :: badparcelcount());
     }



     try {                                       
         typename Seq<Parcel>::iterator it;     
         it = seq->begin();
         while ( it != seq->end() ) {
             real lon, lat;
             lon = rnd.uniform(lon0, lon1);
             lat = ASIN( rnd.uniform(slat0, slat1) ) /RCONV;
             it->setPos( lon, lat );
          
             if ( use_z ) {
                it->setZ( rnd.uniform( z0, z1 ) );
             }
          
             it++;
         }
    } catch (...) {
         throw (ParcelGenerator :: badgeneration());
    }
                                
     

};

Parcel * PGenRnd :: create_array(Parcel parcel, int n 
                                ) 
{
    Parcel* pa;
    
    if ( n <= 0 ) {
       throw (ParcelGenerator :: badparcelcount());
    };  

    try {
       seed();
       pa = new Parcel[n];
       // initialize the parcel value
       for (int i=0; i<n; i++ ) {
          real lon, lat;
          lon = rnd.uniform(lon0, lon1);
          lat = ASIN( rnd.uniform(slat0, slat1) ) /RCONV;
          //lon = (real)(rand() % 32768)/32767.0 * 359.9999;
          //lat = ASIN( 2.0*(real)(rand() % 32768)/32767.0 - 1.0 ) /RCONV;
          pa[i] = parcel;
          pa[i].setPos( lon, lat );
          
          if ( use_z ) {
             pa[i].setZ( rnd.uniform( z0, z1 ));
          }
          
       }
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     

    return pa;


}; 

std::vector<Parcel>* PGenRnd :: create_vector(Parcel parcel, int n 
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
       
       PGenRnd::init(bunch);

    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     
    
    return bunch;

};

std::list<Parcel>* PGenRnd :: create_list(Parcel parcel, int n 
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

       PGenRnd::init(bunch);

    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     
    
    return bunch;

};

std::deque<Parcel>* PGenRnd :: create_deque(Parcel parcel, int n 
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

       PGenRnd::init(bunch);

    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     
    
    return bunch;

};


Flock* PGenRnd :: create_Flock(const Parcel& p, int n
                   , ProcessGrp* pgrp, int r
                   )                        
{
     // the parcel container
     Flock *flock;
     Flock::iterator ip;         
     real lon,lat,z;
     int status = 0;

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
               lon = rnd.uniform(lon0, lon1);
               lat = ASIN( rnd.uniform(slat0, slat1) ) /RCONV;
               //lon = rnd.uniform(0.0, 359.9999);
               //lat = ASIN( rnd.uniform(-1.0, 1.0) ) /RCONV;
               // load the parcel location into the parcel
              ip->setPos( lon, lat );
          
              if ( use_z ) {
                 ip->setZ( rnd.uniform( z0, z1 ));
              }

            } catch (std::ios::failure) {
               throw (ParcelGenerator :: badgeneration());
            }   
        }   
     
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     


    return flock;

};

Swarm* PGenRnd :: create_Swarm(const Parcel& p, int n
                   , ProcessGrp* pgrp, int r
                   )                        
{
     // the parcel container
     Swarm *swarm;
     Swarm::iterator ip;         
     real lon,lat,z;
     int status = 0;

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
               lon = rnd.uniform(lon0, lon1);
               lat = ASIN( rnd.uniform(slat0, slat1) ) /RCONV;
              //lon = rnd.uniform(0.0, 359.9999);
              //lat = ASIN( rnd.uniform(-1.0, 1.0) ) /RCONV;
               // load the parcel location into the parcel
              ip->setPos( lon, lat );

              if ( use_z ) {
                 ip->setZ( rnd.uniform( z0, z1 ));
              }

            } catch (std::ios::failure) {
               throw (ParcelGenerator :: badgeneration());
            }   
        }   
     
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     


    return swarm;

};
