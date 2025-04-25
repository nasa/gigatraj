#ifndef GIGATRAJ_METGRIDDATA_H
#define GIGATRAJ_METGRIDDATA_H

#include <string>
#include <vector>
#include <deque>
#include <map>
#include <set>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/GridFieldSfc.hh"
#include "gigatraj/GridField3D.hh"
#include "gigatraj/HLatLonInterp.hh"
#include "gigatraj/MetData.hh"
#include "gigatraj/FilePath.hh"
#include "gigatraj/FileLock.hh"
#include "gigatraj/PAltOTF.hh"
#include "gigatraj/PAltDotOTF.hh"


namespace gigatraj {

///@name Interprocess Communications Commands
//@{
/// Interprocess Communications Commands: "Transfer 2D metadata"
static const int PGR_CMD_M2M = 100;
/// Interprocess Communications Commands: "Transfer 2D data"
static const int PGR_CMD_M2D = 25;
/// Interprocess Communications Commands: "Transfer 3D metadata"
static const int PGR_CMD_M3M = 30;
/// Interprocess Communications Commands: "Transfer 3D data"
static const int PGR_CMD_M3D = 35;
/// Interprocess Communications Commands: "Transfer 3D vector data"
static const int PGR_CMD_M3DV = 40;
/// Interprocess Communications Commands: "Transfer 2D vector data"
static const int PGR_CMD_M2DV = 45;
//@}


/*!
\ingroup MetSrc
\brief abstract class for a gridded meteorological field data

The MetGridData class is an abstract subclass of MetData.  
An object of this class provides
meteorological data at parcel locations, when the meteorological data
comes in the form of 2D and 3D arrays (grids) of data valid at specific times.

MetGridData handles the operations which are generic across
all gridded data, such as data caching.  
Actually reading grids of data, and translating between internal model
time and external met data timestamps, must be implemented by 
the child classes.  

Note also that child classes are free to implement
their own naming conventions for physical quantities.  For example,
the zonal wind component may be "eastward_wind" as in the UCAR CF conventions,
or "U" as in common use, or even "zonal wind".  The MetData and MetGridData
abstract classes are agnostic with regard to what you call the various
met fields.  But remember that many of the Filter class objects might
not be: a Filter that determines whether a parcel is above or below
the tropopause, for example, might easily be hard-coded to look for  
"tropopause_air_pressure" or "TropP" or "TPRS".  It is to be hoped that
most Filters will be written in a more generic and flexible way,
but there are no guarantees.

In addition to the implementing the virtual methods listed here, 
any class which inherits
the MetGridData class must do the follinwg:

1) If the naming convention for physical quantities does not
   follow the UCAR CF naming conventions, then the class must 
   set the wind_vert_name, wind_ew_name, and wind_ns_name variables
   in its constructor, as well as delete us,vs, and wd, then
   create new MetCache3D variables for these which are initialized
   with the new names.

2) It must implement the set_vertical() method, using the 
   chosen naming conventions to set the name of the vertical wind
   based on the name of the chosen vertical coordinate.

3) It must implement the new_directGrid3D() and new_directGridSfc()
   methods.
   These are the routines that will actually 
   acquire the gridded met data
   from some external source (reading from a file, querying an OPeNDAP 
   server, etc.).

   In the most common cases of new MetGridData implementations,
   these will return GridLatLonField3D and GridLatLonFieldSfc objects, respectively.
   But if your new data source uses met data that are not on a simple
   longitude-latitude grid, then you will need to find or implement 
   new child classes of GridField3D and GridFieldSfc, add 
   new methods that will operate on these grids to the horizontal 
   (hinterp) and vertical (vinterp) interpolator classes, and then
   have your new_directGrid3D and new_directGridSfc methods here return those new
   grid objects.
    
*/
class MetGridData : public MetData {

   public:
   
      /// Error: failed to load data
      class baddataload {};
      
      /// Error: improper grid specification
      class badgridspec {};
      
      /// Error: vertical units mismatch
      class badvunits {};
      
      /// Error: bad grid object
      class badgridobj {};

      /// An exception for trying to use an unknown vertical coordinate
      class badVerticalQuantity {};

      /// An exception for trying to use an unknown vertical coordinate system
      class badVerticalCoord {};

      /// Error: bad time specification
      class badtimespec {};
      
      /// Error: incompatible vector components
      class badIncompatibleVectors {};

      
      /// Default constructor
      /*! This is the default constructor. 
      */
      MetGridData();
      
      /// destructor
      /*! This is the destructor. 
      */
      ~MetGridData();
      
      
      /// copy constructor
      /*! This is the copy constructor. 
      */
      MetGridData( const MetGridData& src );     

      /// assignment operator
      virtual MetGridData& operator=(const MetGridData& src) = 0;

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


