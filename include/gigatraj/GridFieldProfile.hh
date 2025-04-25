#ifndef GIGATRAJ_GRIDFIELDPROFILE_H
#define GIGATRAJ_GRIDFIELDPROFILE_H

#include <string>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/GridField.hh"
#include "gigatraj/GridFieldDim.hh"

namespace gigatraj {

/*!
\ingroup gridfield
\brief class for a gridded meteorological data field
in a one-dimensional vertical profile


The GridFieldProfile class provides a means of holding all of the information
associarted with a vertical profile of data: not only its data values, but 
the name of the quanity, its units, etc.

*/
class GridFieldProfile : public GridField {

   typedef std::vector<real> realvec;


   /// \brief overlays the << operator for writing a GridLatLonFieldSSfc object
   /*! This method serializes the GridFieldProfile object and sends it to an ostream.
   
        \param os the output stream (ostream) object to which the serialized object is to be sent
        \param p the GridFieldProfile object to be serialized and output
   */
   friend std::ostream& operator<<( std::ostream& os, const GridFieldProfile& p);

   /// \brief overlays the >> operator for reading a GridFieldProfile object
   /*! This method reads a GridFieldProfile object's serialzied data from 
        an istream and deserializes it.
   
        \param is the input stream (istream) object from which the serialized object is to be read
        \param p the GridFieldProfile object to be input and deserialzied
   */
   friend std::istream& operator>>( std::istream& is, GridFieldProfile& p);


   public:
   
      /// Default constructor
      /*!
         This is the default contructor for the GridFieldProfile class.
      */
      GridFieldProfile();
      
      /// Default destructor
      /*!
         This is the default destructor for the GridFieldProfile class.
      */
      virtual ~GridFieldProfile();

      /// copy constructor
      /*!
         This is the copy contructor method for the GridFieldProfile class.
      */
      GridFieldProfile(const GridFieldProfile& src);

     /// copy assignment
     /*! 
         This is the copy assignment operator for the GridFieldDim class.
     */
     GridFieldProfile& operator=(const GridFieldProfile& src);

      
      /// clears data contents 
      /*! This method clears the contents of the object, except for information
          related to the process group
      */    
      void clear();
      
      /// returns the name of the vertical coordinate
      /*! This method returns the name of the vertical coordinate (that is, the name
          of the physical quantity which is being used as the vertical 
          coordinate).
          
          \return the name of the vertical coordinate
      */    
      std::string vertical() const;
      
      /// set the name of the vertical coordinate
      /*! This method sets the name of the physical quantity to be used as the vertical coordinate.
          As a side effect, this also resets 
          mksVScale and mksVOffset to 1.0 and 0.0, respectively.
          
          \param vq the name of the quantity to be used as the vertical coordinate
                    (This can be any string, but using the cf conventions is
                    strongly suggested.)
      */
      void set_vertical( const std::string vq );    

      /// returns the units of the vertical coordinate quantity
      /*! This method returns the units of the vertical coordinate quantity.
      
          \param scale a pointer to a real that will hold the scaling value to take a vertical coordinate value to MKS units
          \param offset a pointer to a real that will hold the offset value to take a vertical coordinate value to MKS units
          \return the units of the vertical coordinate
      */    
      std::string vunits(real *scale=NULLPTR, real *offset=NULLPTR) const;
      
      /// sets the units of the vertical coordinate
      /*! This method sets the units of the physical quantity to be used as the vertical 
          coordinate.  
          
          \param vu the new units (This can be any string, but using the uunits conventions
          is strongly suggested.)
          \param scale scaling factor to take the vertical quantity from the givenunits to MKS units.  Defaults to 1.0.
          \param offset scaling factor to take the vertical quantity from the givenunits to MKS units. Defaults to 0.0.
               The transformation applied is mks = given * scale + offset.
      */
      void set_vunits( const std::string vu, real scale=1.0, real offset=0.0 );


      /// returns the readiness status
      /*! This object returns the readiness status: whether the object
          contains valid data, timestamp, dimensions, etc.
      
          \return returns 0 if the object is ready to be used, non-zero otherwise
      */
      int status() const;


