
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

#include "gigatraj/PGenGrid.hh"
#include "gigatraj/Earth.hh"

using namespace gigatraj;


// Use Earth as the default PlanetNav;
// (We are using this only for longitude wrapping calculations,
// so it should not matter if we end up using a different
// planets in the actual model run.
static Earth e;
      

// constructor
PGenGrid::PGenGrid()
{

     
}

// destructor
PGenGrid::~PGenGrid()
{

}

// copy constructor
PGenGrid::PGenGrid(const PGenGrid& src) : ParcelGenerator(src)
{

}

PGenGrid& PGenGrid::operator=(const PGenGrid& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    this->assign( src ) ;
    
    return *this;
}

void PGenGrid::assign( const PGenGrid& src)
{

    ParcelGenerator::assign(src);
    
}



int PGenGrid :: count_gridlons( real beglon, real endlon, real deltalon ) const
{
     int nlon;
     
     
     nlon = 0;
     
     // sanity checking //
     if (    ( deltalon != 0.0 ) 
          || ( ( deltalon == 0.0 ) && ( beglon == endlon ) ) 
          )  {
          
          // longitudes will always be OK, if they wrap
          real lon0 = e.wrap(beglon);
          real lon1 = e.wrap(endlon);
          while ( (deltalon > 0.0 ) && ( lon0 > lon1 ) ) {
             lon1 += e.fullcircle;
          }   
          while ( (deltalon < 0.0 ) && ( lon0 < lon1 ) ) {
             lon1 -= e.fullcircle;
          }
          // handle the full-globe case
          if ( ABS( lon0 - lon1 ) < 0.01 ) {
             lon1 = lon0 + 359.999;
          }   
          
          if ( deltalon != 0.0 ) {
             nlon = (int) ( (lon1 - lon0)/deltalon ) + 1;
          } else {
             nlon = 1;
          }

     }
     
     return nlon;

};

int PGenGrid :: count_gridlats( real beglat, real endlat, real deltalat ) const
{
     int nlat;
     
     nlat = 0;
     
     // sanity checking //
     if (    ( ( deltalat > 0.0 ) && ( beglat <= endlat ) ) 
          || ( ( deltalat < 0.0 ) && ( beglat >= endlat ) )
          || ( ( deltalat == 0.0) && ( beglat == endlat ) ) 
        )  {
          
          if ( deltalat != 0.0 ) {
             nlat = (int) ( (endlat - beglat)/deltalat ) + 1;
          } else {
             nlat = 1;
          }

     }
     
     return nlat;

};

int PGenGrid :: count_gridzs( real begz, real endz, real deltaz ) const
{
     int nz;
     
     nz = 0;
     
     // sanity checking //
     if (    ( ( deltaz > 0.0 ) && ( begz <= endz ) ) 
          || ( ( deltaz < 0.0 ) && ( begz >= endz ) )
          || ( ( deltaz == 0.0) && ( begz == endz ) )
        )  {
          
          // now count the gridpoints
          if ( deltaz != 0.0 ) {
             nz = (int) ( (endz-begz)/deltaz ) + 1;
          } else {
             nz = 1;
          }

     }
     
     return nz;

};

int PGenGrid :: count_gridpoints( real beglon, real endlon, real deltalon
                           , real beglat, real endlat, real deltalat
                           , real begz, real endz, real deltaz 
                           , int* nlon, int* nlat, int* nz
                           ) const
{
     
     int nlons;
     int nlats;
     int nzs;
     
     nlons = count_gridlons( beglon, endlon, deltalon );
     nlats = count_gridlats( beglat, endlat, deltalat );
     nzs = count_gridzs( begz, endz, deltaz );
     
     if ( nlon != NULLPTR ) {
        *nlon = nlons;
     }   
     if ( nlat != NULLPTR ) {
        *nlat = nlats;
     }   
     if ( nz != NULLPTR ) {
        *nz = nzs;
     }   
     
     return nzs*nlats*nlons;          

};



template< template<class U, class = std::allocator<U> > class Seq>
void PGenGrid :: initgrid( Seq<Parcel>* seq, const Parcel& p
                   , real beglon, real endlon, real deltalon
                   , real beglat, real endlat, real deltalat
                   , real begz, real endz, real deltaz  
                   )
{
     real z, lon, lat;
     int nz;
     int nlat;
     int nlon;
     int nn;
     real dlon, dlat, dz;
     
     int maxn;
     
     maxn = seq->size();
     
     nn = count_gridpoints( beglon, endlon, deltalon 
                          , beglat, endlat, deltalat
                          , begz, endz, deltaz 
                          , &nlon, &nlat, &nz ); 
     
     if ( nlon <= 0 ) {
        nlon = 1;
     }
     if ( nlat <= 0 ) {
        nlat = 1;
     }
     if ( nz <= 0 ) {
        nz = 1;
     }
     
     nn = nlon*nlat*nz;
     
     if ( nn <= seq->size() ) {    
          try {
               typename Seq<Parcel>::iterator it;
               it = seq->begin();
               for ( int iz=0; iz < nz; iz++ ) {
                  z = begz + iz*deltaz;
                  for ( int ilat=0; ilat < nlat; ilat++ ) {
                     lat = beglat + ilat*deltalat;
                     for ( int ilon=0; ilon < nlon; ilon++ ) {
                        lon = e.wrap( beglon + ilon*deltalon );

                         *it = p;  // copy the input parcel's settings
                         it->setPos( lon, lat);  // set the horizontal position
                         it->setZ( z );  // set the vertical position
                         it++;  // next parcel                         
                     }
                  }
               }
          } catch (...) {
              throw (ParcelGenerator :: badgeneration());
          }
     } else {
         throw(ParcelGenerator :: badparcelcount()); 
     }
    

};



