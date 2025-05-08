#ifndef GIGATRAJ_GRIDFIELDSFC_H
#define GIGATRAJ_GRIDFIELDSFC_H

#include <string>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/GridField.hh"

namespace gigatraj {

/*! \defgroup gridsfc Surface Gridded Field
\ingroup gridfield

  \brief Holds a surface grid of meteorological data
  
  These classes define objects that hold meteorological fields
  that are defined in two (quasi-)horizontal spatial dimensions.

*/

/*!
\brief abstract class for a gridded meteorological data field
on a two-dimensional quasi-horizontal surface.

 \ingroup gridsfc

The GridFieldSfc class provides a way of holding in one object all of the
information that is needed to deal with such data.

Although the most obvious horizontal grid structure is a simple rectangular
longitude-latitude grid, other possibilities exist as well.  
For example, the two horizontal 
data grid coordinates could be map coordinates whose relation to 
longitude and latitude is non-trivial.  In that case, longitude and latitude
would each be represented by a 2D grid of map coordinates inctead of a single
vector.

See the GridLatLonGridSFC class for the simple longitude-latitude grid.

*/
class GridFieldSfc : public GridField {

   typedef std::vector<real> realvec;


   public:
   
      /// Default constructor
      /*!
         This is the default contructor for the GridFieldSfc class.
      */
      GridFieldSfc();
      
      /// Default destructor
      /*!
         This is the default destructor for the GridFieldSfc class.
      */
      virtual ~GridFieldSfc();

      /// copy constructor
      /*!
         This is the copy contructor method for the GridFieldSfc class.
      */
      GridFieldSfc(const GridFieldSfc& src);

      
      /// clears data contents 
      /*! This method clears the contents of the object, except for information
          related to the process group
      */    
      virtual void clear() = 0;

      /// returns the readiness status
      /*! This object returns the readiness status: whether the object
          contains valid data, timestamp, dimensions, etc.
      
          \return returns 0 if the object is ready to be used, non-zero otherwise
      */
      virtual int status() const = 0;


      /// returns the size of the grid's two dimensions
      /*! This method returns the size of the grid's two dimensions
          \param nx the number of x-dimension values
          \param ny the number of y-dimension values
      */
      virtual void dims( int* nx, int* ny) const = 0;


      /// returns a single data value
      /*! This method returns a single data value.
           \param i the index of the x-coordinate gridpoint
           \param j the index of the y-coordinate gridpoint
           \return the value of the [i,j]th element of the data grid
      */     
      virtual real value( int i, int j ) const = 0;
      
      /// returns a single data value
      /*! This method returns a single data value, indexed directly into the data array.
           \param indx the index directly into the data array
           \return the value of the [idx]th element of the data grid
      */     
      virtual real value( int indx ) const = 0;

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
      virtual real* values( int n, real* vals, int* indices ) const = 0;
      

      /// returns a reference to a single data value
      /*! This method returns a reference to a single data value.
           \param i the index of the x-coordinate gridpoint
           \param j the index of the y-coordinate gridpoint
           \return a reference to the [i,j]th element of the data grid
      */     
      virtual real& valueref( int i, int j ) = 0;
      
      /// operator overlay allowing for a = obj(i,j) syntax      
      /*! This operator overlay for () allows 
          GridFieldSfc objects' data values to be referenced
          using an array-like syntax. Note that the indices are in
          row-major order (i.e., Fortran order).
          
          \param i the x-coordinate index
          \param j the y-coordinate index
          \return the [i,j]th data value
      */    
      real operator()( int i, int j ) const;

      /// operator overlay allowing for obj(i,j) =a syntax      
      /*! This operator overlay for () allows 
          GridFieldSfc objects' data values to be referenced
          using an array-like syntax. Note that the indices are in
          row-major order (i.e., Fortran order).
          This version returns a reference to the data,
          so we can use this syntax on the left-hand side of assignments.
          
          \param i the x-coordinate index
          \param j the y-coordinate index
          \return a reference to the [i,j]th data value
      */    
      real& operator()( int i, int j );
      
