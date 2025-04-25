

#ifndef GIGATRAJ_PARCEL_H
#define GIGATRAJ_PARCEL_H

#include "gigatraj/gigatraj.hh"
#include "gigatraj/PlanetNav.hh"
#include "gigatraj/MetData.hh"
#include "gigatraj/Interpolator.hh"
#include "gigatraj/Integrator.hh"
#include "gigatraj/ProcessGrp.hh"

#include <iostream>
#include <iosfwd>
#include <streambuf>

#include <math.h>


namespace gigatraj {

/*! \defgroup parcelstuff Parcel-Related Classes

\brief parcels, containers of parcels, and filters of parcels

Deal with the states of air parcels whose trajectories are being traced.

*/

/*! \defgroup parcels Parcels
\ingroup parcelstuff

\brief parcels and containers of parcels

The fundamental object in a trajectory model is the air parcel.
These classes implement the Parcel, as well as the Flock container to
hold collections of Parcels.


*/

/*! \brief type ParcelFlag holds flag values to indicate how a Parcel should be processed

  A ParcelFlag type variable holds flags that control how a Parcel
  is to be processed (or note).  
  
  There are Parcel methods of setting, clearing, and querying each of the 
  standard ParcelFlag values defined within gigatraj.
  
  ParcelFlag bitwise values within the range 256 (0x0100) through 65280 (0xFF00)
  will not be defined within gigatraj itself.  Thus, these bits
  are available for user-assigned purposes.
*/
typedef int ParcelFlag;

/*! @name ParcelFlagVals
    \defgroup parcelflagvals Parcel Flag values
    \ingroup parcelstuff
    Parcels can have ParcelFlag values set to control how they will be traced. 
*/
/*! \brief  ParcelFlag value:  NoTrace = Do not trace this parcel
  \ingroup parcelflagvals
*/
const ParcelFlag NoTrace = 0x0001;
/*! \brief  ParcelFlag value:  SyncTrace = Wait to trace this parcel until the model time exceeds the parcel time
  \ingroup parcelflagvals
*/
const ParcelFlag SyncTrace = 0x0002;



/*! \brief type ParcelStatus holds flag values to indicate a Parcel's current status
 
  A ParcelStatus type variable holds flags that signify that a parcel has
  encountered some kind of special condition that should be noted.
  
  There are Parcel methods of setting, clearing, and querying each of the 
  standard ParcelStatus values defined within gigatraj.
  
  ParcelStatus bitwise values within the range 256 (0x0100) through 65280 (0xFF00)
  will not be defined within gigatraj itself.  Thus, these bits
  are available for user-assigned purposes.
*/  
typedef int ParcelStatus;

/*! @name ParcelStatusVals
    \defgroup parcelstatusvals Parcel Status values
    \ingroup parcelstuff
    Parcels can have ParcelStatus values that indicate special conditions that a parcel
    has encountered.  
*/
/*! \brief  ParcelStatus value:  HitBdy = parcel has hit a boundary of the meteorological data 
  \ingroup parcelstatusvals
*/
const ParcelStatus HitBdy = 0x0001;      
/*! \brief  ParcelStatus value:  HitBad = parcel has hit a bad or missing data value in the meteorological data 
  \ingroup parcelstatusvals
*/
const ParcelStatus HitBad = 0x0002;      
/*! \brief  ParcelStatus value:  NonVert = parcel has a vertical coordinate that is different from the vertical coordinate being used by  the meteorological data 
  \ingroup parcelstatusvals
*/
const ParcelStatus NonVert = 0x0004;      
/*! \brief  ParcelStatus value:  Inert = parcel is not being traced for some reason
  \ingroup parcelstatusvals
*/
const ParcelStatus Inert = 0x0008;      
 
 
      
/*!
\ingroup parcels

\brief holds all information that relates to the state of an air parcel

\details

The Parcel class holds all information that relates to the state of an air parcel.

This basic Parcel object holds only the position and velocity of the parcel
in space.  Other classes derived from it may hold other information,
such as meteorological field values, integrated quantities, or
parcel histories


*/
class Parcel {
      

