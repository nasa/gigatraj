#ifndef GIGATRAJ_GRIDFIELD3D_H
#define GIGATRAJ_GRIDFIELD3D_H

#include <string>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/GridField.hh"
#include "gigatraj/GridFieldDim.hh"
#include "gigatraj/GridFieldSfc.hh"

namespace gigatraj {

/*! \defgroup grid3D Three-Dimensional Gridded Field
\ingroup gridfield

  \brief Holds a three-dimensional grid of meteorological data
  
  These classes define objects that hold meteorological fields
  that are defined in three spatial dimensions (two horizontla and one vertical).
*/

/*!
\brief abstract class for a three-dimensional gridded meteorological 
data field, in which the third dimension is a vertical coordinate
which is independent of the two horizontal coordinates.

\ingroup grid3D

The GridField3D class provides a way of holding in one object all of the
information that is needed to deal with such data.

Although the most obvious horizontal grid structure is a simple rectangular
longitude-latitude grid, other possibilities exist as well.  
For example, the two horizontal 
data grid coordinates could be map coordinates whose relation to 
longitude and latitude is non-trivial.  In that case, longitude and latitude
would each be represented by a 2D grid of map coordinates inctead of a single
vector.

See the GridLatLonGrid3D class for the simple longitude-latitude-vertical grid.

*/
class GridField3D : public GridField {

   typedef std::vector<real> realvec;

   public:
   
      /// Default constructor
      /*!
         This is the default contructor for the GridField3D class.
      */
      GridField3D();
      
      /// Default destructor
      /*!
         This is the default destructor for the GridField3D class.
      */
      virtual ~GridField3D();
      
      /// copy constructor
      /*!
         This is the copy constructor method for the GridField3D class.
      */
      GridField3D(const GridField3D& src);


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
      virtual int status() const;

      /// returns the size of the grid's three dimensions
      /*! This method returns the size of the grid's three dimensions
          \param nx the number of x-dimension values
          \param ny the number of y-dimension values
          \param nz the number of z-dimension values
      */
      virtual void dims( int* nx, int* ny, int* nz) const = 0;

      /// returns a single data value
      /*! This method returns a single data value.
           \param i the index of the x-coordinate gridpoint
           \param j the index of the y-coordinate gridpoint
           \param k the index of the z-coordinate gridpoint
           \return the value of the [i,j,k]th element of the data grid
      */     
      virtual real value( int i, int j, int k ) const = 0;

      /// returns a single data value
      /*! This method returns a single data value, indexed directly into the data array.
           \param indx the index directly into the data array
           \return the value of the indx'th element of the data grid
      */     
      virtual real value( int indx ) const = 0;
      
      /// returns a reference to a single data value
      /*! This method returns a reference to a single data value.
           \param i the index of the x-coordinate gridpoint
           \param j the index of the y-coordinate gridpoint
           \param k the index of the z-coordinate gridpoint
           \return a reference to the [i,j,k]th element of the data grid
      */     
      virtual real& valueref( int i, int j, int k ) = 0;

      /// returns an array of data values
      /*! This method fills an array of data values.
      
      
           \param n the number of data values to return
           \param vals a pointer to an n-element array of real values to hold the data.
                       If NULLPTR, then a new array is created, and the calling
                       routine has the responsibility of deleting it.
           \param indices a pointer to an n-element array of indices directly into the data grid.
           \return a pointer to the vals array. If the input vals is not NULLPTR, then this is the same
                   as the input vals. otherwise, it points to the newly-created array of values.                          
      */     
      real* values( int n, real* vals, int* indices ) const;
      

      /// operator overlay allowing for a = obj(i,j,k) syntax      
      /*! This operator overlay for () allows 
          GridField3D objects' data values to be referenced
          using an array-like syntax. Note that the indices are in
          row-major order (i.e., Fortran order).
          
          \param i the x-coordinate index
          \param j the y-coordinate index
          \param k the z-coordinate index
          \return the [i,j,k]th data value
      */    
      real operator()( int i, int j, int k ) const;