      /// returns the size of the grid's single dimension
      /*! This method returns the size of the grid's single dimension
          \param nx the number of values
      */
      void dims( int* nx ) const;


      /// returns a single data value
      /*! This method returns a single data value.
           \param i the index of the gridpoint
           \return the value of the ith element of the data grid
      */     
      real value( int i ) const;
      

      /// returns an array of data values
      /*! This method fills an array of data values.
      
      
           \param n the number of data values to return
           \param vals a pointer to an n-element array of real values to hold the data.
                       If NULLPTR, then a new array is created, and the calling
                       routine has the responsibility of deleting it.
           \param indices a pointer to an n-element array of indices into the grid, that locate each data gridpoint
           \return a pointer to the vals array. If the input vals is not NULLPTR, then this is the same
                   as the input vals. otherwise, it points to the newly-created array of values.                          
      */     
      real* values( int n, real* vals, int* indices ) const;
      

      /// returns a reference to a single data value
      /*! This method returns a reference to a single data value.
           \param i the index of the coordinate gridpoint
           \return a reference to the [i,j]th element of the data grid
      */     
      real& valueref( int i );
      
      /// operator overlay allowing for a = obj(i) syntax      
      /*! This operator overlay for () allows 
          GridFieldProfile objects' data values to be referenced
          using an array-like syntax. 
          
          \param i the coordinate index
          \return the ith data value
      */    
      real operator()( int i ) const;

      /// operator overlay allowing for obj(i) =a syntax      
      /*! This operator overlay for () allows 
          GridFieldProfile objects' data values to be referenced
          using an array-like syntax. 
          This version returns a reference to the data,
          so we can use this syntax on the left-hand side of assignments.
          
          \param i the coordinate index
          \return a reference to the ith data value
      */    
      real& operator()( int i );


      /// returns the vector of vertical levels
      /*! This method returns the vector of the values of the vertical levels of the data.
      
          \return a vector of the values 
      */
      std::vector<real> levels() const;

      /// sets/replaces the vector of vertical levels
      /*! This method sets or replaces the vector of the values of the vertical levels of the data.
      
          \param newvert a vector of the values. If a vector is already defined, this
                         new vector must be of the same length. The intention is
                         to swap out one vertical cooridinate for another that
                         is its exact analyticial equivalent (for example, replacing
                         pressure levels with pressure altitudes on the equivalent
                         surfaces).
      */
      void levels( const std::vector<real> &newvert );

      /// returns a single vertical level gridpoint value
      /*! This method returns a single vertical level gridpoint value, given its index.
      
          \param k index of the vertical coordinate value to be returned
          \return the value of the vertical coordinate at the given level
      */
      real level( const int k ) const;

      /// returns the two indices which straddle a given vertical level value
      /*! This method returns the two indices which straddle a given vertical level value.
      
           \param z the input vertical level value
           \param i1 returns the index that lies lower than the vertical level
           \param i2 returns the index that lies higher than the vertical level
      */     
      void zindex( real z, int* i1, int* i2 ) const;

      
      /// returns the name of the profile
      /*! This method returns the name of the profile represwented by this object ).
      
          \return the name of the profile
      */    
      std::string profile() const;
      
      /// sets the name of the profile
      /*! This method sets the name of the profile represented by this object).
          (This can be any string, but using the cf conventions is
          strongly suggested.)
          
          \param nm the name of the profile
      */
      void set_profile( const std::string nm );    

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
      void transform( const std::string unyts, real scale=1.0, real offset=0.0 ); 
      

      ///  loads data and dimensions into this object
      /*!  This method loads data and dimensions into this object.
      
           \param inx vector of coordinates
           \param indata vector of data values
           \param flags bitwise flags:
      */
      void load( const realvec& inx, const realvec& indata, const int flags=0  );

      ///  loads data only into this object
      /*!  This method loads data only into this object.
           \param indata vector of data values
           \param loadFlags bitwise flags:
      */
      void load( const realvec& indata, const int loadFlags=0  );

      ///  loads dimensions only into this object
      /*!  This method loads dimensions only into this object.
           \param inx vector of coordinates
           \param loadFlags bitwise flags:
                   - GFL_PREFILL if the data grid is to be pre-filled with bad-data flags
                                     (the default is to leave the data undefined)
      */
      void loaddim( const realvec& inx, const int loadFlags=0  );