   /*! \brief outputs the parcel state to a serialized form
   
       This method overlays the "<<" operator, thus enabling a Parcel object's state
       to be written to an output stream using the usual C++ I/O statements.
       Note that "state" includes the time, latitude, longitude, vertical coordinate, status, flags, and tag.
       It does not include a navigational object or meteorological data source.  
   */
   friend std::ostream& operator<<( std::ostream& os, const Parcel& p);

   /*! \brief inputs the parcel state from a serialized form

       This method overlays the ">>" operator, thus enabling a Parcel object
       to be read from an input stream using the usual C++ I/O statements.  
       Note that "state" includes the time, latitude, longitude, vertical coordinate, status, flags, and tag.
       It does not include a navigational object or meteorological data source.  
   */
   friend std::istream& operator>>( std::istream& os, Parcel& p);

   /*! \brief inputs the parcel state from a serialized form
   
       This method overlays the ">>" operator, thus enabling a Parcel object
       to be read from an input stream using the usual C++ I/O statements.  
       Note that "state" includes the time, latitude, longitude, vertical coordinate, status, flags, and tag.
       It does not include a navigational object or meteorological data source.  
   */
   friend std::istream& operator>>( std::istream& os, Parcel* p);


   /// \brief The Flock class, defined elsewhere, will have access to Parcel internals
   friend class Flock;

   /// \brief The Swarm class, defined elsewhere, will have access to Parcel internals
   friend class Swarm;

   public:
   

      /// \brief the type of object this is
      /*!
          The id string member will contain the exact type of the Parcel object,
          making it easier to identify types of Parcels (i.e., subclasses) and
          handle them appropriately.
      */
      static const string id;
  
      /// \brief An exception for improper vertical locations
      class badz {};

      /// \brief An exception for improper times
      class badtime {};


      /// \brief The basic constructor
      /*! 
            This is the basic constructor for a new Parcel object.
      */
      Parcel();

      /// \brief A constructor that initializes position
      /*! 
            This version of the constructor permits initialization
            of the parcel.
            
            \param ilat  latitude, in decimal degrees, positive north 
            \param ilon  longitude, in decimal degrees, positive east
            \param iz  the vertical coordinate
            \param it  the time (is model seconds)
      */
      Parcel( real ilat, real ilon, real iz, double it=0.0 );

      /// \brief Copy-constructor
      /*!
         This method creates a new and independent copy of the object.
      */
      Parcel(const Parcel&);

      /// \brief The destructor
      /*! 
         This is the destructor method for Parcel objects
      */
      ~Parcel(); 

      /// \brief assignment operator
      /*!
         This overlay of the "=" operator permits one Parcel's state information
         to be copied to another.
      */
      Parcel& operator=(const Parcel& src);


      /// \brief returns a pointer to a new Parcel that is a copy of this one
      /*!
          The copy() method creates a new Parcel, copies the current Parcel's
          contents to the new one, and returns a pointer to the new Parcel.
          
          \return a pointer to the newly-created Parcel object
      */
      Parcel* copy() const;
      
      /// \brief changes the planetary navigation used by Parcels
      /*!
          This method changes which planetary navigation object to be used
          in tracing trajectories of Parcels.
          
          The default navigation object is Earth, which is almost certainly
          what most users will want to use.  But if a non-spherical Earth
          is desired, or some other planet, then this method permits
          a change.
         
          Note that the navigational object is shared by all parcels
          in a given process.  That is, setting the navigation object for one parcels
          sets it for all parcels in the process.  If you are
          doing serial processing, just set the navigation for
          one parcel.  If you are doing parallel processsing, with
          different parallel processing tracing distinct batches of 
          Parcels, then each process needs to set the navigation object.
         
          \param newnav the new planetary navigation object
         
      */   
      void setNav( PlanetNav& newnav );
      
      /// \brief Returns a pointer to the current planetary navigation object
      /*!
          This method returns the current planetary navigation object being
          used by all parcles in the current process.
          
          \return a pointer to the PlanetNav object that is being used
      */
      PlanetNav* getNav() const;


