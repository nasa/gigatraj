#ifndef GIGATRAJ_METDATA_H
#define GIGATRAJ_METDATA_H

#include <time.h>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/ProcessGrp.hh"

namespace gigatraj {

/*! @name Meteorological data-handling flags
    These METDATA_* constants are bitwise flag values that are
    used to specify certain aspects of how a meteorological data source
    should behave.
*/
//@{
/// Met Data flag: Convert to standard MKS units
const int METDATA_MKS = 0x0001;
/// Met Data flag; Leave bad-or-missing fill values as-is
const int METDATA_KEEPBAD = 0x0002;
/// Met Data flag; Replace bad-or-missing fill values with NaN (not-a-number)
const int METDATA_NANBAD = 0x0004;
/// Met Data flag; Replace bad-or-missing fill values with infinity
const int METDATA_INFBAD = 0x0008;
/// Met Data flag; Throw a badmetdata error if the met value is a bad-or-missing fill value
const int METDATA_THROWBAD = 0x0010;
//@}

/*!
   \defgroup MetDat Meteorological Data

The Meteorologucal Data classes define a set of objects that 
supply meteorological data to the trajectory model.   

*/


/*!
   \defgroup MetSrc Meteorological Data Sources
\ingroup MetDat

\brief Access meteorological data from sources

The Meteorologucal Data Sources define a set of objects that 
obtain meteorological data from specific sources.   

*/




/*!
\ingroup MetSrc
\brief abstract class for a source of meteorological field data

The MetData class is an abstract class.
An object of this class provides meteorological data at parcel locations.
This is used not only to obtain the winds for tracing the trjectories,
but also to get other meteorological variables for other purposes
(e.g., temperature for determining the minimum temperature a parcel 
has encountered).

Different sources of meteorological data may use different calendar 
systems.  Although most real-life products are expressed in terms of
the Gregorian calendar, model-generated fields may be on a constant 365-day
year (i.e., no leap years, ever) or even a 360-day year.  Thus, the
process of translating between model time and calendar time is
assigned to this object. This abstract method defines means of
translating between the calendar system used byu the data source,
and internal model time.

Different source of meteorological data may also use different 
forms of data (arrays of gridpoints, spherical harmonic coefficients, 
and so on). This abstract class assumes nothing about the form of the data.
It defines means of obtaining values of meteorological fields at 
specific points in space at specific times.

In a parallel processing environment, the available processors may be 
organized into groups, with a processor in each group dedicated to
handing the meteorological data. This may be useful, for example,
if gridded data fiuelds require very large amounts of memory.
In those environments, a MetData object 
will hide such organization, appearing to the user to simply return the
data values needed, when in actuality the parcel-tracing processors
are requesting and receiving data values from the dedicated
meteorological processor. This abstract class defines
methods that implement this ability,

*/

class MetData {

   public:
   
      /// An exception for not having good met data
      class badmetdata {};
   
      /// An exception for being out of the domain of valid met data
      class badoutofboundsmetdata {};
   
      /// An exception for a bad quantity name
      class badquantity {};
   
      /// An exception for a nonexistent calendar date
      class baddate {};
      
      /// An exception involving some unspecified failure to get met data
      class badmetfailure {};

      /// An exception indicating that some feature has not yet been implemented
      class badnotimplemented {};

      /// and exception from trying to do something (like assignment) with two incompatible MetData subclasses
      class badIncompatibleMetDataClass {};

      /*! \brief flags for the MetData class
          
          A MetData object might need flags to control certain aspects
          of its operation.
         
          MetSrcFlag bitwise values are for those. there are held in the MetData flag member.
          
          MetSrcFlag bitwise values within the range 1 to 128 (0x0010) are reserved for 
          use by the (abstract) MetData class and its main subclasses (e.g., MetGridData, MetGridLatLonData).
          The rest of the flags (0x0100 through 0x1100) may be used by the non-abstract subclasses
          for their own purposes,
          (and those uses may conflict between those subclasses)
          
      */
      typedef int MetSrcFlag;
      
      /// flag value: Trace Iso-surface flow (e.g., isentropic for Theta coordinates)
      static const MetSrcFlag MET_ISO = 1; 

      /// interprocess communications tag: "This is a Request"
//      static const int PGR_TAG_REQ = 1040;
      /// interprocess communications command: "All Done For Now"
//      static const int PGR_CMD_DONE = 0;

      /// the default constructor
      /*! This is the default constructor.
      */
      MetData();

      
      /// the destructor
      /*! This is the destructor.
      */
      virtual ~MetData() {};


