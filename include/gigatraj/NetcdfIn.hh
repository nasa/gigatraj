#ifndef GIGATRAJ_NETCDFIN_H
#define GIGATRAJ_NETCDFIN_H

#include <netcdf.h>
#include <stdlib.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <map>
#include <set>


#include "gigatraj/gigatraj.hh"
#include "gigatraj/ParcelInitializer.hh"
#include "gigatraj/Parcel.hh"
#include "gigatraj/Flock.hh"
#include "gigatraj/Swarm.hh"


namespace gigatraj {

/*!
\ingroup parcelinitializers

\brief loads Parcel state information from a netcdf file that follows certain conventions.

The NetcdfIn class reads information about Parcels from a netcdf4 file.
The file must have at least one dimensional variable: an integer parcel ID number, 
called "parcelID". (The information in this variable is not actually used.)
The file must also have data variables "lon" and "lat".
If must also have at least one variable with the "positive" attribute, which
denotes a vertical level. There should be one of these variables
with the attribute :"vertical _coordinate", and that attribute should
be set to "yes". That data varibale is taken as the vertical coordinate
of the Parcels.

The file may have a second dimensional variable, "time". If so, then each of 
the file's data variables is a 2D variable. By default, Parcels from the first 
timestep (not necessarily the earliest in time, but the zeroeth element of the
time array) is used.


*/

class NetcdfIn : public ParcelInitializer {


   public:
   
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
      
      /// An exception for when the input does not match the format
      class badFileConventions: public ParcelInitializer::badinit {};

      /// an exception for trying to re-specify the file after it has been opened
      class badNetcdfTooLate {};

      /// an exception for a badly-formatted format specification string
      class badNetcdfFormatSpec {};
      
   /// default constructor
   /*! this is the default constrictor for the NetcdfIn class
   
   */
   NetcdfIn();
   
   /// destructor
   /*!
       This is the destructor for the NetcdfIn class
   */
   ~NetcdfIn();

   /// \brief Copy-constructor                                        
   /*!                                                                
      This is the copy contructor method for the Filter_Null class.   

           Note that no open netcdf files carry over to the copy.
                                                                      
      \param src the source Filter_Null object to copy from           
   */                                                                 
   NetcdfIn(const NetcdfIn& src);                                                 

   /// \brief copy assignment                                         
   /*!                                                                
       This is the copy assignment operator for the NetcdfIn class.         

           Note that no open netcdf files carry over to the copy.
   */                                                                 
   NetcdfIn& operator=(const NetcdfIn& src);                                      


   /// copies settings from a source object to this one               
   /*!                                                                
        This method copies settings from a source NetcdfIn object           
        to this one.                                                  

           Note that no open netcdf files carry over to the copy.
                                                                      
        \param src the source NetcdfIn object                               
   */                                                                 
   void assign( const NetcdfIn& src);                                       
    

   /// clears the object's specifications
   /*! this method clears all of the flags for writing extra variables, as well as the list of extra meteorological variables.
   */
   void clear();
   
   /// resets the object 
   /*! This method resets all of the object's settings to those of a newly-create object,
       just after closing any open file.
   */
   void reset_all();     


   /// sets the netcdf file name
   /*! This method sets the netcdf file name.
   
       \param file the name of the netcdf file to be opened for input
   */
   void filename( std::string file );
   
   /// returns the name of the netcdf file
   /*! This method returns the name of the netcdf file.
   
       \return a reference to a string object that contains the name of the netcdf file
   */
   std::string& filename();




   /// sets the vertical quantity to use
   /*! This method sets which variable in the file to use as the vertical coordinate
   
       \param vert the name of the vertical quantity in the netcdf file. If this is the empty 
                   string (the default), then the variable with the attribute "vertical_coordinate" set
                   to "yes" will be used.
   */
   void vertical( std::string vert );
   
   /// returns the name of the vertical coordinate to be used
   /*! This method returns the name of variable in the netcdf file that is to be used
       as the vertical coordinate.
   
       \return a reference to a string object that contains the name of the vertical coordinate
   */
   std::string& vertical();



