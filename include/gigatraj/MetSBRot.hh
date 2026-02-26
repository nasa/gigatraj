

#ifndef GIGATRAJ_METSBROT_H
#define GIGATRAJ_METSBROT_H

#include "gigatraj/gigatraj.hh"
#include "gigatraj/CalGregorian.hh"
#include "gigatraj/MetData.hh"
#include "gigatraj/TropOTF.hh"

namespace gigatraj {

/*!
\ingroup MetSrc

\brief provides tilted solid-body rotation winds as fake meteorological fields

The MetSBRot class provides fake meteorological data at parcel locations
for use in testing and debiugging the gigatraj model.  The winds are solid-body
rotation winds.  (Note that the axis of wind rotation can be tilted from the
earth's axis.) Other meteorological fields are purely artificial and are *not*
constructed to be consistent with the winds.

Time is a calendarless system, simply the string encoding of the model time in days.

Meteorological fields do NOT follow the UCAR CF naming conventions, but are as follows:
 -               zonal wind = "u"
 -          meridional wind = "v"
 -            vertical wind = "w"
 -              temperature = "t"
 -    potential temperature = "theta"
 -                 pressure = "p"
 -                  density = "rho"
 -      geopotential height = "z"
 -      (pressure) altitude = "alt"
 -         net heating rate = "heating rate"
 -  local pressure tendency = "omega"

*/

class MetSBRot : public MetData {

   public:
   
      /// interprocess communications tag: "This is a list of coordinates"
      static const int PGR_TAG_COORDS = 51;
      /// interprocess communications tag: "This specifies a quantity"
      static const int PGR_TAG_QUANT = 1050;
      /// interprocess communications tag: "This specifies a time"
      static const int PGR_TAG_TIME = 1055;
      /// interprocess communications tag: "This receives data values"
      static const int PGR_TAG_RECV = 2000;
      /// interprocess communications command: "Send data values"
      static const int PGR_CMD_DATA = 50;
      /// interprocess communications command: "Send wind data values"
      static const int PGR_CMD_UVW = 60;
     
   
   
      /// Default constructor
      /*! This is the default constructor. 
      */
      MetSBRot();
      
      /// Constructor specifying windspeed
      /*! This constructor permits specifying windspeed
      
      \param windspeed the wind speed (in m/s) at the equator
      \param pg a pointer to the process group to which this processor belongs (default is NULL)
      \param met the ID of a processor in pg which is dedicated to handling met data.  If there
                 is no such processor, this should be -1. (default is -1)
      */
      MetSBRot(real windspeed, ProcessGrp* pg=NULL, int met=-1 );

      /// Constructor specifying windspeed and tilt
      /*! This constructor permits specifying windspeed and tilt
      
      \param windspeed the wind speed (in m/s) at the equator
      \param tilt the angle between the planetary axis and the axis of wind rotation, in degrees
      \param pg a pointer to the process group to which this processor belongs (default is NULL)
      \param met the ID of a processor in pg which is dedicated to handling met data.  If there
                 is no such processor, this should be -1. (default is -1)
      */
      MetSBRot(real windspeed, real tilt, ProcessGrp* pg=NULL, int met=-1 );

      /// Constructor specifying windspeed and three Euler angles for wind orientation 
      /*! This constructor permits specifying windspeed and three Euler angles for wind orientation 
      
      \param windspeed the wind speed (in m/s) at the equator
      \param alpha the first Euler angle between the planetary axis and the axis of wind rotation, in degrees
      \param beta the second Euler angle between the planetary axis and the axis of wind rotation, in degrees 
              (This corresponds to the tilt.)
      \param gamma the third Euler angle between the planetary axis and the axis of wind rotation, in degrees
      \param pg a pointer to the process group to which this processor belongs (default is NULL)
      \param met the ID of a processor in pg which is dedicated to handling met data.  If there
                 is no such processor, this should be -1. (default is -1)
      */
      MetSBRot(real windspeed, real alpha, real beta, real gamma, ProcessGrp* pg=NULL, int met=-1 );

      
      /// destructor
      /*! This is the destructor. 
      */
      ~MetSBRot();