      /// the copy constructor
      /*! This is the copy constuctor.
      */
      MetData(const MetData& src);


      /// return the type of MetData object this is
      /*! This method retuns a string that identifies the specific class of MetData this object is,.
      
          \return the namne of the class 
      */
      virtual std::string id() const = 0;

   
   
      
      /// convert model time to a date+time calendar string
      /*! This method converts internal model time to a date+time calendar string.
          The calendar system will depend on the specific data source (i.e., the
          child class being used).

          \param time the model time that is to be converted
          \param format a format code that determines the precision of the calendar output
                  * -999 = use the default format 
                  * 0 = date only, with no time (yyyy-mm-dd)
                  * 1 = date plus the hour (yyyy-mm-ddThh)
                  * 2 = date plus the hour and minute (yyyy-mm-ddThh:mm)
                  * 3 = date plus the hour, minute, and integer second (yyyy-mm-ddThh:mm:ss)
                  * 4 = date plus the hour, minute, and floating second (to four decimal places) (yyyy-mm-ddThh:mm:ss.ssss) 
          \return the time as expressed in some data-specific calendar system
          
      */   
      virtual string time2Cal( double time, int format=-999 )  { return ""; };




      /// convert a date+time calendar string to model time
      /*! This method converts a date+time calendar string to internal model time
          The calendar system will depend on the specific data source (i.e., the
          child class being used).


          \param caldate a time as expressed in some data-specific calendar system
          \return the internal model time that corresponds to the input string
               
      */   
      virtual double cal2Time( string caldate ) { return 0.0; };


      /// sets the equivalence between internal model time and a date+time calendar string
      /*! A data source will usually associate meteorological field values with
          some kind of time stamp, here expressed as a string. The trajectory mode;
          represents time as a double-precision floating-point number.
          This routine defines the equivalence between the two.
          
          \param cal  a time as expressed in some data-specific calendar system 
          \param time the model time that corresponds to cal  
      */
      virtual void defineCal( string cal, double time=0.0 ) { return; };



     /// sets the Met source to trace parcels on an iso-surface of the vertical coordinate
     /*! Sometimes it is desired to trace parcels along an iso-surface of the vertical coordinate.
         for example, when using potential temperature as a vertical coordinate tracing
         parcel trajectories isentropically is often a good approximation.
         This methods sets the meteorological data source to define the vertical wind
         (that is, the time rate of change of the vertical coordinate) as zero, always and everywhere.
     
     */
     void setIsoVertical();

     /// clears the Met source from tracing on an iso-surface of the vertical coordinate
     /*! This method clears the internal flag that indicates that the vertical wind
         is defined as zero. Consequently, the vertical wind native to the data source is
         used instead.
     */
     void clearIsoVertical();
     
     /// returns whether parcles are being traced on an iso-surface of the vertical coordinate 
     /*! This method returns a boolean that indicates whether the parcels are being
         traced on an iso-surface of the vertical coordinate.
     
         \return true if the vertical wind is defined as zero, false otherwise.
     */
     inline bool isoVertical() {
         return (( flags & MET_ISO ) != 0);
     }

      /// configures the met source
      /*! This method provides a means of setting options specific
          to a subclass. In this way, objects of a subclass
          that has special capabilities may be upcast to MetData
          in a program that can use any of several met data sources,
          and yet still be able to configure those special options
          that apply to the subclass.

          \param name the name of the option that is to be configured.
                      Names that are not recognized by a specific subclass are
                      silently ignored.
                      
          \param value the value to be applied to the named configuration option
      
      */
      virtual void setOption( const std::string &name, const std::string &value );

      /// configures the met source
      /*! This method provides a means of setting options specific
          to a subclass. In this way, objects of a subclass
          that has special capabilities may be upcast to MetData
          in a program that can use any of several met data sources,
          and yet still be able to configure those special options
          that apply to the subclass.

          \param name the name of the option that is to be configured.
                      Names that are not recognized by a specific subclass are
                      silently ignored.
                      
          \param value the value to be applied to the named configuration option
      
      */
      virtual void setOption( const std::string &name, int value );

      /// configures the met source
      /*! This method provides a means of setting options specific
          to a subclass. In this way, objects of a subclass
          that has special capabilities may be upcast to MetData
          in a program that can use any of several met data sources,
          and yet still be able to configure those special options
          that apply to the subclass.

          \param name the name of the option that is to be configured.
                      Names that are not recognized by a specific subclass are
                      silently ignored.
                      
          \param value the value to be applied to the named configuration option
      
      */
      virtual void setOption( const std::string &name, float value );

