#ifndef GIGATRAJ_GRIDFIELDDIMLON_H
#define GIGATRAJ_GRIDFIELDDIMLON_H

#include <string>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/GridFieldDim.hh"

namespace gigatraj {

/*!
\brief abstract class to hold information about a single dimension.

\ingroup gridfield

The GridFieldDimlon class provides a mechanism for dealing with the longitude dimension, which
for global data can wrap aound its endpouints.

*/
class GridFieldDimLon : public GridFieldDim {

   typedef std::vector<real> realvec;



   public:
   
      /// Default constructor
      /*!
         This is the default contructor for the GridFieldDimLon class.
      */
      GridFieldDimLon();
      
      /// Default destructor
      /*!
         This is the default destructor for the GridFieldDimLon class.
      */
      virtual ~GridFieldDimLon();

      /// copy constructor
      /*!
         This is the copy contructor method for the GridFieldDimLon class.
      */
      GridFieldDimLon(const GridFieldDimLon& src);

     /// copy assignment
     /*! 
         This is the copy assignment operator for the GridFieldDimLon class.
     */
     GridFieldDimLon& operator=(const GridFieldDimLon& src);
      
      /// clears contents 
      /*! This method clears the contents of the object, except for information
          related to the process group
      */    
      void clear();

     /// sets whether this dimension wraps around its endpoints
     /*! This method informs that object whether the dimension values should be
         considered to wrap around. 
         
         \param wrp true is wrapping, false otherwise
     */
     void wrapping( bool wrp );
     
     /// returns the wrapping interval
     /*! This method re the dimension values should be
         considered to wrap around. This is used for longitudes.
         
         \return wrp The value of the wrapping interval. (360.0 for longitides). 0 for non-wrapping (the default)
     */
     inline bool wrapping() const
     { return wraps; };


      /// returns the two indices which straddle a given data value
      /*! This method returns the two indices which straddle a given vdata value.
      
           \param z the input data value
           \param i1 returns the index that lies lower than the vertical level
           \param i2 returns the index that lies higher than the vertical level
      */     
      void index( real z, int* i1, int* i2 ) const;

      ///  returns a set of gridpoint values
      /*!  This method returns a set of gridpoint values.
      
           \param n the number of gridpoint values desired
           \param is n-element array of indices into the longitude array
           \param vals n-element array of reals to hold the results
           \param flags a set of bitwise flags that control the behavior of the method:
                   0x01 = ignore any multiprocessing and fetch the gridpoints locally
                   0x02 = call svr_done once the gridpoints have been obtained.
      */
      void gridpoints( int n, int* is, real* vals, int flags=0) const;

      /// (parallel processing) receives metadata from a central met processor
      /*! This method receives metadata from a central met processor, if
         we are dedicating processors to handling met data.
         Otherwise, does nothing.  On success, all information
         about the data 
         except for the actual data values will be present in the current
         object.
      */   
      void receive_meta();
      
      /// (parallel processing) handles the server side of the gridpoints transaction
      /*! This method handles the server side of the gridpoints transaction, for multi-processing
         with a dedicated meteorological data processor.
         For serial processing or when there is no dedicated met processor,
         this routine does nothing.
         
         \param id the ID of the processor whose request is to be satisfied
      */   
      void svr_send_vals( int id ) const;

      /// (parallel processing) handles the server side of the receive_metadata transaction
      /*! This method handles the server side of the  receive_metadata transaction, for multi-processing
         with a dedicated meteorological data processor.
         For serial processing or when there is no dedicated met processor,
         this routine does nothing. 
      
         \param id the ID of the processor to which we are sending the metadata
         
      */      
      void svr_send_meta(int id) const;

      /// the type of object this is
      static const string iam;

      /// returns a string with the specific type of object this is
      /*! This method returns the kind of object this is.
      
          \return the class name
      
      */
      std::string id() const { return iam; };

      /// writes the current object to an ostream
      /*! This method serializes the object and writes the serialization to an ostream.

          \param os the output ostream
      */
       void serialize(std::ostream& os) const;

      /// reads the current object from an istream.
      /*!  This method reads a GridField serialization from an istream, and
            deserialized it into the current object.
            
          \param is the input ostream
      */
      void deserialize(std::istream& is);




     /// wraps a given value
     /*! For dimensiona that wrap aroudn their endpoints (i.e., longitudes),
         this method takes a value and wraps it to fit within the
         range of the longitude values
         
         \param val the longitude value ot be wrapped
         /return the wrapped value
     */
     real wrap( real val ) const;
     

      /// "wrap" a longitude index 
      /*! Given an integer index into a wrapping dimension, which might or
          might not actually exist in the dimension, this method
          will "wrap" the index until is fits into the range of
          dimension indices. This is equivalent to adding and subtracting
          the wrapdelta to the longitude value itself until it lies
          within the range of values.
          
          Note that this method will throw a badindex exception if the
          longitude values do not wrap (e.g.., if a set of longitudes do not cover
          the whole globe).
         
            \param i the input  index
            
            \return the "wrapped"  index
      */
      int iwrap( int i ) const;
     
      /// sets the wrap flag based on flags or input data
      /*! This method sets the wrap flag, based on flags or the longitude data.
          
          \param loadFlags If GFL_NOWRAP is set, then wrapping is turned off.
                           If GFL_WRAP is set, then wrapping is turned on.
                           If neither is set, then the difference between the first two values
                           is compared to the difference between the first and last value, modulo wrapdelta.
          
      */
      void setWraps( const int loadFlags=0 ); 
      
     
   protected:

      
      /// true if this dimension is cirular, with the end wrapping arond
      bool wraps;
                  
      /// used by child classes for operator= overriding methods
      void assign( const GridFieldDimLon& src);
      
      
      ///  sets the coordinate direction
      /*! This method sets the coordinate direction, based on whether the coordinates increase
          or decrease with array index number.
            \param loadFlags (reserved for future expension)  
      */
      void setDir( const int loadFlags=0 ); 

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
