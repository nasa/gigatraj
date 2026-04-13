#ifndef PGENNETCDF_H
#define PGENNETCDF_H

#include <deque>
#include <list>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/Parcel.hh"
#include "gigatraj/ParcelGenerator.hh"
#include "gigatraj/PGenRep.hh"
#include "gigatraj/NetcdfIn.hh"


namespace gigatraj {

/*!
\ingroup parcelgenerators

\brief generates collections of parcels from a netcdf file of positions

The PGenNetcdf class generates collections of parcels whose positions
are initialized from a netcdf file. A NetcdfIn object is used to do the 
actual reading.  

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


It is perhaps worth noting that 
the main difference between this class and the NetcdfIn class is
that PGenNetcdf is a parcel generator, which creates collections
of Parcels (arrays, vectors, lists, dequeues, and Flocks)
and initializes them from a netcdf file 
The NetcdfIn class reads from a netcdf file and loads
the data into collections of Parcels that already exist.

*/

class PGenNetcdf: public ParcelGenerator {

   public:
   
      /// default constructor
      /*! This is the default constuctor for the PGenNetcdf class
      
      */
      PGenNetcdf();
      
      /// destructor
      /*! This is the destructor for the PGenNetcdf class.
      
      */
      ~PGenNetcdf();

      /// \brief Copy-constructor
      /*!
         This is the copy contructor method for the Filter_Null class.

           Note that no open netcdf files carry over to the copy.
         
         \param src the source Filter_Null object to copy from
      */
      PGenNetcdf(const PGenNetcdf& src);

      /// \brief copy assignment
      /*!
          This is the copy assignment operator for the PGenNetcdf class.
          
          Note that no open netcdf files carry over to the copy.
      */
      PGenNetcdf& operator=(const PGenNetcdf& src);


      /// copies settings from a source object to this one
      /*! 
           This method copies settings from a source PGenNetcdf object
           to this one.

           Note that no open netcdf files carry over to the copy.
           
           \param src the source PGenNetcdf object
      */     
      void assign( const PGenNetcdf& src);
      

      
      
      /// opens the file without reading the parcel data
      /*! This method opens the netcdf file and obtains such information
          as the number of parcels and the identity of the vertical coordinate.
          
          The apply() methods will close the file once the parcle information has been read.  
          
          \param file the name of the file to open
          
      */
      void open( const std::string &file="" );
      
      /// returns whether the file is open
      /*! This method returns whether the file has been opened
      
          \return true if the file has been opened, false otherwise
      */
      bool isOpen();
      
      /// read locations from a file into an array of Parcels
      /*! This method creates an array of Parcels and initializes their locations from a file.
      
         \param p the input parcel whose settings we are to copy
         \param np the number of parcels is returned in this variable
         \param file the name of the file from which parcel locations are to be read
         
         \return a Parcel pointer that points to an array of Parcel objects.
      */
      Parcel * create_array(const Parcel& p, size_t *np
                           , const std::string &file
                           ); 


      /// read locations from a file into a vector container of Parcels
      /*! This method creates a vector container of Parcels and initializes their locations from a file.
      
         \param p the input parcel whose settings we are to copy
         \param file the name of the file from which parcel locations are to be read
         
         \return a pointer to a vector of Parcel objects; the calling routine must delete this vector once it is no longer needed.
      */
       std::vector<Parcel>* create_vector(const Parcel& p
                           , const std::string &file
                           );
   



      /// read locations from a file into a list container of Parcels
      /*! This method creates a list container of Parcels and initializes their locations from a file.
            
         \param p the input parcel whose settings we are to copy
         \param file the name of the file from which parcel locations are to be read
         
         \return a pointer to a list of Parcel obejcts; the calling routine must delete this list once it is no longer needed.
      */
       std::list<Parcel>* create_list(const Parcel& p
                           , const std::string &file 
                           );
      


      /// read locations from a file into a deque container of Parcels
      /*! This method creates a deque container of Parcels and initializes their locations from a file.
      
         \param p the input parcel whose settings we are to copy
         \param file the name of the file from which parcel locations are to be read
         
         \return a pointer to a deque of Parcel objects; the calling routine must delete this deque once it is no longer needed.
      */
       std::deque<Parcel>* create_deque(const Parcel& p
                           , const std::string &file
                           );
      


      /// read locations from a file into a Flock container of Parcels
      /*! This method creates a Flock container of Parcels and initializes their locations from a file.
          This version uses an open istream and is thus suitable for use with stdin.
      
         \param p the input parcel whose settings we are to copy
         \param file the name of the file from which parcel locations are to be read
         \param pgrp a process-group object that is used for parallel processing
         \param r    the ratio of meteorological-data processors to parcel-tracing processors.
                     (For example, if r=3 then there will be one met processor for
                     every 3 parcel-tracing processors) 
         
         \return a pointer to a Flock of Parcel objects; the calling routine must delete this Flock once it is no longer needed.
      */
       Flock* create_Flock(const Parcel& p
                           , const std::string &file, ProcessGrp* pgrp=NULLPTR, int r=0
                           );
      
            

      /// read locations from a file into a Swarm pseudo-container of Parcels
      /*! This method creates a Swarm pseudo-container of Parcels and initializes their locations from a file.
          This version uses an open istream and is thus suitable for use with stdin.
      
         \param p the input parcel whose settings we are to copy
         \param file the name of the file from which parcel locations are to be read
         \param pgrp a process-group object that is used for parallel processing
         \param r    the ratio of meteorological-data processors to parcel-tracing processors.
                     (For example, if r=3 then there will be one met processor for
                     every 3 parcel-tracing processors) 
         
         \return a pointer to a Swarm of Parcel objects; the calling routine must delete this Swarm once it is no longer needed.
      */
       Swarm* create_Swarm(const Parcel& p
                           , const std::string &file, ProcessGrp* pgrp=NULLPTR, int r=0
                           );
      


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
       as the vaetical coordinate.
   
       \return a reference to a string object that contains the name of the vertical coordinate
   */
   std::string& vertical();

   
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
     
    /// returns the number of parcels
    /*! This method returns the number of parcels in the netcdf file
    
         \return the number of parcels
    */
    size_t n_parcels();
        
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
    
        
      
   
   private:
   
      /// the NetcdfIn object that actuslly does the work
      NetcdfIn ncIn;
      
      /// the PGenRep object that makes the parcels
      PGenRep pgen;
      
      int dbug;
      
      std::string fname;
      std::string vcoord;
      std::string caltime;
      double time0;
      bool atend;
      size_t n;
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