      /// configures the met source
      /*! This method provides a means of setting options specific
          to a subclass. In this way, objects of a subclass
          that has special capabilities may be upcast to MetData
          in a program that can use any of several met data sources,
          and yet still be able to configure those special options
          that apply to the subclass.

          \param name the name of the option that is to be configured.
                      Names that are not recognized by a specific subclass are
                      silently ignored.
                      
          \param value the value to be applied to the named configuration option
      
      */
      virtual void setOption( const std::string &name, double value );


      /// queries configuration of the met source
      /*! This method provides a means of reading options specific
          to a subclass. In this way, objects of a subclass
          that has special capabilities may be upcast to MetData
          in a program that can use any of several met data sources,
          and yet still be able to configure those special options
          that apply to the subclass.

          \param name the name of the option that is to be obtained.
                      Names that are not recognized by a specific subclass 
                      will return an empty string.
                      
          \param value (output) the value to be obtained from the named configuration option
      
      */
      virtual bool getOption( const std::string &name, std::string &value );


      /// queries configuration of the met source
      /*! This method provides a means of reading options specific
          to a subclass. In this way, objects of a subclass
          that has special capabilities may be upcast to MetData
          in a program that can use any of several met data sources,
          and yet still be able to configure those special options
          that apply to the subclass.

          \param name the name of the option that is to be obtained.
                      Names that are not recognized by a specific subclass 
                      will return 0.
                      
          \param value (output) the value to be obtained from the named configuration option
      
      */
      virtual bool getOption( const std::string &name, int &value );


      /// queries configuration of the met source
      /*! This method provides a means of reading options specific
          to a subclass. In this way, objects of a subclass
          that has special capabilities may be upcast to MetData
          in a program that can use any of several met data sources,
          and yet still be able to configure those special options
          that apply to the subclass.

          \param name the name of the option that is to be obtained.
                      Names that are not recognized by a specific subclass 
                      will return 0.
                      
          \param value (output) the value to be obtained from the named configuration option
      
      */
      virtual bool getOption( const std::string &name, float &value );


      /// queries configuration of the met source
      /*! This method provides a means of reading options specific
          to a subclass. In this way, objects of a subclass
          that has special capabilities may be upcast to MetData
          in a program that can use any of several met data sources,
          and yet still be able to configure those special options
          that apply to the subclass.

          \param name the name of the option that is to be obtained.
                      Names that are not recognized by a specific subclass 
                      will return 0.
                      
          \param value (output) the value to be obtained from the named configuration option
      
          \return true if the option was valid; false if the value returned is meaningless
      */
      virtual bool getOption( const std::string &name, double &value );


     /// create a copy of a MetData subclass object, as a MetData superclass object
     /*! This method creates a new copy of a specific MetData object, which 
         ordinarily would mean that the copy would be of the same specific subclass
         as the original. Which in turn would mean that it could not be used in
         the generic, subclass-independent calling routines which know only about
         the MetData superclass. To remedy this, the copy() method creates the copy
         and retuns it as a MetData object.
     
           \return returns a MetData pointer to the copy. Note that the calling routine is 
                   responsible for deleting the new object.

     */
     virtual MetData *genericCopy() = 0;

     /// obtain the value of the zonal wind component at a given point, in m/s
     /*! This method obtains the value of the zonal wind component at a given point.
         The return value is guaranteed to be in meters per second, regardless of 
         the units used in the original data.

         \param time the model time for which data is to be returned  
         \param lon the longitude at which data is to be returned
         \param lat the latitude at which data is to be returned
         \param z the vertical coordinate value at which the data is to be returned.
      
         \return the wind value, in m/s
     
      */
      virtual real get_u( double time, real lon, real lat, real z) { return 0.0; };



     /// obtain the value of the meridional wind component at a given point, in m/s
     /*! This method obtains the value of the meridional wind component at a given point.
         The return value is guaranteed to be in meters per second, regardless of 
         the units used in the original data.

         \param time the model time for which data is to be returned  
         \param lon the longitude at which data is to be returned
         \param lat the latitude at which data is to be returned
         \param z the vertical coordinate value at which the data is to be returned.
     
         \return the wind value, in m/s
      
      */
      virtual real get_v( double time, real lon, real lat, real z) { return 0.0; };



