
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

#include "gigatraj/PGenLine.hh"

using namespace gigatraj;


// constructor
PGenLine::PGenLine()
{

     
}

// destructor
PGenLine::~PGenLine()
{

}

// copy constructor
PGenLine::PGenLine(const PGenLine& src) : ParcelGenerator(src)
{

}

PGenLine& PGenLine::operator=(const PGenLine& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    this->assign( src ) ;
    
    return *this;
}

void PGenLine::assign( const PGenLine& src)
{

    ParcelGenerator::assign(src);
    
}


int PGenLine::count_gridpoints(const Parcel& p,  real beglon, real beglat
                    , real endlon, real endlat, real deltah
                    , real begz, real endz, real deltaz 
                    )
{
     PlanetNav *e;
     int nz;
     int nhoriz;
     real delhoriz;
     real totaldist;
     
     e = p.getNav();
     
     // sanity checking //
     if ( (  ( ( deltaz > 0.0 ) && ( begz <= endz ) ) 
          || ( ( deltaz < 0.0 ) && ( begz >= endz ) )
          || ( ( deltaz == 0.0) && ( begz == endz ) )
          ) and 
          (  deltah > 0.0 ) 
          )  {
          
          
          // The line stretches form (beglon,beglat) to (endlon,endlat)
          totaldist = e->distance( beglon,beglat, endlon,endlat );
          
          // now count the gridpoints
          if ( deltaz != 0.0 ) {
             nz = (int) ( (endz-begz)/deltaz ) + 1;
          } else {
             nz = 1;
          }
          
          if ( totaldist > 0 ){
          
             nhoriz = (totaldist/deltah + 0.5) + 1;
             if ( nhoriz < 2 ) {
                 nhoriz = 2;
             }
             
             // the actual horizontal delta
             delhoriz = totaldist/(nhoriz - 1);
          
          } else {
          
             nhoriz = 1;
             
             delhoriz = 0.0;
          
          }

          return nz*nhoriz;          

     }
     
     return 0;


}


template< template<class U, class = std::allocator<U> > class Seq>
void PGenLine::initgrid( Seq<Parcel>* seq, const Parcel& p
                  , real beglon, real beglat
                  , real endlon, real endlat, real deltah
                  , real begz, real endz, real deltaz  
                  )
{
     PlanetNav *e;
     int nz;
     int nhoriz;
     real delhoriz;
     real totaldist;
     real bear;
     Parcel p0;
     real z, dz, lat, lon, dist;
     typename Seq<Parcel>::iterator it;
     
     e = p.getNav();
     
     // sanity checking //
     if ( (  ( ( deltaz > 0.0 ) && ( begz <= endz ) ) 
          || ( ( deltaz < 0.0 ) && ( begz >= endz ) )
          || ( ( deltaz == 0.0) && ( begz == endz ) )
          ) and 
          (  deltah > 0.0 ) 
          )  {
          
          p0 = p;
          
          // The line stretches form (beglon,beglat) to (endlon,endlat)
          totaldist = e->distance( beglon,beglat, endlon,endlat );
          bear = e->bearing( beglon,beglat, endlon,endlat );
          
          // now count the gridpoints
          if ( deltaz != 0.0 ) {
             nz = (int) ( (endz-begz)/deltaz ) + 1;
          } else {
             nz = 1;
          }
          
          if ( totaldist > 0 ){
          
             nhoriz = (totaldist/deltah + 0.5) + 1;
             if ( nhoriz < 2 ) {
                 nhoriz = 2;
             }
             
             // the actual horizontal delta
             delhoriz = totaldist/(nhoriz - 1);
          
          } else {
          
             nhoriz = 1;
             
             delhoriz = 0.0;
          
          }
               
          try {

               it = seq->begin();
               for ( int iz=0; iz < nz; iz++ ) {
                  
                  z = begz + iz*deltaz;
                  
                  for ( int ih=0; ih < nhoriz; ih++ ) {
                  
                      dist = ih*delhoriz;

                      if ( it == seq->end() ) {
                         nz = 0;
                         throw(ParcelGenerator :: badparcelcount()); 
                        // break;
                      }                       
                      
                      *it = p0;  // copy the input parcel's settings
                      
                       e->displace( beglon, beglat, dist, bear, lon, lat );
                       
                       it->setPos( lon, lat);  // set the horizontal position                     
                       it->setZ( z );  // set the vertical position
                       
                       it++;  // next parcel  

                  }
               }
          } catch (...) {
              throw (ParcelGenerator :: badgeneration());
          }

     } else {
         throw(ParcelGenerator :: badparcelcount()); 
     }

     
    

}