      /// operator overlay allowing for obj(i,j,k) =a syntax      
      /*! This operator overlay for () allows 
          GridField3D objects' data values to be referenced
          using an array-like syntax. Note that the indices are in
          row-major order (i.e., Fortran order).
          This version returns a reference to the data,
          so we can use this syntax on the left-hand side of assignments.
          
          \param i the x-coordinate index
          \param j the y-coordinate index
          \param k the z-coordinate index
          \return a reference to the [i,j,k]th data value
      */    
      real& operator()( int i, int j, int k );
      
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
          \param offset a pointer to a real that will hold the offset value to take a vertical cooridmate value to MKS units
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

      /// takes the provided array of vertical level values as its own
      /*! This method takes an array of vertical level values and
          makes them its own. That is, this GridLatLonField3D object 
         is therafter responsible for deleting the array. The calling routine
         must not delete the array or change any of its elements.
       
          /param n the length of the array
          /param zvals the array of vertical levels to be "absorbed"
            
     */    
      void absorbLevels( int n, real* zvals );

     /// returns a copy of the vertical dimension
     /*! This method returns a copy of the vertical dimension
        of the grid.

        /return a GridFieldDim object holding the vertical dimension
     */
     GridFieldDim getLevels() const;   

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
           \param inz vector of z coordinates
           \param indata vector of data values, in row-major order
           \param flags bitwise flags:
      */
      virtual void load( const realvec& inx, const realvec& iny, const realvec& inz, const realvec& indata, const int flags=0 ) = 0;

      ///  loads dimensions only into this object
      /*!  This method loads dimensions only into this object.
           \param inx vector of x coordinates
           \param iny vector of y coordinates
           \param inz vector of z coordinates
           \param flags bitwise flags:
                   - GFL_PREFILL if the data grid is to be pre-filled with bad-data flags
                                     (the default is to leave the data undefined)
      */
      virtual void load( const realvec& inx, const realvec& iny, const realvec& inz, const int flags=0 ) = 0;

      ///  loads data only into this object
      /*!  This method loads data only into this object.
           \param indata vector of data values, in row-major order
           \param flags bitwise flags:
      */
      virtual void load( const realvec& indata, const int flags=0 ) = 0;

      /// checks whether the dimensions and time of a given GridField3D object are compatible with this object.
      /*! This method checks another GridField3D object for compatibility with this one.
          
          \param obj the GridField3D object with which this object is to be compared
          \param flags bitwise flags:
                - METCOMPAT_STRICT if strict compatibility in all aspects is required (default)
                - METCOMPAT_HORIZ if horizontal compatibility is required
                - METCOMPAT_VERT if vertical compatibility is required
                - METCOMPAT_TIME if time compatibility is required 

          \return true if the dimensions and the times match, false otherwise
      */
      virtual bool compatible( const GridField3D&  obj, int flags = METCOMPAT_STRICT ) const = 0;

      /// checks whether the dimensions and time of a given GridFieldSfc  object are compatible with this object.
      /*! This method checks a GridFieldSfc object for compatibility with this one.
          
          \param obj the GridFieldSfc object with which this object is to be compared
          \param flags bitwise flags:
                - METCOMPAT_STRICT if strict compatibility in all aspects is required (default)
                - METCOMPAT_HORIZ if horizontal compatibility is required
                - METCOMPAT_VERT if vertical compatibility is required (guaranteed not to be compatible!)
                - METCOMPAT_TIME if time compatibility is required 
          \return true if the dimensions and the times match, false otherwise
      */
      virtual bool compatible( const GridFieldSfc& obj, int flags = METCOMPAT_STRICT ) const = 0;

      
      /// extracts a 2D surface corresponding to a vertical level.
      /*! This method extracts a 2D surface corresponding to a vertical level.

          \param k the index of the vertical level to be extracted
          
          \return a pointer to the new surface object.  The calling routine is responsible
          for deleting the new object.
      */
      virtual GridFieldSfc* extractSurface( int k ) const = 0;    

      /// replaces a vertical level with the contents of the given 2D surface.
      /*! This method replaces a vertical level with the contents of the given 2D surface.
          The physical quantity must be the same.  Units will be adjusted automatically
          by converting the surface units to MKS values, and then from MKS to 
          the values being used in the object.
          \param sfc the surface to be inserted
          \param k the level to be replaced
      */
      virtual void replaceLevel( GridFieldSfc& sfc, int k) = 0;    