      /// returns the name of the surface
      /*! This method returns the name of the surface represwented by this object; e.g., "tropopause").
      
          \return the name of the surface
      */    
      std::string surface() const;
      
      /// sets the name of the surface
      /*! This method sets the name of the surface represwented by this object e.e., "tropopause").
          (This can be any string, but using the cf conventions is
          strongly suggested.)
          
          \param sq the name of the surface
      */
      void set_surface( const std::string sq );    

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
      
           \param inx vector of x coordinates
           \param iny vector of y coordinates
           \param indata vector of data values, in row-major order
           \param flags bitwise flags:
      */
      virtual void load( const realvec& inx, const realvec& iny, const realvec& indata, const int flags=0  ) = 0;

      ///  loads dimensions only into this object
      /*!  This method loads dimensions only into this object.
           \param inx vector of x coordinates
           \param iny vector of y coordinates
           \param flags bitwise flags:
                   - GFL_PREFILL if the data grid is to be pre-filled with bad-data flags
                                     (the default is to leave the data undefined)
      */
      virtual void load( const realvec& inx, const realvec& iny, const int flags=0  ) = 0;

      ///  loads data only into this object
      /*!  This method loads data only into this object.
           \param indata vector of data values, in row-major order
           \param flags bitwise flags:
      */
      virtual void load( const realvec& indata, const int flags=0  ) = 0;


      /// checks whether the dimensions and time of a given GridFieldSfc object are compatible with this object.
      /*! This method checks another GridFieldSfc object for compatibility with this one.
          
          \param obj the GridFieldSfc object with which this object is to be compared
          \param flags bitwise flags:
                - METCOMPAT_STRICT if strict compatibility in all aspects is required (default)
                - METCOMPAT_HORIZ if horizontal compatibility is required
                - METCOMPAT_VERT if vertical compatibility is required
                - METCOMPAT_TIME if time compatibility is required 

          \return true if the dimensions and the times match, false otherwise
      */
      virtual bool compatible( const GridFieldSfc& obj, int flags = METCOMPAT_STRICT ) const = 0;


      /// duplicates the current object.
      /*! This method creates a duplicate of the current object.
          There are many routines that can operate on the abstract GridFieldSfc
          class, since they do not depend on the exact nature of how the data
          are gridded (see the iterators).  We do not use a factory class 
          to generate the various kinds of grids because of the great
          number of very many minor variations; instead, we rely on copying
          an existing object of a GridFieldSfc subclass (with its gridding and coordinate
          scheme), upcasting it to GridFieldSfc, and returning a pointer to that.
          
          \return a pointer to the new object. The calling routine is responsible for deleting the newly-created object.
      */
      virtual GridFieldSfc* duplicate() const = 0;

      /// returns a set of gridpoint coordinates
      /*! This method returns the longitude and latitude values
          of a given set of gridpoints
      
      
          \param n the number of gridpoint coordinates desired
           \param is n-element array of first-indices into the data array
           \param js n-element array of second-indices into the data array
           \param olons n-element array of reals to hold the longitudes
           \param olats n-element array of reals to hold the latitudes
      */
      virtual void gridcoords( int n, int* is, int* js, real* olons, real* olats, int flags=0) const =0;

      ///  returns a set of gridpoint values
      /*!  This method returns a set of gridpoint values.
      
           \param n the number of gridpoint values desired
           \param is n-element array of first-indices into the data array
           \param js n-element array of second-indices into the data array
           \param vals n-element array of reals to hold the results
           \param flags a set of bitwise flags that control the behavior of the method:
                   0x01 = ignore any multiprocessing and fetch the gridpoints locally
                   0x02 = call svr_done once the gridpoints have been obtained.
      */
      virtual void gridpoints( int n, int* is, int* js, real* vals, int flags=0) const = 0;

