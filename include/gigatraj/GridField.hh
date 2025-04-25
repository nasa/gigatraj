#ifndef GIGATRAJ_GRIDFIELD_H
#define GIGATRAJ_GRIDFIELD_H

#include <string>
#include <vector>
#include <map>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/ProcessGrp.hh"


namespace gigatraj {

///@name Interprocess Communications Tags
//@{
/// Interprocess Communications Tags: "Grid Request"
static const int PGR_TAG_GREQ = 1000;
/// Interprocess Communications Tags: "Transfer metadata"
static const int PGR_TAG_GMETA = 1005;
/// Interprocess Communications Tags: "Transfer dimensional values"
static const int PGR_TAG_GDIMS = 1010;
/// Interprocess Communications Tags: "How many data points to be transferred"
static const int PGR_TAG_GNUM = 1015;
/// Interprocess Communications Tags: "Transfer data point coordinates"
static const int PGR_TAG_GCOORDS = 1020;
/// Interprocess Communications Tags: "Transfer data point values"
static const int PGR_TAG_GVALS = 1025;
//@}

///@name Interprocess Communications Commands
//@{
/// Interprocess Communications Commands: "Done making met requests for now"
static const int PGR_CMD_GDONE = 2000;
/// Interprocess Communications Commands: "Transfer metadata"
static const int PGR_CMD_GMETA = 2005;
/// Interprocess Communications Commands: "Transfer data"
static const int PGR_CMD_GDATA = 2010;
//@}


/*! @name Meteorological Grid Compatibility Flags
    These METCOMPAT_* flags determine which aspects of two meteorological grids
    (as held within GridField objects) must match in order
    to be considered compatible.
*/
/// strict compatibility in all respects between meteorological fields
const int METCOMPAT_STRICT = 0x07;
/// compatibility in horizontal dimensions between meteorological fields
const int METCOMPAT_HORIZ  = 0x01;
/// compatibility in vertical dimensions between meteorological fields
const int METCOMPAT_VERT   = 0x02;
/// compatibility in valid-at between meteorological fields
const int METCOMPAT_TIME   = 0x04;


/*! @name Meteorological Grid Field Load Flags
    These GFL*_* flags are used to tell the
    GridField object about the data as it is being
    loaded.
*/
/// grid longitudes wrap around (i.e., a global grid)
const int GFL_WRAP       = 0x01;
/// grid longitudes do not wrap around 
const int GFL_NOWRAP     = 0x02;
/// pre-load grid data with fill-values
const int GFL_PREFILL    = 0x04;

/*! @name GridField Specification Flags
    These GFS_* flags specify certain aspects of how 
    a GridField object will behave. 
    
*/
/// GridField flag: the grid has no data
const int GFS_NODATA = 0x0001;
/// GridField flag: the grid is not to be cached
const int GFS_NOCACHE = 0x0002;
/// GridField flag: the grid has no gridpoint coordinates defined
const int GFS_NODIMS = 0x0008;
/// GridField flag: the number of gridpoints does not match what is expected from the coordinate definitions
const int GFS_GRIDERR = 0x0010;
/// GridField flag: missing quantity or units
const int GFS_NOQUANT = 0x0020;
/// GridField flag: invalid calendar time
const int GFS_NOTIME = 0x0040;


/*! \defgroup gridfield Gridded Field Data 
\ingroup MetDat

    \brief Holds gridded field data from a data source
 
    These classes define objecta that hold meteroroloigical fields that are
    defined on a spatial grid of some sort.

*/


/*!
\ingroup gridfield

\brief abstract class for a gridded meteorological field 

The MetGridData class is an abstract class.
An object of this class provides a two- or three-dimensional grid 
of meteorological data which is valid for a given time.
There are no presuppositions made about the kind of grid the data are on.
It might possibly be irregular, or on some sort of triangular 
mesh rather than a strict rectangular grid. The GridField class
provides that which is common to all conceivable gridded data fields.

There are two child classes of this class, both of them abstract as well:
GridFieldSfc holds two-dimensional data on quasi-horizontal surfaces, such
as tropopause temperatures.
GridField3D holds three-dimensional data, in which the third dimension is
vertical and is independent of the two horizontal dimensions; think of it
as a stack of GridFieldSfc objects.

*/
class GridField {