      /// duplicates the current object.
      /*! This method creates a duplicate of the current object.
          There are many routines that can operate on the abstract GridField3D
          class, since they do not depend on the exact nature of how the data
          are gridded (see the iterators).  We do not use a factory class 
          to generate the various kinds of grids because of the great
          number of very many minor variations; instead, we rely on copying
          an existing object of a GridField3D subclass (with its gridding and coordinate
          scheme), upcasting it to GridField3D, and returning a pointer to that.
          
          \return a pointer to the new object. The calling routine is responsible for deleting the newly-created object.
      */
      virtual GridField3D* duplicate() const = 0;

      ///  generates a new, data-less GridField3D object whose dimensionss match this object's
      /*!  This method generates a new object of this class, whose data elements
           at each level are the vertical coordinate values at that level.
           For example, this can be used for generating an array of pressures from data
           that are on pressure surfaces.
           
           \return a pointer to the new GridField3D object. The calling rojtine is responsible for
                   deleting this object when it is no longer needed.
      */
      GridField3D* generateVertical() const;
      
      ///  loads a new set of vertical coordinate gridpoints
      /*!  This method loads a new set of vertical coordinate gridpoints, while preserving
           the horizontal grid metadata. This has the side effect of deleting
           any data that were previously loaded.

           \param newz a vector of new vertical coordinate gridpoints
      */
      void newVertical( const std::vector<real>& newz );       

      /// returns a set of gridpoint coordinates
      /*! This method returns the longitude, latitude, and vertical coordinate values
          of a given set of gridpoints
      
      
          \param n the number of gridpoint coordinates desired
           \param is n-element array of first-indices into the data array
           \param js n-element array of second-indices into the data array
           \param ks n-element array of third-indices into the data array
           \param olons n-element array of reals to hold the longitudes
           \param olats n-element array of reals to hold the latitudes
           \param olevs n-element array of reals to hold the vertical coordinates
           \param flags (reserved for future expansion)
      */
      virtual void gridcoords( int n, int* is, int* js, int* ks, real* olons, real* olats, real* olevs, int flags=0) const =0;
      

      ///  returns a set of gridpoint data values
      /*!  This method returns a set of data values at given gridpoints.

           Warning: if you are getting the values from a centralized met processors,
           you must perform some kind of handshaking with the processor 
           (perhaps by using the ask_for_data() method of the GridField class)
           before calling this method.
      
           \param n the number of gridpoint values desired
           \param is n-element array of first-indices into the data array
           \param js n-element array of second-indices into the data array
           \param ks n-element array of third-indices into the data array
           \param vals n-element array of reals to hold the results
           \param flags a set of bitwise flags that control the behavior of the method:
                   0x01 = ignore any multiprocessing and fetch the gridpoints locally
                   0x02 = call svr_done once the gridpoints have been obtained.
      */
      virtual void gridpoints( int n, int* is, int* js, int* ks, real* vals, int flags=0) const =0;

      /// (parallel processing) receives metadata from a central met processor
      /*! This method receives metadata from a central met processor, if
         we are dedicating processors to handling met data.
         Otherwise, does nothing.  On success, all information
         about the data 
         except for the actual data values will be present in the current
         object.

           Warning: if you are getting the metadata from a centralized met processors,
           you must perform some kind of handshaking with the processor 
           (perhaps by using the ask_for_meta() method of the GridField class)
           before calling this method.
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

      /// the type of object this is
      static const string iam;

      /// returns a string with the specific type of object this is
      /*! This method returns the kind of object this is.
      
          \return the class name
      
      */
      virtual std::string id() const =0;

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
                
          \param i the first index of the gridpoint
          \param j the second index of the gridpoint
          
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

      /// (parallel processing) sets the process group and met processor 
      /*! This method sets the process group and met processor for parallel processing.
      
           \param pg a pointer to the process group being used for parallel processing
      
           \param met the ID of the processor within pg that is dedicated to handling met 
                  data (or -1 if there is none)
      */
      virtual void setPgroup( ProcessGrp* pg, int met);


     class iterator;
     friend class iterator;
     /*!
      \brief iterator for the GridField3D class 

      This iterator permits looping over each element of the grid, or over
      certain subsets of elements..
      If looping is done over identical grid objects (the same child class, 
      populated with the same dimensions) simultaneously, then
      each of the objects' iterators is guaranteed to be pointing
      to the same gridpoint.
      
     */
     class iterator  {
        public:
           