      /// returns a copy of the object, cast to the MetGridData class
      /*!
           Sometimes a routine that deals with objects of a MetGridData subclass
           needs to make a copy of the object, but needs to do so in a way
           that allows the copy to be made with no information about
           the exact subclass being used.
           
           This method, implemented by hte various subclasses, will create
           a copy of the object, cast the new object to the MetData class,
           and then return the result.
           
           \return a pointer to a new MetData object. The calling routine is repsonsible for delteing the
                   object when it is no longer needed 
      */
      virtual MetGridData* MetGridCopy() = 0;

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
      virtual void set_vertical( const std::string quantity, const std::string units ) = 0;

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
          \param levels a pointer to a vector of values of the new vertical coordinate
      */
      virtual void set_vertical( const std::string quantity, const std::string units, const std::vector<real>* levels ) = 0;

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
     
         \param units (output) if not NULL, a pointer to a string in which are returned the units of the quantity
         
         \return a string containing the name of the vertical coordinate
     */
     virtual std::string vertical( std::string *units=NULL ) const; 

      /// gets the units of the vertical coordinate use in the data source
      /*! This method returns the units of the vertical coordinate that is being use in this data source.
      
         \return the name of the vertical coordinate units
      */
      virtual std::string vunits() const;



      /// sets the vertical interpolator
      /*! This method sets the vertical interpolator used to obtain data values on isosurfaces 
          and point locations.
           
           \param vinterp a pointer to the Vinterp object to be the new interpolator. Note that the 
                          pointer is stored as-is, so the calling routine must not delete it
                          until this MetGridData object is deleted or a new Vinterp object is assigned.
      
           \param okToDelete if true, then this MetGridData object can delete the Vinterp object when 
                             it is done using it. 
      */
      void set_vinterp( Vinterp* vinterp, bool okToDelete=false );
      
      /// gets the vertical interpolator
      /*! This method retrieves the vertical interpolator object. It can then be used to obtain data values on isosurfaces 
          and at point locations. 
           
           \return a pointer to the vertical interpolator object
      */
      Vinterp* vinterp() {
           return vin;
      }

      /// sets the horizontal interpolator 
      /*! This method sets the horizontal interpolator used to regrid data surfaces and
          obtain data values at point locations.
           
          \param hinterp a pointer to the HLatLonInterp object to be the new interpolator
          \param okToDelete if true, then this MetGridData object can delete the Hinterp object when 
                             it is done using it. 
     
      */
      void set_hinterp( HLatLonInterp* hinterp, bool okToDelete=false );
      
      /// gets the horizontal interpolator
      /*! This method retrieves the horizontal interpolator object. It can then be used to regrid data surfaces and
           obtain data values at point locations. 
           
           \return a pointer to the horizontal interpolator object

      */
      HLatLonInterp* hinterp() {
           return hin;
      }


      /// retrieve the thinning factor for horizontal data
      /*! This method returns the thinning factor that is applied to horizontal data.
          This determines how many data points are to be skipped. A value
          of 1 or less means that every point is used. A value of 2 means that
          only every other longitude and latitude is retained. A value of three
          means that only every third longitude and latitude is retained.
          (All vertical levels are alays retained.)
          
          \param offset an in pointer to which, if non-NULL, is returned the starting index to be used in longitudes

          \return the thinning factor

      */
      int thinning( int* offset=NULL );
      
      /// sets the thinning factor for horizontal data    
      /*! This method sets the thinning factor that is applied to horizontal data.
          This determines how many data points are to be skipped. A value
          of 1 or less means that every point is used. A value of 2 means that
          only every other longitude and latitude is retained. A value of three
          means that only every third longitude and latitude is retained.
          (All vertical levels are alays retained.)
          
          \param thin the thinning factor to be applied to subsequent data reads
          \param offset a offset index used to determine the starting longitude value;
          
      */
      void set_thinning( int thin, int offset=0 );



      /// deletes a 3D data field object
      /*! A number of MetGridData methods return a pointer to a GridField object that
          holds a gridded data field. And every such object must of course be deleted at some
          point. Some of those objects are held within the MetGridData object--for example, cached data fields 
          and fields that refer to a met processor in a multiprocessing environment.
          IF objects returned to the calling program must be deleted by the calling program,
          then those object must be copies of any such internally-held objects. And
          copying takes take and memory. Potentially lots and lots of memory.
          This method addresses the issue by serving as a replacement for the
          delete instruction in the calling routine. It compares a pointer to 
          the various internal pointers that the MetGridData object holds internally,
          and if there is no match, the pointer object is deleted. The only thing the 
          calling rojtine has to be careful of is to hand over for deletion only
          those pointers which were obtained from the same MetGridData object.
          
          /param a pointer to a GridField3D object that the calling routine no longer needs
      
      */
      void remove( GridField3D* field );