Parcel * PGenGrid :: create_array(const Parcel& p, int *np
                     , real beglon, real endlon, real deltalon
                     , real beglat, real endlat, real deltalat
                     , real begz, real endz, real deltaz 
                   )
{                                                                              
    Parcel* pa;                                                                
     real z, lon, lat;
     int nz;
     int nlat;
     int nlon;
     real dlon, dlat, dz;
                                                                               
    *np = -1;                                                                  
                                                                               
    int n = PGenGrid::count_gridpoints(beglon,endlon,deltalon                  
                                      ,beglat,endlat,deltalat                  
                                      ,begz,endz,deltaz
                                      , &nlon, &nlat, &nz);                      
     if ( nlon <= 0 ) {
        nlon = 1;
     }
     if ( nlat <= 0 ) {
        nlat = 1;
     }
     if ( nz <= 0 ) {
        nz = 1;
     }
     
     n = nlon*nlat*nz;

     if ( n <= 0 ) {                                                            
        throw (ParcelGenerator :: badparcelcount());                            
     };                                                                         

     *np = n;
                                                                               
    try {                                                                      
       
       pa = new Parcel[n];                                                     
       
       int i = 0;                                                         
       for ( int iz=0; iz < nz; iz++ ) {
          z = begz + iz*deltaz;
          for ( int ilat=0; ilat < nlat; ilat++ ) {
             lat = beglat + ilat*deltalat;
             for ( int ilon=0; ilon < nlon; ilon++ ) {
                lon = e.wrap( beglon + ilon*deltalon );

                pa[i] = p;  // copy the input parcel's settings          
                pa[i].setPos( lon, lat);  // set the horizontal position 
                pa[i].setZ( z );  // set the vertical position           
                
                i++;  // next parcel                                     
             }
          }
       }
                                                                               
    } catch(...) {                                                             
       throw ( ParcelGenerator :: badgeneration() );                           
    };                                                                         
    return pa;                                                                 


};                                                                             