     /// obtain the value of the vertical wind component at a given point, in m/s
     /*! This method obtains the value of the vertical wind component at a given point.
         The return value is guaranteed to be in meters per second, regardless of 
         the units used in the original data.

         \param time the model time for which data is to be returned  
         \param lon the longitude at which data is to be returned
         \param lat the latitude at which data is to be returned
         \param z the vertical coordinate value at which the data is to be returned.
     
         \return the wind value, in m/s
     
      */
      virtual real get_w( double time, real lon, real lat, real z) { return 0.0; };


     /// returns the name of the U-wind in this met data source
     /*! This method returns the name (and optionally the units) of the zonal wind in this data source.
     
         \param units (output) if not NULL, a pointer to a string in which are returned the units of the quantity
         
         \return a string containing the name of the zonal wind
     */
     virtual std::string u_wind( std::string *units=NULL ) const; 

     /// returns the name of the V-wind in this met data source
     /*! This method returns the name (and optionally the units) of the meridional wind in this data source.
     
         \param units (output) if not NULL, a pointer to a string in which are returned the units of the quantity
         
         \return a string containing the name of the meridional wind
     */
     virtual std::string v_wind( std::string *units=NULL ) const; 

     /// returns the name of the w-wind in this met data source
     /*! This method returns the name (and optionally the units) of the vertical wind in this data source.
     
         \param units (output) if not NULL, a pointer to a string in which are returned the units of the quantity
         
         \return a string containing the name of the vertical wind
     */
     virtual std::string w_wind( std::string *units=NULL ) const; 

     /// returns the name of the vertical coordinate in this met data source
     /*! This method returns the name (and optionally the units) of the vertical coordinate in this data source.
         This is not necessarily the vertical coordinate that is native to the data source,
         but the one that is actually used for tracing parcels' trajectories. 
         (Any conversions between any native coordinates and the parcel-tracing
         coordinate are done internally to the class/subclass.) 
     
         \param units (output) if not NULL, a pointer to a string in which are returned the units of the quantity
         
         \return a string containing the name of the vertical coordinate
     */
     virtual std::string vertical( std::string *units=NULL ) const; 

      /// sets the name of the vertical coordinate used in the data source
      /*! This method forces the meteorological data source object to use a specific vertical coordinate.
          If the original data for this source use a different vertical coordinate,
          data fields read in will be vertically interpolated to the new coordinate system
          automatically whenever they are read in.
       
          This enables a single data source to be used for kinematic trajectories
          (in which the vertical coordinate is usually pressure or pressure altitude)
          and isentropic trajectories (in which the vertical coordinate is
          potential temperature).
          
          \param quantity the desired quantity to be used for the vertical coordinate
          \param units the units of the desired vertical coordinate
      */
      virtual void set_vertical( const std::string quantity, const std::string units ) 
      {
          throw (badnotimplemented());
      }
      
      /// returns whether the vertical coordinate increases or decreases with altitude
      /*! The method returns whether the current vertical coordinate increases or decreases with altitude.
      
           \return +1 if the vertical coordinate increases, -1 if it decreases.
      
      */
      virtual int vIncrease() const
      {
          throw (badnotimplemented());
      }

      /// gets the units of the vertical coordinate use in the data source
      /*! This method returns the units of the vertical coordinate that is being use in this data source.
      
         \return the name of the vertical coordinate units
      */
      virtual std::string vunits() const;

     /// obtain the values of all three wind components at a given point, in m/s
     /*! This method obtains the value of the zonal, meridional, and vertical wind component at a given point.
         The return value is guaranteed to be in meters per second, regardless of 
         the units used in the original data.

         \param time the model time for which data is to be returned  
         \param lon the longitude at which data is to be returned
         \param lat the latitude at which data is to be returned
         \param z the vertical coordinate value at which the data is to be returned.
         \param u the returned zonal wind value
         \param v the returned meridional wind value
         \param w the returned vertical wind value
     
      */
      virtual void get_uvw( double time, real lon, real lat, real z, real *u, real *v, real *w) 
      { *u=0.0; *v=0.0; *w=0.0; };

     /// obtain the values of all three wind components for arrays of given points, in m/s
     /*! This method obtains the values of the zonal, meridional, and vertical 
         wind component at set of given points.
         The return values are guaranteed to be in meters per second, regardless of 
         the units used in the original data. Invalid (bad or missing data) values are set to Not-a-Number (NaN).

         \param time the model time for which data is to be returned  
         \param n the number of elements in the lons, lats, and zs arrays.
         \param lons a pointer to an array of longitudes at which data are to be returned
         \param lats a pointer to an array of latitudes at which data is are be returned
         \param zs a pointer to an array of vertical coordinate values at which the data are to be returned.
         \param u a pointer to an array for the returned zonal wind values
         \param v a pointer to an array for the returned meridional wind values
         \param w a pointer to an array for the returned vertical wind values
     
      */
      virtual void get_uvw( double time, int n, real* lons, real* lats, real* zs
      , real *u, real *v, real *w);