      /// deletes a Sfc data field object
      /*! A number of MetGridData methods return a pointer to a GridField object that
          holds a gridded data field. And every such object must of course be deleted at some
          point. Some of those objects are held within the MetGridData object--for example, cached data fields 
          and fields that refer to a met processor in a multiprocessing environment.
          IF objects returned to the calling program must be deleted by the calling program,
          then those object must be copies of any such internally-held objects. And
          copying takes take and memory. Potentially lots and lots of memory.
          This method addresses the issue by serving as a replacement for the
          delete instruction in the calling routine. It compares a pointer to 
          the various internal pointers that the MetGridData object holds internally,
          and if there is no match, the pointer object is deleted. The only thing the 
          calling rojtine has to be careful of is to hand over for deletion only
          those pointers which were obtained from the same MetGridData object.
          
          /param a pointer to a GridFieldSfc object that the calling routine no longer needs
      
      */
      void remove( GridFieldSfc* field );

      /// get a 3D data field valid at a certain time, using basic access
      /*! This method reads meteorological data from some source and returns
          it in a GridField3D object. 
          
          It takes the data directly from the source, with no caching or
          met data client/server interactions.
          
           \param quantity the (internal or cf-convention) name of the quantity desired
           \param time the valid-at datestamp string for which data is desired

           \return a pointer to a GridField3D object that holds the data. This object
           may be cached, or it may be a met client in a multiprocessing environment.
           Consequently, when the calling routine has no further use for the object
           it should use the MetGridData remove() method to delete it, instead of deleting it
           itself.

      */
      virtual GridField3D* new_directGrid3D( const std::string quantity, const std::string time ) = 0;

      
      /// get a 2D data field valid at a certain time, using basic access
      /*! This method reads meteorological data from some source and returns
          it in a GridFieldSfc object.
          
          It takes the data directly from the source, with no caching or
          met data client/server interactions.
          
           \param quantity the (internal or cf-convention) name of the quantity desired
           \param time the valid-at datestamp string for which data is desired

           \return a pointer to a GridFieldSfc object that holds the data. This object
           may be cached, or it may be a met client in a multiprocessing environment.
           Consequently, when the calling routine has no further use for the object
           it should use the MetGridData remove() method to delete it, instead of deleting it
           itself.

      */
      virtual GridFieldSfc* new_directGridSfc( const std::string quantity, const std::string time ) = 0;



      ///  get a 3D data field valid at a certain time, with managemenmt
      /*! This method reads meteorological data from some source and returns
          it in a GridField3D object. It uses the obtain3d() method to
          do this, and along the way it handles caching and met data client/server
          interactions (in a parallel-processing environment).
          Thus, the usual routine for child classes to call is new_mgmtGrid3D, not new_directGrid3D.
          
           \param quantity the (internal or cf-convention) name of the quantity desired
           \param time the valid-at datestamp string for which data is desired
           \return a pointer to a GridField3D object that holds the data. This object
           may be cached, or it may be a met client in a multiprocessing environment.
           Consequently, when the calling routine has no further use for the object
           it should use the MetGridData remove() method to delete it, instead of deleting it
           itself.


      */
      GridField3D* new_mgmtGrid3D( const std::string& quantity, const std::string& time );

      ///  get a 2D data field valid at a certain time, with management
      /*! This method reads meteorological data from some source and returns
          it in a GridFieldSfc object. It uses the new_directGridSfc() method to
          do this, and along the way it handles caching and met data client/server
          interactions (in a parallel-processing environment).
          Thus, the usual routine for child classes to call is new_mgmtGridSfc, not new_directGridSfc.
          
           \param quantity the (internal or cf-convention) name of the quantity desired
           \param time the valid-at datestamp string for which data is desired

           \return a pointer to a GridFieldSfc object that holds the data. This object
           may be cached, or it may be a met client in a multiprocessing environment.
           Consequently, when the calling routine has no further use for the object
           it should use the MetGridData remove() method to delete it, instead of deleting it
           itself.

      */
      GridFieldSfc* new_mgmtGridSfc( const std::string& quantity, const std::string& time );


      /// set the time base/offset and delta to be imposed
      /*! Sometimes it is desired to use only a subset of data snapshots in tracing trajectories.
          For example, if the data are available every three hours starting at 00Z daily, you might want to actually
          use data only at 12Z daily.  In this case, the offset would be 12 hours, and the delta would be 24 hours.
          Normally, the default base and delta are determined from the data source. This method 
          permits imposing a base and delta of the user's own choosing. 
          
          \param otbase the time offset of the first snapsshot for a day, in hours. A negative value means that the default will be used.
                        values greater than or equal to 24 will throw an error.
          
          \param otspace the time delta between successive snapshots to be used, in hours. A negative or zero value means
                         that the default will be used.              
      
      */
      virtual void impose_times( double otbase, double otspace );
      
