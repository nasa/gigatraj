
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

#include "gigatraj/Filter_Advance.hh"

using namespace gigatraj;

// constructor
Filter_Advance::Filter_Advance( double timestep, Integrator* integ )
{
     tstep = timestep;
     intg = integ;
}

// destructor
Filter_Advance::~Filter_Advance()
{

}

Filter_Advance::Filter_Advance(const Filter_Advance& src) : ParcelFilter(src)
{

    tstep = src.tstep;
    intg = src.intg;

}

Filter_Advance& Filter_Advance::operator=(const Filter_Advance& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    ParcelFilter::assign(src);
    
    this->assign( src ) ;
    
    return *this;
}

void Filter_Advance::assign( const Filter_Advance& src)
{
    tstep = src.tstep;
    intg = src.intg;
}


double Filter_Advance::timestep() const
{
    return tstep;
}

void Filter_Advance::timestep( double value )
{
     tstep = value;
}

Integrator* Filter_Advance::integrator() const
{
    return intg;
}

void Filter_Advance::integrator( Integrator* value )
{
     intg = value;
}


void Filter_Advance::apply( Parcel& p )
{
      Integrator* save;
      
      if ( intg != NULLPTR ) {
         save = p.integrator();
         p.integrator( intg );
      }
      
      p.advance( tstep );

      if ( intg != NULLPTR ) {
         p.integrator( save );
      }
}

void Filter_Advance::apply( Parcel * const p, const int n )
{          
      int i;
      MetData* met;
      double tyme;
      Parcel* px;
      
      if ( n > 0 ) {
      
          real* const lons = new real[n];
          real* const lats = new real[n];
          real* const zs = new real[n];
          ParcelFlag* const flags = new ParcelFlag[n];
          ParcelStatus* const statuses = new ParcelStatus[n];
          int* const traceflags = new int[n];

       
          px = &(p[0]);
          tyme = px->getTime();
          Integrator* integ = px->integrator();
          if ( intg != NULLPTR ) {
             integ = intg;
          }
          PlanetNav* nav = px->getNav();
          met = px->getMet();

          for ( int j = 0; j < n; j++ ) {

              px = &(p[j]);              
          
              px->getPos( &(lons[j]), &(lats[j]) );
              zs[j] = px->getZ();
              flags[j] = px->flags();
              statuses[j] = px->status();
              
              traceflags[j] = 0;
              
              if ( ( statuses[j] & (HitBad | HitBdy) )
                || ( flags[j] & NoTrace ) 
                || ( (flags[j] & SyncTrace) && (px->getTime() >= tyme) )
              ) {
                 traceflags[j] = 1;
              }
          }

          integ->go( n, lons, lats, zs, traceflags, tyme, met, nav, tstep ); 
          
          for ( int j = 0; j < n; j++ ) {

              px = &(p[j]);           
          
              px->setPos( lons[j], lats[j] );
              px->setZ( zs[j] );
              px->setTime( tyme );
              
              if ( traceflags[j] ) {
                 statuses[j] = statuses[j] | HitBad;
                 flags[j] = flags[j] | NoTrace;
              }
              
              px->setFlags( flags[j] );
              px->setStatus( statuses[j] );
          }
          
          delete[] traceflags;
          delete[] statuses;
          delete[] flags;
          delete[] zs;
          delete[] lats;
          delete[] lons;
           
      }

}

void Filter_Advance::apply( std::vector<Parcel>& p )
{
      Integrator* integ;
      std::vector<Parcel>::iterator px;
      int n;
      double tyme;
      MetData* met;
      int j;
      
      n = p.size();
      
      if ( n > 0 ) {
      
          real* const lons = new real[n];
          real* const lats = new real[n];
          real* const zs = new real[n];
          ParcelFlag* const flags = new ParcelFlag[n];
          ParcelStatus* const statuses = new ParcelStatus[n];
          int* const traceflags = new int[n];

       
          px = p.begin();
          tyme = px->getTime();
          Integrator* integ = px->integrator();
          if ( intg != NULLPTR ) {
             integ = intg;
          }
          PlanetNav* nav = px->getNav();
          met = px->getMet();

          j = 0;
          for ( px = p.begin(); px != p.end(); px++ ) {

              px->getPos( &(lons[j]), &(lats[j]) );
              zs[j] = px->getZ();
              flags[j] = px->flags();
              statuses[j] = px->status();
              
              traceflags[j] = 0;
              
              if ( ( statuses[j] & (HitBad | HitBdy) )
                || ( flags[j] & NoTrace ) 
                || ( (flags[j] & SyncTrace) && (px->getTime() >= tyme) )
              ) {
                 traceflags[j] = 1;
              }
              
              j++;
          }

          integ->go( n, lons, lats, zs, traceflags, tyme, met, nav, tstep ); 
          
          j = 0;
          for ( px = p.begin(); px != p.end(); px++ ) {
          
              px->setPos( lons[j], lats[j] );
              px->setZ( zs[j] );
              px->setTime( tyme );
              
              if ( traceflags[j] ) {
                 statuses[j] = statuses[j] | HitBad;
                 flags[j] = flags[j] | NoTrace;
              }
              
              px->setFlags( flags[j] );
              px->setStatus( statuses[j] );
              
              j++;
          }
          
          delete[] traceflags;
          delete[] statuses;
          delete[] flags;
          delete[] zs;
          delete[] lats;
          delete[] lons;
   
      }      

}