     /// returns the forecast lead time for the U wind 
     /*! This method returns the forecast lead time, in hours, for the U wind.

         \param time the model time for which the U wind's forecast lead time is desired.
         
         \return The forecast lead time, in hours. If zero, then the data are from an analysis or assimilation.
                 If less than zero, then the data source is a free-running model for which he concept of
                 forecast lead time does not apply. If NaN, then the information is not available
                 from this data source. 
          
     */
     virtual double forecastLeadTime( double time );


      /// check the validity of a quantity name
      /*! This method checks that a quantity name is recognized by this data source.
          
          \param quantity  the name of the physical quantity (meteorological field) to be tested  
      
          \return true if recognized, false otherwise
      */
      virtual bool legalQuantity( const std::string quantity ) { return true; };    
      
      /// obtain the units of a quantity provided by the data source
      /*! This method provides the units of a quantity that the meteorological data soirce
          provides. The meaning of the returned string is determined solely
          by the data source itself, according to whatever conventions is happens
          to follow.
          
          \param quantity the quantity whose units are desired
          \param time the model time for which data is valid. (This is used if a data file has to be read in order to determine the units)
          \param flags flag values:
                  METDATA_MKS = return the units of the quantity if converted to MKS units
                  METDATA_FAILBAD =(ignored)
                  METDATA_NANBAD = (ignored)
                  METDATA_INFBAD = (ignored)
                  METDATA_KEEPBAD = (ignored)
          \return a string with the units. An empty string "" indicates a unitless quantity. If the units
                  cannot be determined, then "UNKNOWN" is returned.
      */
      virtual std::string units( const std::string& quantity, double time=0, int flags=0  ) { return "UNKNOWN"; };    

      /// obtain the value of a meteorological field at a given point 
      /*! this mwthod obtain the value of a meteorological field at a given point. 
          Unlike the get_u(), get_v(), get_w(), and get_uvw()
          methods, quantities returned by getData() are not guaranteed to be in MKS units.
      
          \param quantity a string that  holds the name of the desired quantity.      
          \param time the model time for which data is to be returned  
          \param lon the longitude at which data is to be returned
          \param lat the latitude at which data is to be returned
          \param z the vertical coordinate value at which the data is to be returned.
                    If the quantity is an inherently two-dimensional quasihorizontal surface,
                    then this parameter is ignored.
          \param flags flag values:
                  METDATA_MKS = convert the return value to MKS units
                  METDATA_FAILBAD = throw an error if the value to be returned is invalid.
                  METDATA_NANBAD = return a NaN value if the value to be returned is invalid.
                  METDATA_INFBAD = return a value of infinity if the value to be returned is invalid.
                  METDATA_KEEPBAD = return an invalid value as-is.

          \return the value of the quantity at the given location and time

      */
     // virtual real getData( string quantity, double time, real lon, real lat, real z, int flags=0) { return 0.0; };
      virtual real getData( string quantity, double time, real lon, real lat, real z, int flags=0) { return 0.0; };

      /// obtain the value of a meteorological field at a set of given points 
      /*! This method obtains values of a meteorological field, given arrays of given points. 
          Unlike the get_u(), get_v(), get_w(), and get_uvw()
          methods, quantities returned by getData() are not guaranteed to be in MKS units.
      
          \param quantity a string that  holds the name of the desired quantity.      
          \param time the model time for which data is to be returned  
          \param n  the number of points at which values are desired
          \param lons a pointer to an array of longitudes at which data are to be returned
          \param lats a pointer to an array of latitudes at which data are to be returned
          \param zs a pointer to an array of vertical coordinate values at which the data are to be returned.
                    If the quantity is an inherently two-dimensional quasihorizontal surface,
                    then this parameter is ignored.
          \param values a pointer to an array of values of the quantity at the given location and time
          \param flags flag values:
                  METDATA_MKS = convert the return value to MKS units
                  METDATA_FAILBAD = throw an error if the value to be returned is invalid.
                  METDATA_NANBAD = return a NaN value if the value to be returned is invalid.
                  METDATA_INFBAD = return a value of infinity if the value to be returned is invalid.
                  METDATA_KEEPBAD = return an invalid value as-is.

      */
      virtual void getData( string quantity, double time, int n, real* lons, real* lats, real* zs, real* values, int flags=0 );

