#ifndef GIGATRAJ_METMYGEOS_H
#define GIGATRAJ_METMYGEOS_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>


#include <netcdf.h>
#include <stdlib.h>
#include <unistd.h>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/MetGridLatLonData.hh"
#include "gigatraj/Catalog.hh"
#include "gigatraj/CalGregorian.hh"
#include "gigatraj/PressOTF.hh"
#include "gigatraj/ThetaOTF.hh"
#include "gigatraj/ThetaDotOTF.hh"
#include "gigatraj/PAltOTF.hh"
#include "gigatraj/PAltDotOTF.hh"
#include "gigatraj/SZAOTF.hh"
#include "gigatraj/TropOTF.hh"

namespace gigatraj {

/*!
\ingroup MetSrc
\brief class for reading the GEOS data products from local files.

This class implements access to local files of products output by
the Goddard Earth Observing System (GEOS) pf
the Global Modeling and Assimilation Office (GMAO) of
NASA's Goddard Space Flight Center. 

It requires a configuration file, used by a Catalog object, The confLocator string to use is "MyGEOSCatalog".

*/



class MetMyGEOS : public MetGridLatLonData {

   public:

      /// An exception for an unknown coordinate grid specification
      class badCoordinateGrid {};

      /// An exception for failing to find GEOS quantity
      class badDataNotFound {};

      /// An exception for given dimensions not matching the file name's specs
      class badDimsMismatch {};

      /// An exception for given dimensions not being in the proper form
      class badDimsForm {};

      /// An exception for a quantity not having the right number of dimensions
      class badDimensionality {};

      /// An exception for an unknown dimension in a variable
      class badUnknownDim {};

      /// An exception for trying to read a nonexistent dimension
      class badMissingDim {};

      /// An exception for failing to allocate buffer memory
      class badNoMem {};

      /// An exception for failing to find the requested time snapshot
      class badTimeNotFound {};
      
      /// an exception for missing or bad Catalog config file
      class badConfig {};

      /// an exception for things that should never happen
      class badImpossible {};

      /// An exception for failing to open the GEOS netcdf file
      class badNetcdfOpen {
         public:
            /// error code
            int error;
            
            /// constructor
            badNetcdfOpen(int err) { error = err; }
            
      };

      /// An exception for some miscellaneous error while trying to read the GEOS netcdf file
      class badNetcdfError {
         public:
            /// error code
            int error;
            /// constructor
            badNetcdfError(int err) { error = err; }
            
      };
      

      /// Default constructor
      /*! This is the default constructor. 
      */
      MetMyGEOS();
      
      /// Constructor with base date specified
      /*! In this version of the constructor, the base date is specified that is equivalent to internal model time zero
       is explicitly given

          param date a string containing a date in ISO8601 format (e.g., "2005-04-25")
                      that will serve as the base time from which all model times
                      will be referenced.
      */
      MetMyGEOS( std::string& date );                



      /// destructor
      /*! This is the destructor. 
      */
      ~MetMyGEOS();

      
      /// copy constructor
      /*! This is the copy constructor. 
      */
      MetMyGEOS( const MetMyGEOS& src );

      /// copies content from a given MetMyGEOS object into this MetMyGEOS object
      /*! This method makes the current objetc a copy of anohter MetMyGEOS.
          Note, however, that any open files in the original are not open in the copy.
          
          \param src the object to be copied into this one
      */    
      void assign(const MetMyGEOS& src);
   
      /// assignment operator
      MetMyGEOS& operator=(const MetMyGEOS& src);

      /// assignment operator
      MetGridLatLonData& operator=(const MetGridLatLonData& src);

      /// return the type of MetData object this is
      /*! This method retuns a string that identifies the specific class of MetData this object is,.
      
          \return the name of the class 
      */
      std::string id() const { return iam; };

      /// convert model time to a date+time calendar string
      /*! This method converts internal model time to an ISO8601 date string
          (e.g., "2003-07-15T12:35").

          \param time the model time that is to be converted
          \param format = the output format of the date:
                  * -999 = use the default format 
                  * 0 = date only, with no time (yyyy-mm-dd)
                  * 1 = date plus the hour (yyyy-mm-ddThh)
                  * 2 = date plus the hour and minute (yyyy-mm-ddThh:mm)
                  * 3 = date plus the hour, minute, and integer second (yyyy-mm-ddThh:mm:ss)
                  * 4 = date plus the hour, minute, and floating second (to four decimal places) (yyyy-mm-ddThh:mm:ss.ssss) 

          \return the time as expressed in ISO8601 format.
          
      */   
      std::string time2Cal( const double time, int format=-999 );

      /// convert a date+time calendar string to model time
      /*! This method converts an ISO8601 date string to internal model time.

          \param date a timestamp in ISO8601 format (e.g., "2005-04-20T18:45:03")
          
          \return the internal model time that corresponds to the input string
               
      */   
      double cal2Time( const std::string date );

      /// sets the equivalence between internal model time and a date+time calendar string
      /*! A data source will usually associate meteorological field values with
          some kind of time stamp, here expressed as a string. The trajectory mode;
          represents time as a double-precision floating-point number.
          This routine defines the equivalence between the two.
          
          \param caldate  a time as expressed in some data-specific calendar system 
          \param time the model time that corresponds to cal  
      */
      void defineCal( string caldate, double time=0.0 );


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
                      Allowed names are:
                      * DataSetID - the string label for the GEOS data set being used--see the metTag() method 
                      * ModelRun - a string label that identifies a particular forecast model run           
                      
          \param value the value to be applied to the named configuration option
      
      */
      void setOption( const std::string &name, const std::string &value );

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
                      
                      Allowed names are:
                      * HorizontalGridThinning - the thining factor
                      * HorizontalGridOffset - the longititude offset specified w/ thinning
                      * ForecastOnly - if 1, then read only forecast data
                      * AnalysisOnly - if 1, then read only analysis data
                      * AnalysisAndForecast - if 1 then read either analysis or forecast data
                      * Delay - a number of seconds to wait befoe opening a new URL
                      
          \param value the value to be applied to the named configuration option
      
      */
      void setOption( const std::string &name, int value );

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
      void setOption( const std::string &name, float value );

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
      void setOption( const std::string &name, double value );


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
                      Allowed names are:
                      * DataSetID - the string label for the GEOS data set being used--see the metTag() method             
                      * ModelRun - a string label that identifies a particular forecast model run           
                      
          \param value (output) the value to be obtained from the named configuration option
      
      */
      bool getOption( const std::string &name, std::string &value );

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
                      
                      Allowed names are:
                      * HorizontalGridThinning - the thining factor
                      * HorizontalGridOffset - the longititude offset specified w/ thinning
                      * ForecastOnly - 1 if reading only forecast data; 0 otherwise
                      * AnalysisOnly - 1 if reading  only analysis data; 0 otherwise
                      * AnalysisAndForecast - 1 if reading  either analysis or forecast data; 0 otherwise
                      
          \param value (output) the value to be obtained from the named configuration option
      
      */
      bool getOption( const std::string &name, int &value );


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
      bool getOption( const std::string &name, float &value );


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
      bool getOption( const std::string &name, double &value );


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
      MetData *genericCopy();

     /// returns the forecast lead time for the U wind 
     /*! This method returns the forecast lead time, in hours, for the U wind.

         \param time the model time for which the U wind's forecast lead time is desired.
         
         \return The forecast lead time, in hours. If zero, then the data are from an analysis or assimilation.
                 If less than zero, then the data source is a free-running model for which he concept of
                 forecast lead time does not apply. If NaN, then the information is not available
                 from this data source. 
          
     */
     double forecastLeadTime( double time );

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
      void set_vertical( const std::string quantity, const std::string units );

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
      virtual void set_vertical( const std::string quantity, const std::string units, const std::vector<real>* levels );

      /// returns whether the vertical coordinate increases or decreases with altitude
      /*! The method returns whether the current vertical coordinate increases or decreases with altitude.
      
           \return +1 if the vertical coordinate increases, -1 if it decreases.
      
      */
      int vIncrease() const;

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
      
      /*! Check that a quantity name is recognized by this data source
          Returns true if recognized, false otherwise
          
       \param quantity  a string that  holds the name of the quantity   
      */
      bool legalQuantity( const std::string quantity );     