   public:
      
      /// An exception for input grids incompatibility
      class badgrid {};

      /// An exception for failed to load data
      class baddataload {};
      
      /// An exception for tried to access nonexistent data
      class baddatareq {};
      
      /// An exception for bad indexing 
      class baddataindex {};

      /// An exception for memory allocation problem
      class badmemreq {};
      
      /// An exception for bad processor selection
      class badProcReq {};

      /// An exception for bad coordinate set
      class badcoords {};
      
      /// An exception for incompatible coordinates
      class badincompatcoords {};
      
      /// An exception for no grid/dimensional specifications loaded
      class badnodims {};
      
      /// An exception for no data loaded
      class badnodata {};
      
      /// An exception for missing attribute
      class badMissingAttr {};

      /// An exception for mismatched grid dimensions
      class badDimensionMismatch {};

      /// An exception for attempting an invalid operation
      class badInvalidOp {};
      
      /// An exception for non-monotonic dimension values
      class badNonmonotonic {};

      
      /// Default constructor
      /*!
         This is the default contructor for the GridField class.
      */
      GridField();
      
      /// Default destructor
      /*!
         This is the default destructor for the GridField class.
      */
      virtual ~GridField();
      
      /// copy constructor
      /*!
         This is the copy contructor method for the GridField class.
      */
      GridField(const GridField& src);

      /* Note: There is no copy assignment operator for this pure cirtual class.
               Instantiatable subclasses will have a copy assignment operator
               that calls the assign() method, which will next properly
               up through its parent classes. */


      /// returns the valid-at model time of the met data
      /*! This method retrusn the valid-at time of the data the object holds.
          Note that this is the internal model time, not a real-world calendar
          date+time.
          
          \return the data time 
      */
      virtual double time() const 
      {
         return mtime;
      };
      
      /// returns the valid-at calendar string time of the met data
      /*! This method returns the valid-at time of the datathe object holds.
          Note that this is a string reflecting the real-world date_time
          according to the cinvetions use dby the data source.
      
          \return the valid-at timestamp of the data
      */    
      virtual std::string met_time()  const
      {
         return ctime;
      };

      /// sets the time and calendar
      /*! This method sets the model time and calendar valid-at time of the data.
      
         The meteorological data are valid at a particular time.
         The gigatraj model uses an internal time which is a double precision
         floating-point number.  The meteorological data sources
         will use some string label to denote the valid-at time; this
         is usually an ISO-format date+time string in the Gregorian
         calendar.  The mapping between model time and meteorological
         calendar time is arbitrary and is handled elsewhere.
         (See any of hte subclasses of the MetGridData class.)
         Because the GridField class knows nothing about how to
         map between the two kinds of time, setting the time
         here requires that both forms of the itme be specified
         as parameters.
         
         \param tyme the internal model time
         \param caltag the real-world calendar-oriented date+time stamp
      
      */     
      virtual void set_time( const double tyme, const std::string caltag )
      {
          mtime = tyme;
          ctime = caltag;
      } 
      
      
      /// returns the fill-value;
      /*! This method returns the fill value (i.e., the in-band value that
          indicates a bad or missing data value).
      
          \return the fill value
      */    
      virtual real fillval() const 
      {
         return fill_value;
      };
      
      /// sets the fill-value
      /*! This method sets the fill value (i.e., the in-band value that
          indicates a bad or missing data value).
          
          \param val the fill value
      */
      virtual void set_fillval(const real val);
      
      
      /// returns the name of the quantity
      /*! This method returns the name of the physical quantity whose values are held
          by this object.
          
          \return the quantity name
      */    
      std::string quantity() const
      {
         return quant;
      }   