      /// (parallel processing) returns the process group to which this processor and MetData belong
      /*!  This method returns the process group to which the processor that
           is running it belongs. It also returns the id within that group
           of the processor that is dedicated to handling meteorological data, if
           such exists.

            \param met a pointer to an integer to hold the ID of a processor
                   within the processor group that is dedicated to 
                   managing meteorological data. 
            
            \return a pointer to the ProcessGrp to which the processor met belongs       
      */
      ProcessGrp* getPgroup( int* met=NULL) const;
      
      /// (parallel processing) sets the process group to which this processor and MetData belong
      /*! This method sets the processor group to which the processor that
           is running it will belong. It can also set the id within that group
           of the processor that is dedicated to handling meteorological data.

         \param pg a pointer to the process group to be used
         \param met the processor ID within process group pg, that indicates a dedicated meteorological data processor.  
                    An ID of -1 indicates that no dedicated met processor is to be used.
  
      */
      void setPgroup( ProcessGrp *pg, int met = -1 );
    

      /// (parallel processing) indicates whether this processor and MetData object is a client for met data.
      /*! This methods returns whether this processor is a client for met data--that is, whether is
          is a tracer processor that traces parcel trajectories.
      
          \return true if this processor is a client for met data

      */
      inline bool isMetClient()
      {
          return ( my_pgroup != NULLPTR && my_metproc >= 0 && my_pgroup->id() != my_metproc );
      }
      
      /// (parallel processing) indicates whether this processor and MetData object is a server for met data.
      /*! This methods returns whether this processor is a server for met data, providing data to
          tracer processor clients.
      
          \return true if this processor is a server for met data

      */
      inline bool isMetServer()
      {
          return ( my_pgroup != NULLPTR && my_metproc >= 0 && my_pgroup->id() == my_metproc );
      }
      
      /// synchronizes multiple processors
      /*! This methods synchronizes this processors with the others in its processor group., or
          optionally the processors in the parent of the current group.
          
          \param mode  if 0, then synchronization is with the current group, which means
                       that the met server and tracing client processors will be synchronized.
                       If 1, then synchronization is done with the current group's parents,
                       which means that the different subgroups will be synchronized.
      */
      void sync( int mode );                 

      /// waits a certain time
      /*! This method waits for a time, to avoid have a large number of processors
          running in parallel, all attempting to access the same network resource at the
          same moment. Instead, the processors of a single group may all
          delay together for a randomly-determined tie, to offset them from other
          processor groups.
          
          In the base class here, the wait time is fixed at zero, and so there is 
          is no delay. 
          
          Subclasses which require such delays may override this virtual method
          with their own.
      */
      virtual void delay();

      /// (parallel processing) get from a met data server process, the status of a recent request
      /*! In a parallelprocessing environment, this method reads a status code sent by a dedicated
          met data server processor that
          reflects the current status of a recent request that this client has made to that server.
          
          /return an integer containing a status code, one of the PGR_STATUS_* codes.
      */ 
      int receive_svr_status();   
    
      /// (parallel processing) sends to a met client process, from a met data server process, the status of a recent request
      /*! In a parallelprocessing environment, this method causes a dedicated
          met data server processors to send a PGR_STATUS_* status code 
          to a met cleint processor that
          reflects the current status of a recent request that the client has made to this server.
          
          /param status an integer containing a status code, one of the PGR_STATUS_* codes.
          /param client the ID of the client processor to whom the staus is to be sent
      */ 
      void send_svr_status( int status, int client );   
    
      ///  (parallel processing) listen for requests from met data clients 
      /*! In a parallel processing environment, this method causes a dedicated 
          met data server processor to begin listening for requests from
          the client (tracer) processors in its processor group.
          
          It returns when all the client processors have called 
          this class's signalMetDone() method.
          
          If not in a parallel processing environment, or if this is not
          a met data server processor, then this method returns immediately.

      */
      virtual void serveMet() = 0;
     