      /// returns the units of a desired quantity
      /*! This method returns the units of a quantity, if known.
      
           Note that the units returned are not authoritative.
           The only way to obtain an authoritative answer is to
           actually opent the data source and read the metadata,
           if the units are even included.
           Because multiple data sources may be drawn on to 
           read the data, and these data sources may contain
           the data in different units, only the first
           (primary) data source is consulted.
           
          
          \param quantity the quantity whose units are desired
          \return the units of the quantity
      */
      virtual std::string units( const std::string quantity );    

      /// waits a certain time
      /*! This method waits for a time, to avoid have a large number of processors
          running in parallel, all attempting to access the same network resource at the
          same moment. Instead, the processors of a single group may all
          delay together for a randomly-determined tie, to offset them from other
          processor groups.
          
          If parallel processing is not being used, the wait time is fixed at zero, and so there is 
          is no delay. 
          
      */
      void delay();

      /// sets the debugging level 
      /*! This method sets foe debugging level of both the MetMyGEOS object and its Catalog member.
          The debugging level determined the verbosity of messages being printed out to stderr.
          
          \param level the debugging level.
      */
      virtual void debug( int level ); 
         
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
      virtual MetGridData* MetGridCopy();


      /// calculate an on-the-fly 3D quantity
      /*! This method reads meteorological data and calculates an on-the-fly product from it
          as a GridField3D object.
      
           \param quantity the (internal or cf-convention) name of the quantity desired
           \param time the valid-at datestamp string for which data is desired
           \param OTFquants a vector of strings, each element of which is the name of an ingredient
                             used to calculate the on-the-fly product.
           \param grid3d a pointer to a GridField3D object that holds the data. 

      */
      void get_OTF( const std::string quantity, const std::string time, const std::vector<std::string> OTFquants, GridLatLonField3D* grid3d );      
      
      /// calculate an on-the-fly 2D quantity
      /*! This method reads meteorological data and calculates an on-the-fly product from it
          as a GridFieldSfc object.
      
           \param quantity the (internal or cf-convention) name of the quantity desired
           \param time the valid-at datestamp string for which data is desired
           \param OTFquants a vector of strings, each element of which is the name of an ingredient
                             used to calculate the on-the-fly product.

           \param gridsfc a pointer to a GridFieldSfc object that holds the data.
      */
      void get_OTF( const std::string quantity, const std::string time, const std::vector<std::string> OTFquants, GridLatLonFieldSfc* gridsfc );      

      /// get a 3D data field valid at a certain time, using basic access
      /*! This method reads meteorological data from some source and returns
          it in a GridField3D object. 
          
          It takes the data directly from the source, with no caching or
          met data client/server interactions.
          
           \param quantity the (internal or cf-convention) name of the quantity desired
           \param time the valid-at datestamp string for which data is desired

           \return a pointer to a GridField3D object that holds the data. The caller is
                   responsible for deleting this object, using the MetGridData remove() method.

      */
      GridLatLonField3D* new_directGrid3D( const std::string quantity, const std::string time );

      /// get a 2D data field valid at a certain time, using basic access
      /*! This method reads meteorological data from some source and returns
          it in a GridFieldSfc object.
          
          It takes the data directly from the source, with no caching or
          met data client/server interactions.
          
           \param quantity the (internal or cf-convention) name of the quantity desired
           \param time the valid-at datestamp string for which data is desired

           \return a pointer to a GridFieldSfc object that holds the data. The caller is
                   responsible for deleting this object, using the MetGridData remove() method.

      */
      GridLatLonFieldSfc* new_directGridSfc( const std::string quantity, const std::string time );


      /// returns a default set of vertical coordinate values
      /*! This method returns a vector of vertical coordinate values, depending on the coordinate system desired.
      
          \param coordSys a pointer to a string giving the name of the vertical coordinate. If NULLPTR, the 
                        meteorological source's current vertical coordinate is used.
           \return a pointer to a vector of coordinate values corresponding to the usual or default values
                        for this data souirce on the desired coordinates.              
      
      */
      std::vector<real>* vcoords( const std::string *coordSys ) const;


      /// generate a file path to a cache file for a gridded met field
      /*! This method returns a pointer to a full file path for a disk cache file, or NULL
          if the data are not to be cached on disk.

          \param item the GridFieldSfc data object which is to be cached

          \return the cache file path name
      */
      FilePath* cachefile( const GridFieldSfc* item ) const;

      /// generate a fle path to a cache file for a gridded met field
      /*! This method returns a pointer to a full file path for a disk cache file, or NULL
          if the data are not to be cached on disk.

          \param item the GridField3D data object which is to be cached
          
          \return the cache file path name
      */
      FilePath* cachefile( const GridField3D* item ) const;


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
      bool vConvert( GridField3D *input, std::string quant, std::string units, real scale=1.0, real offset=0.0 );


      /// find the two met data source times that bracket the desired model time
      /*! This method finds the two times from the meteorological data source that bracket the
          desired model time. This assumes that the met data are available at discrete
          snapshots in time. 

         \param quantity the desired quantity (Some data sources have different
                time spacings for different quantities)
         \param time the desired model time
         \param t1 the last data-valid time before or at the desired time
         \param t2 the first data-valid time after or at the desired time.
                   If t1 == t2, then the model time is exctly at a snapshot time.
         \return true if the first bracketing time is the same as the desired time; false if they are different.
      */   
      bool bracket( const std::string &quantity, double time, double *t1, double *t2);


      /// find the two calendar string timestamps that bracket the desired calendar string model time
      /*! This method finds the two calendar string timestamps that bracket the
          desired model time.

         \param quantity the desired quantity (Some data sources have different
                time spacings for different quantities)
         \param time the desired model time, expressed as a calendar string.
         \param t1 the last data-valid time before or at the desired time
         \param t2 the first data-valid time after or at the desired time
         \return true if the first bracketing time is the same as the desired time; false if they are different.
      */   
      bool bracket( const std::string &quantity, const std::string &time, std::string *t1, std::string *t2);

  
      /// set up for data access
      /*! Given a quantity and time, this method sets up any internal parameters that 
          may be used repeatedly during the course of data access.

           \param quantity the name of the quantity desired
           \param time the valid-at time for which data is desired
           \return  the number of dimensions of a data object (i.e, 2 or 3)
      */
      int setup(  const std::string quantity, const double time );    
  
      /// set up for data access
      /*! Given a quantity and time, this method sets up any internal parameters that 
          may be used repeatedly during the course of data access.

           \param quantity the name of the quantity desired
           \param time the valid-at datestamp string for which data is desired
           \return  the number of dimensions of a data object (i.e, 2 or 3)
      */
      int setup(  const std::string quantity, const std::string &time );    



      /// \brief sets the label to identify the GEOS data set being used
      /*! This method sets the label string that identifies the GEOS data set that
          is being used. This is used mainly for reading in the Catalog configuration file
          for the data set, and for constructing the names of cache files.
          It can be used, for example, to distinguish the output of different GEOS model runs.
        
          The default value is "MetMyGEOS".
          
          \param mytag the label used to identify the data set
      */    
      void metTag( const std::string mytag );

      /// \brief returns the label that identifies the GEOS data set being used
      /*! This method returns the label string that identifies the GEOS data set 
          that is being used. 
          
          \return the label string
      */ 
      std::string metTag() const;
      
      
      /// \brief sets a string that identifies a particular morecast model run
      /*! This method sets a string that identifies a particular model run.
          In the GEOS system, this is usually of the form "yyyymmdd_hh".
          
          The default value is "", which means that the dataset will attempt to figur eout
          what the correct model run to use is.
          
          \param fcst the string that identifies the model run. 
      */
      void modelRun( const std::string fcst );
      
      /// \brief returns the string that identifies a particular model run
      /*! This rethod sets a string that identifies a particular model run.
      
          \return the model run label
      */
      std::string modelRun() const; 
      

      /// return the default bad-or-missing data fill value
      /*! This method returns the default value used to fill in for bad or missing data values.
        
           /return the fill value
      */
      real defaultBad() const;

      /// sets the default bad-or-missing data fill value
      /*! This method sets the default value used to fill in for bad or missing data values.
        
           /param defBad the fill value
      */
      void defaultBad( real defBad );
      