      /// get the time base/offset
      /*! This method returns the current user-imposed time base hour offset from 0Z of the first data snapshot to be used.
      
          \return the time offset, in hours. A negative value indicates the default time base (usually 0) is used. 
      */
      double get_imposed_timebase();
      
      /// get the time delta
      /*! This method returns the current user-imposed time delta between data snapshots
      
          \return the time delta, in hours. A negaztive or zero value indicates the default time delta (usually 0) is used.
      */
      double get_imposed_timedelta();



      /// set the top-most directory of the local disk met data cache
      /*! This method sets the top-most directory of the local disk cache for this data source.
      
          \param path the name of the directory.  If the null string is given, then 
                      disk caching is turned off.
      
      */
      void setCacheDir( std::string path );

      /// set the top-most directory of the local disk met data cache
      /*! This method sets the top-most directory of the local disk cache for this data source.
      
          \param path a pointer to a FilePath object. (The actual object is copied into a new
                      object; the pointer is not merely copied.)
      
      */
      void setCacheDir( FilePath *path );

      /// print a state report
      /* This method prints a report to stderr about the current state of the MetData object.
         This mainly involves printing the status of all data caches.
      */
      void report() const;


      /// (parallel processing) prepare for data acquisition
      /*! This method is used as an easy way to set up for
          data acquisition in a parallel processing environment,
          regardless of whether the processor running it 
          traces parcel trajectories (met data client) or
          manages meteorologicla fields (met data server).
          
          If multiprocessing is being used, and if
          this processor is not dedicated to handling meteorological data for
          the other processors in this ProcessGrp, then this routine returns 1.
          Otherwise, it calls the svr_listen() method and then returns 0.
          This method can thus be used in an if-then statement
          to make it easy to work with data in the multiprocessor environment.
          
          As an example:
          \code
          
          if ( metsrc.svr_start() ) {
             
             // statement that should be executed only by 
             // the tracing processors, not by the dedicated
             // met-data-handling processors
             metsrc.receive_meta();
          
             // tells the met-data-handling processor that this client 
             // is done making requests, for now
             metsrc.signalMetDone();

          }
          
          \endcode
          
          
          \return 1 if this is a parcel-tracing processor, 0 otherwise

      */
      int svr_start();

      /// (parallel processing) listen for met data client requests
      /*! If parallel processing, and if this processor is dedicated to 
          handling meteorological data, then this method listens for requests 
          from other processors in this object's processor group,
          returning when all the other processors have called 
          this object's signalMetDone() method.
          
          Otherwise, this method returns immediately.

      */
      void serveMet();

      /// send a request for metadata to the Met server processor
      /*! If parallel processing with a dedicated met processor,
          send a request to the met server processor asking for 
          3D data metadata (dimensions, units, etc.).  
          This method serves the same handshaking
          role as the ask_for_meta() method of the GridField class.
           \param quantity the name of the quantity desired
           \param time the valid-at datestamp string for which metadata is desired
      */
      void request_meta3D( const std::string& quantity, double time );
       
      /// (parallel processing) send a 3D data request to a met data server process
      /*! If parallel processing with a dedicated met processor,
          send a request to the met server processor asking for 
          3D data gridpoints.  This method serves the same handshaking
          role as the ask_for_data() method of the GridField class.
           \param quantity the name of the quantity desired
           \param time the valid-at datestamp string for which data is desired
      */
      void request_data3D( std::string& quantity, std::string& time ); 

      /// (parallel processing) send a 3D data request to a met data server process
      /*! If parallel processing with a dedicated met processor,
          send a request to the met server processor asking for 
          3D data gridpoints of a (two-dimensional) vector field.  
          This method serves the same handshaking
          role as the ask_for_data() method of the GridField class.
           \param xquantity the name of the x-component of the vector quantity desired
           \param yquantity the name of the y-component of the vector quantity desired
           \param time the valid-at datestamp string for which data is desired
      */
      void request_data3D( std::string& xquantity, string& yquantity, std::string& time ); 

      /// send a request for metadata to the Met server processor
      /*! If parallel processing with a dedicated met processor,
          send a request to the met server processor asking for 
          Sfc data metadata (dimensions, units, etc.).  
          This method serves the same handshaking
          role as the ask_for_meta() method of the GridField class.
           \param quantity the name of the quantity desired
           \param time the valid-at datestamp string for which metadata is desired
      */
      void request_metaSfc( const std::string& quantity, double time );

     
      /// (parallel processing) send a 2D data request to a met data server process
      /*! If parallel processing with a dedicated met processor,
          send a request to the met processor asking for 
          2D data gridpoints.  This method serves the same handshaking
          role as the ask_for_data() method of the GridField class.
           \param quantity the name of the quantity desired
           \param time the valid-at datestamp string for which data is desired
      */
      void request_dataSfc( std::string& quantity, std::string& time ); 

