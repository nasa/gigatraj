
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

#include "gigatraj/Parcel.hh"
#include "gigatraj/Earth.hh"
#include "gigatraj/MetSBRot.hh"
#include "gigatraj/IntegRK4a.hh"
#include "gigatraj/IntegRK4.hh"

using namespace gigatraj;


// Use Earth as the default PlanetNav;
static Earth e;

PlanetNav* Parcel::nav = &e;

// Use MetSBRot as the default MetData
static MetSBRot met;
MetData* Parcel::metsrc = &met;

// Use IntegRK4a as the default Integrator
static IntegRK4a irk4;
Integrator* Parcel::integ = &irk4;


const std::string Parcel::id = "Parcel";


/*!
The base constructor initializes the parcel state to 0
*/
Parcel :: Parcel() 
{

   lat = 0.0;
   lon = 0.0;
   z = 0.0;
   t = 0.0;
   flagset = 0;
   statuses = 0;
   tg = 0.0;

};

/*!
The initializing constructor permits setting the position only.
*/
Parcel :: Parcel( real ilat, real ilon, real iz, double it) 
{

   lat = ilat;
   lon = ilon;
   z = iz;
   t = it;
   flagset = 0;
   statuses = 0;
   tg = 0.0;

};

/*! This initializing constructor pemits copying data
from an existing Parcel to this new one.
*/
Parcel :: Parcel( const Parcel& p ) 
{
       lat = p.getLat();
       lon = p.getLon();
         z = p.getZ();
         t = p.getTime();
   flagset = p.flags();
  statuses = p.status();
        tg = p.tag();

};  

/*!
The Parcel destructor
*/
Parcel :: ~Parcel()
{

};

Parcel& Parcel :: operator=(const Parcel& src) {
    
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    lat = src.lat;
    lon = src.lon;
    t = src.t;
    z = src.z;
    flagset = src.flagset;
    statuses = src.statuses;
    tg = src.tg;

    // nav, metsrc, and integ are shared among all parcels,
    // so we do not need to copy their values.

    return *this;
};

Parcel* Parcel::copy() const
{
    Parcel* new_parcel;

    new_parcel = new Parcel; 
    new_parcel->lat = lat;
    new_parcel->lon = lon;
    new_parcel->t = t;
    new_parcel->z = z;
    new_parcel->flagset = flagset;
    new_parcel->statuses = statuses;
    new_parcel->tg = tg;

    return new_parcel;
};

void Parcel :: setNav( PlanetNav& newnav )
{
   nav = &newnav;
};

PlanetNav* Parcel :: getNav() const
{
   return nav;
};

void Parcel :: setMet( MetData& newmet )
{
   metsrc = &newmet;
};

MetData* Parcel :: getMet() const
{
   return metsrc;
};


void Parcel :: addPos( real deltalon, real deltalat )
{
    if ( isfinite(deltalon) && isfinite(deltalat) ) {
       nav->deltapos( &lon, &lat, deltalon, deltalat );
    } else {
       throw (PlanetNav::badlocation());
    }   
};



void Parcel :: addZ( real deltaz )
{
    if ( isfinite(deltaz) ) {
       z += deltaz;
    } else {
       throw (Parcel::badz());
    } 
};


int Parcel :: advance( double dt ) 
{
    try {
       if ( ! queryNoTrace() && ! queryNonVert() ) {
          integ->go( lon, lat, z, t, metsrc, nav, dt );
       }   
    } catch(MetData::badmetdata) {
       setHitBad();
       setNoTrace();
    } catch(Interpolator::badoutofdomain) {
       setHitBdy();
       setNoTrace();
    }
    return 0;
};

real Parcel :: field( std::string quantity, MetData* metsource ) const
{
     real result;
     MetData *met;
     
     met = metsrc;
     if ( metsource != NULLPTR ) {
        met = metsource;
     }
     
     result = met->getData( quantity, t, lon, lat, z );
     
     return result;
}                 

void Parcel :: setPos( real newlon, real newlat )
{

   nav->checkpos( newlon, newlat );
   lat = newlat;
   lon = newlon;

};

