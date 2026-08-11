#ifndef GIGATRAJ_METGRIDSBROT_H
#define GIGATRAJ_METGRIDSBROT_H

#include <string>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/MetGridLatLonData.hh"
#include "gigatraj/MetSBRot.hh"
#include "gigatraj/TropOTF.hh"

namespace gigatraj {

/*!
\ingroup MetSrc
\brief class for a gridded version of the tilted solid-body rotation meteorological field data source

The MetGridSBRot class provides a source of meteorological data
derived from three-dimensional grids (longitude, latitude,
vertical-coordinate) of tilted solid-body rotation winds.
This class is meant for testing the gigatraj model.  

While the MetSBRot class provides meteorological field values that are 
evaluated directly from analytic functions, this class
evaluates those functions only at discrete points on a grid.
Values provided to calling routines in gigatraj are obtained
by interpolation from the grid.  In this way, we can perform
experiments to separate integration errors from interpolation errors.

In order to test the ability of the Grid-handling functions to handle
different naming conventions, this class does not use the UCAR CF conventions.
See the MetSBRot class for information on met field naming conventions
(which are not UCAR CF) and time-calendar equivalence.

Surfaces are named this way:  
 -  "sfc" = surface
 - "trop" = tropopause

*/

class MetGridSBRot : public MetGridLatLonData {

   public:


     /// Default constructor
     /*! this is the default constructor.
      
           \param latspc the spacing in degrees between latitude gridpoints
           \param lonspc the spacing between longitude gridpoints
           \param windspeed the maximum wind speed of the solid body rotation
           \param tilt the angle between the axis of the solid body rotation, 
                  and the earth's axis
      */
      MetGridSBRot( real lonspc=1.0, real latspc=1.0
      , real windspeed=20
      , real tilt=30.0
      );
//      , real windspeed=40
//      , real tilt=0.0
      
      /// alternative constructor
      /*! This is an alternative  constructor.

           \param latspc the spacing in degrees between latitude gridpoints
           \param lonspc the spacing between longitude gridpoints
           \param windspeed the maximum wind speed of the solid body rotation
           \param alpha  the first Euler angle that specifies the tilt of the rotation axis with repsect to the planet's rotation axis 
           \param beta the second Euler angle that specifies the tilt of the rotation axis with repsect to the planet's rotation axis 
           \param gamma the third Euler angle that specifies the tilt of the rotation axis with repsect to the planet's rotation axis 
      */
      MetGridSBRot( real lonspc, real latspc
      , real windspeed
      , real alpha, real beta, real gamma
      );
      
      /// Full Constructor 
      /*! This is the full constructor 
           \param quantity the desired quantity to be used for the vertical coordinate
           \param units the units of the desired vertical coordinate
           \param levels a pointer to a vector of values of the new vertical coordinate
           \param lons vector of gridpoint longitudes
           \param lats vector of gridpoint latitudes
           \param windspeed the maximum wind speed of the solid body rotation
           \param tilt the angle between the axis of the solid body rotation, 
                  and the earth's axis
                 
      */
      MetGridSBRot(
        const std::string quantity, const std::string units
      , const std::vector<real>& levels
      , const std::vector<real>& lons
      , const std::vector<real>& lats
      , real windspeed=20.0, real tilt=30.0 );  


      /// destructor
      /*! This is the destructor. 
      */
      ~MetGridSBRot();

      
      /// destructor
      /*! This is the destructor. 
      */
      MetGridSBRot( const MetGridSBRot& src );

      /// assignment operator
      MetGridSBRot& operator=(const MetGridSBRot& src);

      /// assignment operator for next level up
      MetGridLatLonData& operator=(const MetGridLatLonData& src); 

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

      /// set the amplitude and period of the vertical component of the wind
      /*! This method sets the amplitude and period of the vertical component of the wind
      
      \param amp the amplitude of the vertical wind, in m/s
      \param period the period of the vertical wind, in days, If less
             than or equal to 0, then the vertical wind will not vary in time. 
      */
      void setvert( real amp, real period)
      {
           metfcn->setvert( amp, period );
      };    