      /// set the base time
      /*! This method sets the base time: the ISO8601 tdate string that corresponds to internal model time zero.
          \param date the date (in ISO8601 format) from which all model times
                 are to be referenced.
      */
      void set_BaseTime( std::string& date );           

      /// return the base time
      /*! This method returns the base time, as an ISO8601 string that corresponds to 
          the internal model time 0.000000.  Note that a new string is created for the return
          value, and it is up to the calling routine to delete it when it is
          no longer needed.
       */
      std::string BaseTime( ) const;  
      
      /// return a 3D data field object 
      /*! This method returns a 3D field object full of data valid at a given time.
        
           \param quantity the name of the desired meteorological data field
           \param time the ISO8601 data string specifying the valid-at time of the desired data
           
           \return a pointer to a new gridded-field object that holds the data. The caller is
                   responsible for deleting this object, using the MetGridData remove() method.
      
      */
      GridLatLonField3D* Get3D( const std::string quantity, const std::string time );         
      
      /// return a 2D data field object 
      /*! This method returns a 2D quasi-horizontal surface field object full of data valid at a given time.
        
           \param quantity the name of the desired meteorological data field
           \param time the ISO8601 data string specifying the valid-at time of the desired data
           
           \return a pointer to a new gridded-field object that holds the data. The caller is
                   responsible for deleting this object, using the MetGridData remove() method.
      
      */
      GridLatLonFieldSfc* GetSfc( const std::string quantity, const std::string time );


      /// obtain the units of a quantity provided by the data source
      /*! This method provides the units of a quantity that the meteorological data soirce
          provides. The meaning of the returned string is determined solely
          by the data source itself, according to whatever conventions is happens
          to follow.
          
          \param quantity the quantity whose units are desired
          \param time the model time for which data is valid. (This is used if a data file has ot be read in order to determine the units)
          \param flags flag values:
                  METDATA_MKS = return the units of the quantity if converted to MKS units
                  METDATA_FAILBAD =(ignored)
                  METDATA_NANBAD = (ignored)
                  METDATA_INFBAD = (ignored)
                  METDATA_KEEPBAD = (ignored)
          \return a string with the units. An empty string "" indicates a unitless quantity. If the units
                  cannot be determined, then "UNKNOWN" is returned.
      */
      virtual std::string units( const std::string& quantity, double time=0, int flags=0 );

      
          
   protected:

      /// sets quantity names in any OTF members from current quantity name values
      /*!
           This class (and potentially its subclasses) contain some MetOnTheFly (OTF)
           objects as members. These often contain the names of various physical quantities
           used in their calculations, in order to recognize from their methods' aprameters
           which calculations they need to do.
           
           However, when those quantity names are altered form their defaults in this class (as for example
           when they are read in from a Catalog), those quantity names must be
           set in the OTF member objects as well.
           
           This method does that.
           
      */
      virtual void updateOTF();

       
       /// the type of object this is
       static const string iam;
       
       /// junk stuff
       int junkstuff;


//   private:

       /// holds horizontal grid specifications
       class HGridSpec {
         public:
           /// File name code letter for the horizontal grid ("NM", "FN")
           std::string code;
           /// starting longitude
           real startLon;
           /// ending longitude
           real endLon;
           /// longitude spacing
           real deltaLon;
           /// number of longitudes
           int   nLons;
           /// starting latitude
           real startLat;
           /// ending latitude
           real endLat;
           /// latitude spacing
           real deltaLat;
           /// number of latitudes
           int   nLats;
           /// factor by which to thin out lons and lats
           int thin;
           /// offset for thinning longitudes
           int thin_offset;
           /*! bitwise indicators for which items have been specified (as opposed to computed)
                 0x01 = startlon
                 0x02 = endlon
                 0x04 = deltalon
                 0x08 = nlons
                 0x10 = startlat
                 0x20 = endlat
                 0x40 = deltalat
                 0x80 = nlats
           */
           int given;
           
           /// constructor
           HGridSpec();
           
           /// destructor
           ~HGridSpec();
           
           /// clears the spec
           void clear();
           
           /// sets horizontal specs
           /*! This method sets horizontal grid specifications.
           
               Some specifications are redundant. For example, if a starting longitude,
               delta longitude, and number of longitudes are all given, then the ending longitude is
               unneeded. This method uses whichever data are specified to compute the rest,
               insofar as that is possible. If all data are specified, the set is
               tested for consistency.
           
               \param lonstart a pointer to the starting longitude. If NULLPTR, then it is unspecified
               \param lonend a pointer to the ending longitude. If NULLPTR, then it is unspecified
               \param londelta a pointer to the interval between successive longitudes. If NULLPTR, then it is unspecified
               \param nlon a pointer to the number of longitudes. If NULLPTR, then it is unspecified
               \return true if the given specs are complete and consistent, false otherwise
               \param latstart a pointer to the starting latitude. If NULLPTR, then it is unspecified
               \param latend a pointer to the ending latitude. If NULLPTR, then it is unspecified
               \param latdelta a pointer to the interval between successive latitudes. If NULLPTR, then it is unspecified
               \param nlat a pointer to the number of latitudes. If NULLPTR, then it is unspecified
               \return true if the given specs are complete and consistent, false otherwise
           */    
           bool set( const real* lonstart, const real* lonend, const real* londelta, const int* nlon 
                  ,  const real* latstart, const real* latend, const real* latdelta, const int* nlat);
           
           /// tests whether a horizontal grid spec is compatible with this horizontal grid spec
           /*! This method checks whether a given HGridSpec object's settings 
               are compatible with those if this HGridSpec.
           
               \param cmp the HGridSpec object that is to be compared with this one. 
               \return true if cmp object is compatible with this HGridSpec, false otherwise
           */    
           bool test( const HGridSpec& cmp ) const;

       };
       
       /// holds vertical grid specifications
       class VGridSpec {
         public:
         
           /*! for vertical grid. The first character must be one of:
             * "P":  standard pressure coordinates (42 levels from 1000 to 0.1 hPa)
             * "H":  potential temperature (tHeta) coordinates
             * "z":  altitude cooordinates
             * "a": Pressure altitude (PAlt) coordinates
             * "L": model levels (72 levels, from 0 to 71)
             * "E": model layer edges (73 levels, from 0 to 71)
             * "2": two-dimensional quantity (no vertical coordinate)
             And if the code is "", then the vertical grid is unknown/indeterminate
           */
           std::string code;
           
           /// quantity name
           std::string quant;
           /// units of the quantity
           std::string units;
           /// scale to take these unjits into MKS
           real mksScale;
           /// offset to take these unjits into MKS
           real mksOffset;
           
           /// the set of vertical levels
           std::vector<real> levs;
           /// number of vertical levels
           int nLevs;
           
           /// constructor
           VGridSpec();
           
           /// destructor
           ~VGridSpec();
           
           /// clears the spec
           void clear();
           
           
           /// sets the vertical grid specs
           /*! This method sets the characteristics of the vertical grid.
           
               It can load an explicit list of vertical levels, or it can rely on
               a default set for the given vertical quantity.
               
               \param vcode a string with the code of the vertical coordinate
               \param quantity a string with the quantity of the vertical coordinate. Ignored if code is 2
               \param qunits a sring with the units of the vertical coordinate quantity. Ignored if code is 2
               \param scale a pointer to a scaling factor that takes the given vertical coordinates into MKs units. Ignored if code is 2
               \param offset a pointer to an offset that takes the given verticla coordinate into MKS units. Ignored if code is 2
               \param levels a pointer to a vector of vertical levels, explicitly specified. Ignored if code is 2
               \return true if the given settings are acceptable
           
           */
           bool set( const std::string& vcode, const std::string& quantity, const std::string& qunits, real* scale=NULLPTR, real* offset=NULLPTR, std::vector<real>* levels=NULLPTR );
           
           /// tests whether a vertical grid spec is compatible with this vertical grid spec
           /*! This method checks whether a given VGridSpec object's settings 
               are compatible with those if this time spec.
           
               \param cmp the VGridSpec object that is to be compared with this one. 
               \return true if cmp object is compatible with this VGridSpec, false otherwise
           */    
           bool test( const VGridSpec& cmp ) const;
           
       };
       
       
       /// holds time grid specifications
       class TGridSpec {
         public:

           /// time-averaging: 
           //  ""   = unknown
           //  "P"  = permanent, unchanging value
           //  "I"  = stantaneous
           //  "A1" = one-hour average
           //  "A3" = three-hour average
           std::string tave;
           
           /// the time of the first snapshot in the file, in day1900 format
           double start;
           /// the time of the first snapshot in the next file, in day1900 format
           double next;
           /// the delta between successive timesteps within the file, in days
           double delta;
           /// the time of the last snapshot in the file, in day1900 format
           double end;
           /// the number of time snapshots in the file
           int n;
           /// The time of the first snapshot of the file, in day fractions
           // (i.e., fractional part of start)
           double toff;
           /*! bitwise indicators for which items have been specified (as opposed to computed)
                 0x01 = start
                 0x02 = next
                 0x04 = delta
                 0x08 = n
                 0x10 = end
           */
           int given;
           
           /// constructor
           TGridSpec();
           
           /// destructor
           ~TGridSpec();
           
           /// clears the spec
           void clear();

           /// sets time specs, computing missing ones where needed
           /*! This method sets time specifications.
           
               Some specifications are redundant. For example, if a starting time,
               delta time, and number of times are all given, then the ending time is
               unneeded. This method uses whichever data are specified to compute the rest,
               insofar as that is possible. If all data are specified, the set is
               tested for consistency.
           
               \param tstart the day1900 starting time of the file. 
               \param tnext the day1900 starting time of the next file. If < tstart, it is unspecified.
               \param tdelta the day1900 interval between successive timestamps. If <= 0, it is unspecified
               \return true if the given specs are complete and consistent, false otherwise
           */    
           bool set( double tstart, double tnext, double tdelta );

           /// sets time specs, computing missing ones where needed
           /*! This method sets time specifications.
           
               Some specifications are redundant. For example, if a starting time,
               delta time, and number of times are all given, then the ending time is
               unneeded. This method uses whichever data are specified to compute the rest,
               insofar as that is possible. If all data are specified, the set is
               tested for consistency.
           
               \param tstart the day1900 starting time of the file. 
               \param tend the day1900 ending time of the  file.  If < tstart, it is unspecified.
               \param tn the number of time stamps in the file. If <= 0, then it is unspecified
               \return true if the given specs are complete and consistent, false otherwise
           */    
           bool set( double tstart, double tend, int tn );

           /// sets time specs, computing missing ones where needed
           /*! This method sets time specifications.
           
               Some specifications are redundant. For example, if a starting time,
               delta time, and number of times are all given, then the ending time is
               unneeded. This method uses whichever data are specified to compute the rest,
               insofar as that is possible. If all data are specified, the set is
               tested for consistency.
           
               \param tstart the day1900 starting time of the file. 
               \param tnext the day1900 starting time of the next file. If < tstart, it is unspecified.
               \param tn the number of time stamps in the file. If <= 0, then it is unspecified
               \param tdelta the day1900 interval between successive timestamps. If <= 0, it is unspecified
               \param tend the day1900 ending time of the  file.  If < tstart, it is unspecified.
               \return true if the given specs are complete and consistent, false otherwise
           */    
           bool set( double tstart, double tnext, int tn, double tdelta, double tend );


           /// tests whether a time grid spec is compatible with this time grid spec
           /*! This method checks whether a given TGridSpec object's settings 
               are compatible with those if this time spec.
           
               \param cmp the TGridSpec object that is to be compared with this one. 
               \return true if cmp object is compatible with this TGridSpec, false otherwise
           */    
           bool test( const TGridSpec& cmp ) const;
           
           /// tests whether a given time is compatible with this time grid apec
           /*! this method determines whether a given time can be contained
               within the time grid specifications.
               
               \param t the time to be tested
               \return true if the time is OK, false otherwise
          */
          bool inside( double t );     
          
          /// merges settings from another TGridSpec onject into this onw
          /*! This method takes settings from a given TGridSpec object and merges them into this one.
          
              The merge takes place if and only if two two TGridSpec objects are comaptible, as givne
              by the return value of the test() method. 
              
              This is used, for example, to take settigns read from a file and merge them into
              the TGridSpec used to access that file.
              
              \param in a TGridSpec object
              \return true of the merge was successful, false if the two were incompatible
          */
          bool merge( TGridSpec& in );    
          
          /// returns key information about where a desired time lies within the TGridSpec's time grid.
          /*! Given a desired time, this method returns information about where in the time grid that desired time lies.
              
              \param desired_time the double precision time whose position is desired
              \param myStart a reference to a double that will hold the starting time of the time grid
              \param myN a reference to an integer that will hold the number of grid intervals before the desired time
              \param myInc a reference to a double that will hold the increment between successive times in the grid
              \return true if there was enough information to retrieve the time position, false otherwise
              
          */
          bool get( double desired_time, double& myStart, int& myN, double& myInc ) const;

       };
       

      /*! a string label used to distinguish this data set from otherss. 
          This can be used, for example, to distinguish between the output
          of different GEOS model runs.
      */     
      std::string mettag;
      
      /// a string used to identify a particular forecast model run
      std::string modelrun;

      /// maps quantity+time to URLs/filenames
      Catalog catlog;
      
      /// set to true if the Catalog has been loaded
      bool ready;
      
      /// met base time -- this is the day1900 number that corresponds to intenral model time 0.0
      double basetime;
      
      /// catalog basetime -- this is the offset between model time and catalog time
      double catTimeOffset;
      
      /// Object for dealing with Dates
      gigatraj::CalGregorian cal;
      
      /// the name for the temperature quantity
      std::string temperature_name;
      /// the name for the temperature derivative quantity
      std::string temperatureDot_name;
      /// the name of the model level coordinate (not necc. the same as levelName)
      std::string modellevel_name;
      /// the name of the model edge coordinate (not necc. the same as levelName)
      std::string modeledge_name;
      /// the name of the solar zenith angle
      std::string sza_name;
      /// the name of the WMO tropopause
      std::string trop_name;

      /// describes vertical wind quantities that are available for the different vertical coordinates
      std::map<std::string, Catalog::DataSource> verticalWinds;
      
      /// holds the remote file's legal variable names for longitude, latitude, vertical level, and time
      std::vector<std::string> legalDims;
      
      /// identifies OTF quantities not supplied (or not satisfactorily supplied) by the data source
      std::map<std::string, std::string> otfIDs;

      /// on-the-fly calculator for theta (potential temperature)
      ThetaOTF gettheta; 
      /// on-the-fly calculator for the local time derivative of theta
      ThetaDotOTF getthetadot; 
      /// on-the-fly calculator for pressure
      PressOTF getpress; 
      /// on-the-fly calculator for pressure altitude
//      PAltOTF getpalt;
      /// on-the-fly calculator for time rate of change of pressure altitude
//      PAltDotOTF getpaltdot;
      /// on-the-fly calculator for the solar zenith angle
      SZAOTF getsza;
      /// on-the-fly calculator for the WMO tropopause
      TropOTF gettrop;
      
      /// true if a netcdf file is currently open
      bool is_open;
      /// true if the file to be opened is a URL
      bool is_url;
      /// the name of a netcdf file either openeed or about to be opened
      std::string current_file;
      /// The set of data sources currently under consideration.
      std::vector<Catalog::DataSource> ds;
      /// index of a DataSource being examined (but not necessarily opened)
      int test_dsrc;
      /// the index of a DataSource one of whose (pre- or post-) files has been opened; -1 if none
      int opened_dsrc;
      /// the name of the opened URL
      std::string opened_url;
      /// maximum number of data values to be read in with a single netcdf call
      size_t max_data; // useful when reading from a URL
      /// a desired time to be tested against some dataset target
      double target_time;
      /// desired target date to be tested against some dataset target
      std::string target_date;
      /// true if the target time and date have been set
      bool target_time_valid;

      /// the default bad-or-missing data fill value to be used
      real fillval;
       
       /// number of times to try a given netcdf/OPeNDAP operation 
       int ntries;
       
       /// a native base time from which times in the remote file are referenced, in their native format.
       double time_zero;
       