      /// checks whether the dimensions and time of a given GridFieldProfile object are compatible with this object.
      /*! This method checks another GridFieldProfile object for compatibility with this one.
          
          \param obj the GridFieldProfile object with which this object is to be compared
          \param flags bitwise flags:
                - METCOMPAT_STRICT if strict compatibility in all aspects is required (default)
                - METCOMPAT_VERT if vertical compatibility is required
                - METCOMPAT_TIME if time compatibility is required 

          \return true if the dimensions and the times match, false otherwise
      */
      bool compatible( const GridFieldProfile& obj, int flags = METCOMPAT_STRICT ) const;


      /// checks for metadata compatibility with a given GridFieldProfile object
      /*! This method checks whether another GridFieldProfile object has the same metadata as this one.
          Metadata include the quantity name and units, and the vertical coordinate
          name and units.
          
          \param obj the GridFieldProfile object with which this object is to be compared

          \return true if the metadata are the same, false otherwise
      */
      bool match( const GridFieldProfile& obj ) const;

      /// returns the number of possible data points in the grid.
      /*! This method returns the number of possible data points in the grid.
          
          \return the total number of gridpoints, or zero
          if the grid has been specified but no data have been loaded.
          
      */
      int dataSize() const;   

      ///  returns a set of gridpoint values
      /*!  This method returns a set of gridpoint values.
      
           \param n the number of gridpoint values desired
           \param is n-element array of indices into the data array
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


      /// scale factor for converting the vertical coordinate to MKS units
      /*! This is the scale factor for converting the physical quantity's units to standard MKS units.
          It is applied using the equation:   quantityMKS = quantityCurrent * mksVScale + mksVOffset
      */
      real mksVScale;
      
      /// offset for converting the vertical coordinate to MKS units
      /*! This is the offfset for converting the vertical coordinate's units to standard MKS units.
          It is applied using the equation:   quantityMKS = quantityCurrent * mksVScale + mksVOffset
      */ 
      real mksVOffset;   

      /// (parallel processing) sets the process group and met processor 
      /*! This method sets the process group and met processor for parallel processing.
      
           \param pg a pointer to the process group being used for parallel processing
      
           \param met the ID of the processor within pg that is dedicated to handling met 
                  data (or -1 if there is none)
      */
      void setPgroup( ProcessGrp* pg, int met);


      /// takes the provided arrays of dimensional values and data values as its own
      /*! This method takes an array of dimensional values and
          an array of data values and
          makes them its own. That is, this GridFieldProfile object 
         is therafter responsible for deleting the arrays. The calling routine
         must not delete the arrays or change any of heir elements.
       
          /param n the length of the arrays
          /param vals the array of data values to be "absorbed"
          /param dvals if non-null, the array of dimensional values to be "absorbed"
            
     */    
      void absorb( int n, real* vals , real* dvals=NULLPTR);


      /// takes the provided array of dimensional values as its own
      /*! This method takes an array of dimensional values and
          makes them its own. That is, this GridFieldProfile object 
         is therafter responsible for deleting the array. The calling routine
         must not delete the array or change any of its elements.
       
          /param n the length of the array
          /param dvals the array of values to be "absorbed"
            
     */    
      void absorbZs( int n, real* dvals );




     class iterator;
     friend class iterator;
     /*!
      \brief iterator for the GridFieldProfile class.

      This iterator permits looping over each element of the grid.
      If looping is done over identical grid objects (the same child class, 
      populated with the same dimension) simultaneously, then
      each of the objects' iterators is guaranteed to be pointing
      to the same gridpoint.
      
     */
     class iterator  {
        public:
           
           /// default constructor for the GridFieldProfile::iterator (do not use)
           /*! This is the default constructor for the GridFieldProfile::iterator.
               Note that since it is not initialized by GridFieldProfile::begin() or GridFieldProfile::end(), 
               this default constructor is useless.
           */
           iterator();
           
