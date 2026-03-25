#ifndef GIGATRAJ_NETCDFOUT_H
#define GIGATRAJ_NETCDFOUT_H

#include <netcdf.h>
#include <stdlib.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <map>
#include <set>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/ParcelReporter.hh"
#include "gigatraj/MetData.hh"
#include "gigatraj/Parcel.hh"
#include "gigatraj/Flock.hh"
#include "gigatraj/Swarm.hh"


namespace gigatraj {

/*!
\ingroup parcelreporters

\brief writes Parcel information to a netcdf file

The NetcdfOut class writes parcel information to a netcdf file.

It has a counterpart for input, the NetcdfIn class.


*/

class NetcdfOut : public ParcelReporter {

      /// An exception for failing to open the netcdf file
      class badNetcdfOpen {
         public:
            /// error code
            int error;
            
            /// constructor
            badNetcdfOpen(int err) { error = err; }
            
      };

      /// An exception for some miscellaneous error while trying to read the netcdf file
      class badNetcdfError {
         public:
            /// error code
            int error;
            /// constructor
            badNetcdfError(int err) { error = err; }
            
      };

      /// an exception for trying to re-specify the file after it has been opened
      class badNetcdfTooLate {};

      /// an exception for trying to write more parcels for a given time than the file will hold, or fewer than one parcel
      class badNetcdfBadNumberParcels {};

      /// an exception for a badly-formatted format specification string
      class badNetcdfFormatSpec {};
      
   public:

      /// default constructor
      /*! this is the default constrictor for the NetcdfOut class
   
      */
      NetcdfOut();

      /// destructor
      /*! This is the destructor method for the NetcdfOut class.
      
      */
      ~NetcdfOut();

      /// sets the netcdf file name
      /*! This method sets the netcdf file name.
   
          \param file the name of the netcdf file to be opened for output
      */
      void filename( std::string file );
   
      /// returns the name of the netcdf file
      /*! This method returns the name of the netcdf file.
   
          \return a reference to a string object that contains the name of the netcdf file
      */
      std::string& filename();




      /// sets the vertical quantity to use
      /*! This method sets which variable in the file to use as the vertical coordinate
   
          \param vert the name of the vertical quantity in the netcdf file. This variable 
                      will be marked in the file with the attribute "vertical_coordinate" set
                      to "yes".
          \param units the units of the verticla coordinate quantity
          \param dir +1 if the vertical quantity increases with altitude, -1 if it decreases with altitude
      */
      void vertical( const std::string& vert, const std::string& units="UNKNOWN", int dir=0 );
   
      /// returns the name of the vertical coordinate to be used
      /*! This method returns the name of variable in the netcdf file that is to be used
          as the vertical coordinate.
   
          \return a reference to a string object that contains the name of the vertical coordinate
      */
      std::string& vertical();


      /// sets whether the Parcel status is to be written
      /*! This method determines whether the Parcel status is to be written to a "status" variable
          in the netcdf file.  The default is not to write status.
          
          If method cannotbe called after the file has beenopened.
          
          \param doit true if the status is to be written, false otherwise

      */
      void writeStatus( bool doit );
      
      /// returns whether the Parcel status is written
      /*! This method returns whether the Parcel status is being written to the netcdf file
          as the "status" variable.
          
          \return true if the status is being written, false otherwise
      */
      bool writeStatus();
      

      /// sets whether the Parcel flags are to be written
      /*! This method determines whether the Parcel flags are to be written to a "flags" variable
          in the netcdf file.  The default is not to write flags.
          
          If method cannotbe called after the file has beenopened.
          
          \param doit true if the flags is to be written, false otherwise

      */
      void writeFlags( bool doit );
      
      /// returns whether the Parcel flags are written
      /*! This method returns whether the Parcel flags is being written to the netcdf file
          as the "flags" variable.
          
          \return true if the flags is being written, false otherwise
      */
      bool writeFlags();
      