      /// the type of object this is
      static const string iam;  

      /// return the type of MetData object this is
      /*! This method retuns a string that identifies the specific class of MetData this object is,.
      
          \return the namne of the class 
      */
      std::string id() const { return iam; };

      /// set the windspeed
      /*! This method sets the windspeed.
      
      \param windspeed the wind speed (in m/s) at the equator.
             (Note: setting the windspeed does not change the tilt of the rotation.)
      */
      void set(real windspeed);

      /// set windspeed and tilt
      /*! This method sets windspeed and tilt.
      
      \param windspeed the wind speed (in m/s) at the equator
      \param tilt the angle (in degrees) between the planetary axis and the axis of wind rotation
             (Note: setting the tilt angle resets the other two Euler angles to 0.0.)
      */
      void set(real windspeed, real tilt);

      /// sets windspeed and three Euler angles for wind orientation 
      /*! This method sets windspeed and three Euler angles for wind orientation.
      
      \param windspeed the wind speed (in m/s) at the equator
      \param alpha the first Euler angle between the planetary axis and the axis of wind rotation, in degrees
      \param beta the second Euler angle between the planetary axis and the axis of wind rotation, in degrees 
              (This corresponds to the tilt.)
      \param gamma the third Euler angle between the planetary axis and the axis of wind rotation, in degrees
      */
      void set(real windspeed, real alpha, real beta, real gamma);

      /// set the amplitude and period of the vertical component of the wind
      /*! This method sets the amplitude and period of the vertical component of the wind
      
      \param amp the amplitude of thr vertical wind, in m/s
      \param period the period of the vertical wind, in days, If less
             than or equal to 0, then the vertical wind will not vary in time. 
      */
      void setvert( real amp, real period);



      /// set the time periodicity of the wind field 
      /*! This method sets the time periodicity of the wind field. (The default is no time veriation.)
          \param period the period, in days
      */    
      void set_period( real period );




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
      string time2Cal( const double time, int format=-999  );


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
          
          \param caldate  a time as expressed in some data-specific calendar system 
          \param time the model time that corresponds to cal
      */
      void defineCal( string caldate, double time=0.0 );





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
      real get_u( double time, real lon, real lat, real z);


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
      real get_v( double time, real lon, real lat, real z);


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
      real get_w( double time, real lon, real lat, real z);



     /// returns the name of the U-wind in this met data source
     /*! This method returns the name (and optionally the units) of the zonal wind in this data source.
     
         \param units (output) if not NULL, a pointer to a string in which are returned the units of the quantity
         
         \return a string containing the name of the zonal wind
     */
     std::string u_wind( std::string *units=NULL ) const; 

     /// returns the name of the V-wind in this met data source
     /*! This method returns the name (and optionally the units) of the meridional wind in this data source.
     
         \param units (output) if not NULL, a pointer to a string in which are returned the units of the quantity
         
         \return a string containing the name of the meridional wind
     */
     std::string v_wind( std::string *units=NULL ) const; 

     /// returns the name of the w-wind in this met data source
     /*! This method returns the name (and optionally the units) of the vertical wind in this data source.
     
         \param units (output) if not NULL, a pointer to a string in which are returned the units of the quantity
         
         \return a string containing the name of the vertical wind
     */
     std::string w_wind( std::string *units=NULL ) const; 

     /// returns the name of the vertical coordinate in this met data source
     /*! This method returns the name (and optionally the units) of the vertical coordinate in this data source.
     
         \param units (output) if not NULL, a pointer to a string in which are returned the units of the quantity
         
         \return a string containing the name of the vertical coordinate
     */
     std::string vertical( std::string *units=NULL ) const; 