           /// the usual constructor for the GridFieldProfile::iterator
           /*! This is the usual constructor for the GridFieldProfile::iterator.
           
               \param grid a pointer to the GridFieldProfile object being iterated over
               \param n the index into the data vector, to which
                        this iterator will be initialized
           */
           iterator( GridFieldProfile* grid, int n=0 );
           
           /// override operator *, returns a reference to the current data element
           real& operator*() const;
           
           /// override operator == ; tests two iterators for equallity
           bool operator==(const iterator& x) const;
           
           /// override operator != ; tests two iterators for inequality
           bool operator!=(const iterator& x) const;
           
           /// override operator ++ ; increments the iterator to point to the next data element
           iterator& operator++(int n);
           
           /// override operator =
           iterator& operator=(const iterator& src); 
     
           /// returns the index of the gridpoint the interator points to
           /*! This method returns the index into the GridFieldProfile object's data
               of the grid point at which the iterator is pointing.
               
               \param i a pointer to the index
           */
           void indices( int* i ) const;

           /// assigns a value to the data element pointed to by the iterator
           /*! This method uses the iterator to assign a value to the GridFieldProfile object's data
               at the current gridpoint.
               
               \param val the value to be assigned
           */    
           void assign( real val ); 


        protected:

           /// index into the data element
           int my_index;
           /// which data vector is this an iterator for?
           GridFieldProfile* my_grid;
     
     };


     class const_iterator;
     friend class const_iterator;
     /*!
     \brief iterator for a const object of the GridFieldProfile class.

      Like the regular iterators, this iterator permits looping over each
      element of the grid.
      Unlike the regular iterator, const_iterator cannot be used to assign
      values to the data vector.
      
     */
     class const_iterator  {
        public:
           
           /// default constructor for the GridFieldProfile::const_iterator (do not use)
           /*! default constructor for the GridFieldProfile::const_iterator
               Note that since it is not initialized by GridFieldProfile::begin() or GridFieldProfile::end(), 
               this default constructor is useless.
           */
           const_iterator();
           
           /// the usual constructor for the GridFieldProfile::const_iterator
           /*! This is the usual constructor for the GridFieldProfile::const_iterator.

               \param grid a pointer to the GridFieldProfile object being iterated over
               \param n the index into the data vector, to which
                        this iterator will be initialized
           */
           const_iterator( const GridFieldProfile* grid, int n=0 );
           
           /// override operator *, returns the current data element
           real operator*() const;
           
           /// override operator == ; tests two const_iterators for equallity
           bool operator==(const const_iterator& x) const;
           
           /// override operator != ; tests two const_iterators for inequality
           bool operator!=(const const_iterator& x) const;
           
           /// override operator ++ ; increments the const_iterator to point to the next data element
           const_iterator& operator++(int n);
           
           /// override operator =
           const_iterator& operator=(const const_iterator& src); 
     
           /// returns the index of the gridpoint the interator points to
           /*! This method returns the index into the GridFieldProfile object's data
               of the grid point at which the iterator is pointing.
               
               \param i a pointer to the index
           */
           void indices( int* i ) const;

        protected:

           /// index into the data element
           int my_index;
           /// which data vector is this an iterator for?
           const GridFieldProfile* my_grid;
     
     };


     /// returns the beginning of the Grid
     /*! This method initializes an iterator to the beginning of a gridded field
         
         \return an iterator that points to the beginning of a grid
     */
     iterator begin();

     /// returns (just past) the end of the Grid
     /*! This method initializes an iterator to the end of a gridded field, or more
         precisely one past the last element of the grid.
         
         \return an iterator that points to the end of a grid
     */
     iterator end();

     /// returns the beginning of the Grid
     /*! This method initializes an immutable iterator to the beginning of a gridded field
         
         \return n immutable iterator that points to the beginning of a grid
     */
     const_iterator begin() const;

     /// returns (just past) the end of the Grid
     /*! This method initializes an immutable iterator to the end of a gridded field, or more
         precisely one past the last element of the grid.
         
         \return an immutable iterator that points to the end of a grid
     */
     const_iterator end() const;

     
   protected:

      /// identifies what the vertical coordinate is for this grid
      std::string prof;
      
      /// vector of coordinates
      GridFieldDim zs;
            
      /// used by child classes for operator= overriding methods
      void assign( const GridFieldProfile& src);

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