           /// default constructor for the GridField3D::iterator (do not use)
           /*! This is the default constructor for the GridField3D::iterator.
               Note that since it is not initialized by GridField3D::begin() or GridField3D::end(), 
               this default constructor is useless.
           */
           iterator();
           
           /// the usual constructor for the GridField3D::iterator
           /*! This is the usual constructor for the GridField3D::iterator.
           
               \param grid a pointer to the GridField3D object being iterated over
               \param n the index into the data vector, to which
                        this iterator will be initialized
               \param beg the index of the first element of the data vector in the iteration
               \param fin the index of the last element of the data vector in the iteration
           */
           iterator( GridField3D* grid, int n=0, int beg=0, int fin=0 );
           
           /// override operator *, returns the reference to current data element
           real& operator*() const;
           
           /// override operator == ; tests two iterators for equallity
           bool operator==(const iterator& x) const;
           
           /// override operator != ; tests two iterators for inequality
           bool operator!=(const iterator& x) const;
           
           /// override operator ++ ; increments the iterator to point to the next data element
           iterator& operator++(int n);
           
           /// override operator =
           iterator& operator=(const iterator& src); 
     
           /// returns the three indices of the gridpoint the interator points to
           /*! This method returns the three indices into the GridField3D object's data
               of the grid point at which the iterator is pointing.
               
               \param i a pointer to the first horizontal index
               \param j a pointer to the second horizontal index
               \param k a pointer to the vertical coordinate index
           */
           void indices( int* i, int*j, int *k ) const;

           /// assigns a value to the data element pointed to by the iterator
           /*! This method uses the iterator to assign a value to the GridField3D object's data
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
           /// the first possible index
           int first;
           /// the last possible index
           int last;
           /// which data vector is this an iterator for?
           GridField3D* my_grid;
     
     };


     class const_iterator;
     friend class const_iterator;
     /*!
     \brief iterator for a const object of the GridField3D class. 

      Like the regular iterators, this iterator permits looping over each
      element of the grid, or over certain subets of elements.
      Unlike the regular iterator, const_iterator cannot be used to assign
      values to the data vector.
      
     */
     class const_iterator  {
        public:
           
           /// default constructor for the GridField3D::const_iterator (do not use)
           /*! default constructor for the GridField3D::const_iterator
               Note that since it is not initialized by GridField3D::begin() or GridField3D::end(), 
               this default constructor is useless.
           */
           const_iterator();
           
           /// the usual constructor for the GridField3D::const_iterator
           /*! This is the usual constructor for the GridField3D::const_iterator.

               \param grid a pointer to the GridField3D object being iterated over
               \param n the index into the data vector, to which
                        this iterator will be initialized
               \param beg the index of the first element of the data vector in the iteration
               \param fin the index of the last element of the data vector in the iteration
           */
           const_iterator( const GridField3D* grid, int n=0, int beg=0, int fin=0 );
           
           /// override operator *, returns the reference to current data element
           real operator*() const;
           
           /// override operator == ; tests two const_iterators for equallity
           bool operator==(const const_iterator& x) const;
           
           /// override operator != ; tests two const_iterators for inequality
           bool operator!=(const const_iterator& x) const;
           
           /// override operator ++ ; increments the const_iterator to point to the next data element
           const_iterator& operator++(int n);
           
           /// override operator =
           const_iterator& operator=(const const_iterator& src); 
     
           /// returns the three indices of the gridpoint the interator points to
           /*! This method returns the three indices into the GridField3D object's data
               of the grid point at which the iterator is pointing.
               
               \param i a pointer to the first horizontal index
               \param j a pointer to the second horizontal index
               \param k a pointer to the vertical coordinate index
           */
           void indices( int* i, int*j, int *k ) const;

           /// returns the area of a grid cell centered on this iteration's gridpoint
           /*! This method returns the normalized area of the horizontal grid cell centered
                     on this iteration's gridpoint.
                     (Normalized here means that the sum of the areas over the globe is 4*pi: the unit sphere.)
           */          
           real area() const;

        protected:

           /// index into the data element
           int my_index;
           /// the first possible index
           int first;
           /// the last possible index
           int last;
           /// which data vector is this an iterator for?
           const GridField3D* my_grid;
     
     };

