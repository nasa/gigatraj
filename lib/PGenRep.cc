
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

#include "gigatraj/PGenRep.hh"

using namespace gigatraj;


// constructor
PGenRep::PGenRep()
{

     
}

// destructor
PGenRep::~PGenRep()
{

}

// copy constructor
PGenRep::PGenRep(const PGenRep& src) : ParcelGenerator(src)
{

}

PGenRep& PGenRep::operator=(const PGenRep& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    this->assign( src ) ;
    
    return *this;
}

void PGenRep::assign( const PGenRep& src)
{

    ParcelGenerator::assign(src);
    
}


Parcel * PGenRep :: create_array(Parcel parcel, int n ) 
{
    Parcel* pa;
    
    if ( n <= 0 ) {
       throw (ParcelGenerator :: badparcelcount());
    };  
    
    try {
       pa = new Parcel[n];
       // initialize the parcel value
       for (int i=0; i<n; i++ ) {
          pa[i] = parcel;
       }
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     

    return pa;


}; 

std::vector<Parcel>* PGenRep :: create_vector(Parcel parcel, int n )
{
    std::vector<Parcel>*flock;
    
    if ( n <= 0 ) {
       throw (ParcelGenerator :: badparcelcount());
    };  
    
    try {
       flock = new std::vector<Parcel>;
       flock->reserve(n);
    
       for (int i=0; i<n; i++ ) {
          flock->push_back(parcel);
       }
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     
    
    return flock;

};

std::list<Parcel>* PGenRep :: create_list(Parcel parcel, int n )
{
    std::list<Parcel>*flock;
    
    if ( n <= 0 ) {
       throw (ParcelGenerator :: badparcelcount());
    };  
    
    try {
       flock = new std::list<Parcel>;
    
       for (int i=0; i<n; i++ ) {
          flock->push_back(parcel);
       }
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     
    
    return flock;

};

std::deque<Parcel>* PGenRep :: create_deque(Parcel parcel, int n )
{
    std::deque<Parcel>*flock;
    
    if ( n <= 0 ) {
       throw (ParcelGenerator :: badparcelcount());
    };  
    
    try {
       flock = new std::deque<Parcel>;
    
       for (int i=0; i<n; i++ ) {
          flock->push_back(parcel);
       }
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     
    
    return flock;

};



Flock* PGenRep :: create_Flock(const Parcel& p, int n
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
        // this automatically replicates the example Parcel into the members of the Flock
        flock = new Flock( p, pgrp, n, r);

        // sync all the processors
        if ( pgrp != NULLPTR ) {
           pgrp->sync();
        }
   
     
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     


    return flock;

};


Swarm* PGenRep :: create_Swarm(const Parcel& p, int n
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
        // this automatically replicates the example Parcel into the members of the Swarm
        swarm = new Swarm( p, pgrp, n, r);

        // sync all the processors
        if ( pgrp != NULLPTR ) {
           pgrp->sync();
        }
   
     
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     


    return swarm;

};