   /// open a netcdf file for input
   /*! This method opens a netcdf file for input
   
       \param file the name of the file to open. If the name begins with "-", then
              the file is set to the rest of the string, and at_end( true ) is called
              to ensure that the parcels are read form the end of the file instead 
              of the beginning.
   */
   void open( std::string file="" );    


   /// returns whether the file has been opened
   /*! This method returns whether the file has been opened
       (and is thus available for setting file characteristcis)

       \return true if the file has beenopened, false otherwise
   */
   bool isOpen();    

   /// closes the netcdf file
   /*! this method clsoes the netcdf file
   */
   void close();
   
   
   /// returns the base calendar time read from the file
   /*! This method returns the base calendar time read from the netcdf file.
       This is the time that corresponds to the model time 0.
       It is not necessarily the time at which the parcels are initialized.
       
       \return the calendar base time string
   */    
   std::string& cal();
   
   /// returns the parcel model time read from the file
   /*! This method returns the parcel time read from the netcdf file.
       This is either the beginning or the endoing time from the netcdf file.
       
       \return the time
   */    
   double time();

    /// sets which time 
    /*! This method sets whether the parcel time should be read from the end of the netcdf file.
        The default is false, so the first time in the file is used.
    */    
    void at_end( bool select );
     
    /// resturns which time 
    /*! This method returns whether the parcel time will be read from the end of the netcdf file.
    */    
    bool at_end();
     
    /// sets whether parcel flags are to be read from the file
    /*! This method sets whether Parcel flags are to be read from the file.
        The default is to read them if they are available.
        This may be called only before the file is opened.
        
        \param mode 1 if the flags are to be read, 0 if they are to be ignored, and -1 if the flags 
                    may be read if available
    */
    void readFlags( int mode );
    
    /// returns whether the parcel flags are to be read
    /*! This method returns a flag that indicates whether Parcel flags are to be read from the file. 
    
        \return mode 1 if the flags are to be read, 0 if they are to be ignored, and -1 if the flags 
                    may be read if available
    */
    int readFlags();
    
    /// sets whether parcel status is to be read from the file
    /*! This method sets whether Parcel statuses are to be read from the file.
        The default is to read them if they are available.
        This may be called only before the file is opened.
        
        \param mode 1 if the statuses are to be read, 0 if they are to be ignored, and -1 if the statuses 
                    may be read if available
    */
    void readStatus( int mode );
    
    /// returns whether the parcel status is to be read
    /*! This method returns a flag that indicates whether Parcel statuses are to be read from the file. 
    
        \return mode 1 if the statuses are to be read, 0 if they are to be ignored, and -1 if the statuses 
                    may be read if available
    */
    int readStatus();
    
    /// sets whether parcel tags are to be read from the file
    /*! This method sets whether Parcel tags are to be read from the file.
        The default is to read them if they are available.
        This may be called only before the file is opened.
        
        \param mode 1 if the tags are to be read, 0 if they are to be ignored, and -1 if the tags 
                    may be read if available
    */
    void readTag( int mode );
    
    /// returns whether the parcel tags are to be read
    /*! This method returns a flag that indicates whether Parcel tags are to be read from the file. 
    
        \return mode 1 if the tags are to be read, 0 if they are to be ignored, and -1 if the tags 
                    may be read if available
    */
    int readTag();
    
  
      /// specifies a format string for the input
      /*! This method specifies a format string that controls what variables get read from the output
          netcdf file. The idea is to be able to use the same format strings that one would use
          with the StreamRead class here, as a convenience. The same end can be accomplished by
          calling the readTag(), readStatus(), and readFlags() methods.
          
          The format string is the same as used by StreamRead, but it does behave differently.
          %-codes merely turn on and off the reading of variables, with string literals and numeric 
          precision specifiers ignored. The model time, longitude, latitude, and vertical coordinate 
          are all mandatory for gigatraj netcdf files, so they will be written out regardless of whether
          they are specified in the format string. 
          
          See the documentation for the StreamRead class's format() method for the oriignal format string
          specification. 
          
          Here, the codes are:
          
          * Any text up to a "%" character is ignored
          
          * "%%" is ignored.
          
          * "%T" is tignored
          
          * "%t" is the parcel's model time, This is effectively ignored, as the time is read regardless of
            whether this is present or not.
          
          * "%o" is the parcel's longitude. This is effectively ignored, as the longitude is read
             regardless of whether this is present or not.
             
          * "%a" is the parcel's latitude. This is effectively ignored, as the latitude is read
             regardless of whether this is present or not.
             
          * "%v" is the parcel's vertical coordinate. This is effectively ignored, as the the vertical coordinate is read
             regardless of whether this is present or not.
          
          * "%g" causes the parcel's  tag value to be read from the file in the variable "tag".
          
          * "%f" causes the parcel's flags to be read from the variable "flags".
          
          * "%s" causes the parcel's status to be read from the variable "status".
          
          * "%i" is ignored.
          
          * "%{fieldname}m" is ignored

          * "%c" is ignored.
          
          * "%x" is ignored.
          
          
          \param fmt the format string, as described above.
          
      */
      void format( std::string fmt );
      