      /// (parallel processing) send a 2D data request to a met data server process
      /*! If parallel processing with a dedicated met processor,
          send a request to the met processor asking for 
          2D data gridpoints of a (two-dimensional) vector field.  This method serves the same handshaking
          role as the ask_for_data() method of the GridField class.
           \param xquantity the name of the quantity desired
           \param yquantity the name of the y-component of the vector quantity desired
           \param time the valid-at datestamp string for which data is desired
      */
      void request_dataSfc( std::string& xquantity, string& yquantity, std::string& time ); 
         

   protected:

      /// sets the name of the vertical coordinate used in the data source
      /*! This method forces the meteorological data source object to use a specific vertical coordinate.
          
          This function gets called by the various subclasses's set_vertical() functions.
          
          \param quantity the desired quantity to be used for the vertical coordinate
          \param units the units of the desired vertical coordinate
          \param levels a pointer to a vector of values of the new vertical coordinate
          \param scale a scale factor to be applied to this quantity that would convert it to MKS units
          \param offset an offset to be applied to this quantity (after the scale factor) that would convert it to MKS units
      */
      virtual void set_verticalBase( const std::string quantity, const std::string units, const std::vector<real>* levels=NULLPTR, real scale=1.0, real offset=0.0 );

      /// sets the vertical coordinates from a GridField3D object
      /*! This method forces the meteorological data source object to use vertical coordinates taken form a GridField3D object.
      
          \param grid the grid whose vertical coordinates are to be used.
      */
      virtual void set_verticalBase( GridField3D *grid );    

      /// determines scaling factors for vertical coordinate
      /*! Given units for a vertical coordinate, this method determines the
          linear transformation that takes the given units to MKS units.
          The transformation is:  MKSvalue = originalValue*scale + offset
          
          \param quantity  the name of the vertical coordinate quantity ("km", "m", "hPa", "K")
          \param units the units of the vertical coorinate
          \param scale a pointer to the scaling factor to be returned
          \param offset a pointer to the offset to be returned
      */    
      void vMKS( const std::string quantity, const std::string units, real* scale, real* offset) const; 

      /// returns a default set of vertical coordinate values
      /*! This method returns a vector of vertical coordinate values, depending on the coordinate system desired.
      
          \param coordSys a pointer to a string giving the name of the vertical coordinate. If NULLPTR, the 
                        meteorological source's current vertical coordinate is used.
           \return a pointer to a vector of coordinate values corresponding to the usual or default values
                        for this data souirce on the desired coordinates.              
      
      */
      virtual std::vector<real>* vcoords( const std::string *coordSys ) const = 0;

      /// Converts the vertical coordinate of a GridData3D object 
      /*! For some data sources, a vertical coordinate quantity may be an 
          analytical function of another quantity. For example, pressure
          altitude is an analytical function of pressure.
          This method checks if that is the case and, if it is,
          converts the vertical coordinate.
          
          \param input a pointer to a GridData3D object 
          \param quant the quantity desired as the vertical coordinate
          \param units the units desired for the new vertical coordinate
          \param scale the scale factor to take the new vertical coordinate into MKS units
          \param offset the offset to take the new vertical coordinate into MKS units
          \return a boolean that is true if the conversion was done, and false otherwise
      
      */
      virtual bool vConvert( GridField3D *input, std::string quant, std::string units, real scale=1.0, real offset=0.0 );


      /// generate a fle path to a cache file for a gridded met field
      /*! This method returns a pointer to a full file path for a disk cache file, or NULL
          if the data are not to be cached on disk.

          \param item the GridFieldSfc data object which is to be cached

          \return the cache file path name
      */
      virtual FilePath* cachefile( const GridFieldSfc* item ) const = 0;

      /// generate a fle path to a cache file for a gridded met field
      /*! This method returns a pointer to a full file path for a disk cache file, or NULL
          if the data are not to be cached on disk.

          \param item the GridField3D data object which is to be cached
          
          \return the cache file path name
      */
      virtual FilePath* cachefile( const GridField3D* item ) const = 0;


      /// write an MetGridField3D object to disk cache
      /*! This method writes a MetGridField3D object to disk cache

          \param item the GridField3D data object which is to be cached
      */
      virtual void writeCache( const GridField3D* item ) const = 0;
      
      /// write an MetGridFieldSfc object to disk cache
      /*! This method writes a MetGridFieldSfc object to disk cache

          \param item the GridFieldSfc data object which is to be cached
      */
      virtual void writeCache( const GridFieldSfc* item ) const = 0;

      ///  read a MetGridField3D object from disk cache
      /*!  This method reads a MetGridField3D object from disk cache.

           \param quantity the name of the quantity desired
           \param time the valid-at datestamp string for which data is desired

           \return a pointer to a GridField3D object that holds the data
      */
      virtual GridField3D* readCache3D( const std::string quantity, const std::string time ) = 0;
      