      ///  (parallel processing) signal the met data server that no more data are needed for now 
      /*! In a parallel processing environment, this method causes a parcel-tracing
          processors to signal its dedicated met data server processor that this processor 
          will need no more meteorological data for the time being.
          
          If not in a parallel processing environment, or if this is not
          a met data client processor, then this method returns immediately.

      */
      void signalMetDone();
     
   
      /// (parallel processing) prepare for data acquisition
      /*! This method is used as an easy way to set up for
          data acquisition in a parallel processing environment,
          regardless of whether the processor running it 
          traces parcel trajectories (met data client) or
          manages meteorologicla fields (met data server).
          
          If this processor is dedicated
          to handling meteorological data, then this method calls the serveMet()
          method, and when serveMet() returns having satisfied all
          client processor requests, this method returns 0.
          
          If this processor is a
          client of a dedicated met processor (or if it is running outside
          of a parallel-processing environment), then this method returns
          1 immediately. 
          
          Thus, this method may be used in an if-statement to let clients
          execute data-handling code conditionally:
          
          \code
             MetData *metsrc;
             
             // Code to create a specific kind of MetData source
             //   object and make the pointer variable "metsrc" point to it, should go here
             
             // read the data if we are a client
             if ( metsrc->useMet() ) {
                
                // Get temperature data at a certain location and time from this source.
                // If serial processing, this code actually reads the data.
                // If parallel processing, then this code is executed by clients
                // who ask the met server process for data values at certain gridpoints.
                // The server process is stuck in the useMet() call above, anwering the
                requests of the other processors.
                xval = metsrc->getData("temperature","2009-10-23T12", 10.34, 34.56, 10.0 );
                
                // A call to this method is needed for the clients to tell the met
                // server processor that they are done.  The met server process exits
                // the useMet() call above, and all the processors sync up and
                // continue from the end of this if statement
                metsrc->signalMetDone()
             }
              
          \endcode
      */
      int useMet();


      /// sets the configuration file name
      /*! A MetData object may read in a configuration file that contains
          settings. If this is the empty string, then no configuration file is read
          in. A MetData subclass may set a default configuration file name
          and it may call the readConfig() method in its constructor.
          But a user may slso set the file name here, in which case it will 
          if different from the current configuration file name will
          trigger a readConfig() call.
          
          \param filename a string with the name of the configuration file
          
      */
      void configFile( const std::string &filename );
      
      /// returns the configuration file name
      /*! This method returns the name of the configuration file.
      
          \return the name of the file
      */
      std::string configFile() const;
      
      /// reads settings form a configuration file
      /*! This method reads setting from a configuration file and applies them
          to the MetData object.
          
          The format of the file is as follows:
            
            - lines containing only whitespace are ignored
            - lines whos efirst non-whitespace character is '#' are ignored as comments
            - all other lines consis of a keyword, a type, and a value, separated by ':'
            - valid keywords consist of a leading alphabetic characterm followed by
              zero or more alphanumeric characters and underscore characters.
            - valid types are: 'I' (integer), 'F' (float), 'S' (string), and
              'B' (boolean; either 'T' or 'F').  
      
      */    
      void readConfig();      
      
      


      /*! This controls printing of lots of debugging/trace messages.
          It is public, so that it can be set and queried directly by external routines.
          But it is recommended that the debug() methods be used for this instead,
          in most circumstances
      */
      int dbug;
      
      /// sets the debugging level
      /*! This method sets the debugging level, which causes more
          messages (hier number) or fewer messages (lower number) 
          to be printed to stderr. A level of zero or less indicates that no
          debugging messages are to be printed.
          
          \param level the desired debugging level.

      */
      virtual void debug( int level );
      
      
      /// returns the current debugging level
      /*! This method retusn the debugging level.
      */
      inline int debug() const
      {
          return dbug;
      };
      
   protected:
   
      /// pointer to a ProcessGrp object to which this processor belongs
      ProcessGrp *my_pgroup;

      /// processor ID of a dedicated met data processor in this process group
      int my_metproc;


      /// short label for this data source
      static std::string shortdesc;
      
      /// long descriptive string for this data source
      static std::string longdesc;


      /// initial time string that maps to internal model time 0.0000
      std::string t0;
      
      /// flags concerning this data source (the tne MetSrcFlag values)
      int flags;
      
      /// copy a given object into this object
      /*! This method copies properties of a given MetaData object
          into this object.
      
          \param src the object to be copied
      */    
      void assign( const MetData& src );
      
      /// the current time
      /*! The data obtained through this object may have an expiration
         date. We need a time stamp against which to compare the expiration
         date. Doing this dynamically, calling time() whenever we need
         to do the comparison, is likely to introduce issues in
         reproducability. So we take a single time reading when
         the met source object is created, and we use that value throughout
         a model run.
      */   
      time_t now;
      
      /// a scale factor by which to multiple the vertical wind, to
      /// match the vertical coordinates
      real wfctr;