      /// returns the current format string. 
      /*! This method returns the format string that controls which variables are read from the 
          netcdf file.
          
          If the format() method has been called to set this string, then that string is returned here.
          If not, then a format string is returned that reflects the current settings of the
          NetcdfIn object.
          
          \returns the format string
      */    
      std::string format();
      
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
          between the gigatraj internal model time 
          and the time in the netcdf file.
          
          Model time is in fractional days elapsed since some reference time.
          But it is sometimes desired to have the time in the netcdf file be
          in some other units, such as hour elapsed since a different reference time.
          
          If Tn is the netcdf variable time, Tg is the internal model time, s is the scale factor,
          and o is the offset, then
          
          \code
                Tn = Tg * s + o
                
                Tg = ( Tn - o )/s
          \endcode
      
          \param scale a pointer to a double precision scalimg factor
          \param offset a pointer to a double precision offset
      */
      void getTimeTransform( double* scale, double* offset );     
    
    
    /// returns the number of parcels
    /*! This method returns the number of parcels in the netcdf file
    
         \return the number of parcels
    */
    size_t n_parcels();
    
    /// returns the index of the next Parcel to be read from the file
    /*! This method returns the index of the next Parcel to be read
        from the netcdf file.
        
        \return the index of the next Parcel. If all the Parcels have already been read, -1 is returned.
    */
    int next_parcel();
    
    /// resets the parcel index
    /*! This method sets the next-parcel index to zero.
    
    */
    void reset();
    
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
    
    
    /// finds the vertical coordinate in the input file
    /*! This method finds the name of the variable in the input file
        that is marked as the vertical coordinate. This is useful if
        the input file was prepared for a different meteorological dataset that does
        not use the same names for quantities.
        
        The vertical coordinate is found by looking at each varibale's attributes.
        The variable that has the "vertical_coordinate" attribute set to "yes" is
        the vertical coordinate.
        
        \return a string with the name of vertical coordimate variablr
     */
     std::string findVertical();   
        
      /// Initialize a single Parcel
      /*! This method initializes a single Parcel object.
      
          \param p the Parcel object to be initialized

      */
      void apply( Parcel& p ); 

      /// Initialize an array of Parcels
      /*! This method initializes an array of Parcels.
      
          \param p the array of Parcel objects to be initialized
           
          \param n the number of Parcel objects to be initialized

      */
      void apply( Parcel * p, const int n ); 

      /// Initialize a vector of parcels
      /*! This method initializes a vector of Parcels
      
          \param p the vector of Parcel objects to be initialized
           
      */
      void apply( std::vector<Parcel>& p ); 

      /// Initialize a list of parcels
      /*! This method initializes a list of Parcels
      
          \param p the list of Parcel objects to be initialized
           
      */
      void apply( std::list<Parcel>& p ); 

      /// Initialize a deque of parcels
      /*! This method initializes a deque of Parcels
      
          \param p the deque of Parcel objects to be initialized
           
      */
      void apply( std::deque<Parcel>& p ); 


      /// Initialize a Flock of parcels
      /*! This method initializes a Flock of Parcels
      
          \param p the Flock of Parcel objects to be initialized
           
      */
      void apply( Flock& p ); 