     class profileIterator;
     friend class profileIterator;
     /*!
     \brief profile iterator for the GridField3D class 

      This iterator permits looping over each horizontal element of the grid,
      returning references to vectors of data elements that constitute a vertical
      profile at a specific point in latitude and longitude.
      If looping is done over identical grid objects (the same child class, 
      populated with the same dimensions) simultaneously, then
      each of the objects' iterators is guaranteed to be pointing
      to the same gridpoint.
      
     */
     class profileIterator  {
        public:
           
           /// default constructor for the GridField3D::profileIterator (do not use)
           /*! This is the default constructor for the GridField3D::profileIterator.
               Note that since it is not initialized by GridField3D::profileBegin() or GridField3D::profileEnd(), 
               this default constructor is useless.
           */
           profileIterator();
           
           /// the usual constructor for the GridField3D::profileIterator
           /*! This is the usual constructor for the GridField3D::profileIterator.
           
               \param grid a pointer to the GridField3D object being iterated over
               \param n the index (into the data vector) of the first element of the first profile
               \param len the number of data points in a profile
               \param beg the index of the first element of the data vector in the iteration
               \param fin the index of the last element of the data vector in the iteration
               \param skp the interval between indices (into the data vector) of successive
                      points in the profile
           */
           profileIterator( GridField3D* grid, int n=0, int len=0, int beg=0, int fin=0, int skp=0 );
           
           /// override operator *; returns a vertical profile from this object
           /*! This method overrides operator *, returning a pointer to the current data profile.
               
               \return a pointer to a vector that contains values of the vertical profile.
               Note that this is not a reference to an existing
               vector, but a pointer to a brand new vector created for the
               occasion.  The calling routine is responsible for deleting this 
               vector when it is no longer needed. 
           */
           std::vector<real>* operator*() const;
           
           /// override operator == ; tests two profileIterators for equallity
           bool operator==(const profileIterator& x) const;
           
           /// override operator != ; tests two profileIterators for inequality
           bool operator!=(const profileIterator& x) const;
           
           /// override operator ++ ; increments the profileIterator to point to the next data profile
           profileIterator& operator++(int n);
           
           /// override operator =
           profileIterator& operator=(const profileIterator& src);
           
           /// puts elements from an input vector into  
           void assign( const std::vector<real>& src );
     
           /// returns the two indices of the profile the interator points to
           /*! This method returns the two horizontal indices into the GridField3D object's data
               of the profile at which the iterator is pointing.
               
               \param i a pointer to the first horizontal index
               \param j a pointer to the second horizontal index
           */
           void indices( int* i, int*j ) const;

           /// returns the area of a grid cell centered on this iteration's profile
           /*! This method returns the normalized area of the horizontal grid cell centered
                     on this iteration's profile.
                     (Normalized here means that the sum of the areas over the globe is 4*pi: the unit sphere.)
           */          
           real area() const;

        protected:

           /*! index into the data element (points only into those elements which
               corresponde ot the first vertical level.
           */    
           int my_index;
           /// length of the profile
           int plen;
           /// the first possible index (into the first vertical level)
           int first;
           /// the last possible index (into the first vertical level)
           int last;
           /*! interval between indices (that is, from an element in one vertical level
               to the corresponding element in the next vertical level)
           */    
           int skip;
           /// which data grid is this an iterator for?
           GridField3D* my_grid;
     
     };

     class const_profileIterator;
     friend class const_profileIterator;
     /*!
     \brief profile iterator for a const GridField3D object 

      This iterator is like the profileIteratork, except it interates
      over s const GridField3D object.  That is, the object being iteratated
      over cannot be changed.
      
     */
     class const_profileIterator  {
        public:
           
           /// default constructor for the GridField3D::const_profileIterator (do not use)
           /*! This is the default constructor for the GridField3D::const_profileIterator.
               Note that since it is not initialized by GridField3D::profileBegin() or GridField3D::profileEnd(), 
               this default constructor is useless.
           */
           const_profileIterator();
           
           /// the usual constructor for the GridField3D::profileIterator
           /*! This is the usual constructor for the GridField3D::profileIterator.
           
               \param grid a pointer to the GridField3D object being iterated over
               \param n the index (into the data vector) of the first element of the first profile
               \param len the number of data points in a profile
               \param beg the index of the first element of the data vector in the iteration
               \param fin the index of the last element of the data vector in the iteration
               \param skp the interval between indices (into the data vector) of successive
                      points in the profile
           */
           const_profileIterator( const GridField3D* grid, int n=0, int len=0, int beg=0, int fin=0, int skp=0 );
           