      /// \brief changes the meteorological data source used by Parcels
      /*! This method changes the meteorological data source
      
         The default meteorological data source is MetSBRot, or
         tilted solid-body rotation winds.  This data source is
         useful for testing only, so for any practical experiment 
         a call should be made to this method.
      
         Note that the meteorological object is shared among all
         parcels for a given process.  That is, setting the 
         meteorological source for one parcel
         sets it for all parcels in the process. If you are
         doing serial processing, just set the met source for
         one parcel.  If you are doing parallel processsing, with
         different parallel processing tracing distinct batches of 
         Parcels, then each process needs to set the met source object.
         
         \param newmet the new met data source object
         
      */   
      void setMet( MetData& newmet );
      
      /// \brief Returns a pointer to the current met data source object
      /*! This method returns the current meteorological data source being used
          by all Parcels in the current process.
          
          \return a pointer to the MetData object that is being used
      */    
      MetData* getMet() const;



      /// \brief Returns the parcel longitude
      /*! This method returns the parcel's current longitude.
      
         \return the longitude, in decimal degrees, positive east
      */
      inline real getLon() const
      {
         return lon;
      };    

      /// \brief Returns the parcel latitude
      /*! This method returns the parcel's current latitude.
      
         \return the latitude, in decimal degrees, positive north
      */
      inline real getLat() const 
      {
         return lat;
      };    
      
      /// \brief Retrieves the parcel horizontal position (longitude and latitude)
      /*!
         The getPos() method returns the current longitude and latitude of
         the Parcel.
         
        \param longitude (returned) the longitude
        \param latitude (returned) the latitude
      */  
      inline void getPos( real *longitude, real *latitude ) const 
      {
          *longitude = lon;
          *latitude = lat;
      };      
      
      /// \brief Sets the horizontal position (longitude and latitude)
      /*!
        This setPos() method sets the Parcel's longitude and latitude
        
        \param newlon the new longitude
        \param newlat the new latitude
      */  
      void setPos( real newlon, real newlat );

      /// \brief returns the vertical coordinate
      /*!
         The getZ() method returns the current value of the Parcel's
         vertical coordinate.
         
         The nature of the vertical coordinate is a matter of
         user-imposed convention: it can be altitude, pressure, potential temperature
         or any intrinsic physical attribute that varies (mostly) monotonically
         with real altitude.  The only restrcition is that the meteorological
         data source must be instructed to use the same quantity
         as its vertical coordinate.  (Many of the MetData classes have vertical interpolation
         schemes built in to allow the use of vertical coordinates other than the
         data source's native coordinate.)
         
         \return the current value of the Parcel's vertical coordinate
      */
      inline real getZ()  const
      {
         return z;
      }   
      
      /// \brief  sets the vertical coordinate
      /*!
          This method sets the vertical coordinate of the Parcel.
          
         \param newz the new value for the vertical coordinate
      */
      inline void setZ( real newz )
      {
         if ( isfinite(newz) ) {
            z = newz;
         } else {
            throw (Parcel::badz());
         }   
      }       
      
      /// \brief returns the time
      /*! This method returns the current time at which the Parcel's current
          state is valid.
          
          The time is expressed in terms of internal elapsed model time, in seconds.
          
          (Translating between internal model time and some form of calendar-based
          time is done separately.  The MetData classes will provide
          methods for doing such conversions.)
                    
          \return the time, in internal model elapsed seconds.  
      */    
      inline double getTime() const 
      {
         return t;
      }   
      
      /// \brief  sets the time
      /*!
          This method sets the time at which the state of the Parcel is valid.
          
         \param newt the new value for the time, in seconds of elapsed model time.
      */
      inline void setTime( double newt = 0.0 )
      {
         if ( isfinite(newt) ) {
            t = newt;
         } else {
            throw (Parcel::badtime());
         }   
      }       
      
      /// \brief returns the set of parcel flags
      /*! A Parcel object keeps a set of flags that determine
          its behavior during trajectory calculations.
          This method returns the bitwise flags that are set for this Parcel.
          The flags include:
          - NoTrace
      
          return the flags as a bitwise ParcelFlag value
      */
      inline ParcelFlag flags() const
      {
          return flagset;
      }
      
      /// \brief sets this Parcel's ParcelFlags in one go
      /*! This method permits all a Parcel's ParcelFlag settings
          to be specified simultaneously in one
          bitwise integer.
          
          For example, 
          <tt>obj.setFlags( NoTrace | SyncTrace )</tt>
          will set both flags at once.  
        
        \param flags the set of ParcelFlag flags
      */  
      inline void setFlags( ParcelFlag flags ) 
      {
          flagset = flags;
      }
      