      /// sets whether the Parcel tag is to be written
      /*! This method determines whether the Parcel tag is to be written to a "tag" variable
          in the netcdf file.  The default is not to write tag.
          
          If method cannotbe called after the file has beenopened.
          
          \param doit true if the tag is to be written, false otherwise
          \param quant a string holding the name of the tag quantity
          \param units a string holding the units of the tag quantity
          \param desc a string holding a descriptive name of the tag quantity

      */
      void writeTag( bool doit, const std::string& quant="", const std::string& units="UNKNOWN", const std::string& desc="" );
      
      /// returns whether the Parcel tag is written
      /*! This method returns whether the Parcel tag is being written to the netcdf file
          as the "tag" variable.
          
          \return true if the tag is being written, false otherwise
      */
      bool writeTag( );

      /// sets whether timestamps are to be written 
      /*! This method determines whether the Parcel time is to be translated into a user-=readable timestamp string
          and written to the file in the one-dimensional "timestamp" variable..
          
          This method cannot be called after the file has been opened.
          
          \param mode true if the timestamps are to be written, false otherwise.
      */
      void writeTimestamp( bool mode );
      
      /// returns whether timestamps are to be wirtten
      /*! This method returns a flag that indicates whether Parcel time is to be translated into a user-=readable timestamp string
          and written to the file in the one-dimensional "timestamp" variable..
          
          \return true if the timestamps are to be written, false otherwise.
      */
      bool writeTimestamp();
      
      /// sets the Contents global attribute for the netcdf file
      /*! This method sets the "Contents" global attribute for the netcdf file.
          It can only be called before the file is opened.
          
          \param desc a string containing to which the Contents global attribute should be set

      */
      void contents( std::string desc );
      
      /// returns the Contents global attribute for the netcdf file
      /*!
         This method returns the Contents global attribute that will be written to the file.
         
         \return the contents global attribute string
      */
      std::string& contents();
      
      /// sets the Contact global attribute for the netcdf file
      /*! This method sets the "Contact" global attribute for the netcdf file.
          It can only be called before the file is opened.
          
          \param addr a string containing to which the Contact global attribute should be set

      */
      void contact( std::string addr );
      
      /// returns the Contact global attribute for the netcdf file
      /*!
         This method returns the Contact global attribute that will be written to the file.
         
         \return the contact global attribute string
      */
      std::string& contact();
      

      /// returns the parcel model time to be used as the base time
      /*! This method returns the parcel time to be used as th ebase time for the netcdf file.
          
          \return the time
      */    
      double time0();
   
      /// returns the base calendar time 
      /*! This method returns the base calendar time.
          This is the time that corresponds to the model time 0.
          It is not necessarily the time at which the parcels are initialized.
          
          \return the calendar base time string
      */    
      std::string& cal();


      /// set the bador-missing-data value used in the netcdf file
      /*! This method sets the value sthat is to be used for bad or missing data values.
      
          For a given time vaclue, if a parcel has been marked as not to be traced, 
          then its data for that time is record with the bad or missing data value.
          By default, this is the NaN (not a number) value. This applies to the longitude, latitude,
          vertical coordinate, tag, and any meteorological fields that have been requested.
          The parcel status and flags, however, will (if requested to be output) continue to be 
          output as-is.
          
          Once the file has bene opened, this value can no longe rbe changed.
          
          \param value the new bad-or-missing-data value
      */      
       void bad( real value );
       
       /// returns the value currently used for bad o rmissing data values
       /*! This method returns the current value used in place of bad or missing parcel data.

           \return the bad-or-missing-data value
       */
       real bad();
           

      /// returns the number of parcels being written to the file
      /*! This method returns the number of parcels being written to ahe netcdf file.
          Because the netcdffile istwo-dimensional (time x id), the number of
          parcels is fixed.
          
          \return the number of parcels
      */
      int N() { return pnum; };    