           /// override operator *; returns a vertical profile from this object
           /*! This method overrides operator *, returning a pointer to the current data profile.
               
               \return a pointer to a vector that contains values of the vertical profile.
               Note that this is not a reference to an existing
               vector, but a pointer to a brand new vector created for the
               occasion.  The calling routine is responsible for deleting this 
               vector when it is no longer needed. 
           */
           std::vector<real>* operator*() const;
           
           /// override operator == ; tests two const_profileIterators for equallity
           bool operator==(const const_profileIterator& x) const;
           
           /// override operator != ; tests two const_profileIterators for inequality
           bool operator!=(const const_profileIterator& x) const;
           
           /// override operator ++ ; increments the const_profileIterator to point to the next data profile
           const_profileIterator& operator++(int n);
           
           /// override operator =
           const_profileIterator& operator=(const const_profileIterator& src);
           
           /// returns the two indices of the profile the interator points to
           /*! This method returns the two horizontal indices into the GridField3D object's data
               of the profile at which the iterator is pointing.
               
               \param i a pointer to the first horizontal index
               \param j a pointer to the second horizontal index
           */
           void indices( int* i, int*j ) const;

     
           /// returns the area of a grid cell centered on this iteration's profile
           /*! This method returns the normalized area of the horizontal grid cell centered
                     on this iteration's profile.
                     (Normalized here means that the sum of the areas over the globe is 4*pi: the unit sphere.)
           */          
           real area() const;

     
        protected:

           /*! index into the data element (points only into those elements which
               corresponde ot the first vertical level.
           */    
           int my_index;
           /// length of the profile
           int plen;
           /// the first possible index (into the first vertical level)
           int first;
           /// the last possible index (into the first vertical level)
           int last;
           /*! interval between indices (that is, from an element in one vertical level
               to the corresponding element in the next vertical level)
           */    
           int skip;
           /// which data grid is this an iterator for?
           const GridField3D* my_grid;
     };


     /// returns the beginning of the Grid
     /*! This method initializes an iterator to the beginning of a gridded field
         
         \return an iterator that points to the beginning of a grid
     */
     virtual iterator begin() = 0;

     /// returns (just past) the end of the Grid
     /*! This method initializes an iterator to the end of a gridded field, or more
         precisely one past the last element of the grid.
         
         \return an iterator that points to the end of a grid
     */
     virtual iterator end() = 0;


     /// returns the beginning of just one layer of the Grid
     /*! This method returns the beginning of just one layer of the Grid.
     
        \param k the index of the grid layer over which we will iterate
        
        \return an iterator that points to the beginning of the selected grid layer
     */   
     virtual iterator begin(int k) = 0;

     /// returns (just past) the end of just one layer of the Grid
     /*! This method returns the end of just one layer of the Grid, or more
         precisely one past the last element of the grid layer.
     
        \param k the index of the grid layer over which we will iterate
        
        \return an iterator that points to the end of the selected grid layer
     */   
     virtual iterator end(int k) = 0;


     /// returns the beginning of the Grid
     /*! This method initializes an immutable iterator to the beginning of a gridded field
         
         \return an immutable iterator that points to the beginning of a grid
     */
     virtual const_iterator begin() const = 0;

     /// returns (just past) the end of the Grid
     /*! This method initializes an immutable iterator to the end of a gridded field, or more
         precisely one past the last element of the grid.
         
         \return an immutable iterator that points to the end of a grid
     */
     virtual const_iterator end() const = 0;

     /// returns the beginning of just one layer of the Grid
     /*! This method returns the beginning of just one layer of the Grid.
     
        \param k the index of the grid layer over which we will iterate
        
        \return an immutable iterator that points to the beginning of the selected grid layer
     */   
     virtual const_iterator begin(int k) const = 0;

     /// returns (just past) the end of just one layer of the Grid
     /*! This method returns the end of just one layer of the Grid, or more
         precisely one past the last element of the grid layer.
     
        \param k the index of the grid layer over which we will iterate
        
        \return an immutable iterator that points to the end of the selected grid layer
     */   
     virtual const_iterator end(int k) const = 0;