      /// read an MetGridFieldSfc object from disk cache
      /*! This method reads an MetGridFieldSfc object from disk cache.

           \param quantity the cf-convention name of the quantity desired.  If the
                  desired quanity is on a specified surface, then a "@" is appended,
                  followed by the surface.  For example, "total column ozone"
                  might be simply "total column ozone", but "tropopause temperature"
                  might be "temperature@tropopause".
           \param time the valid-at datestamp string for which data is desired

           \return a pointer to a GridFieldSfc object that holds the data
      */
      virtual GridFieldSfc* readCacheSfc( const std::string quantity, const std::string time ) = 0;

      /// (parallel processing) gets a 3D field valid at a certain time, as a client of a met data sertver
      /*! This method contacts a meteorological data server to obtain a new gridded data object,
          instead of reading the data itself. 
          
          To conserve memory, the GridField object produced by this method
          will contain only the metadata, including dimensional values, but no actual
          data values. Thus the value() method and related methods will not work with the
          resulting object. Instead, the met source's request_data3D() method should be called
          to alert the met server that data will soon be required, and then the 
          object's gridpoints() method should be called to obtain data values on
          a sets of gridpoints, by asking the met server for them.

           \param quantity the (internal or cf-convention) name of the quantity desired
           \param time the valid-at datestamp string for which data is desired
           \return a pointer to a GridField3D object that holds the metadata. 
           This object
           may be cached, or it may be a met client in a multiprocessing environment.
           Consequently, when the calling routine has no further use for the object
           it should use the MetGridData remove() method to delete it, instead of deleting it
           itself.

      */
      virtual GridField3D* new_clientGrid3D( const std::string& quantity, const std::string& time ) = 0;
          
      /// (parallel processing) gets a 2D field valid at a certain time, as a client of a met data sertver
      /*! This method contacts a meteorological data server to obtain a new gridded data object,
          instead of reading the data itself. 
          
          To conserve memory, the GridField object produced by this method
          will contain only the metadata, including dimensional values, but no actual
          data values. Thus the value() method and related methods will not work with the
          resulting object. Instead, the met source's request_dataSfc() method should be called
          to alert the met server that data will soon be required, and then the 
          object's gridpoints() method should be called to obtain data values on
          a sets of gridpoints, by asking the met server for them.

           \param quantity the (internal or cf-convention) name of the quantity desired
           \param time the valid-at datestamp string for which data is desired
           \return a pointer to a GridFieldSfc object that holds the metadata. 
           This object
           may be cached, or it may be a met client in a multiprocessing environment.
           Consequently, when the calling routine has no further use for the object
           it should use the MetGridData remove() method to delete it, instead of deleting it
           itself.

      */
      virtual GridFieldSfc* new_clientGridSfc( const std::string& quantity, const std::string& time ) = 0;
          



       /// override value for data temporal spacing
       double override_tspace;

       /// override value for data temporal offset (base time within a day)
       double override_tbase;

       
       /// skip factor in the horizontal grid (0=use every point, 1=use every other point, etc.
       int skip;
       /// longitude offset factor to be used when skipping
       int skoff;


      /*!
        \brief  used to cache 3D met data in memory/disk
        
        The MetCache3D class manages disk caching of gridded meteorological data fields.
        for each 3D physical quantity being cached, there should be one MetCache3D object.
        
      */   
      class MetCache3D {
         public:
      
             /// the most-used constructor
             /*! This is the most-used constructor.

                 \param  quantity the physical quantity to be cached
                 \param size the size of the cache: how many data grid snapshots are to be stored 
             */
             MetCache3D( std::string quantity, int size=3 );

             /// destructor
             /*! This is the destructor.
             */
             ~MetCache3D();

             /// copy constructor
             /*! This is the copy constructor.
             */
             MetCache3D(const MetCache3D& src);


             /// set maximum cache size
             /*! This method sets the maximum cache size.
             
                 \param n the size of the cache (i.e., the number of fields of this quantity to cache)
             */    
             void setSize(int n);
             
             /// try to retrieve a cached met field
             /*! This method retrieves a cached met data field that is valid
                 for a given time.

                 If the snapshot is not currently in cache, NULL is returned.
                 If the snapshot is in the cache, the pointer returned points
                 to that object, and not to a copy.  This is done
                 to avoid unnecessary copying of large gridded data arrays. 
                 Thus, when the calling routine no longer needs the snapshot object, it
                 should delete it only by calling the remove() method of the
                 MetGridData object that created the snapshot being deleted.
                   
                   \param time the internal model time of the desired snapshot.
                   \param flag if set, then any query of an existing object will
                               raise that object's priority in the queue.  Use 0
                               for pure querying that leaves the priorities untouched.

                   \return a pointer to a GridField3D object that holds the met field, or NULL if not cached
             */
             GridField3D* query( double time, int flag=1 );
             