      /// adds a meteorological variable to be written out
      /*! This method adds a meteorological variable to those to be written out to the netcdf file.
          Quantities can be added only befofre a file has been opened.
      
          \param quantity the name of the quantity to be added. If the quantity is already in the list, no action will be taken.
          \param units a string holding the units of the quantity.
          \param desc a string that holds a descriptive name for the quantity
      */
      void addQuantity( const std::string& quantity, const std::string& units="UNKNOWN", const std::string& desc="" );
      
      /// removes a meteorological variable from being written out
      /*! This method deletes a meteorological variable from the list of those to be written out to the netcdf file.
          Quantities can be deleted only befofre a file has been opened.
      
          \param quantity the name of the quantity to be deleted. If the quantity is not in the list, no action will be taken.
      */
      void delQuantity( std::string quantity );

      
      /// sets the met data source
      /*! This method sets the meteorological data source to be used by the NetcdfOut object;
      
          \param metsrc a pointer to a MetData object that will b4e used as the source of the met data.
                        Note that is can be a different MetData object than is sued to trace the Parcels.
     */
     void setMet( MetData* metsrc );
     
     /// sets the met data source from a Parcel
     /*! This method sets the meteorological data source to be used by the NetcdfOut object.
         The MetData object is taken fomr the given Parcel.
         
         \param p a pointer to a Parcel object. the Parcel's meteorological data source will be used.
         
     */
     void setMet( Parcel* p );
     
     /// returns the meteorologicla data source for netcdf output                     
     /*! this method returns the meteorlogical data that is being used for output to the netcdf file.
     
         \return a pointer to a MetData object
     */
     MetData* getMet();    

      /// sets debug mode
      /*! This method sets the debugging mode
    
           \param mode the debugging mode. 0 = no diagnostic messages. 
                       The higher the mode number, the more diagnostic messages are printed to std::cerr
                        
      */
      void debug( int mode );
    
      /// returns the debug mode
      /*! This method returns the current debug mode setting
    
          \return the current debug mode setting
      */
      int debug();
    
    
      /// set the maximum number of parcels to write at a time
      /*!/ This method sets the maximum number of parcels to write to the netcdf file at one time.
           If the total number of Parcels is massive, is may be inconvenient to write them all out
           to the file at once, so NetcdfOUt will break them into sequebces that are written.
           The end result is the same as if they had been written all at once, but it can be more efficient.
           The maximum size of the sequences is set by this method. The default is 1000. 
    
           \param n the maximum number of parcels to write out at once
      */
      void maxSequence( int n );
      
      /// returns the maximum number of parcels to write at a time     
      /*! This method returns the maximum number of parcles to write out at once. Lerger
          numbers of parcels are broken into subsets which are then written to the 
          netcdf file.
      
           \return the maximum number of parcels to write out at once.
      */
      int maxSequence();
      
      /// sets the trajectory direction
      /*! This method sets whether the trajectories are running backwards or forwards in time.
          Normally, this is determined automatically when successive times are written, but
          it can be set here. If used, this must be set before openening the file.
          
          \param mode -1 for back-trajectories, +1 for forward. Other values are ignored.
      */
      void direction( int mode );
      
      /// returns the trajctory direction
      /*! This method returns the current direction of the trajectories.
      
           \return  -1 for back-trajectories, +1 for forward, 0 for undetermined
      */
      int direction();
      
      
      /// sets a linear tranformation between model time and netcdf time
      /*! This method sets a scalig factor and offset for transforming between internal
          gigatraj model time and the time used in the "time" variable of the netcdf file.
          
          Model time is in fractional days elapsed since some reference time.
          But it is sometimes desired to have the time in the netcdf file be
          in some other units, such as hour elapsed since a different reference time.
          
          If Tn is the netcdf variable time, Tg is the internal model time, s is the scale factor,
          and o is the offset, then
          
          \code
                Tn = Tg * s + o
                
                Tg = ( Tn - o )/s
          \endcode
          
          This method sets the variables "s" and "o" in these transformations.
          It not be called after the file has been opened.
      
          \param scale the time scaling factor. the default is 1.0.
          \param offset the time offset. the default is 0.0.
          
      */
      void setTimeTransform( double scale, double offset=0.0 );
      