       /// holds first/last/delta/size specs as read from a dimension of some numeric type
       typedef union spantrip {
          /// holds first/last/delta/size specs for integer data
          struct inttrip {
             /// first index
             int first;
             /// final index
             int last;
             /// index spacing
             int delta;
             /// number of points
             int size;
          } 
          /// name of the integer specifier struct
          intSpec;
          /// holds first/last/delta/size specs for float data
          struct floattrip {
             /// first index
             float first;
             /// final index
             float last;
             /// index spacing
             float delta;
             /// number of points
             int size;
          } 
          /// name of the float specifier struct
          floatSpec;
          /// holds first/last/delta/size specs for double data
          struct doubletrip {
             /// first index
             double first;
             /// final index
             double last;
             /// index spacing
             double delta;
             /// number of points
             int size;
          } 
          /// name of the double specifier struct
          doubleSpec;
       } SpanTriplet;
       
       /// holds netcdf attribute value in any of several formats
       typedef struct spanattr {
              /// netcdf data type
              nc_type type;
              /// netcdf data name
              std::string name;
              /// vector of data values (integer format)
              std::vector<int> ivalue;
              /// vector of data values (float format)
              std::vector<float> fvalue;
              /// vector of data values (double format)
              std::vector<double> dvalue;
              /// string data value
              std::string svalue;
       } SpanAttr;
       
      /// the name of the time netcdf variable
      std::string timeName;
      /// the name of the longitude netcdf variable
      std::string lonName;
      /// the name of the latitude netcdf variable
      std::string latName;
      /// the name of the vertical netcdf variable
      std::string levelName;
      

      /// the specs of the horizontal grid of the data source to be read
      HGridSpec hgrid;
      /// the specs of the vertical grid of the data source to be read
      VGridSpec vgrid;
      /// the specs of the time grid of the data source to be read
      TGridSpec tgrid;
      /// the trgrid spec for an open URL
      TGridSpec url_tgrid;

      /// time to wait between tries (seconds)
      unsigned int waittry;
      /// time to wait betweeen url opens (to lessen the load on the server). If negative, it's a random time between zero and abs(openwait)
      int openwait;


      // NETCDF-related variables:
      /// the netcdf handle
      int ncid;
      /// the number of variable in the netcdf file which are dimensions 
      int ndimens;
      /// the number of variable in the netcdf file
      int nvars;
      /// the number of global attributes in the netcdf file
      int ngatts;
      /// the index of the unbounded dimension in the netcdf file, if any
      int unlimdim_idx;
      /// global attributes--string form
      std::map<std::string,std::string> gattr_strings;
      /// global attributes--real form
      std::map<std::string,real> gattr_reals;
      /// iterator for string global attributes
      std::map<std::string,std::string>::const_iterator gs_iter;
      /// iterator for real global attributes
      std::map<std::string,real>::const_iterator gr_iter;

      
      /// refreah the legal coordinates
      void load_legalDims();

      /// refreah the legal coordinates
      void load_vertWindInfo();
      
      /// refresh the OTF settings
      void refresh_OTF();
      
      /// pre-use initialization, such as loading the Catalog, 
      void init();
      
      
      /// sets up for some basic variable identifications
      void setup_vars();

      /// reset the internals
      /*! This method resets the internal state of this object.
          Among other things, this closes any open netcdf files.
      */
      void reset();
 
      /// calculates an expiration time for the data returned by this object.
      /*! This method calculates an expiration time from the model run,
          for use with GridField objects created by this class.
          The time at which the data should no longer be used depends on 
          both the model run and the valid-at time of the data,
          since the different GMAO model runs extend out to different
          forecast periods.
          
          \param valid_at a string contaiing the valid-at time of the data
                          whose expiration is to be calculated.
         
          \return the expiration timestamp beyond which the data should not be used,
                  in Unix epoch format.
      */
      time_t expiration( const std::string &valid_at );

      /// creates a new MetMERRA2 object
      /*! This method  duplicates the current object,
          creating a new MetMERRA2 object
          with the same key characteristics and settings 
          as the old. However, the new object has no open files
          or met data held in memory cache.
          
          This is useful for calculating quantities on the fly
          from consitituent quantities that have to be read in 
          separately.
          
          \return a pointer to the new object
      */
      MetMyGEOS* myNew();
       
      /// \brief Returns a vector of prerequisite quantities 
      /*! Returns a vector of quantities that must exist for the given quantity to exist
          
       \param quantity  a string that  holds the name of the desired quantity. 
      
        \return a pointer to a vector of strings each of which contains the name of a quantity
                that needs to be read to obtain the desired quantity. If the 
                desired quantity is not an on-the-fly quantoity, then the result will
                be a single-element vector that contains the name of that quantity. 
                It is the responsiblity of the calling routine to delete the vector
                once it is no longer needed.   
      */
      std::vector<std::string> *testQuantity( const std::string& quantity );

      
      
      /// set up for data access
      /*! Given a quantity and time, this method sets up any internal parameters that 
          may be used repeatedly during the course of data access.
          This is called by the setup() methods,, to do the actual work.

           \param quantity the name of the quantity desired
           \param time the valid-at datestamp string for which data is desired
           \return  the number of dimensions of a data object (i.e, 2 or 3)
      */
      int prep(  const std::string quantity, const std::string& time );

      /// initializes a vector of Datasources for a given quantity and time
      /*! This method queries the internal catalog for data sources that
          can provide the given quantity for the given time. Results are stored internally.
          
          This leaves the internal test index set to 0 and the open-file index set to -1.
          If no data are found, then an error is thrown.

         \param quantity the desired quantity 
         \param time the desired model time, expressed as a calendar string.
         \return true if a valid vector of DataSource objects is available for use, false otherwise
         
      */
      bool dsInit( const std::string& quantity="", const std::string& time = "" );     

      /// \brief checks whether the vector of catalog data sources is valid for use
      /*! This method checks whether the vector of data sources last returned
          by a Catalog query, is still valid for a desired quantity and time.
        
          \param quant the name of he quantity. This is chacked against the current data source "name" field.
          \param t the time MetMyGEOS time. This is checked against the range of times in the current data source.
          
          \return true if the data source vector is ok to use, false otherise
      
      */
      bool DsValid( const std::string& quant, double t );

      /// \brief convert catalog time to met time (
      /*!  This method converts a numeric time used in the Catalog (number of days since 1899-12-31T00)
           to a model time (number of days elapsed from a specified time).
          
           \param catTime the input Catalog time
           \return the output MetMyGEOS time
      */
      inline double catTime2metTime( double catTime ) const {
          return catTime + catTimeOffset;
      };     

      /// \brief convert met time to catalog time  (
      /*!  This method converts a numeric model time (number of days elapsed from a specified time)
           to a Catalog (number of days since 1899-12-31T00) time.
          
           \param metTime the input MetMyGEOS time
           \return the output Catalog time
      */
      inline double metTime2catTime( double metTime ) const {
          return metTime - catTimeOffset;
      };     

      /// \brief queries a test DataSource for its quantity code
      /*! This method returns the quantity of a DataSource
      
          \param index if >=0, the index into the internal vector oif DataSources, selecting the one to be queried
                       By default, the internal current test index is used.
          \return the name of the quantity.             
      
      */    
      std::string queryQuantity( int index=-1 );

      /// \brief queries a test DataSource for its data units
      /*! This method returns the data units of a DataSource
      
          \param index if >=0, the index into the internal vector of DataSources, selecting the one to be queried
                       By default, the internal current test index is used.
          \return the units          
      
      */    
      std::string queryUnits( int index=-1 );

      /// \brief queries a test DataSource for its time
      /*! This method returns the valid-at time of a DataSource
      
          \param index if >=0, the index into the internal vector oif DataSources, selecting the one to be queried
                       By default, the internal current test index is used.
          \param pre a pointer to a string that will receive the data set time that preceeeds the valid-at time
          \param post a pointer to a string that will receive the data set time that preceeeds the valid-at time
          \return the calendar timestamp            
      
      */    
      std::string queryTime( int index=-1, std::string* pre=NULLPTR, std::string* post=NULLPTR );

      /// \brief queries whether a given quantity is an on-the-fly quantity
      /*! This method determines whether a given quantity represents a quantity
          that must be calculated on the fly from other quantities.
          
          \param quantity the name of the quantity to be tested
          \param dependents a vector of quantity names that must be read for the OTF calculation to be carrie dout
          \return true if the quantity is OTF, false otherwise           
      
      */
      bool queryOTF( const std::string& quantity, std::vector<std::string>& dependents );