      /// sets the quantity name
      /*! This method sets the quantity name.  As a side effect, it also resets 
          the MKS-transform variables mksScale and mksOffset to 1.0 and 0.0, respectively.
          
          \param quantname the name of the quantity.
                  This may be any string, but following the cf conventions
                  is strongly suggested.
      */     
      void set_quantity(const std::string quantname )
      {
         quant = quantname;
         mksScale = 1.0;
         mksOffset = 0.0;
      }
      
      /// returns the units of the quantity
      /*! This method returns the units of the physical quantity whose values are held by this object.
      
          \param scale a pointer to a scale factor that takes the values into MKs units
          \param offset a pointer to an offset that takes the values into MKS units
          \return the quantity's units 
      */
      std::string units( real* scale=NULLPTR, real* offset=NULLPTR ) const
      {
         if ( scale != NULLPTR ) {
            *scale = mksScale;
         }
         if ( offset != NULLPTR ) {
            *offset = mksOffset;
         }
         return uu;
      } 
      
      /// sets the units of the quantity
      /*! This method sets the units of the physical quantity.  
           As a side effect, this also resets the MKS-transform variables
          mksScale and mksOffset to 1.0 and 0.0, respectively.
          
          \param unts the units of the quantity.
                 This may be any string, but following the uunits conventions
                 is strongly suggested.
          
          \param scale optional scale factor that would take data in the given units to MKS units (defaults to 1.0)
          
          \param offset optional offset that would take data in the given units to MKS units (defaults to 1.0)
                 The transformation to go from the given units to MKS units is mks = given * scale + offset
      */     
      void set_units(const std::string unts, real scale=1.0, real offset=0.0)
      {
         uu = unts;
         mksScale = scale;
         mksOffset = offset;
      }
      
      /// applies a linear transformation to the data values
      /*!  This method applies a linear transformation to the data in this object.
           This is not the same as the units() method, which sets the units
           and optionally the scale factor and offset to take the data into MKS units,
           but leaves any data in the object unchanged.  In contrast,
           the transform() method actually applies the scaling factor and offset to
           the data and to the MKS scale and offset as well.
      
           \param unyts the new units that the data will have after the transformation
      
           \param scale a scaling factor to be applied to the data
      
           \param offset an offset to be applied to the data.
      */
      virtual void transform( const std::string unyts, real scale=1.0, real offset=0.0 ) = 0; 
      
      /// Returns the number of data gridpoints
      /*! This method returns the number of possible data points in the grid.
          The actual number of data points is either this number, or zero
          if the grid has been specified but no data have been loaded.
          
          \return the number of gridoints
      */
      virtual int dataSize() const = 0;   

      /// deletes any data that have been loaded.
      /*! This method deletes/clears any data that have been loaded.
      */
      void flushData();

      /// (parallel processing) sets the process group and met processor 
      /*! This method sets the process group and met processor for parallel processing.
      
           \param pg a pointer to the process group being used for parallel processing
      
           \param met the ID of the processor within pg that is dedicated to handling met 
                  data (or -1 if there is none)
      */
      virtual void setPgroup( ProcessGrp* pg, int met);

      /// (parallel processing) syncs with other processors
      /*! In a parallel processing environment, this method
          pauses until all of the other processors have reached he
          same point.
      */    
      void sync() const;

      /// (parallel processing) syncs with other processors
      /*! In a parallel processing environment, this method
          pauses until all of the other processors have reached he
          same point.
      */    
      void sync(const std::string msg) const;

      /// (parallel processing) Initiate data access from a met data server
      /*!
          This method sends a message to the member of the process group that
          suplied meteorological data, telling it that this client needs access
          to meteorological data.
      
          If multiprocessing is being used, and if
          this processor is not dedicated to handling meteorological data for
          the other processors in this ProcessGrp, then this routine returns 1.
          Otherwise, it calls the svr_listen() method and then returns 0.
          This method can thus be used in an if-then statement
          to make it easy to work with data in the multiprocessor environment.
          
          As an example:
          \code
          
          if ( metobj.svr_start() ) {
             
             // statement that should be executed only by 
             // the tracing processors, not by the dedicated
             // met-data-handling processors
             metobj.receive_meta();
          
             // tells the met-data-handling processor that this client 
             // is done making requests, for now
             metobj.svr_done();

          }
          
          \endcode
      */
      int svr_start() const;