void Parcel :: send( const ProcessGrp* pgroup, const int id) const
{
    real loc[3];
    int info[2];
    double tt;
    
    // put the Parcel's data into Arrays, according to type
    loc[0] = lat;
    loc[1] = lon;
    loc[2] = z;
    
    info[0] = flagset;
    info[1] = statuses;
    
    // send the information to the process group
    tt = t;
    pgroup->send_reals(id, 3, loc, 1);
    pgroup->send_doubles(id, 1, &tt, 1);
    pgroup->send_ints(id, 2, info, 1);

}

void Parcel :: receive( const ProcessGrp* pgroup, const int id)
{
    real loc[3];
    int info[2];
    
    // in case the receive does nothing (e.g., serial processing)
    loc[0] = lat;
    loc[1] = lon;
    loc[2] = z;
    info[0] = flagset;
    info[1] = statuses;
    
    // receive the information from the process group
    pgroup->receive_reals(id, 3, loc, 1);
    pgroup->receive_doubles(id, 1, &t, 1);
    pgroup->receive_ints(id, 2, info, 1);

    // unpack the information received
    lat = loc[0];
    lon = loc[1];
    z   = loc[2];
    
    flagset  = info[0];
    statuses = info[1];
    
}

void Parcel :: integrator( Integrator *intg )
{
    if ( intg != NULLPTR ) {
       integ = intg;
    } else {
       integ = &irk4;
    }
}

Integrator* Parcel :: integrator()
{
    return integ;
}

void Parcel :: conformal( int mode )
{
    nav->conformal( mode );
    integ->conformal( mode );
    //if (mode == 0 ) {
    //   integ = &irk4;
    //} else {
    //   // use the integ
    //   integ = new IntegRK4;    
    //}
}

int Parcel :: conformal()
{
    return integ->conformal();
}