             /// try to retrieve a cached met field
             /*! This method retrieves a cached met data field that is valid
                 for a given time.

                 If the snapshot is not currently in cache, NULL is returned.
                 If the snapshot is in the cache, the pointer returned points
                 to that object, and not to a copy.  This is done
                 to avoid unnecessary copying of large gridded data arrays. 
                 Thus, when the calling routine no longer needs the snapshot object, it
                 should delete it only by calling the remove() method of the
                 MetGridData object that created the snapshot being deleted.
                   
                   \param time the meteorological datestamp string of the desired snapshot.
                   \param flag if set, then any query of an existing object will
                               raise that object's priority in the queue.  Use 0
                               for pure querying that leaves the priorities untouched.

                   \return a pointer to a GridField3D object that holds the met field, or NULL if not cached
             */
             GridField3D* query( const std::string time, int flag=1 );


             /// determines if a field is cached
             /*! This method looks up a given pointer to see if it corresponds to
                 a cached field object.
                 
                 \param field a pointer to a gridded field object to be searched for
                 \return true if the object is cached, false otherwise
             */    
             bool has( GridField3D* field ); 
             
             /// adds a GridField3D object to the cache.
             /*! This method adds a GridField3D object to the cache.

                 \param field a pointer to the GridField3D object to be cached here. 
                        No copy is made of the object pointed to; if that object
                        is deleted or otherwise changed by the calling routine, 
                        then the object in the cache is also affected. 
             */
             void add( GridField3D* field );
             
             /// print a cache report
             /*! This method prints a report to stderr that describes
                 the current state of the cache.  This can be helpful for debugging.
             */
             void report() const;
             
             
             /// the physicaal quantity associated with this cache
             std::string quant;

         protected:
             /// the data being cached
             std::deque<GridField3D*> data;
             /// the maximum capacity of this cache, in GridField objects
             int max;
      
         private:
            /// the default constructor
            /*! This is the default constructor. It cannot be used for anything
            */
            MetCache3D() {};
      };



      /*!
        \brief  used to cache met data in memory/disk
        
        The MetCacheSfc class manages disk caching of gridded meteorological data fields.
        for each 2D physical quantity being cached, there should be one MetCacheSfc object.
        
      */   
      class MetCacheSfc {
         public:
      
             /// the most-used constructor
             /*! This is the most-used constructor.

                 \param  quantity the physical quantity to be cached
                 \param  size the maximum number of GridField objects this cache can hold
             */
             MetCacheSfc( std::string quantity, int size=3 );

             /// destructor
             /*! This is the destructor.
             */
             ~MetCacheSfc();

             /// copy constructor
             /*! This is the copy constructor.
             */
             MetCacheSfc(const MetCacheSfc& src);


             /// set maximum cache size
             /*! This method sets the maximum cache size.
             
                 \param n the size of the cache (i.e., the number of fields of this quantity to cache)
             */    
             void setSize(int n);
             
             /// try to retrieve a cached met field
             /*! This method retrieves a cached met data field that is valid
                 for a given time.

                 If the snapshot is not currently in cache, NULL is returned.
                 If the snapshot is in the cache, the pointer returned points
                 to that object, and not to a copy.  This is done
                 to avoid unnecessary copying of large gridded data arrays. 
                 Thus, when the calling routine no longer needs the snapshot object, it
                 should delete it only by calling the remove() method of the
                 MetGridData object that created the snapshot being deleted.
                   
                   \param time the internal model time of the desired snapshot.
                   \param flag if set, then any query of an existing object will
                               raise that object's priority in the queue.  Use 0
                               for pure querying that leaves the priorities untouched.

                   \return a pointer to a GridFieldSfc object that holds the met field, or NULL if not cached
             */
             GridFieldSfc* query( double time, int flag=1 );
             
             /// try to retrieve a cached met field
             /*! This method retrieves a cached met data field that is valid
                 for a given time.

                 If the snapshot is not currently in cache, NULL is returned.
                 If the snapshot is in the cache, the pointer returned points
                 to that object, and not to a copy.  This is done
                 to avoid unnecessary copying of large gridded data arrays. 
                 Thus, when the calling routine no longer needs the snapshot object, it
                 should delete it only by calling the remove() method of the
                 MetGridData object that created the snapshot being deleted.
                   
                   \param time the meteorological datestamp string of the desired snapshot.
                   \param flag if set, then any query of an existing object will
                               raise that object's priority in the queue.  Use 0
                               for pure querying that leaves the priorities untouched.

                   \return a pointer to a GridFieldSfc object that holds the met field, or NULL if not cached
             */
             GridFieldSfc* query( const std::string time, int flag=1 );