      /// \brief returns the value of the NoTrace flag setting
      /*! This method returns the value of the NoTrace flag
          for a Parcel:" True if the Parcel's trajectory is to not be traced,
          False otherwise.
          
          \return the boolean value of the flag
      */
      inline bool queryNoTrace() const 
      {
         return ( (flagset & NoTrace) != 0 );
      } 
      /// \brief sets the NoTrace flag
      /*! This method sets the NoTRace flag for this Parcel.
          If set, then this Parcel will be ignored in any
          further trajectory calculations. (This may be used to 
          deal with boundary conditions, for example.)
      */
      inline void setNoTrace() {
         flagset = flagset | NoTrace;
      }     
      /// \brief clears the value of the NoTrace flag
      /*! This method clears the NoTrace flag for this Parcel.
          If cleared (the default setting), then this Parcel's trajectory can be
          calculated.
      */
      inline void  clearNoTrace() {
         flagset = flagset & ( ~ NoTrace );
      }     
      
      
      
      /// \brief returns the full parcel status
      /*!  A Parcel keeps track of several status settings as its trajectory
           is being traced. This method returns all of them together
           as a bitwise ParcelStatus value.
           
           Possible settings include:
           - BitBdy (the parcel has hit a boundrary)
           
           \return a bitwise ParcelStatus value contaiing all of the 
           current status settings
          
      */
      inline ParcelStatus status() const
      {
         return statuses;
      }
      
      /// \brief sets all parcel status bits
      /*! This method sets all of the Parcel status settings at once.
      
         \param status the set of all parcel status bits
      */      
      inline void setStatus( ParcelStatus status ) 
      {
         statuses = status;
      }
         
      /// \brief returns the HitBdy status 
      /*! This method returns True if the Parcel's HitBdy status
      condition has been set, False otherwise.
      
      \return a bool value reflecting the current HitBdy status.
      */
      inline bool queryHitBdy() const
      {
         return ( (statuses & HitBdy) != 0 );
      } 
      /// \brief sets the value of the HitBdy status
      /*! This method sets the HitBdy status of the Parcel,
          to indicate that its trajectory has intersected with
          some kind of boundary, such as the limit of the meteorological
          data domain. In practice with the default trajectory integrators,
          setting this status should cause the NoTrace flag to be set as well,
          and the Parcel's trajectory will no longer be calculated.
          But it is important to note that this method merely sets
          the status and leaves the implementation of any action to take
          in the hands of the trajectory calculation routines. 
      */
      inline void setHitBdy() {
         statuses = statuses | HitBdy;
      }     
      /// \brief clears the value of the HitBdy status
      /*! This method clears the HitBdy status of the Parcel.
          This means that the Parcel's trajectory has not
          encountered any kind of boundary that would prvent
          further trajectory calculation.
      */    
      inline void  clearHitBdy() {
         statuses = statuses & ( ~ HitBdy );
      }     
      

         
      /// \brief returns the HitBad status 
      /*! This method returns True if the Parcel's HitBad status
      condition has been set, False otherwise.
      
      \return a bool value reflecting the current HitBad status.
      */
      inline bool queryHitBad() const
      {
         return ( (statuses & HitBad) != 0 );
      } 
      /// \brief sets the value of the HitBad status
      /*! This method sets the HitBad status of the Parcel,
          to indicate that its trajectory has encountered an invalid value
          of the meteorological wind fields.
          In practice with the default trajectory integrators,
          setting this status should cause the NoTrace flag to be set as well,
          and the Parcel's trajectory will no longer be calculated.
          But it is important to note that this method merely sets
          the status and leaves the implementation of any action to take
          in the hands of the trajectory calculation routines. 
      */
      inline void setHitBad() {
         statuses = statuses | HitBad;
      }     
      /// \brief clears the value of the HitBad status
      /*! This method clears the HitBad status of the Parcel.
          This means that the Parcel's trajectory has not
          encountered any kind of boundary that would prvent
          further trajectory calculation.
      */    
      inline void  clearHitBad() {
         statuses = statuses & ( ~ HitBad );
      }     
      