namespace gigatraj {



// outputs the parcel state
std::ostream& operator<<( std::ostream& os, const Parcel& p)
{
  std::ios_base::fmtflags os_flags;
  int prec;
  double tagg;
  
  try {
      // save current ostream flag settings
      os_flags = os.flags();

  
      // set unit buffering (flush after each I/O event
      os.setf(std::ios::unitbuf);
  
      // set to output in decimal format
      os.setf(std::ios::dec, std::ios::basefield);
      // set format for lat and lon, saving the old values
      prec = os.precision(15);
  
      os <<    p.getLon() << " ";
      os <<    p.getLat() << " ";
  
      // set format for alt and time, saving the old values
      //(void) os.precision(4);
  
      os <<      p.getZ() << " ";
      os <<      p.getTime() << " ";
  
      // restore precision 
      (void) os.precision(prec);

      // output flag and status info     
      os << p.flags() << " ";
      os << p.status() << " ";
      
      // output the tag
      tagg = p.tag();
      os <<   tagg    << " ";

      os << std::endl;

      // restore ostream flags
      os.setf(os_flags);

  } catch (...) {
      throw;
  }    
  
  return os;

};


int Parcel::bsize() 
{
   int n;

   n = sizeof(real) // lon
      + sizeof(real) // lat
      + sizeof(real) // z
      + sizeof(double)  // t
      + sizeof(double)  // tg
      + sizeof(ParcelFlag) // flagset
      + sizeof(ParcelStatus); // statuses
/*   

   n = sizeof(*this);
*/

   return n;
}

char* Parcel::bserialize( int *n, char *content )
{
     int i;
     int start;
     int sz;
     char *bp;
     
     *n = bsize();
     
     if ( content == NULL ) {
        content = new char[*n];
     }
     
     start = 0;
     
     bp = reinterpret_cast<char *>(&(this->lon));
     sz = sizeof(real);
     for ( i=0; i<sz; i++ ) {
         content[i + start] = bp[i];
     }
     start += sz;
     
     bp = reinterpret_cast<char *>(&(this->lat));
     sz = sizeof(real);
     for ( i=0; i<sz; i++ ) {
         content[i + start] = bp[i];
     }
     start += sz;
     
     bp = reinterpret_cast<char *>(&(this->z));
     sz = sizeof(real);
     for ( i=0; i<sz; i++ ) {
         content[i + start] = bp[i];
     }
     start += sz;
     
     bp = reinterpret_cast<char *>(&(this->t));
     sz = sizeof(double);
     for ( i=0; i<sz; i++ ) {
         content[i + start] = bp[i];
     }
     start += sz;
     
     bp = reinterpret_cast<char *>(&(this->tg));
     sz = sizeof(double);
     for ( i=0; i<sz; i++ ) {
         content[i + start] = bp[i];
     }
     start += sz;
     
     bp = reinterpret_cast<char *>(&(this->flagset));
     sz = sizeof(ParcelFlag);
     for ( i=0; i<sz; i++ ) {
         content[i + start] = bp[i];
     }
     start += sz;
     
     bp = reinterpret_cast<char *>(&(this->statuses));
     sz = sizeof(ParcelStatus);
     for ( i=0; i<sz; i++ ) {
         content[i + start] = bp[i];
     }
     
     return content;
};


void Parcel:: bdeserialize( char *content, int n )
{
     int i;
     int start;
     int sz;
     char *bp;
     
     if ( n <= 0 ) {
        n = bsize();     
     }
     
     lon = 0.0;
     lat = 0.0;
     z= 0.0;
     t = 0.0;
     tg = 0.0;
     flagset = 0;
     statuses = 0;
     
     start = 0;
     
     bp = reinterpret_cast<char *>(&(this->lon));
     sz = sizeof(real);
     if ( (start + sz ) <= n ) {
        for ( i=0; i<sz; i++ ) {
            bp[i] = content[i + start];
        }
        start += sz;
     
        bp = reinterpret_cast<char *>(&(this->lat));
        sz = sizeof(real);
        if ( (start + sz ) <= n ) {
           for ( i=0; i<sz; i++ ) {
               bp[i] = content[i + start];
           }
           start += sz;
     
           bp = reinterpret_cast<char *>(&(this->z));
           sz = sizeof(real);
           if ( (start + sz ) <= n ) {
              for ( i=0; i<sz; i++ ) {
                  bp[i] = content[i + start];
              }
              start += sz;
     
              bp = reinterpret_cast<char *>(&(this->t));
              sz = sizeof(double);
              if ( (start + sz ) <= n ) {
                 for ( i=0; i<sz; i++ ) {
                     bp[i] = content[i + start];
                 }
                 start += sz;
     
                 bp = reinterpret_cast<char *>(&(this->tg));
                 sz = sizeof(double);
                 if ( (start + sz ) <= n ) {
                    for ( i=0; i<sz; i++ ) {
                        bp[i] = content[i + start];
                    }
                    start += sz;
     
                    bp = reinterpret_cast<char *>(&(this->flagset));       
                    sz = sizeof(ParcelFlag);                               
                    if ( (start + sz ) <= n ) {                            
                       for ( i=0; i<sz; i++ ) {                            
                           bp[i] = content[i + start];                     
                       }                                                   
                       start += sz;                                        

                       bp = reinterpret_cast<char *>(&(this->statuses));   
                       sz = sizeof(ParcelStatus);                          
                       if ( (start + sz ) <= n ) {                         
                          for ( i=0; i<sz; i++ ) {                         
                              bp[i] = content[i + start];                  
                          }                                                
                          start += sz;                                     
                                                                           
                       }                                                   
                    }
                 }
              }
           }
        }
     }                        

};

// inputs the parcel state
std::istream& operator>>( std::istream& is, Parcel& p)
{
   real lat,lon,z;
   double t, tagg;
   bool yesno;

   try {   
       is >> lon >> lat;
       p.setPos(lon,lat);
   
       is >> z;
       p.setZ(z);
   
       is >> t;
       p.setTime(t);
   
       is >> yesno;
       p.setFlags(yesno);
       is >> yesno;
       p.setStatus(yesno);

       is >> tagg;
       p.tag(tagg);
   
  } catch (PlanetNav::badlocation) {
      is.setstate(std::ios::failbit);
      throw std::ios::failure("bad earth lat/lon position data");
  } catch (...) {
      throw;
  }    
   
  return is;

};

// inputs the parcel state
std::istream& operator>>( std::istream& is, Parcel* p)
{
   real lat,lon,z;
   double t, tagg;
   bool yesno;

   try {   
       is >> lon >> lat;
       p->setPos(lon,lat);
   
       is >> z;
       p->setZ(z);
   
       is >> t;
       p->setTime(t);
   
       is >> yesno;
       p->setFlags(yesno);
       is >> yesno;
       p->setStatus(yesno);

       is >> tagg;
       p->tag(tagg);
   
  } catch (PlanetNav::badlocation) {
      is.setstate(std::ios::failbit);
      throw std::ios::failure("bad earth lat/lon position data");
  } catch (...) {
      throw;
  }    
   
  return is;

};


}
   