     /// returns the first vertical profile of the horizontal grid
     /*! This method initializes a profileIterator to the first profile of the horizontal grid.
         
         \return a profileIterator that points to the vertical profile at the first horizontal gridpoint
     */
     virtual profileIterator profileBegin() = 0;

     /// returns (just past) the last profile of the horizontal grid
     /*! This method initializes a profileIterator to one past the last profile of the horizontal grid
         
         \return a profileIterator that points to the vertical profile past the last horizontal gridpoint
     */
     virtual profileIterator profileEnd() = 0;

     /// returns the first vertical profile of the horizontal grid
     /*! This method initializes an immutable profileIterator to the first profile of the horizontal grid.
         
         \return an immutable profileIterator that points to the vertical profile at the first horizontal gridpoint
     */
     virtual const_profileIterator profileBegin() const = 0;

     /// returns (just past) the last profile of the horizontal grid
     /*! This method initializes an immutable profileIterator to one past the last profile of the horizontal grid
         
         \return an immutable profileIterator that points to the vertical profile past the last horizontal gridpoint
     */
     virtual const_profileIterator profileEnd() const = 0;

     /// returns a specified vertical profile from a 3D grid
     /*! This method initializes a profileIterator so that it points to a specific vertical profile
         that corresponds to two given indices on the horizontal grud.

         \param i the first horizontal gridpoint index
         \param j the second horizontal gridpoint index

         \return a profileIterator that points to the vertical profile at the [i,j]th horizontal gridpoint
     */
     virtual profileIterator profileBegin( int i, int j ) = 0;

     /// returns a specified vertical profile from a 3D grid
     /*! This method initializes an immutable profileIterator so that it points to a specific vertical profile
         that corresponds to two given indices on the horizontal grud.

         \param i the first horizontal gridpoint index
         \param j the second horizontal gridpoint index

         \return an immutable profileIterator that points to the vertical profile at the [i,j]th horizontal gridpoint
     */
     virtual const_profileIterator profileBegin( int i, int j ) const = 0;

      /// splits a single index into the data array into three dimensional indices
      /*! Given a simple index into the data in the GridField3D object,
          this returns the two horizontal indices and the vertical index into the data grid that 
          correspond to the simple index.
          
          \param index the simple index
          \param i a pointer to the first horizontal index
          \param j a pointer to the second horizontal index
          \param k a pointer to the vertical coordinate index
      */
      virtual void splitIndex( int index, int* i, int* j, int* k ) const = 0;

      /// splits single indices into the data array into three dimensional indices
      /*! Given an array of simple indices into the data in the GridField3D object,
          this returns the two horizontal indices and the vertical indices into the data grid that 
          correspond to the simple indices.
          
          \param n the number of indices to be split 
          \param index a pointer to an n-element array of simple indices
          \param i a pointer to an n-element array of the first horizontal indices
          \param j a pointer to an n-element array of the second horizontal indices
          \param k a pointer to an n-element array of the vertical coordinate indices
      */
      virtual void splitIndex( int n, int *index, int* i, int* j, int* k ) const = 0;
      
      /// joins three dimensional indices into a single index into the data array 
      /*! Given three dimensional indices, this routine returns a single simple index directly into 
          into the data in the GridField3D object,
          
          \return the simple direct index into a data array.
          \param i the first horizontal index
          \param j the second horizontal index
          \param k the vertical coordinate index
      */
      virtual int joinIndex( int i, int j, int k ) const = 0;
      
      /// joins three dimensional indices into single indices into the data array 
      /*! Given three arrays of dimensional indices, this routine returns a single 
          array of simple indices directly into a data array. 
                
          
          \param n the number of indices to be split 
          \param index a pointer to an n-element array that will hold the simple indices retutrned.
                       if NULLPTR, then a new array will be ctrated.
          \param i a pointer to an n-element array of the first horizontal indices
          \param j a pointer to an n-element array of the second horizontal indices
          \param k a pointer to an n-element array of the vertical coordinate indices
          \return the pointer to the index array.
      */
      virtual int* joinIndex( int n, int *index, int* i, int* j, int* k ) const = 0;


   protected:

      /// identifies what the vertical coordinate is for this grid
      std::string vquant;
      
      /// the units of the vertical coordinate
      std::string vuu;

      /// used by child classes for operator= overriding methods
      void assign( const GridField3D& src);

      /// vector of vertical coordinates
      GridFieldDim zs;
     
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
