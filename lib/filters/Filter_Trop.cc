
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

#include "gigatraj/Filter_Trop.hh"

using namespace gigatraj;

// constructor
Filter_Trop::Filter_Trop( int direction, real tolerance, std::string quantity, std::string field)
{
     neg = false;
     dir = direction;
     tol = tolerance;
     if ( tol < 0 ) {
        neg = true;
        tol = - tol;
     }
     if ( quantity != "" ) {
        tquant = quantity;
     } else {   
        tquant = "Trop";
     }
     if ( field != "" ) {
        tkind = field;
     } else {   
        tkind = "alt";
     }
     
}

// destructor
Filter_Trop::~Filter_Trop()
{

}

std::string Filter_Trop::quantity() const
{
    return tquant;
}

void Filter_Trop::quantity( std::string q )
{
     tquant = q;
}

std::string Filter_Trop::field() const
{
    return tkind;
}

void Filter_Trop::field( std::string q )
{
     tkind = q;
}

int Filter_Trop::direction() const
{
    return dir;
}

void Filter_Trop::direction( int d )
{
     switch (d) {
     case -1: dir = -1;
            break;
     case  0: dir = 0;
            break;
     case  1: dir = 1;
            break;
     }       

}

real Filter_Trop::threshold() const
{
    return tol;
}

void Filter_Trop::threshold( real t )
{
     tol = t;
}

bool Filter_Trop::negation() const
{
    return neg;
}

void Filter_Trop::negation( bool n )
{
     neg = n;
}

void Filter_Trop::apply( Parcel& p )
{
     MetData* metsrc;
     double time;
     real lat, lon, vert;
     real tropp, tropz;
     real z;
     std::string paname;
     bool halt;
     real zdif;
     std::string trop_units;
     int vertdir;

     // we only test Parcels that are bieng traced
     if ( ! p.queryNoTrace() ) {

        metsrc = p.getMet();
        time = p.getTime();
        p.getPos( &lon, &lat );
        vert = p.getZ();
        if ( ! metsrc->getOption( "PressureAltitudeName", paname ) ) {
           paname = "PAlt";
        }
     
        // does the vertical coordinate increase or decrease with altitude?
        vertdir = metsrc->vIncrease();
        
        // we assume that we will halt the Parcel,
        // so that Parcels with invalid tropopause values
        // will be halted, in addition to those that
        // trigger the tropopause tests below.
        halt = true;
        
        // get the trop vert coord at this parcel's location
        tropz = metsrc->getData( tquant, time, lon, lat, vert, METDATA_NANBAD );

        if ( FINITE(tropz) ) {

           if ( metsrc->vertical() == tkind ) {
              z = vert;
           } else {
              // convert the parcel's vertical coordinate to tropopause quantity
              z = metsrc->getData( tkind, time, lon, lat, vert, METDATA_NANBAD );
           }   
           if ( FINITE(z) ) {
     
              // the distance of the parcel from the topropause
              zdif = z - tropz;
           
              switch (dir*vertdir) {
              case -1: // must be below the trop 
                 // (note that we are testing for Parcels that we *keep*,
                 // not the ones that we want to halt!)
                 if ( zdif > (-tol) ) {
                    halt = false;
                 }
                 break;
              case  1: // must be above the trop
                 if ( zdif < tol ) {
                    halt = false;
                 }
                 break;
              case 0: // must be away from the trop in either direction
                 if ( ABS(zdif) < tol ) {
                    halt = false;
                 }
                 break;
              }
           
              if ( neg ) {
                 halt = ! halt;
              }
           
              if ( halt ) {
                 p.setNoTrace();
              }
           }     
        }
     }
}

void Filter_Trop::apply( Parcel * const p, const int n )
{          
    int i;
    
    if ( n < 0 ) {
       throw (ParcelFilter::badparcelnum());
    };
    
    for ( i=0; i<n; i++ ) {
    
       apply( p[i] );
    
    }   
}

void Filter_Trop::apply( std::vector<Parcel>& p )
{
   std::vector<Parcel>::iterator ip;
    
   for ( ip=p.begin(); ip != p.end(); ip++ ) {
    
       apply( *ip );

   }

}

void Filter_Trop::apply( std::list<Parcel>& p )
{
   std::list<Parcel>::iterator ip;
    
   for ( ip=p.begin(); ip != p.end(); ip++ ) {
    
       apply( *ip );

   }

}

void Filter_Trop::apply( std::deque<Parcel>& p )
{
   std::deque<Parcel>::iterator ip;    
    
   for ( ip=p.begin(); ip != p.end(); ip++ ) {
    
       apply( *ip );

   }   

}

void Filter_Trop::apply( Flock& p )
{
    Flock::iterator iter;
    
    for ( iter=p.begin(); iter != p.end(); iter++ ) {
        apply( *iter );
    }
    
}

void Filter_Trop::apply( Swarm& p )
{
    Swarm::iterator iter;
      
    for ( iter=p.begin(); iter != p.end(); iter++ ) {
        apply( *iter );
    }

}