      /// set the time periodicity of the wind field 
      /*! This method sets the time periodicity of the wind field. (The default is no time veriation.)
          \param period the period, in days
      */    
      void set_period( real period )
      {
           metfcn->set_period( period );
      };    


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
      std::string time2Cal( const double time, int format=-999  );

      /// convert a date+time calendar string to model time
      /*! This method converts a date+time calendar string to internal model time
          The calendar system will depend on the specific data source (i.e., the
          child class being used).


          \param cal a time as expressed in some data-specific calendar system
          \return the internal model time that corresponds to the input string
               
      */   
      double cal2Time( const std::string cal );

      /// sets the equivalence between internal model time and a date+time calendar string
      /*! A data source will usually associate meteorological field values with
          some kind of time stamp, here expressed as a string. The trajectory mode;
          represents time as a double-precision floating-point number.
          This routine defines the equivalence between the two.
          
          \param cal  a time as expressed in some data-specific calendar system 
          \param time the model time that corresponds to cal  
      */
      void defineCal( string cal, double time=0.0 );



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
      void set_vertical( const std::string quantity, const std::string units, const std::vector<real>* levels );

      /// returns whether the vertical coordinate increases or decreases with altitude
      /*! The method returns whether the current vertical coordinate increases or decreases with altitude.
      
           \return +1 if the vertical coordinate increases, -1 if it decreases.
      
      */
      int vIncrease() const;


     /// returns the forecast lead time for the U wind 
     /*! This method returns the forecast lead time, in hours, for the U wind.

         \param time the model time for which the U wind's forecast lead time is desired.
         
         \return The forecast lead time, in hours. If zero, then the data are from an analysis or assimilation.
                 If less than zero, then the data source is a free-running model for which he concept of
                 forecast lead time does not apply. If NaN, then the information is not available
                 from this data source. 
          
     */
     inline double forecastLeadTime( double time ) {
         return -1.0;
     }

      /*! Check that a quantity name is recognized by this data source
          Returns true if recognized, false otherwise
          
       \param quantity  a string that  holds the name of the quantity   
      */
      bool legalQuantity( const std::string quantity );     


      /// return the type of MetData object this is
      /*! This method retuns a string that identifies the specific class of MetData this object is,.
      
          \return the namne of the class 
      */
      std::string id() const { return iam; };

     
      /// sets the calendar system used
      /*! The MetGridSBRot class can operate in any of several defined
          calendaring systems, to help in testing. The set_cal method
          selects which one is to be used.
          
          \param cal = 
                     - 0 for "raw" time, a text representation of the internal model time (default)
                     - 1 for the Gregorian calendar.
      */
      void set_cal( int cal );

      /// returns the calendaring system
      /*! This method returns in integer indicating which calendaring
          system is being used. This detemrines how internal model time is
          mapped to and from an test representation.
          
          \return an integer identifying the calendar system. (see the set_cal() method)
      */
      int get_cal() const;     

      /// configures the met source
      /*! This method provides a means of setting options specific
          to a subclass. In this way, objects of a subclass
          that has special capabilities may be upcast to MetData
          in a program that can use any of several met data sources,
          and yet still be able to configure those special options
          that apply to the subclass.
          The MetGridSBRot class ignores all options.

          \param name the name of the option that is to be configured.
                      Names that are not recognized by a specific subclass are
                      silently ignored.
                      Options accepted by MetGridSBRot are those accpted by MetSBRot,
                      plus those accepted by MetGridLatLonData.
                      
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
          The MetGridSBRot class ignores all options.

          \param name the name of the option that is to be configured.
                      Names that are not recognized by a specific subclass are
                      silently ignored.
                      
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
          The MetGridSBRot class ignores all options.

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
          The MetGridSBRot class ignores all options.

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
          The MetGridSBRot class ignores all options.

          \param name the name of the option that is to be obtained.
                      Names that are not recognized by a specific subclass 
                      will return an empty string.
                      
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
          The MetGridSBRot class ignores all options.

          \param name the name of the option that is to be obtained.
                      Names that are not recognized by a specific subclass 
                      will return 0.
                      
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
          The MetGridSBRot class ignores all options.

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
          The MetGridSBRot class ignores all options.

          \param name the name of the option that is to be obtained.
                      Names that are not recognized by a specific subclass 
                      will return 0.
                      
          \param value (output) the value to be obtained from the named configuration option
      
          \return true if the option was valid; false if the value returned is meaningless
      */
      bool getOption( const std::string &name, double &value );