      /// returns the linear transformation between model time and netcdf time
      /*! This method returns the scale factor and offset used to transform
          between the gigatraj internal model time (days elapsed since a reference time)
          and the time in the netcdf file, which might be in any units elaqpsed from another reference time.
          
          \param scale a pointer to a double precision scalimg factor
          \param offset a pointer to a double precision offset
      */
      void getTimeTransform( double* scale, double* offset );     
      
      /// open a netcdf file for output
      /*! This method opens a netcdf file for input. 
          Note that, ordinarily, the calling routine will not call open() directly
          but will simply starting calling apply() after calling any setup methods.
          the apply() methods will call open() if the file is not already open.
   
          \param file the name of the file to open
          
          \param p a pointer to a Parcel object that is typical of thsoe to be used.
                   If given, then the meteorological data source for the NetcdfOout object is taken from this Parcel.
          
          \param n the number of parcels to be written
                   
      */
      void open( std::string file="", Parcel* p=NULLPTR, unsigned int n=0 );    
    
    
      /// closes the netcdf file
      /*! this method clsoes the netcdf file.
      */
      void close();
      
      
      /// initialize the object
      /*! This method sets certain specifications of the object, such as the vertical coordinates.
          It cannot be called once the file has been opened.
          
          \param p a pointer to a Parcel object (whose MetData object is used)
          \param n the number of parcels

      */    
      void init( Parcel *p, unsigned int n=0);

      /// initialize the object
      /*! This method sets certain specifications of the object, such as the vertical coordinates.
          It cannot be called once the file has been opened.
          
          \param met a pointer to a MetData object
          \param n the number of parcels
      */    
      void init( MetData *met, unsigned int n=0);

  
  
      /// specifies a format string for the output
      /*! This method specifies a format string that controls what variables get written to the output
          netcdf file. The idea is to be able to use the same format strings that one would use
          with the StreamPrint class here, as a convenience. The same end can be accomplished by
          calling the writeTag(), writeStatus(), writeFlags(), and addQuantity() methods.
          
          The format string is the same as used by StreamPrint, but it does behave differently.
          %-codes merely turn on and off the writing of variables, with string literals and numeric 
          precision specifiers ignored. The model time, longitude, latitude, and vertical coordinate 
          are all mandatory for gigatraj netcdf files, so they will be written out regardless of whether
          they are specified in the format string.
          
          See the documentation for the StreamPrint class's format() method for the oriignal format string
          specification. 
          
          Here, the codes are:
          
          * Any text up to a "%" character is ignored
          
          * "%%" is ignored.
          
          * "%T" is the human-readable string equivalent of the model time, written as the "timestamp" variable.
          
          * "%t" is the parcel's model time, This is effectively ignored, as the time is output regardless of
            whether this is present or not.
          
          * "%o" is the parcel's longitude. This is effectively ignored, as the longitude is output
             regardless of whether this is presetn or not.
             
          * "%a" is the parcel's latitude. This is effectively ignored, as the latitude is output
             regardless of whether this is presetn or not.
             
          * "%v" is the parcel's vertical coordinate. This is effectively ignored, as the the vertical coordinate is output
             regardless of whether this is presetn or not.
          
          * "%g" causes the parcel's  tag value to be written to the file in the variable "tag".
          
          * "%f" causes the parcel's flags to be written to the variable "flags".
          
          * "%s" causes the parcel's status to be written to the variable "status".
          
          * "%i" is ignored.
          
          * "%{fieldname}m" interpolates the meteorological fields "fieldname" to the parcel's position and time
             and writes it to the variable with the same name as the field.

          * "%c" is ignored.
          
          * "%x" is ignored.
          
          
          \param fmt the format string, as described above.
          
      */
      void format( std::string fmt );
      