void Filter_Advance::apply( std::list<Parcel>& p )
{
      Integrator* integ;
      std::list<Parcel>::iterator px;
      int n;
      double tyme;
      MetData* met;
      int j;
      
      n = p.size();
      
      if ( n > 0 ) {
      
          real* const lons = new real[n];
          real* const lats = new real[n];
          real* const zs = new real[n];
          ParcelFlag* const flags = new ParcelFlag[n];
          ParcelStatus* const statuses = new ParcelStatus[n];
          int* const traceflags = new int[n];

       
          px = p.begin();
          tyme = px->getTime();
          Integrator* integ = px->integrator();
          if ( intg != NULLPTR ) {
             integ = intg;
          }
          PlanetNav* nav = px->getNav();
          met = px->getMet();

          j = 0;
          for ( px = p.begin(); px != p.end(); px++ ) {

              px->getPos( &(lons[j]), &(lats[j]) );
              zs[j] = px->getZ();
              flags[j] = px->flags();
              statuses[j] = px->status();
              
              traceflags[j] = 0;
              
              if ( ( statuses[j] & (HitBad | HitBdy) )
                || ( flags[j] & NoTrace ) 
                || ( (flags[j] & SyncTrace) && (px->getTime() >= tyme) )
              ) {
                 traceflags[j] = 1;
              }
              
              j++;
          }

          integ->go( n, lons, lats, zs, traceflags, tyme, met, nav, tstep ); 
          
          j = 0;
          for ( px = p.begin(); px != p.end(); px++ ) {
          
              px->setPos( lons[j], lats[j] );
              px->setZ( zs[j] );
              px->setTime( tyme );
              
              if ( traceflags[j] ) {
                 statuses[j] = statuses[j] | HitBad;
                 flags[j] = flags[j] | NoTrace;
              }
              
              px->setFlags( flags[j] );
              px->setStatus( statuses[j] );
              
              j++;
          }
          
          delete[] traceflags;
          delete[] statuses;
          delete[] flags;
          delete[] zs;
          delete[] lats;
          delete[] lons;
   
      }      

      

}

void Filter_Advance::apply( std::deque<Parcel>& p )
{
      Integrator* integ;
      std::deque<Parcel>::iterator px;
      int n;
      double tyme;
      MetData* met;
      int j;
      
      n = p.size();
      
      if ( n > 0 ) {
      
          real* const lons = new real[n];
          real* const lats = new real[n];
          real* const zs = new real[n];
          ParcelFlag* const flags = new ParcelFlag[n];
          ParcelStatus* const statuses = new ParcelStatus[n];
          int* const traceflags = new int[n];

       
          px = p.begin();
          tyme = px->getTime();
          Integrator* integ = px->integrator();
          if ( intg != NULLPTR ) {
             integ = intg;
          }
          PlanetNav* nav = px->getNav();
          met = px->getMet();

          j = 0;
          for ( px = p.begin(); px != p.end(); px++ ) {

              px->getPos( &(lons[j]), &(lats[j]) );
              zs[j] = px->getZ();
              flags[j] = px->flags();
              statuses[j] = px->status();
              
              traceflags[j] = 0;
              
              if ( ( statuses[j] & (HitBad | HitBdy) )
                || ( flags[j] & NoTrace ) 
                || ( (flags[j] & SyncTrace) && (px->getTime() >= tyme) )
              ) {
                 traceflags[j] = 1;
              }
              
              j++;
          }

          integ->go( n, lons, lats, zs, traceflags, tyme, met, nav, tstep ); 
          
          j = 0;
          for ( px = p.begin(); px != p.end(); px++ ) {
          
              px->setPos( lons[j], lats[j] );
              px->setZ( zs[j] );
              px->setTime( tyme );
              
              if ( traceflags[j] ) {
                 statuses[j] = statuses[j] | HitBad;
                 flags[j] = flags[j] | NoTrace;
              }
              
              px->setFlags( flags[j] );
              px->setStatus( statuses[j] );
              
              j++;
          }
          
          delete[] traceflags;
          delete[] statuses;
          delete[] flags;
          delete[] zs;
          delete[] lats;
          delete[] lons;
   
      }      

      
    
}

void Filter_Advance::apply( Flock& p )
{
      Integrator* save;
      Parcel* px;

      
      if ( intg != NULLPTR ) {
         px = p.parcel(0,0);
         save = px->integrator();
         // note: this sets the integrator for ALL parcel, whether
         // they are in the Flockor not.
         px->integrator( intg );
      }
      
      p.advance( tstep );

      if ( intg != NULLPTR ) {
         px->integrator( save );
      }
          
}

void Filter_Advance::apply( Swarm& p )
{
      Integrator* save;
      Parcel* px;

      
      if ( intg != NULLPTR ) {
         px = p.parcel(0,0);
         save = px->integrator();
         // note: this sets the integrator for ALL parcel, whether
         // they are in the Flockor not.
         px->integrator( intg );
      }
      
      p.advance( tstep );

      if ( intg != NULLPTR ) {
         px->integrator( save );
      }
      

}