      /// (parallel processing) receives metadata from a central met processor
      /*! This method receives metadata from a central met processor, if
         we are dedicating processors to handling met data.
         Otherwise, does nothing.  On success, all information
         about the data 
         except for the actual data values will be present in the current
         object.
      */   
      virtual void receive_meta() = 0;
      
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
      virtual void svr_send_meta(int id) const = 0;

      /// returns a string with the specific type of object this is
      /*! This method returns the kind of object this is.
      
          \return the class name
      
      */
      virtual std::string id() const = 0;

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

      /// returns the normalized area of a grid cell, given two grid indices
      /*! This method returns the normalized (i.e., unit-sphere) horizontal area associated with a grid cell centered
                on a given gridpoint
                
          \param i the first (longitudinal) index of the gridpoint
          \param j the second (latitudinal) index of the gridpoint
          
          \return the area of the [i,j]th gridpoint
      */         
      virtual real area(int i, int j) const = 0;

      /// returns the normalized area of a grid cell, given a single grid index
      /*! This method returns the normalized (unit-sphere) horizontal area associated with a grid cell centered
                on a given gridpoint

          \param i the index for the gridpoint
          
          \return the area of the [i]th gridpoint
      */         
      virtual real area(int i) const = 0;

      /// returns a surface of horizontal grid cell areas
      /*! This method returns a surface object containing the normalized areas of each grid point cell.
          (Normalized here means that the sum of the areas over the globe is 2*pi.)
          
          \return a pointer to the new surface object.  The calling reoutine is responsible
          for deleting the new object.
      */
      virtual GridFieldSfc* areas() const = 0;




     class iterator;
     friend class iterator;
     /*!
      \brief iterator for the GridFieldSfc class.

      This iterator permits looping over each element of the grid.
      If looping is done over identical grid objects (the same child class, 
      populated with the same dimensions) simultaneously, then
      each of the objects' iterators is guaranteed to be pointing
      to the same gridpoint.
      
     */
     class iterator  {
        public:
           
           /// default constructor for the GridFieldSfc::iterator (do not use)
           /*! This is the default constructor for the GridFieldSfc::iterator.
               Note that since it is not initialized by GridFieldSfc::begin() or GridFieldSfc::end(), 
               this default constructor is useless.
           */
           iterator();
           
           /// the usual constructor for the GridFieldSfc::iterator
           /*! This is the usual constructor for the GridFieldSfc::iterator.
           
               \param grid a pointer to the GridFieldSfc object being iterated over
               \param n the index into the data vector, to which
                        this iterator will be initialized
           */
           iterator( GridFieldSfc* grid, int n=0 );
           
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
     
           /// returns the two indices of the gridpoint the interator points to
           /*! This method returns the two indices into the GridFieldSfc object's data
               of the grid point at which the iterator is pointing.
               
               \param i a pointer to the first index
               \param j a pointer to the second index
           */
           void indices( int* i, int*j ) const;

           /// assigns a value to the data element pointed to by the iterator
           /*! This method uses the iterator to assign a value to the GridFieldSfc object's data
               at the current gridpoint.
               
               \param val the value to be assigned
           */    
           void assign( real val ); 

           /// returns the area of a grid cell centered on this iteration's gridpoint
           /*! This method returns the normalized area of the horizontal grid cell centered
                     on this iteration's gridpoint.
                     (Normalized here means that the sum of the areas over the globe is 4*pi: the unit sphere.)
           */          
           real area() const;

        protected:

           /// index into the data element
           int my_index;
           /// which data vector is this an iterator for?
           GridFieldSfc* my_grid;
     
     };


     class const_iterator;
     friend class const_iterator;
     /*!
     \brief iterator for a const object of the GridFieldSfc class.

      Like the regular iterators, this iterator permits looping over each
      element of the grid.
      Unlike the regular iterator, const_iterator cannot be used to assign
      values to the data vector.
      
     */
     class const_iterator  {
        public:
           