      /// returns the current format string. 
      /*! This method returns the format string that controls which variables are written to the 
          netcdf file.
          
          If the format() method has been called to set this string, then that string is returned here.
          If not, then a format string is returned that reflects the current settings of the
          NetcdfOut object.
          
          \returns the format string
      */    
      std::string format();
      
      
      /// Output a single Parcel
      /*! This method outputs a single Parcel object.
      
          \param p the Parcel object to be outputed

      */
      void apply( Parcel& p ); 

      /// Output an array of Parcels
      /*! This method outputs an array of Parcels.
      
          \param p the array of Parcel objects to be outputed
    
          \param n the number of Parcel objects to be operated on

      */
      void apply( Parcel * const p, const int n ); 


      /// output a vector of parcels
      /*! This method outputs a vector of Parcels
      
          \param p the vector of Parcel objects to be outputd
           
      */
      void apply( std::vector<Parcel>& p ); 

      /// output a list of parcels
      /*! This method outputs a list of Parcels
      
          \param p the list of Parcel objects to be outputed
           
      */
      void apply( std::list<Parcel>& p ); 

      /// output a deque of parcels
      /*! This method outputs a deque of Parcels
      
          \param p the deque of Parcel objects to be outputed
           
      */
      void apply( std::deque<Parcel>& p ); 

      /// output a Flock of parcels
      /*! This method outputs a Flock of Parcels
      
          \param p the Flock of Parcel objects to be outputed
           
      */
      void apply( Flock& p ); 


      /// output a Swarm of parcels
      /*! This method outputs a Swarm of Parcels
      
          \param p the Swarm of Parcel objects to be outputed
           
      */
      void apply( Swarm& p ); 
      
      /// set the SI unjits flag
      /*! This method sets or clears the flag that determines whether
          output quantities are written in SI units or default units.
          Note: it is an error to try to set this while the output file
          is open.
          
          \param value true if SI units are to be used, false if the default units are to be used
          
       */
       void si( bool value );
       
       /// returns the current SI units flag value
       /*! This method returns the current setting of the flag that determines whether
           output quantities are to be written in SI units.
           
           \return true if SI units are used, false otherwise
      */
      bool si() const;     
       
    
   private:
   
      /// the name of the netcdf file
      std::string fname;
      
      /// the contents string for metadata
      std::string hdr_contents;
      
      /// the contacts string for metadata
      std::string hdr_contact;
      
      /// the direction: -1 = back-trajectories, 1 = forward trajectories, 0 = undetermined 
      int dir;
      
      /// says whether the file has bene opened or not
      bool is_open;
      
      /// the vertical coordinate being used
      std::string vcoord;
      /// the vertical coordinate units
      std::string vunits;
      /// >0 if direction if upward positive, -1 if upwards negative
      int vdir;
      /// the vertical coordinate long name
      std::string vdesc;
      /// factor by which to multiply the vertical coorindaste to put it in SI units
      real vfactor;
      
      /// flag for converting output quantities to SI units before writing them out
      bool do_si;
      
      /// the base (model) time of the simulation
      double t0;
      
      /// the calendar string time that corresponds to t0
      std::string tstamp;
      
      /// the current model time
      double tyme;
      
      /// the number of parcels
      size_t pnum;
      
      // current parcel id index
      unsigned int ip;
      
      
      /// the number of times written so far
      size_t tnum;
      
      /// a list of metdata quantities to be written out
      std::vector<std::string> other;
      /// units of the "other" quantities
      std::vector<std::string> other_units;
      /// descriptive string of the "other" quantities
      std::vector<std::string> other_desc;
      /// factor by which the "other" quantities should be multiplied to get SI units
      std::vector<real> other_factor;
      
      /// should we be writing Parcel status?
      bool do_status;
      
      /// should we be writing Parcel flags?
      bool do_flags;
      
      /// should we be writing the Parcel tag value?
      bool do_tag;
      
      /// should we be writing timestamps?
      bool do_tstamp;
           