      /// \brief queries a test DataSource for whether is is an on-the-fly quantity
      /*! This method determines whether a test DataSource represents a quantity
          that must be calculated on the fly from other quantities.
          
          \param dependents a vector of quantity names that must be read for the OTF calculation to be carrie dout
          \param index if >=0, the index into the internal vector of DataSources, selecting the one to be queried
                       By default, the internal current test index is used.
          \return true if the quantity is OTF, false otherwise           
      
      */
      bool queryOTF( std::vector<std::string>& dependents, int index=-1 );

      /// \brief queries a test DataSource for its dimensionality
      /*! This method returns the number of spatial dimensions associated with a DataSource
      
          \param index if >=0, the index into the internal vector oif DataSources, selecting the one to be queried
                       By default, the internal current test index is used.
          \return the number of dimensions        
      
      */    
      int queryDimensionality( int index=-1 );

      /// \brief queries a test DataSource for an attribute setting
      /*! This method returns the value of an attribute of a DataSource
      
          \param attr the name of the attribute to be queried             
          \param index if >=0, the index into the internal vector oif DataSources, selecting the one to be queried
                       By default, the internal current test index is used.
          \return the value of the attribute
      
      */    
      std::string queryAttr( std::string& attr, int index=-1 );


      /// \brief queries a test DataSource for its time spacing
      /*! This method returns the base time and time increment of a DataSource
      
          \param quantity the name of the quantity whose timings are being queried
          \param validAt the (internal model) time for which valid data are desired.
          \param tinc a reference to the output time increment
          \param toff a reference to the output time offset
          \param index if >=0, the index into the internal vector oif DataSources, selecting the one to be queried
                       By default, the internal current test index is used.
      
      */    
      void queryTimeSpacing( const std::string& quantity, double validAt, double& tinc, double& toff, int index=-1 );

      /// \brief queries a test DataSource for its vertical coordinate setting
      /*! This method returns a code representing th evertical coordinates used by a DataSource

          This method assumes that the vertical coordinate used is reflected in a Datasource Target
          attribute, 'VertCoord',  The value is then coded as one of:
             * "":  unknown/indeterminate
             * "P":  standard pressure coordinates (42 levels from 1000 to 0.1 hPa)
             * "H":  potential temperature (tHeta) coordinates
             * "z":  altitude cooordinates
             * "a": Pressure altitude (PAlt) coordinates
             * "L": model (layer mid-) levels (72 levels, from 0 to 71)
             * "E": model layer edges (73 levels, form 0 to 72)
             * "2": two-dimensional quantity (no vertical coordinate)
      
          \param index if >=0, the index into the internal vector oif DataSources, selecting the one to be queried
                       By default, the internal current test index is used.
          \return the vertical coordniate code
      
      */    
      std::string queryVertical( int index=-1 );
      
      /// \brief  queries a test DataSource for its horizontal coordinate setting
      /*! This method returns a string that encodes the horizontal coordiats used by a DataSource.
      
          This method assumes that the horizontal coordinate system used is reflected in a Datasource Target
          attribute, 'HorizCoord',  The value is then coded as one of:
             "1x1" : a one-degree by one-degree global grid, from -90 to 90 in latitude, from -180. to +179 in longitude
             "0.5x0.5": a half-degree by half-degree global grid, from -90 to 90 in latitude, from -180 to +179.5 in longitude
             "FN": the GEOS-FP native lat-lon grid, 721 latitudes from -90 to 90, 1152 longitudes from -80 to +179.6875
             "MN": the MERRA2 native lat-lon grid, 361 latitudes from -90 to 90, 576 longitudes from -180 to 179.375.
      
          \param index if >=0, the index into the internal vector oif DataSources, selecting the one to be queried
                       By default, the internal current test index is used.
          \return the horizontal coordinate code
          
      */
      std::string queryHorizontal( int index=-1 );

       /// waits a given time interval
       /*! This method is a utility function for use in
           netcdf read operations. When accessing the GMAO data source,
           temporary failures sometimes occur. Most of the time, waiting
           for a bit of time and and then re-trying the operation
           will succeed. You need want to try multiple times
           before the operation succeeds.
           
           This function takes two arguments. The first, "error", is
           compared to the netcdf library's NC_NOERR value.
           If it is equal to NO_NOERR, then false is returned.
           Otherwise, we consider that an error has occurred and we must decide
           whether to re-try the operation.
           The second argument, the trial number "trial" is incremented and compared to
           this object's ntries element. If lesz than ntries,
           the function will wait for the number of seconds in 
           (waittry*trial) seconds and then return true.
           Otherwise, the function will return false immediately.
           
           Thus, you can use this function like this:
           
           \code
               int trial, err;
               
               trial = 0;
               do {
                  err = nc_do_something(...);
               } while ( try_again( err, trial ) );
               if (err != NC_ERR ) {
                  throw(netcdfErrorHere());
               }
           \endcode
           
           \param error an error status form the calling routine. This is compared
                        to NC_NOERR.
           \param trial the trial number. This is typically initialized to 0 before
                        first attempting the operation.         

           \return true if an opeation should try again, false if not.

       */
       bool try_again( const int error, int &trial ) const;
           
     
       /// open the GEOS Source url  
       /*! This method opens a GEOS Data Source
       
           The URL is taken from the either the pre or the post URL of a DataSource.
           The DataSource must have been obtained already from the Catalog using its query() method.
           The query() method obtains a vector of DataSource objects, and the most promising
           element of the vector has its index stored in an internal index. Ordinarily an attempt to open
           that DataSource is made first. But if an index is given here then that will be used instead.
           If this first attempt to open a Datasource's URL fails, then each DataSource will be 
           attempted, in order starting with the zeroeth, until an open succeeds or an error is thrown.
           

            \param pre true if the DataSource's pre-valid-at time snapshot is to be used, false if the post- snapshot is to be used
            \param index the index into the current vector of DataSource objects, if overriding the default (i.e./, if not -1)
                         
       */
       void Source_open( bool pre=true, int index=-1 );

       /// take care of housekeeping after openeing a url
       /*! This method loads certain metadata immediately after
           successfully opening a url. It is called from within Source_open().
           
           \param index the index into the current vector of DataSource objects, of th eobject just opened
           
           \return true if all went well, false otherise (in which case another URL should be opened)
       */
       bool Source_postOpen( int index );    

       /// close a GEOS Source url
       /*! This method closes an open GEOS source.
       */
       void Source_close();

       /// sets a desired time
       /*! This method sets a desired time that is used to check an opened url
       
            \param t the calendar time
            
       */
       void Source_setDesiredTime( double t );
       
       /// sets a desired time
       /*! This method sets a desired time that is used to check an opened url
       
            \param d the qeuivalent atestring corresponding to the time
            
       */
       void Source_setDesiredTime( const std::string& d );
       
       /// clears the desired time
       /*! This method ensures that no url will be checked for the presens of a desired time
       */
       void Source_clearDesiredTime();
       
       /// tests the open URL for the desired time
       /*! This method checks an open uRL's metadata to ensure
           that the currently-set desired time, if any,
           is contained within the URL.
           
           \return true if the url contained the desired time, or if no desired time is currently set
        */
        bool Source_testDesiredTime();
        
       /// load dimensional informaton from the GEOS source
       /*! This method loads dimensional informaton from the GEOS source.
           Dimensional information is stored in this object.
       
           \param nvdims the number of dimensions to retrieve
           \param dimids an nvdims-element array containing the indices of each GEOS source grid dimension 
       */
       void Source_checkdims(const int nvdims, const int*dimids );

       /// converts time from the file internal format to day1900
       /*! This method converts a time value from the format used in the original remote data file,
           to day1900 (i.e., days elasped since 1900-01-01T00:00:00).
           
           \param nativeTime the time value, in file-native format
       */    
       double Source_time_nativeTo1900( double nativeTime );

       /// converts a time interval from the file internal format to day1900
       /*! This method converts a time delta (interval) value from the format used in the original remote data file,
           to day1900 (i.e., days).
           
           \param nativeDelTime the time value, in file-native format 
       */    
       double Source_deltime_nativeTo1900( double nativeDelTime );