   protected:

       /// source of the solid body rotation fields
       MetSBRot* metfcn;
       
       
       /// the type of object this is
       static const string iam;

      /// copy a given object into this object
      /*! This method copies properties of a given MetaData object
          into this object.
      
          \param src the object to be copied
      */    
       void assign( const MetGridSBRot& src );

      /// returns a default set of vertical coordinate values
      /*! This method returns a vector of vertical coordinate values, depedning onthe coordinate system desired.
      
          \param coordSys a pointer ot a string giving the name of the vertical coordinate. If NULLPTR, the 
                        meteorological source's current vertical coordinate is used.
           \return a pointer to a vector of coordinate values corresponding to the usual or default values
                        for this data souirce on the desired coordinates.              
      
      */
      std::vector<real>* vcoords( const std::string *coordSys ) const;


      /// set up for data access
      /*! Given a quantity and time, this method sets up any internal parameters that 
          may be used repeatedly during the course of data access.
          Because this function is used independently of the caching mechanism, 
          individual subclass implementations should not actually open access to 
          original data sources (files, OPeNDAP servers, etc.), since that
          would partially defeat the purpose of any caching that might be done.

           \param quantity the name of the quantity desired
           \param time the valid-at time for which data is desired
           \return  the number of dimensions of a data object (i.e, 2 or 3)
      */
      virtual int setup(  const std::string quantity, const double time );    
  
      /// set up for data access
      /*! Given a quantity and time, this method sets up any internal parameters that 
          may be used repeatedly during the course of data access.
          Because this function is used independently of the caching mechanism, 
          individual subclass implementations should not actually open access to 
          original data sources (files, OPeNDAP servers, etc.), since that
          would partially defeat the purpose of any caching that might be done.

           \param quantity the name of the quantity desired
           \param time the valid-at datestamp string for which data is desired
           \return  the number of dimensions of a data object (i.e, 2 or 3)
      */
      virtual int setup(  const std::string quantity, const std::string &time );    
 
      /// get a 3D data field valid at a certain time, using basic access
      /*! This method reads meteorological data from some source and returns
          it in a GridField3D object. 
          
          It takes the data directly from the source, with no caching or
          met data client/server interactions.
          
           \param quantity the (internal or cf-convention) name of the quantity desired
           \param time the valid-at datestamp string for which data is desired

           \return a pointer to a GridField3D object that holds the data

      */
      GridLatLonField3D* new_directGrid3D( const std::string quantity, const std::string time );

      /// get a 2D data field valid at a certain time, using basic access
      /*! This method reads meteorological data from some source and returns
          it in a GridFieldSfc object.
          
          It takes the data directly from the source, with no caching or
          met data client/server interactions.
          
           \param quantity the (internal or cf-convention) name of the quantity desired
           \param time the valid-at datestamp string for which data is desired

           \return a pointer to a GridFieldSfc object that holds the data

      */
      GridLatLonFieldSfc* new_directGridSfc( const std::string quantity, const std::string time );


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
         \return true if the bracketing times are the same; false if they are different.
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
         \return true if the bracketing times are the same; false if they are different.
      */   
      bool bracket( const std::string &quantity, const std::string &time, std::string *t1, std::string *t2);

      /// generate a fle path to a cache file for a gridded met field
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


   private:
       
       /// tropopause generator
       TropOTF tropgen;
       
      /// set the horizontal grid spacing
      /*! This method sets the horizontal grid spacing.
      
           \param latspc the spacing in degrees between latitude gridpoints
           \param lonspc the spacing between longitude gridpoints
      */
       void setgrid( real latspc=1.0, real lonspc=1.0 );

      /// set the horizontal grid
      /*! This method sets the horizontal grid by specifying its longuitudes and latitudes explicitly.
      
           \param lons vector of gridpoint longitudes
           \param lats vector of gridpoint latitudes
      */
       void setgrid( const std::vector<real>&lons, const std::vector<real>& lats );

     // sets up met field names for vert coords and their derivatoves      
     void setup_vars();
     
     // set up initial vert coord
     void setup_vGrid();

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