      /// (parallel processing) Finish data access from a met data server
      /*! This method tells the dedicated met server that this processor is finished
         making requests for now.
      */   
      void svr_done() const;
      
      
      /// (parallel processing) listen for met data cient requests   
      /*! This method is what a dedicated met data processor
          uses to act as a met data server for the other processors in
          its group.  
         
          It causes the processor to listen for requests
          for metadata and for gridpoint values 
          from the other processors, until they are done.
          
          \param client The ID of the processor to listen to. 
                        If -1, then the server listens to all other processors in its group.
      */
      void svr_listen( int client = -1 ) const;

      /// clears data and metadata contents 
      /*! This method clears the contents of the object, except for information
          related to the process group
      */    
      virtual void clear() = 0;

      /// clears just the data, not the metadata
      /*! This method clears the data content of the object without
          changing the metadata.
      */    
      void clearData();

      /// returns the readiness  
      /*! This object returns the readiness status: whether the object
          contains valid data, timestamp, dimensions, etc.
      
          \return returns 0 if the object is ready to be used, non-zero otherwise
      */
      virtual int status() const = 0;             

      /// (parallel processing) sends a client metadata request to a met data server
      /*! this method sends a request to a centralized met processor that is running the svr_listen()
          method, asking for metadata
         
          If no dedicated met processor is being used, this method does nothing.  

          After calling this method, call the receive_meta() method immediately.
         
      */   
      void ask_for_meta();
      
      /// (parallel processing) sends a client data request to a met data server
      /*! This method sends a request to a centralized met processor that is running the svr_listen()
          method, asking for data.
         
         If no dedicated met processor is being used, this method does nothing.  

         After calling this method, call the gridpoints() method immediately.
         
         \param local set to 0 to pretend not to be using a central met processor.
                This corresponds to the "local" parameter of the gridpoints() method.
      */   
      void ask_for_data(int local=0) const;
      

      /// (parallel processing) handles the server side of a data-points-transfer transaction
      /*! This method handles the server side of a data-points-transfer transaction, for multi-processing
          with a dedicated meteorological data processor.
          For serial processing or when there is no dedicated met processor,
          this method does nothing.
         
          \param id the ID of the processor whose request is to be satisfied
      */   
      virtual void svr_send_vals( int id ) const = 0;

      /// (parallel processing) handles the server side of the metadata-receive transaction
      /*! This method handles the server side of the metadata-receive transaction, for multi-processing
          with a dedicated meteorological data processor.
          For serial processing or when there is no dedicated met processor,
          this routine does nothing. 
      
         \param id the ID of the processor to which we are sending the metadata
         
      */      
      virtual void svr_send_meta(int id) const = 0;

      /// indicates whether this object contains valid data
      /*! This method returns a status that indciates wther the object has been
          loded with data yet.
          
          \return returns 1 if this object contains data (and not just metadata), 0 otherwise
      */    
      bool hasdata() const;

      /// returns a string with the specific type of object this is
      /*! This method returns the kind of object this is.
      
          \return "", as no object is of this abstract class alone
      
      */
      std::string id() const { return ""; };


      /// writes the current object to an ostream
      /*! This method serializes the object and writes the serialization to an ostream.

          \param os the output ostream
      */
      virtual void serialize(std::ostream& os) const = 0;

      /// reads the current object from an istream.
      /*!  This method reads a GridField serialization from an istream, and
            deserialized it into the current object.
            
          \param is the input ostream
      */
      virtual void deserialize(std::istream& is) = 0;
      

      /// scale factor for converting to MKS units
      /*! This is the scale factor for converting the physical quantity's units to standard MKS units.
          It is applied using the equation:   quantityMKS = quantityCurrent * mksScale + mksOffset
      */
      real mksScale;
      