       /// find the GEOS Source time closest to the requested time
       /*! This method finds the data valid-at time in the data source that is
           close the desired time.
           
           \param quantity the name of the quantity whose values are desired
           \param desired_time the (internal model) valid-at time desired
           
           \return the index into the GEOS source time dimension that corresponds to the desired time.
                   If the nearest time is more than two minutes away from a GEOS source time value, an error
                   message is printed.
       
       */
       int Source_findtime( const std::string& quantity, const double desired_time );



       /// get netcdf attributes of a given dimension
       /*! This method reads the netcdf attributes associated with a given dimension.
       
           \param dname (referenced but not actually used)
           \param var_id the dimension ID whose attributes are to be obtained
           \param nattrs the numnber of attributes
       
       */
       void Source_dim_attrs(const std::string &dname, const int var_id, const int nattrs );


       /// get netcdf attributes of a given 3D variable
       /*! This method reads the netcdf attributes associated with a given 3D variable.
           Such attributes include the units and bad-or-missing-data fill values.
       
           \param var_id the netcdf variable ID
           \param nattrs the numnber of attributes
           \param datagrid a pointer to a GridLatLonField3D object into which the attributes will be loaded.
       
       */
       void Source_getattrs(const int var_id, const int nattrs, GridLatLonField3D *datagrid );

       /// get netcdf attributes of a given 2D surface variable
       /*! This method reads the netcdf attributes associated with a given 2D surface variable.
           Such attributes include the units and bad-or-missing-data fill values.
       
           \param var_id the netcdf variable ID
           \param nattrs the numnber of attributes
           \param datagrid a pointer to a GridLatLonFieldSfc object into which the attributes will be loaded.
       
       */
       void Source_getattrs(const int var_id, const int nattrs, GridLatLonFieldSfc *datagrid );

       /// reads an attribute of type chars into a string
       /*! This method reads a character-based attribute from an open netcdf file
              \param result the value of the attribute, returned as a string 
              \param attr_name the name of the attribute (null-terminated C string)
              \param varid the ID of the attribute (NC_GLOBAL for global attributes)
              \param len the number of characters expected in the attribute value
       */
       void Source_read_attr( std::string& result, char *attr_name, int varid, size_t len );

       /// reads an attribute of type byte into a vector of unsigned chars
       /*! This method reads a byte-based attribute from an open netcdf file.
              \param result the value of the attribute, returned as a vector of unsigned chars
              \param attr_name the name of the attribute (null-terminated C string)
              \param varid the ID of the attribute (NC_GLOBAL for global attributes)
              \param len the number of bytes expected in the attribute value
       */
       void Source_read_attr( std::vector<unsigned char> &result, char *attr_name, int varid, size_t len );

       /// reads an attribute of type short into a vector of shorts
       /*! This method reads a short attribute from an open netcdf file.
              \param result the value of the attribute, returned as a vector of shorts
              \param attr_name the name of the attribute (null-terminated C string)
              \param varid the ID of the attribute (NC_GLOBAL for global attributes)
              \param len the number of short expected in the attribute value; only the first one is returned.
       */
       void Source_read_attr( std::vector<short> &result, char *attr_name, int varid, size_t len );

       /// reads an attribute of type int into a vector of ints
       /*! This method reads an int attribute from an open netcdf file.
              \param result the value of the attribute, returned as a vector of ints
              \param attr_name the name of the attribute (null-terminated C string)
              \param varid the ID of the attribute (NC_GLOBAL for global attributes)
              \param len the number of short expected in the attribute value; only the first one is returned.
       */
       void Source_read_attr( std::vector<int> &result, char *attr_name, int varid, size_t len );

       /// reads an attribute of type float into a vector of floats
       /*! This method reads a float attribute from an open netcdf file.
              \param result the value of the attribute, returned as a vector of floats
              \param attr_name the name of the attribute (null-terminated C string)
              \param varid the ID of the attribute (NC_GLOBAL for global attributes)
              \param len the number of floats expected in the attribute value; only the first one is returned.
       */
       void Source_read_attr( std::vector<float> &result, char *attr_name, int varid, size_t len );

       /// reads an attribute of type double into a vector of doubles
       /*! This method reads a double attribute from an open netcdf file.
              \param result the value of the attribute, returned as a vector of doubles
              \param attr_name the name of the attribute (null-terminated C string)
              \param varid the ID of the attribute (NC_GLOBAL for global attributes)
              \param len the number of doubles expected in the attribute value; only the first one is returned.
       */
       void Source_read_attr( std::vector<double> &result, char *attr_name, int varid, size_t len );


       /// reads a dimension from an open remote file
       /*! This method reads dimensional values from an open remote file.
           It actually reads just the first and last values and determines
           the delta between successive dimensional values.
           
           \param dim_name the name of the dimension to be read
           
           \param dim_type the numeric type of the dimensional values: 
                  NC_DOUBLE, NC_FLOAT, or NC_INT.
           
           \param span a union that holds the first dimensional value (.first), 
                       the last dimensional value (.last),
                       the delta between successive values (.delta),
                       and the number of values (.size).
                       There are three elements in the union:
                       intSpec for values of type int,
                       floatSpec for values of type float,
                       and doubleSpec for values of type double.
                       Each in turn has the elements lists above: 
                       first, last, delta, and size. Thus, if the
                       return type is NC_DOUBLE, then you would
                       access the delta value using span.doubleSpec.delta.
           
           \param scale a pointer to a scale factor to be applied to the first, last, and delta values of the span
           \param offset a pointer to an offset to be applied to the first and last values of the span
       */
       void Source_read_dim( std::string &dim_name, nc_type &dim_type, SpanTriplet &span, double* scale, double*offset);

       /// reads a dimension from an open remote file
       /*! This method reads dimensional values from an open remote file.
       
           \param dim_name the name of the dimension to be read
           \param vals (output) a vector of dimensional values. All numeric types
                       are converted to type real.
       */
       void Source_read_dim( std::string &dim_name, std::vector<real> &vals);

       /// updates the horizontal grid
       /*! This method updates the met source's horizonal grid information,
           based on characteristics in cur_hgrid, which comes from the current open file.
           This affects the id of cur_hgrid, as well as the lons and lats elements
           of this object,
       */    
       void update_hgrid();
       
       /// updates the vertical grid
       /*! This method updates the met source's vertical grid information,
           based on characteristics in cur_vgrid, which comes from the current open file.
           This affects the id of cur_vgrid, as well as the native_zs and native_nzs elements
           of this object,
       */    
       void update_vgrid();

       /// updates the time grid
       /*! This method updates the met source's time grid information,
           based on characteristics in cur_vgrid, which comes from the current open file.
           This affects tbase and tspace of cur_tgrid.
       */    
       void update_tgrid();
       
       /// updates the forecast lead time, if any
       /*! This method consults the Catalog entry Target 
           for an attribute called "FCST". If present, then
           its numeric value is returned. Otherwise, NaN is returned.
           
           \return Returns the forecast lead time, in model time, if greater than zzero.
                   If equal to zero, then nthe data are analysis or assimilation values.
                   If NaN, then the information is not available.
       */
       double get_fcst();            
                   
       /// reads the standard set of dimension from an open remote file
       /*! This method reads the time, lon, lat, and (if present) lev dimensions
           from an open remote file.
           This results in the cur_hgrid, cur_vgrid, and cur_tgrid obejcts
           all being set to values consistent with the characteristics of
           the open file.
       */    
       void Source_read_all_dims();

       /// reads a key set of double dimension values
       /*! This method reads the first and last value of a set of dimensional values;
              \param first (output) the first dimensional value
              \param last (output) the last dimensional value
              \param delta (output) the delta betweel two successive dimensional values
              \param varid the ID of the dimension variable
              \param len the number of doubles expected in the dimension
       */
       void Source_read_dim_doubles( double &first, double &last, double &delta, int varid, size_t len );

       /// reads a set of double dimension values
       /*! This method reads a set of double dimension values (longitude, latitude, vertical level) from an open netcdf file
              \param vals returned vector of reals containing the dimensional values
              \param varid the ID of the dimension variable
              \param len the number of doubles expected in the dimension
       */
       void Source_read_dim_doubles( std::vector<real>&vals, int varid, size_t len );