      /// Initialize a Swarm of parcels
      /*! This method initializes a Swarm of Parcels
      
          \param p the Swarm of Parcel objects to be initialized
           
      */
      void apply( Swarm& p ); 

   private:

      // file name
      std::string fname;
   
      // vertical coordinate
      std::string vcoord;
   
      // vertical coordinate units
      std::string vunits;
  
      // factor by which to multiply vertical coordinate for get desired units
      real vfactor;
       
      // is the file open or not?
      bool is_open;
   
      // the reference time string (where 0 = model time)
      std::string time0;
   
      // the model time of the parcels being read
      double t0;
   
      // get time from beginning or end of input?
      bool end;
   
   
      // the number of parcels
      size_t np;
      
      // the index of the next parcel to be read
      size_t ip;
   
      // the number of times in the file
      size_t nt;
   
      /// index of the time to be read
      size_t it;

      /// time offset
      double to;
      /// time scale factor
      double ts;
      

      // debug setting
      int dbug;
   
      // bad-value for longitudes
      // (if the longitude is bad, then all of the Parcel info except
      // for status and flags is also bad.)
      real badlon;
      // similarly for the other components
      real badlat;
      real badvert;
      double badtag;
      // and we use the met source bad-values for interpolated met fields )
      
      // flags to turn off reading flags, status, tags
      // -1 = read if available
      //  0 = do not read
      //  1 = must read
      int do_tag;
      int do_flags;
      int do_status;
      
      // format spec string
      std::string fmtspec;
      
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
      
      /// retrieves the variable ID for a netcdf variable
      /*! This method obtains the netcdf varibale ID, given 
          the desitred variable name and a few other pieces of information.
          
          \param varname this is a reference to a string that contains the nameof the desired variable
          \param required a boolean flag, set to true if the variable must be present in the file.
                 If it is required but not present, then a badFileConventions error is thrown. 
          \param flag A reference to a string that contains the name of an attribute. If the varname
                      parameter is the empty string, and if flag is non-empty, then the variables
                      are seatched for an attribute with this name, whose value is set to "yes".
                      That variable's ID is then returned.
          
          \return the netcdf variable ID of the desired variable. If the value is -1 , then the variable
                   was not found. 
      */                               
      int get_var_id( const std::string &varname, bool required, const std::string &flag, int *vtype = NULLPTR );


      /// reads real numbers from a netcdf variable
      /*! This method reads one or more real numbers from a variable
          in the netcdf file.
          
          \param var_id the netcdf varible ID to be read from
          \param var_type the netcdf type of the variable
          \param n the number of reals to be read
          \param destination a pointer to aa eal or array of reals to hold the data read in

      */
      void rd_real( int var_id, int var_type, size_t n, real* destination ); 
      

      /// reads integer numbers from a netcdf variable
      /*! This method reads one or more integer numbers from a variable
          in the netcdf file.
          
          \param var_id the netcdf varible ID to be read from
          \param var_type the netcdf type of the variable
          \param n the number of integers to be read
          \param destination a pointer to aa eal or array of integers to hold the data read in

      */
      void rd_int( int var_id, int var_type, size_t n, int* destination ); 

     /// convert netcdf time to model time
     /*! This method converts netcdf time to model time
     
         \param t the netcdf time
         \return the model time
     */
     inline double tconv( double t )
     { 
          return ( t - to )/ts;
     }
     
     /// sets the time transforma scale and offset fomr the units of the netcdf file's time variable 
     /*! This method interprets a units string to try to extract the
         time offset and scaling factor that will allow converting netcdf time to gigatraj model time.
         
         NOTE: this method is not yet fully implemented!
         
         \param tunits a string containing the units description
     */       
     void parseTimeUnits( std::string tunits );
     
     /// findhe the vertical coordinate units
     /*! This method finds the vnits of the   vertical cooordinate, givne the
         netcdf variable Id of that coordinate..It first looks for the "units"
         attribute of the variable. and if that is not found then it makes a guess,
         based on the name of the variable.
         
         \param vid the netcdf variable ID of the vertical coordinate variable
         \return a string containing the units
     */
     std::string findVunits( int vid );
         
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