      /// offset for converting to MKS units
      /*! This is the offfset for converting the physical quantity's units to standard MKS units.
          It is applied using the equation:   quantityMKS = quantityCurrent * mksScale + mksOffset
      */ 
      real mksOffset;   

      /// indicates whether this object's data are cacheable
      /*! This method returns a boolean that indicates wether its data
          may be cached. For example, a gridded wind field from a meteorological 
          analysis may be cached (because it is unlikely to change tomorrow), 
          but a gridded wind field from a forecast model
          output probably should not be cached (because it will most likely be replaced with
          a newer forecast soon).
      
          \return true if the data in this object can be cached, false if it should not be cached
      */
      inline bool cacheable() const {
          return ( (flags & GFS_NOCACHE) != 0 );
      };
      
      /// declares that the data in this object may be cached
      /*! This method sets the flag that indicates that this object's data may be cached.
      */
      inline void set_cacheable() {
          flags = flags | GFS_NOCACHE;
      } ;       

      /// declares that the data in this object may be not cached
      /*! This method clears the flag that indicates that this object's data may be cached.
      */
      inline void clear_cacheable() {
          flags = flags & ~GFS_NOCACHE;
      };
      
      
      /// sets a data attribute in this object
      /*! This method sets a data attribute in this object.
      
             \param key the name of the attribute to set
             \param value the value of the attribute
      */
      void set_attribute( const std::string key, const std::string value);
      
      
      /// returns a data attribute from this object
      /*! This method retrieves a data attribute from this object
      
          \param key the name of the attribute whose value is desired
          \return the value of the attribute
      */
      std::string attribute( const std::string key ) const;    


      /// returns the object's expiration time
      /*! A MetData object may hold forecast data, in which case its validity should expire
          at some point in time. 
          This method returns the time past which this object should no longer be considered valid.
      
          \return the expiration time. If 0, then the data never expires. Otherwise, the value is in Unix epoch (for ease of comparison with the output of time()). 
      */
      time_t expires() const;
      
      /// sets the object's expiration time 
      /*! A MetData object may hold forecast data, in which case its validity should expire
          at some point in time. 
          This method sets the time past which this object should no longer be considered valid.
      
          \param exptime If 0, then the data never expires. Otherwise, the value is in Unix epoch (for ease of comparison with the output of time()).
      */
      void set_expires( time_t exptime );    

      /// returns the data as a single vector (in row-major order)
      std::vector<real> dump() const;

   protected:

      /// identifies what field this object holds
      std::string quant;

      /// hold the units of the field this object holds
      std::string uu;

      /// time of these data
      std::string ctime;
      
      /// internal model time equivalent of 'time'
      double mtime;
      
      /// expiration time
      time_t expiration;
      
      /// bad-or-missing data flag
      real fill_value;
      
      /// process group (NULL if not using)
      ProcessGrp* pgroup;
      
      /// ID of met processor in ProcessGrp pgroup
      int metproc;
      
      /// Hash of metadata
      unsigned long int metaID;
      
      /*! flags:
            - GFS_NODATA this object is not expected to contain actual data
            - GFS_NOCACHE this object is not to be cached
      */      
      int flags;
      
      
      /// holds attributes
      std::map<std::string, std::string> attrs;
      

      /// clear the has-no-data condition
      inline void clear_nodata() {
         flags = flags & ~GFS_NODATA;
      }
      
      /// set the has-no-data condition
      inline void set_nodata() {
         flags = flags | GFS_NODATA;
      }
             
      /// vector containing the gridded data values (in row-major order)
      std::vector<real> data;
      
      /// flag to use arrays instead of vectors
      bool use_array;
      /// number of data 
      int nd;
      /// data array
      real* dater;

      /// used by child classes for operator= overriding methods
      void assign( const GridField& src);

      /// checks longitudes 
      void checkLons( const std::vector<real> lons ) const; 
      
      /// checks latitudes
      void checkLats( const std::vector<real> lats ) const; 
      
      /// checks vertical levels
      void checkLevs( const std::vector<real> levs ) const; 
      
      
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