             /// determines if a field is cached
             /*! This method looks up a given pointer to see if it corresponds to
                 a cached field object.
                 
                 \param field a pointer to a gridded field object to be searched for
                 \return true if the object is cached, false otherwise
             */    
             bool has( GridFieldSfc* field ); 
             
             /// adds a GridFieldSfc object to the cache.
             /*! This method adds a GridFieldSfc object to the cache.

                 \param field a pointer to the GridFieldSfc object to be cached here. 
                        No copy is made of the object pointed to; if that object
                        is deleted or otherwise changed by the calling routine, 
                        then the object in the cache is also affected. Delete the object
                        only by calling the MetGridData remove() method.
             */
             void add( GridFieldSfc* field );
             
             /// print a cache report
             /*! This method prints a report to stderr that describes
                 the current state of the cache.  This can be helpful for debugging.
             */
             void report() const;
             
             
             /// the physicaal quantity associated with this cache
             std::string quant;

         protected:
             /// the data being cached
             std::deque<GridFieldSfc*> data;
             /// the maximum number of GridField objects this cache object can hold
             int max;
      
         private:
            /// the default constructor cannot be used for anything
            MetCacheSfc() {};
      };


      /// zonal winds
      MetCache3D *us;
      /// meridional winds
      MetCache3D *vs;
      /// vertical winds
      MetCache3D *ws;
      /// other 3D fields
      std::map< std::string, MetCache3D* > field3Ds;
      /// quasi-horizontal 2D fields
      std::map<std::string, MetCacheSfc*> field2Ds;

      /// 3D field for use as client to a met  data server processor
      GridField3D *x3D;
      /// 2D field for use as client to a met  data server processor
      GridFieldSfc *xSfc;
      

      /// identifies what the vertical coordinate is for the grids used here
      std::string vquant;      
      /// identifies the units of vquant
      std::string vuu;
      /// identifies the scale factor to apply to take the units to MKS units
      real vMKSscale;
      /// identifies the offset to apply to take the units to MKS units
      real vMKSoffset;      
      /// number of vertical levels
      int nzs;
      /// vector of vertical levels
      std::vector<real> zs;

      // (These are possible vertical coordinate quantities)
      /// the name of altitude in this data source
      std::string altitude_name;
      /// the name of pressure in this data source
      std::string pressure_name;
      /// the name of pressure altitude in this data source
      std::string palt_name;
      /// the name of potential temperature in this data source
      std::string pottemp_name;
      
      /// vertical-wind quantities associated with vertical coordinates
      typedef struct {
         /// vertical wind quantity name
         std::string quantity;
         /// vertical wind units
         std::string units;
         /// scale factor to take the vertical wind units to MKS units
         real MKSscale;
         /// offset to take the vertical wind units to MKS units
         real MKSoffset;
         
      } vWindStuff; 

      /// information about the vertical wind      
      std::map<std::string, vWindStuff> vertwind_quants;

      /// identifies the vertical velocity of the vertical coordinate used here
      std::string wind_vert_name;
      /// identifies the east-west wind component
      std::string wind_ew_name;
      /// identifies the norht-south wind component
      std::string wind_ns_name;

      /* the presence or adbsence of other on-the-fly calculation objects 
         depends on whether the particular met source offers those fields,
         or whether they must be calculated on the fly.
         Since Pressure Altitude (PAlt) is purely calculated from pressure
         and is never a field in a met data source, we define these
         on-the-fly objects here. */
      /// on-the-fly calculator for pressure altitude
      PAltOTF getpalt;
      /// on-the-fly calculator for time rate of change of pressure altitude
      PAltDotOTF getpaltdot;
      
      /// pointer to the vertical interpolator used
      Vinterp *vin;
      /// indicates whether vin has been supplied externally (true = No) (I.e., do we need to delete the interpolator when we are done with it?)
      bool myVin;
      
      /// pointer to the horizontal interpolator used
      HLatLonInterp *hin;
      /// indicates whether hin has been supplied externally (true = No) (I.e., do we need to delete the interpolator when we are done with it?)
      bool myHin;
      
      /// copy an object into this one
      /*! This method is used for copying objects

          \param src the object to be copied
      */    
      void assign( const MetGridData& src );
      
      /// maximum number of time snapshots to keep in memory 
      int maxsnaps;
      
      /// set the number of grid snapshots to hold in memory.
      /*! This method sets the number of grid snapshots to hold in memory (a supposed to disk caching).

           \param n the maximum number of time snapshots to keep in memory
      
      */
      inline void set_snaps( int n ) {
          if ( n >= 0 ) {
             maxsnaps = n;
          }
      };
      
      /// drop all (memory-)cached data
      /*! This method drops all data held in memory cache.
      */
      void flush_cache();

      /// cache directory path
      FilePath* diskcachedir;
      /// flag: are we caching?
      bool diskcaching;
      

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