      /// \brief returns the value of the NonVert status setting
      /*! This method returns the value of the NonVert status
          for a Parcel: True if the Parcel's vertical coordinate does not 
          match the meteorological data source's vertical coordinate,
          False otherwise.
          
          \return the boolean value of the status
      */
      inline bool queryNonVert() const 
      {
         return ( (statuses & NonVert) != 0 );
      } 
      /// \brief sets the NonVert flag
      /*! This method sets the NonVert status for this Parcel.
          If set, then this Parcel's vertical coordinate is not the same as
          the meteorologicl data source's vertical coordinate.
      */
      inline void setNonVert() {
         statuses = statuses | NonVert;
      }     
      /// \brief clears the value of the NonVert status
      /*! This method clears the NonVert status for this Parcel.
          If cleared (the default setting), then this Parcel's vertical coordinate
          is the same as the meteorlogical data source's vertical coordinate.
      */
      inline void  clearNonVert() {
         statuses = statuses & ( ~ NonVert );
      }     
      
      
      /// \brief increments the horizontal position
      /*! This method applies a change in longitude and latitude
          to this Parcel's position.
          
         \param deltalon the change in longitude to be applied
         \param deltalat the change in latitude to be applied
      */   
      void addPos( real deltalon, real deltalat );
      
      /// \brief increments the vertical position
      /*! This method applies a change in vertical coordinate
          to this Parcel's position.
          
         \param deltaz the increment to be added to the vertical coordinate position
      */
      void addZ( real deltaz );
      
      
      /// \brief advance the position of the parcel
      /*! This method advances the parcel's position
          forward over a time interval.
      
         \param dt the delta-time over which the parcel is to advance
         
         \return always returns zero.
         
      */
      int advance( double dt );
      
      /// \brief return the value of a meteorological field
      /*! This method returns the value of a meteorologicla field at
          the parcel's location and time.
          
          \param quantity the name of the field quantity whose value is ot be returned.
                          The name must conform to the variable name conventions of whatever
                          meteorological data set the parcels are using.
        
          \param metsource a pointer to a MetData object that acts as the source of the fleld.
                           By default, the Parcel's own met data source is used. But this
                           parameter permits an another, alternative met data source to be used.
                        
      */
      real field( std::string quantity, MetData* metsource=NULLPTR ) const;                    

      /// \brief Send this Parcel's information to a processor in the given process group
      /*! This method is used in parallel processing. It sends 
          all information about this Parcel to a processor in a given process group
          
          \param pgroup an object of some subclass of ProcessGrp
          \param id the ID of a processor within the pgroup progress group
      */
      void send( const ProcessGrp* pgroup, const int id) const;

      /// \brief Receive information for this Parcel from a processor in a given process group.
      /*! This method is used in parallel processing. It receives 
          all information about this Parcel from a processor in a given process group.
          
          \param pgroup an object of some subclass of ProcessGrp
          \param id the ID of a processor within the pgroup progress group
      */
      void receive( const ProcessGrp* pgroup, const int id);


      /// \brief sets the integrator for all parcels
      /*! This method replaces the integrator that is used for tracing parcels
      
          \param intg a pointer to the Integrator object to be used for all Parcels
      */
      void integrator( Integrator *intg );
      
      /// \brief returns the integrator being used
      /*!
           This method returns a pointer to the Integrator being used to calculate 
           all Parcel trajectories.
           
           \return a pointer to the Integrator being used
      */
      Integrator* integrator();
      
      /// \brief sets the conformal adjustment method for the Parcel integrator
      /*! 
           This method sets the kind of adjustent 
           to interpolated horizontal wind vectors to account for the curvature of the planer
           near the poles.
           
           \param mode  The kind of adjustment to be applied. A value of zero
                        means no adjustment is applied. The meaning of other values
                        depends on the kind of interpolator being used.
      */
      void conformal( int mode );
      
      /// \brief returns the conformal adjustment method for the Parcel integrator                  
      /*! 
           This method returns the kind of adjustent 
           to interpolated horizontal wind vectors to account for the curvature of the planer
           near the poles.
           
           \return      The kind of adjustment to be applied. A value of zero
                        means no adjustment is applied. The meaning of other values
                        depends on the kind of interpolator being used.
      */
      int conformal();