           /// default constructor for the GridFieldSfc::const_iterator (do not use)
           /*! default constructor for the GridFieldSfc::const_iterator
               Note that since it is not initialized by GridFieldSfc::begin() or GridFieldSfc::end(), 
               this default constructor is useless.
           */
           const_iterator();
           
           /// the usual constructor for the GridFieldSfc::const_iterator
           /*! This is the usual constructor for the GridFieldSfc::const_iterator.

               \param grid a pointer to the GridFieldSfc object being iterated over
               \param n the index into the data vector, to which
                        this iterator will be initialized
           */
           const_iterator( const GridFieldSfc* grid, int n=0 );
           
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
     
           /// returns the two indices of the gridpoint the interator points to
           /*! This method returns the two indices into the GridFieldSfc object's data
               of the grid point at which the iterator is pointing.
               
               \param i a pointer to the first index
               \param j a pointer to the second index
           */
           void indices( int* i, int*j ) const;

           /// returns the area of a grid cell centered on this iteration's gridpoint
           /*! This method returns the normalized area of the horizontal grid cell centered
                     on this iteration's gridpoint.
                     (Normalized here means that the sum of the areas over the globe is 4*pi: the unit sphere.)
           */          
           real area() const;

        protected:

           /// index into the data element
           int my_index;
           /// which data vector is this an iterator for?
           const GridFieldSfc* my_grid;
     
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


      /// splits a single index into the data array into two dimensional indices
      /*! Given a simple index into the data in the GridFieldSfc object,
          this returns the two horizontal indices into the data grid that 
          correspond to the simple index.
          
          \param index the simple index
          \param i a pointer to the first horizontal index
          \param j a pointer to the second horizontal index
      */
      virtual void splitIndex( int index, int* i, int* j ) const = 0;

      /// splits single indices into the data array into two dimensional indices
      /*! Given an array of simple indices into the data in the GridFieldSfc object,
          this returns the two horizontal indices into the data grid that 
          correspond to the simple indices.
          
          \param n the number of indices to be split 
          \param index a pointer to an n-element array of simple indices
          \param i a pointer to an n-element array of the first horizontal indices
          \param j a pointer to an n-element array of the second horizontal indices
      */
      virtual void splitIndex( int n, int *index, int* i, int* j ) const = 0;
      
      /// joins three dimensional indices into a single index into the data array 
      /*! Given three dimensional indices, this routine returns a single simple index directly into 
          into the data in the GridFieldSfc object,
          
          \return the simple direct index into a data array.
          \param i the first horizontal index
          \param j the second horizontal index
      */
      virtual int joinIndex( int i, int j ) const = 0;
      
      /// joins three dimensional indices into single indices into the data array 
      /*! Given three arrays of dimensional indices, this routine returns a single 
          array of simple indices directly into a data array. 
                
          
          \param n the number of indices to be split 
          \param index a pointer to an n-element array that will hold the simple indices retutrned.
                       if NULLPTR, then a new array will be ctrated.
          \param i a pointer to an n-element array of the first horizontal indices
          \param j a pointer to an n-element array of the second horizontal indices
          \return the pointer to the index array.
      */
      virtual int* joinIndex( int n, int *index, int* i, int* j ) const = 0;

     
   protected:

      /// identifies what the vertical coordinate is for this grid
      std::string sfc;
            
      /// used by child classes for operator= overriding methods
      void assign( const GridFieldSfc& src);

      /*! Given a simple index into the data in the GridFieldSfc object,
          this returns the two horizontal indices into the data grid that 
          correspond to the simple index.
          
          \param index the simple index
          \param i a pointer to the first horizontal index
          \param j a pointer to the second horizontal index
      */
      virtual void getindices( int index, int* i, int*j ) const = 0;
      
      
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