      /// gets the units of the vertical coordinate use in the data source
      /*! This method returns the units of the vertical coordinate that is being use in this data source.
      
         \return the name of the vertical coordinate units
      */
      std::string vunits() const;

      /// returns whether the vertical coordinate increases or decreases with altitude
      /*! The method returns whether the current vertical coordinate increases or decreases with altitude.
      
           \return +1 if the vertical coordinate increases, -1 if it decreases.
      
      */
      int vIncrease() const;


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
      void get_uvw( double time, real lon, real lat, real z, real *u, real *v, real *w);

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
      void get_uvw( double time, int n, real* lons, real* lats, real* zs
      , real *u, real *v, real *w);

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

          \return the value of the quantity at the given location and time

      */
      real getData( string quantity, double time, real lon, real lat, real z, int flags=0);

    
      ///  (parallel processing) listen for requests from met data clients 
      /*! In a parallel processing environment, this method causes a dedicated 
          met data server processor to begin listening for requests from
          the client (tracer) processors in its processor group.
          
          It returns when all the client processors have called 
          this class's signalMetDone() method.
          
          If not in a parallel processing environment, or if this is not
          a met data server processor, then this method returns immediately.

      */
      void serveMet();
     
      /// sets the calendar system used
      /*! The MetSBRot class can operate in any of several defined
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
          The MetSBRot class ignores all options.

          \param name the name of the option that is to be configured.
                      Names that are not recognized by a specific subclass are
                      silently ignored.
                      
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
          The MetSBRot class ignores all options.

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
          The MetSBRot class ignores all options.

          \param name the name of the option that is to be configured.
                      Names that are not recognized by a specific subclass are
                      silently ignored.
                      Accepted names are: "title" to ste the title of the axis, and "maxwind"
                      to set the maximum wind speed.
                      
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
          The MetSBRot class ignores all options.

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
          The MetSBRot class ignores all options.

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
          The MetSBRot class ignores all options.

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
          The MetSBRot class ignores all options.

          \param name the name of the option that is to be obtained.
                      Names that are not recognized by a specific subclass 
                      will return 0.
                      Accepted names are: "title" to ste the title of the axis, and "maxwind"
                      to set the maximum wind speed.
                      
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
          The MetSBRot class ignores all options.

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

  protected:
           
      /// set to 0 for default calendar system, 1 for Gregorian calendar
      int calsys;

      /// time offset to equivalence model time with calendar time, in days
      double basetime;

      /// copy a given object into this object
      /*! This method copies properties of a given MetaData object
          into this object.
      
          \param src the object to be copied
      */    
      void assign( const MetSBRot& src );

      /// The maximum wind speed, in m/s 
      real ws;

      /// first Euler angles giving the orientation of the wind rotation axis with respect to the planetary axis 
      real aa;
      /// second Euler angles giving the orientation of the wind rotation axis with respect to the planetary axis 
      real bb;
      /// third Euler angles giving the orientation of the wind rotation axis with respect to the planetary axis 
      real gg;
      
      /// the amplitude of the vertical wind component, in m/s 
      real vs;
      /// the frequency of the vertical wind component, in s^-1
      real fr;
      
      /// the frequency of the horizontal wind components, in days 
      real pr;

      //// US std atmos values
      /*! This method returns U.S. Standard Atmsphere values

         \param quantity the quantity to be returned
         \param vertical the vertical coordinate being used
         \param z the input altitude at which the quantity is desired
         
         \return the value of the quantity, according to the U.S. standard Atmsphere profile
      */   
      real usta76( string quantity, string vertical, real z );

  protected:
      
      /// unusad integer for testing getOption() and setOption()
      int   testInt;
      /// unusad foat for testing getOption() and setOption()
      float testFloat;
      /// unusad double for testing getOption() and setOption()
      double testDouble;
      /// unusad boolean for testing getOption() and setOption()
      bool   testBool;
      /// unusad string for testing getOption() and setOption()
      std::string testString;
      /// finds the WMO tropopause
      TropOTF wmo_trop;
           
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