      /// \brief returns the tag value for this Parcel
      /*! This method returns the value of the tag member of this Parcel.
      
          \return The tag value of the Parcel. This is an arbitrary user-supplied
                  value that the user may use to identify the Parcel.
      */
      inline double tag() const {
          return tg;
      }
                 
      /// \brief sets the tag value for this Parcel
      /*! This method sets the value of the tag member of this Parcel.
      
          \param value  The tag value of the Parcel. This is an arbitrary user-supplied
                        value that the user may use to identify the Parcel.
      */
      inline void tag( double value ) {
          tg = value;
      }
      
      
      /// \brief serializes to a binary form
      /*! This method serializes the Parcel, storing its state 
          into an array of chars in a binary format.
          
          Note that the meteorological data source and the navigation object
          are not included in the serialization or deserializetion.

      
          \param n a pointer to an integer that will be set to the number of bytes in the output array.

          \param content a pointer to an array of bytes that contain the serialized Parcel. If the pointer
                   is NULL, then a new char array is crated and returned. In that case, 
                  it is the responsibility of the caller routine to delete this array
                  once it is no longer needed. 

          \return the content pointer. If that is NULL, then a pointer to a newly created
                  char array is returned.
      */    
      char* bserialize( int *n, char *content=NULL );

      /// \brief deserializes from a binary form
      /*! This method sets the state of the Parcel be deserializing an array of 
          chars in binary format.
          
          Note that the meteorological data source and the navigation object
          are not included in the serialization or deserializetion.

      
          \param content a pointer to an array of bytes that contain the serialized Parcel.

          \param n the number of bytes in the content array. If this is not enough to set 
          all of the parts of the parcel state, then as much of the content as possible 
          will be used to set as much of the state as possible, in this order: longitude,
          latitude, vertical coordinate, time, tag, flags, and status.
      */    
      void bdeserialize( char *content, int n );
     
      /// \brief returns the size of a binary serialzied Parcel object
      /*! This method returns the size, in bytes, of a binary serialized Parcel
          object, as prodcued by the bserialize() method.
          
          It is useful for situations in which the size of the serialized Parcel
          is needed without actually serializing a Parcel.
          
          \returns the number of bytes needed to store a binary serialized Parcel
      */
      int bsize();    
                 
   protected:
      
      //! Longitude
      /*!
       The longitude is in decimal degrees, positive east,
       and bounded by the range [-360.0, 360.0]  
      */
      real lon;

      //! Latitude
      /*!
       The latitude is in decimal degrees, positive north,
       and bounded by the range [-90.0, 90.0]  
      */
      real lat;

      //! Vertical position
      /*!
      The vertical position is the height in terms of 
      whatever vertical coordinates are being used:
      Kelvin for isentropic, millibar for pressure,
      km for altitude.
      */
      real z;

      //! Time
      /*!      
      The time of this parcel, in internal model units.
      Note that the time must be double float, to allow for fine-scale
      time resolutions even when the time must vary in terms of dozens 
      of years.
      (The MetData meteorological data source class will provide
      methods for converting between model time and external
      calendars.) 
      */
      double t;
      
      //! Tag
      /*!
      The parcel tag member is a double-precision floating-point number
      that holds an arbitary user-defined value that the use may use
      to identify the parcel (parhaps uniquely, or perhaps as a member of a larger group).
      */
      double tg;
      
      //! Flags
      /*!      
      The parcel flags member is a bitmask whose elements specify how the 
      parcel will be dealt with:
      */
      ParcelFlag flagset;

      //! Status
      /*!      
      The parcel status member is a bitmask that describes certain conditions of the parcel:
      */
      ParcelStatus statuses;

      //! Navigation
      /*!   The planetary navigation object to be used for all parcels
      */
      static PlanetNav *nav;
      
      //! Meteorological data source
      /*! The meteorological data source to be used for all parcels
      */
      static MetData *metsrc;

      //* Integration method
      /*! the integration method to be used to trace the trajectories
      */
      static Integrator *integ;


};

   
}

#endif




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