      /// the name of the tag quantity
      std::string tagquant;
      /// units of the tag quantity
      std::string tagunits;
      /// descriptive string for the tagged quantity
      std::string tagdesc;
      
      /// the format string
      std::string fmtspec;
      
      /// holds a NaN value for dead Parcels's info
      real NaN;
      /// also need a double value for this
      double dNaN;
      /// the bad-or-missing-data value
      real badval;
      /// double version of the bad-or-missing data value
      double dbadval;
      
      /// debugging flag
      int dbug;
      
      
      /// max number of parcels to write out at a time
      int maxchunk;
      
      /// Met data object
      MetData *met; 
      
      /// time offset
      double to;
      /// time scale factor
      double ts;
      
      // //// NETCDF-related variables:
      /// the netcdf handle
      int ncid;
      /// the number of variable in the netcdf file
      int nvars;
      /// the number of global attributes in the netcdf file
      int ngatts;
      
      /// dimensional ID of the time
      int did_time;
      /// dimensional ID of the parcel ID
      int did_id;

      /// var id of the time
      int vid_time;
      /// var id of the parcel id
      int vid_id;
      /// the var id of the parcel longitudes
      int vid_lon;
      /// the var id of the parcel latitudes
      int vid_lat;
      /// the var id of the parcel vertical coordinates
      int vid_z;
      /// the var id of the parcel statuses
      int vid_status;
      /// the var id of the parcel flags
      int vid_flags;
      /// the var id of the parcel tag
      int vid_tag; 
      /// the var ids of any "other" variables
      std::vector<int> vid_other;
      /// the var id of the timestamp variable
      int vid_tstamp;

      /// the var type of the parcel longitudes
      int vtyp_lon;
      /// the var type of the parcel latitudes
      int vtyp_lat;
      /// the var type of the parcel vertical coordinates
      int vtyp_z;
      /// the var type of the parcel statuses
      int vtyp_status;
      /// the var type of the parcel flags
      int vtyp_flags;
      /// the var type of the parcel tag
      int vtyp_tag; 
      /// the var type of any "other" variables
      std::vector<int> vtyp_other;
      /// the var type of the timestamp variable
      int vtyp_tstamp;
   
      /// writes out arrays of parcel data to the netcdef file
      /*! This method write out arrays of parcle information to the netcdf file.
      
          \param t the time of the parcels. (Any parcels whose time does not match this time has been marked as bad/missing data.)
          \param n the lengths of the data arrays. This may be less than the number of parcels, but never greater.
          \param lons a pointer to an array of longitude data
          \param lats a pointer to an array of latitude data
          \param zs a pointer to an array of vertical coordinate data
          \param flags either NULL or a pointer to an array of parcel flag data
          \param statuses either NULL or a pointer to an array of parcel status data
          \param tags either NULL or a pointer to an array of parcel tag data
          \param stuff either NULL or a pointer to an array of pointers, each of which point to an array of arbitratry meteorological data values
     */
     void writeout( double t, unsigned int n, real *lons, real *lats, real *zs, int *flags, int *statuses, double *tags, real **stuff );
    
     /// returns whether this process is the root processor
     /*! this method returns whether this process is the root process.
     
        \return true if this is a scalar processor or the root processor in a multiprocessing environment, false otherwise
     */
     bool is_root();
  
     /// resets the object's specifications
     /*! this method clears all of the flags for writing extra variables, as well as the list of extra meteorological variables.
     */
     void clear();
     
     
     /// convert model time to netcdf time
     /*! This method converts model time to netcdf time
     
         \param t the model time
         \return the netcdf time
     */
     inline double tconv( double t )
     { 
          return t*ts + to;
     }    
     
     
     /// get the units of the netcdf time
     /*! This method returns a string that describes the units used by
         the "time" veriable in the netcdf file.
     
         \return the time units string
     */
     std::string tunits();
     
     /// tweaks the verticla units to produce SI output
     /*! This method sets up to output the vertical coordinate in SI units,
         if the do_si flag has been set that way.
        
     */
     void tweakUnits();
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