      /// holds the name of a config file with settings that can be read in
      std::string cfgFile;


      /// trims whitespace from a string (used for reading configs)
      /*! This method removes leading and trailing whitepsace from an input string
          It is used in processing lines read from a comnfiguration file.
      
          \param line a reference to the input string. The string is trimmed in place.
          
      */
      void trimString( std::string& line ) const;
  
      /// extracts a keyword from a config line
      /*! This method extracts a keyword from a configuration line.
          A keyword must begin with an alphabetic character (upper
          or lower case), followed by zero or more alphabetic or
          numeric characters, or an underscore character.
          It is terminated by the separator character. Any leading or
          trailing whitespace is removed..
          
          \param line a reference to the input configuration string
          \param pos a reference to the character position in the string at which
                     scanning is to begin. If a syntax error is found, then pos
                     remains unchanged. Otherwise, it is set to one character past 
                     the separator.
          \param sep the separator character
          \return a string containing the keyword           
      */
      std::string get_cfg_keyword( std::string& line, int* pos, char sep ); 

      /// extracts a type from a config line
      /*! This method extracts a type from a configuration line.
          A type must be one of 'I' (integer), 'F' (float),
          'D' (double), 'S' (string), or B (boolean).

          It must be terminated by the separator character. Any leading or
          trailing whitespace is removed..
          
          \param line a reference to the input configuration string
          \param pos a reference to the character position in the string at which
                     scanning is to begin. If a syntax error is found, then pos
                     remains unchanged. Otherwise, it is set to one character past 
                     the separator.
          \param sep the separator character
          \return a string containing the type           
      */
      std::string get_cfg_type( std::string& line, int* pos, char sep ); 

      /// extracts a value from a config line
      /*! This method extracts a value from a configuration line.
          A value may contain any characters, but it should conform
          to the specified type. All characters form the
          startig position to the end of the string are used.
          Any leading or trailing whitespace is removed.

          \param line a reference to the input configuration string
          \param pos a reference to the character position in the string at which
                     scanning is to begin. If a syntax error is found, then pos
                     remains unchanged. Otherwise, it is set to one character past 
                     the separator.
          \return a string containing the value           
      */
      std::string get_cfg_value( std::string& line, int* pos ); 

      /// converts a string to an integer
      /*! This method converts a string to an integer
      
            \param value a reference to the string to be converted
            \param result a pointer to the output converted value
            \return true if the conversion was successful, false otherwise
      */
      bool str2int(const std::string &value, int* result  ) const;
      /// converts a string to a float
      /*! This method converts a string to a float
      
            \param value a reference to the string to be converted
            \param result a pointer to the output converted value
            \return true if the conversion was successful, false otherwise
      */
      bool str2float(const std::string &value, float* result) const;
      /// converts a string to a double
      /*! This method converts a string to a double
      
            \param value a reference to the string to be converted
            \param result a pointer to the output converted value
            \return true if the conversion was successful, false otherwise
      */
      bool str2dbl(const std::string &value, double* result) const;
      /// converts a string to a boolean
      /*! This method converts a string to a boolean
      
            \param value a reference to the string to be converted
            \param result a pointer to the output converted value
            \return true if the conversion was successful, false otherwise
      */
      bool str2bool(const std::string &value, bool* result) const;

      /// converts an integer to a string
      /*! This method converts a integer to a string
            \param value the int to be converted
            \param result a reference to the output string
            \return true if the conversion was successful, false otherwise
      */
      bool int2str( int value, std::string& result ) const;
      
      /// converts a float to a string
      /*! This method converts a floating point number to a string
            \param value the float to be converted
            \param result a reference to the output string
            \return true if the conversion was successful, false otherwise
      */
      bool float2str( float value, std::string& result ) const;
      
      /// converts a double to a string
      /*! This method converts a double precision number to a string
            \param value the double to be converted
            \param result a reference to the output string
            \return true if the conversion was successful, false otherwise
      */
      bool dbl2str( double value, std::string& result ) const;
      
      /// converts a boolean to a string
      /*! This method converts a boolean to a string
            \param value the boolean to be converted
            \param result a reference to the output string
            \return true if the conversion was successful, false otherwise
      */
      bool bool2str( bool value, std::string& result ) const;
      

      /// returns an invalid value for the time
      /*! This method returns a Not-a-Number (NaN) for the time value.
          It can be used to mark times that are invalid.
          
          \return a double-precision NaN value
      */
      double NaNTime() const;    

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