Parcel * PGenLine::create_array(const Parcel& p, int *np
                    , real beglon, real beglat
                    , real endlon, real endlat, real deltah
                    , real begz, real endz, real deltaz 
                    )
{
    Parcel* pa;                                                                
    PlanetNav *e;
    int nz;
    int nhoriz;
    real delhoriz;
    real totaldist;
    real bear;
     real z, dz, lat, lon, dist;
     
    e = p.getNav();
                                                                               
    *np = -1;                                                                  
                                                                               
    int n = PGenLine::count_gridpoints(p, beglon,beglat                  
                                      ,endlon,endlat,deltah                 
                                      ,begz,endz,deltaz);                      

    if ( n <= 0 ) {                                                            
       throw (ParcelGenerator :: badparcelcount());                            
    };                                                                         
                                                                               
    try {                                                                      

       // initialize the parcel value                                          

       // The line stretches form (beglon,beglat) to (endlon,endlat)
       totaldist = e->distance( beglon,beglat, endlon,endlat );
       bear = e->bearing( beglon,beglat, endlon,endlat );
       
       // now count the gridpoints
       if ( deltaz != 0.0 ) {
          nz = (int) ( (endz-begz)/deltaz ) + 1;
       } else {
          nz = 1;
       }
       
       if ( totaldist > 0 ){
       
          nhoriz = (totaldist/deltah + 0.5) + 1;
          if ( nhoriz < 2 ) {
              nhoriz = 2;
          }
          
          // the actual horizontal delta
          delhoriz = totaldist/(nhoriz - 1);
       
       } else {
       
          nhoriz = 1;
          
          delhoriz = 0.0;
       
       }
       
       n = nz*nhoriz;
                                                                                      
       pa = new Parcel[n];                                                     

       try {                                                                   
            int i = 0;                                                         
            
            for ( int iz=0; iz < nz; iz++ ) {
               
               z = begz + iz*deltaz;
               
               for ( int ih=0; ih < nhoriz; ih++ ) {

                    dist = ih*delhoriz;

                    pa[i] = p;  // copy the input parcel's settings     
                      
                    e->displace( beglon, beglat, dist, bear, lon, lat );
                                             
                    pa[i].setPos( lon, lat);  // set the horizontal position 
                    pa[i].setZ( z );  // set the vertical position           

                    i++;  // next parcel
               }                                                               
            }

            *np = n;                                                           

       } catch (...) {                                                         
           throw (ParcelGenerator :: badgeneration());                         
       }                                                                       


    } catch(...) {                                                             
       throw ( ParcelGenerator :: badgeneration() );                           
    };                                                                         

    return pa;                                                                 

}