       /// reads a key set of float dimension values
       /*! This method reads the first and last value of a set of dimensional values;
              \param first (output) the first dimensional value
              \param last (output) the last dimensional value
              \param delta (output) the delta betweel two successive dimensional values
              \param varid the ID of the dimension variable
              \param len the number of floats expected in the dimension
       */
       void Source_read_dim_floats( float &first, float &last, float &delta, int varid, size_t len );

       /// reads a set of float dimension values
       /*! This method reads a set of float dimension values (longitude, latitude, vertical level) from an open netcdf file
              \param vals returned vector of reals containing the dimensional values
              \param varid the ID of the dimension variable
              \param len the number of floats expected in the dimension
       */
       void Source_read_dim_floats( std::vector<real>&vals, int varid, size_t len );

       /// reads a key set of int dimension values
       /*! This method reads the first and last value of a set of dimensional values;
              \param first (output) the first dimensional value
              \param last (output) the last dimensional value
              \param delta (output) the delta betweel two successive dimensional values
              \param varid the ID of the dimension variable
              \param len the number of ints expected in the dimension
       */
       void Source_read_dim_ints( int &first, int &last, int &delta, int varid, size_t len );

       /// reads a set of int dimension values
       /*! This method reads a set of int dimension values (longitude, latitude, vertical level) from an open netcdf file
              \param vals returned vector of reals containing the dimensional values
              \param varid the ID of the dimension variable
              \param len the number of ints expected in the dimension
       */
       void Source_read_dim_ints( std::vector<real>&vals, int varid, size_t len );

       /// reads a variable's float-format data
       /*! This method reads a data variable from an open netcdf file.
              \param vals the returned vector of data, as a vectors of floats in row-major order
              \param var_id the netcdf variable id of the data to be read
              \param ndims 3 for three-dimensional (lon,lat,level) data, 2 for two-dimensional (lon, lat) data.
              \param starts a 3- or 2-element array of starting-indices into the data variable. 
                     For 3D data, the indices are, in order: time (0), level (1), latitude (2), and longitude (3).
                     For 3D data, the indices are: time (0), latitude (1), and longitude (2).
                     (This reflects the column-major order of storage on the remote end.)
                     Typically, all elements are zero.
              \param counts a 3- or 2-element array of how many value are to be read from the data variable.
                     The index order is the same as for starts[].
                     Typically, the 0th elements is 0, as only one time snapshot is read,
                     and the other elements are the lengths of their respective dimensions.
              \param strides a 3- or 2-element array of how many values are to be skipped along each dimension.
                     The index order is the same as for starts[] and counts[].
                     Typically all elements are 1.        
       */
       void Source_read_data_floats( std::vector<real>&vals, int var_id, int ndims, size_t *starts, size_t *counts, ptrdiff_t *strides );

       /// reads a variable's float-format data
       /*! This method reads a data variable from an open netcdf file.
              \param vals a pointer ot the returned array of data, as an array of floats in row-major order
              \param var_id the netcdf variable id of the data to be read
              \param ndims 3 for three-dimensional (lon,lat,level) data, 2 for two-dimensional (lon, lat) data.
              \param starts a 3- or 2-element array of starting-indices into the data variable. 
                     For 3D data, the indices are, in order: time (0), level (1), latitude (2), and longitude (3).
                     For 3D data, the indices are: time (0), latitude (1), and longitude (2).
                     (This reflects the column-major order of storage on the remote end.)
                     Typically, all elements are zero.
              \param counts a 3- or 2-element array of how many value are to be read from the data variable.
                     The index order is the same as for starts[].
                     Typically, the 0th elements is 0, as only one time snapshot is read,
                     and the other elements are the lengths of their respective dimensions.
              \param strides a 3- or 2-element array of how many values are to be skipped along each dimension.
                     The index order is the same as for starts[] and counts[].
                     Typically all elements are 1.        
       */
       void Source_read_data_floats( real** vals, int var_id, int ndims, size_t *starts, size_t *counts, ptrdiff_t *strides );


       /// read just the desired 3D variable from the data source 
       /*! This method reads just the desired 3D variable from the data source.
       
           \param quantity the name of the meteorological fields to be read
           \param time the internal model time for which the data are desired
           \param grid3d a pointer to a GridLatLonField3D object into which the data are to be loaded
       
       */
       void Source_getvar(const std::string& quantity, const double time, GridLatLonField3D* grid3d );

       /// read just the desired 2D surface variable from the data source
       /*! This method reads just the desired 2D surface variable from the data source.
       
           \param quantity the name of the meteorological fields to be read
           \param time the internal model time for which the data are desired
           \param grid2d a pointer to a GridLatLonFieldSfc object into which the data are to be loaded
       
       */
       void Source_getvar(const std::string& quantity, const double time, GridLatLonFieldSfc* grid2d );


       /// read a 3D meteorological field from the data source
       /*! This method reads the desired 3D quantity from the data source.
           It sets up default attributes, handles opening and closing ULSs, and 
           calls the Source_getvar() method, 
       
           \param quantity the name of the meteorological fields to be read
           \param time the internal model time for which the data are desired
           \param grid3d a pointer to a GridLatLonField3D object into which the data are to be loaded
       
       */
       void readSource( const std::string& quantity, const std::string time, GridLatLonField3D* grid3d );

       /// read a 2D surface meteorological field from the data source
       /*! This method reads the desired 2D surface quantity from the data source.
           It sets up default attributes, handles opening and closing ULSs, and 
           calls the Source_getvar() method, 
       
           \param quantity the name of the meteorological fields to be read
           \param time the internal model time for which the data are desired
           \param grid2d a pointer to a GridLatLonFieldSfc object into which the data are to be loaded
       
       */
       void readSource( const std::string& quantity, const std::string time, GridLatLonFieldSfc* grid2d );

       /// converts a data source native time to day1900
       /*! This method converts a time in a DataSource's native system to day1900 format.
           (That is, the number of days elapsed since 1899-12-31T00)
           
            \param nativeTime the native time
            \return the day1900 time
      */      
       double Source_time_nativeTo1900( double nativeTime ) const;

       /// converts a data source native time interval to a day1900 interval
       /*! This method converts a time delta in a DataSource's native system to 
           a number of days.
           
            \param nativeDelTime the native time delta
            \return the day1900 time delta
      */      
       double Source_deltime_nativeTo1900( double nativeDelTime ) const;


       /// queries a horizontal grid for its longitude and latitude values 
       /*! 
           This method returns the set of 
           longitudes and latitudes used by an HGridSpec horizontal grid specification.

           \param qhgrid the HGridSpec object being queried
           \param lons the vector of longitudes returned
           \param lats the vector of latitudes returned       
       */
       void query_hgrid( const HGridSpec& qhgrid, std::vector<real>& lons, std::vector<real>& lats ) const; 

       /// queries a horizontal grid for its longitude and latitude values 
       /*! 
           This method returns the set of 
           longitudes and latitudes used by an HGridSpec horizontal grid specification.

           \param qhgrid the HGridSpec object being queried
           \param nlons a pointer to the number of longitudfes returned
           \param lons a pointer ot an array of longitudes returned
           \param nlats a pointer to the number of latitudfes returned
           \param lats a pointer to an array of latitudes returned       
       */
       void query_hgrid( const HGridSpec& qhgrid, int* nlons, real** lons, int* nlats, real** lats ) const;

       /// queries a vertical grid for its level values 
       /*! 
           This method returns the set of 
           vertical levels used by an VGridSpec vertical grid specification.

           \param qvgrid the VGridSpec object being queried
           \param levels the vector of vertical levels returned
           \param q a reference to a string that will hold the vertical coordinate quantity
           \param u a reference to a string that will hold the units of the vertical coordinate quantity
       */
       void query_vgrid( const VGridSpec& qvgrid, std::vector<real>& levels, std::string& q, std::string& u ) const; 

       /// queries a vertical grid for its level values 
       /*! 
           This method returns the set of 
           vertical levels used by an VGridSpec vertical grid specification.

           \param qvgrid the VGridSpec object being queried
           \param nlevs a pointer to the number of levels
           \param levels a pointer to an array of vertical levels returned
           \param q a reference to a string that will hold the vertical coordinate quantity
           \param u a reference to a string that will hold the units of the vertical coordinate quantity
       */
       void query_vgrid( const VGridSpec& qvgrid, int* nlevs, real** levels, std::string& q, std::string& u  ) const;


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