std::vector<Parcel>* PGenGrid :: create_vector(const Parcel& p
                    , real beglon, real endlon, real deltalon
                    , real beglat, real endlat, real deltalat
                    , real begz, real endz, real deltaz 
                   )
{
    std::vector<Parcel> *bunch;

    int n = PGenGrid::count_gridpoints(beglon,endlon,deltalon
                                      ,beglat,endlat,deltalat
                                      ,begz,endz,deltaz);
    if ( n <= 0 ) {
       throw (ParcelGenerator :: badparcelcount());
    };  
    
    try {
       bunch = new std::vector<Parcel>;
       bunch->reserve(n);
    
       for (int i=0; i<n; i++ ) {
          bunch->push_back(p);
       }
       
       PGenGrid::initgrid( bunch, p
                         ,beglon,endlon,deltalon
                         ,beglat,endlat,deltalat
                         ,begz,endz,deltaz ); 
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     
    
    return bunch;

};



std::list<Parcel>* PGenGrid :: create_list(const Parcel& p
                    , real beglon, real endlon, real deltalon
                    , real beglat, real endlat, real deltalat
                    , real begz, real endz, real deltaz 
                   )
{
    std::list<Parcel>*bunch;
    
    int n = PGenGrid::count_gridpoints(beglon,endlon,deltalon
                                      ,beglat,endlat,deltalat
                                      ,begz,endz,deltaz);
    if ( n <= 0 ) {
       throw (ParcelGenerator :: badparcelcount());
    };  
    
    try {
       bunch = new std::list<Parcel>;
    
       for (int i=0; i<n; i++ ) {
          bunch->push_back(p);
       }
       PGenGrid::initgrid( bunch, p
                         ,beglon,endlon,deltalon
                         ,beglat,endlat,deltalat
                         ,begz,endz,deltaz ); 
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     
    
    return bunch;

};



std::deque<Parcel>* PGenGrid :: create_deque(const Parcel& p
                    , real beglon, real endlon, real deltalon
                    , real beglat, real endlat, real deltalat
                    , real begz, real endz, real deltaz 
                   )
{
    std::deque<Parcel>*bunch;
    
    int n = PGenGrid::count_gridpoints(beglon,endlon,deltalon
                                      ,beglat,endlat,deltalat
                                      ,begz,endz,deltaz);
    if ( n <= 0 ) {
       throw (ParcelGenerator :: badparcelcount());
    };  
    
    try {
       bunch = new std::deque<Parcel>;
    
       for (int i=0; i<n; i++ ) {
          bunch->push_back(p);
       }
       PGenGrid::initgrid( bunch, p
                         ,beglon,endlon,deltalon
                         ,beglat,endlat,deltalat
                         ,begz,endz,deltaz ); 
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     
    
    return bunch;

};


Flock* PGenGrid :: create_Flock(const Parcel& p
                   , real beglon, real endlon, real deltalon
                   , real beglat, real endlat, real deltalat
                   , real begz, real endz, real deltaz 
                   , ProcessGrp* pgrp, int r
                   )                        
{
     // the parcel container
     Flock *flock;
     real lon,lat,z;
     int status = 0;
     int n;
     Parcel *pcl;
     int nz;
     int nlat;
     int nlon;
     real dlon, dlat, dz;

     n = PGenGrid::count_gridpoints(beglon,endlon,deltalon
                                  ,beglat,endlat,deltalat
                                  ,begz,endz,deltaz
                                  , &nlon, &nlat, &nz);
     if ( nlon <= 0 ) {
        nlon = 1;
     }
     if ( nlat <= 0 ) {
        nlat = 1;
     }
     if ( nz <= 0 ) {
        nz = 1;
     }
     
     n = nlon*nlat*nz;

     if ( n <= 0 ) {
        throw (ParcelGenerator :: badparcelcount());
     };  

     try {
        // now create a Flock os that many parcels
        flock = new Flock( p, pgrp, n, r);

        // sync all the processors before we start loading
        if ( pgrp != NULLPTR ) {
           pgrp->sync();
        }
        
        pcl = p.copy();
          
        try {
             int i=0;
             for ( int iz=0; iz < nz; iz++ ) {
                z = begz + iz*deltaz;
                for ( int ilat=0; ilat < nlat; ilat++ ) {
                   lat = beglat + ilat*deltalat;
                   for ( int ilon=0; ilon < nlon; ilon++ ) {
                       lon = e.wrap( beglon + ilon*deltalon );
                       
                       flock->sync();
                       
                       pcl->setPos( lon, lat);  // set the horizontal position
                       pcl->setZ( z );  // set the vertical position
                       
                       // If we are the root processor, the parcel pcl
                       // is valid, and it is sent to the processor
                       // to which it belongs.
                       // If we are not the root processor, then
                       // then pcl has no valid value, but the
                       // processor will ignore it and receive its
                       // parcel value from the root processor.
                       flock->set( i, *pcl, 0 );
                       
                       i++;
                       
                   }
                }
             }
        } catch (...) {
            throw (ParcelGenerator :: badgeneration());
        }
        
        delete pcl;
        
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     


    return flock;

};


Swarm* PGenGrid :: create_Swarm(const Parcel& p
                   , real beglon, real endlon, real deltalon
                   , real beglat, real endlat, real deltalat
                   , real begz, real endz, real deltaz 
                   , ProcessGrp* pgrp, int r
                   )                        
{
     // the parcel container
     Swarm *swarm;
     real lon,lat,z;
     int status = 0;
     int n;
     Parcel *pcl;
     int nz;
     int nlat;
     int nlon;
     real dlon, dlat, dz;

     n = PGenGrid::count_gridpoints(beglon,endlon,deltalon
                                  ,beglat,endlat,deltalat
                                  ,begz,endz,deltaz
                                  , &nlon, &nlat, &nz);
     if ( nlon <= 0 ) {
        nlon = 1;
     }
     if ( nlat <= 0 ) {
        nlat = 1;
     }
     if ( nz <= 0 ) {
        nz = 1;
     }
     
     n = nlon*nlat*nz;

     if ( n <= 0 ) {
        throw (ParcelGenerator :: badparcelcount());
     };  

     try {
        // now create a Swarm os that many parcels
        swarm = new Swarm( p, pgrp, n, r);

        // sync all the processors before we start loading
        if ( pgrp != NULLPTR ) {
           pgrp->sync();
        }
        
        pcl = p.copy();
          
        try {
             int i=0;
             for ( int iz=0; iz < nz; iz++ ) {
                z = begz + iz*deltaz;
                for ( int ilat=0; ilat < nlat; ilat++ ) {
                   lat = beglat + ilat*deltalat;
                   for ( int ilon=0; ilon < nlon; ilon++ ) {
                       lon = e.wrap( beglon + ilon*deltalon );
                       
                       swarm->sync();
                       
                       pcl->setPos( lon, lat);  // set the horizontal position
                       pcl->setZ( z );  // set the vertical position
                       
                       // If we are the root processor, the parcel pcl
                       // is valid, and it is sent to the processor
                       // to which it belongs.
                       // If we are not the root processor, then
                       // then pcl has no valid value, but the
                       // processor will ignore it and receive its
                       // parcel value from the root processor.
                       swarm->set( i, *pcl, 0 );
                       
                       i++;
                       
                   }
                }
             }
        } catch (...) {
            throw (ParcelGenerator :: badgeneration());
        }
        
        delete pcl;
             
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     


    return swarm;

};