std::vector<Parcel>* PGenLine::create_vector(const Parcel& p
                    , real beglon, real beglat
                    , real endlon, real endlat, real deltah
                    , real begz, real endz, real deltaz 
                    )
{
    std::vector<Parcel> *bunch;
    int n;
    
    n = count_gridpoints(p, beglon,beglat
                                     ,endlon,endlat,deltah
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
       
       initgrid( bunch, p
                         ,beglon,beglat
                         ,endlon,endlat,deltah
                         ,begz,endz,deltaz ); 
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     
    
    return bunch;

}

std::list<Parcel>* PGenLine::create_list(const Parcel& p
                    , real beglon, real beglat
                    , real endlon, real endlat, real deltah
                    , real begz, real endz, real deltaz 
                    )
{
    std::list<Parcel> *bunch;

    int n = PGenLine::count_gridpoints(p, beglon,beglat
                                      ,endlon,endlat,deltah
                                      ,begz,endz,deltaz);
    if ( n <= 0 ) {
       throw (ParcelGenerator :: badparcelcount());
    };  
    
    try {
       bunch = new std::list<Parcel>;
    
       for (int i=0; i<n; i++ ) {
          bunch->push_back(p);
       }
       
       PGenLine::initgrid( bunch, p
                         ,beglon,beglat
                         ,endlon,endlat,deltah
                         ,begz,endz,deltaz ); 
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     
    
    return bunch;

}

std::deque<Parcel>* PGenLine::create_deque(const Parcel& p
                    , real beglon, real beglat
                    , real endlon, real endlat, real deltah
                    , real begz, real endz, real deltaz 
                    )
{
    std::deque<Parcel> *bunch;

    int n = PGenLine::count_gridpoints(p, beglon,beglat
                                      ,endlon,endlat,deltah
                                      ,begz,endz,deltaz);
    if ( n <= 0 ) {
       throw (ParcelGenerator :: badparcelcount());
    };  
    
    try {
       bunch = new std::deque<Parcel>;
    
       for (int i=0; i<n; i++ ) {
          bunch->push_back(p);
       }
       
       PGenLine::initgrid( bunch, p
                         ,beglon,beglat
                         ,endlon,endlat,deltah
                         ,begz,endz,deltaz ); 
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     
    
    return bunch;


}


Flock* PGenLine::create_Flock(const Parcel& parcel
                    , real beglon, real beglat
                    , real endlon, real endlat, real deltah
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
     PlanetNav *e;
     int nz;
     int nhoriz;
     real delhoriz;
     real totaldist;
     real bear;
     real dist;
     
     e = parcel.getNav();
     

     n = PGenLine::count_gridpoints(parcel, beglon,beglat
                                  ,endlon,endlat,deltah
                                  ,begz,endz,deltaz);
     if ( n <= 0 ) {
        throw (ParcelGenerator :: badparcelcount());
     };  

     try {
        // now create a Flock of that many parcels
        flock = new Flock( parcel, pgrp, n, r);

        // sanity checking //
        if ( (  ( ( deltaz > 0.0 ) && ( begz <= endz ) ) 
             || ( ( deltaz < 0.0 ) && ( begz >= endz ) )
             || ( ( deltaz == 0.0) && ( begz == endz ) )
             ) and 
             (  deltah > 0.0 ) 
             )  {
             
             
             // The line stretches form (beglon,beglat) to (endlon,endlat)
             totaldist = e->distance( beglon,beglat, endlon,endlat );
             bear = e->bearing( beglon,beglat, endlon,endlat );
             
             // now count the gridpoints
             if ( deltaz != 0.0 ) {
                nz = (int) ( (endz-begz)/deltaz ) + 1;
             } else {
                nz = 1;
             }
             
             if ( totaldist > 0 ){
             
                nhoriz = (totaldist/deltah + 0.5) + 1;
                if ( nhoriz < 2 ) {
                    nhoriz = 2;
                }
                
                // the actual horizontal delta
                delhoriz = totaldist/(nhoriz - 1);
             
             } else {
             
                nhoriz = 1;
                
                delhoriz = 0.0;
             
             }
             
             // sync all the processors before we start loading    
             if ( pgrp != NULLPTR ) {                              
                pgrp->sync();                                      
             }                                                     
             pcl = parcel.copy();
               
             try {
                  int i=0;
                  
                  for ( int iz=0; iz < nz; iz++ ) {
                     
                     z = begz + iz*deltaz;
                     
                     for ( int ih=0; ih < nhoriz; ih++ ) {
                     
                         dist = ih*delhoriz;
               
                         flock->sync();
                         
                         e->displace( beglon, beglat, dist, bear, lon, lat );
                         
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
             } catch (...) {
                 throw (ParcelGenerator :: badgeneration());
             }
             
             delete pcl;
             
        } else {
            throw(ParcelGenerator :: badparcelcount()); 
        }
     
     
     } catch(...) {
        throw ( ParcelGenerator :: badgeneration() );
     };     

     return flock;


}


Swarm* PGenLine::create_Swarm(const Parcel& parcel
                    , real beglon, real beglat
                    , real endlon, real endlat, real deltah
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
     PlanetNav *e;
     int nz;
     int nhoriz;
     real delhoriz;
     real totaldist;
     real bear;
     real dist;
     
     e = parcel.getNav();
     

     n = PGenLine::count_gridpoints(parcel, beglon,beglat
                                  ,endlon,endlat,deltah
                                  ,begz,endz,deltaz);
     if ( n <= 0 ) {
        throw (ParcelGenerator :: badparcelcount());
     };  

     try {
        // now create a Swarm of that many parcels
        swarm = new Swarm( parcel, pgrp, n, r);

        // sanity checking //
        if ( (  ( ( deltaz > 0.0 ) && ( begz <= endz ) ) 
             || ( ( deltaz < 0.0 ) && ( begz >= endz ) )
             || ( ( deltaz == 0.0) && ( begz == endz ) )
             ) and 
             (  deltah > 0.0 ) 
             )  {
             
             
             // The line stretches form (beglon,beglat) to (endlon,endlat)
             totaldist = e->distance( beglon,beglat, endlon,endlat );
             bear = e->bearing( beglon,beglat, endlon,endlat );
             
             // now count the gridpoints
             if ( deltaz != 0.0 ) {
                nz = (int) ( (endz-begz)/deltaz ) + 1;
             } else {
                nz = 1;
             }
             
             if ( totaldist > 0 ){
             
                nhoriz = (totaldist/deltah + 0.5) + 1;
                if ( nhoriz < 2 ) {
                    nhoriz = 2;
                }
                
                // the actual horizontal delta
                delhoriz = totaldist/(nhoriz - 1);
             
             } else {
             
                nhoriz = 1;
                
                delhoriz = 0.0;
             
             }
             
             // sync all the processors before we start loading    
             if ( pgrp != NULLPTR ) {                              
                pgrp->sync();                                      
             }                                                     
             pcl = parcel.copy();
               
             try {
                  int i=0;
                  real z1, z2;
                  if ( begz < endz ) {
                     z1 = begz;
                     z2 = endz;
                  } else {
                     z1 = endz;
                     z2 = begz;
                  }
               
                  for ( int iz=0; iz < nz; iz++ ) {
                     
                     z = begz + iz*deltaz;
                     
                     for ( int ih=0; ih < nhoriz; ih++ ) {
                  
                         dist = ih*delhoriz;
                            
                         swarm->sync();
                         
                         e->displace( beglon, beglat, dist, bear, lon, lat );
                         
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
             } catch (...) {
                 throw (ParcelGenerator :: badgeneration());
             }
             
             delete pcl;
             
        } else {
            throw(ParcelGenerator :: badparcelcount()); 
        }
     
     
     } catch(...) {
        throw ( ParcelGenerator :: badgeneration() );
     };     

     return swarm;


}

