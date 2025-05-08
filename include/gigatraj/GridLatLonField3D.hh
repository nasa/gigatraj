#ifndef GIGATRAJ_GRIDLATLONFIELD3D_H
#define GIGATRAJ_GRIDLATLONFIELD3D_H

#include <string>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/GridFieldDim.hh"
#include "gigatraj/GridFieldDimLon.hh"
#include "gigatraj/GridField3D.hh"
#include "gigatraj/GridLatLonFieldSfc.hh"

namespace gigatraj {

/*!
\brief class for a three-dimensional lat-lon-z gridded meteorological field data

\ingroup grid3D

The GridLatLonField3D class provides a way of holding in one object all of the
information that is needed to deal with such a data field.

A subclass of the more general GridField3D, GridLatLonField3D has
its three dimensions (longitude, latitude,
and the vertical coordinate) all independent of each other.  The data
field may therefore be thought of as a three-dimensional array which is
indexed by longitude, latitude, and the vertical coordinate.

*/

class GridLatLonField3D : public GridField3D {

   typedef std::vector<real> realvec;

   /// \brief overlays the << operator for writing a GridLatLonField3D object
   /*! This method serializes the GridLatLonField3D object and sends it to an ostream.
   
        \param os the output stream (ostream) object to which the serialized object is to be sent
        \param p the GridLatLonField3D object to be serialized and output
   */
   friend std::ostream& operator<<( std::ostream& os, const GridLatLonField3D& p);

   /// \brief overlays the >> operator for reading a GridLatLonField3D object
   /*! This method reads a GridLatLonField3D object's serialzied data from 
        an istream and deserializes it.
   
        \param is the input stream (istream) object from which the serialized object is to be read
        \param p the GridLatLonField3D object to be input and deserialzied
   */
   friend std::istream& operator>>( std::istream& is, GridLatLonField3D& p);

   public:
   
   
      /// Default constructor
      /*!
         This is the default contructor for the GridLatLobField3D class.
      */
      GridLatLonField3D();
      
      /// Default destructor
      /*!
         This is the default destructor for the GridLatLobField3D class.
      */
      virtual ~GridLatLonField3D();
      
      /// copy constructor
      /*!
         This is the copy contructor method for the GridLatLobField3D class.
      */
      GridLatLonField3D(const GridLatLonField3D&);

     /// copy assignment
     /*! 
         This is the copy assignment operator for the GridLatLonField3D class.
     */
     GridLatLonField3D& operator=(const GridLatLonField3D& src);
      
      /// clears data and metadata contents 
      /*! This method clears the contents of the object, except for information
          related to the process group
      */    
      void clear();

      /// returns the readiness status
      /*! This object returns the readiness status: whether the object
          contains valid data, timestamp, dimensions, etc.
      
          \return returns 0 if the object is ready to be used, non-zero otherwise
      */
      int status() const;

      /// returns the size of the grid's three dimensions
      /*! This method returns the size of the grid's three dimensions
          \param nlon the number of longitude values
          \param nlat the number of latitude values
          \param nv the number of vertical level values
      */
      void dims( int* nlon, int* nlat, int* nv) const;

      /// returns a single data value
      /*! This method returns a single data value.
           \param i the index of the x-coordinate gridpoint
           \param j the index of the y-coordinate gridpoint
           \param k the index of the z-coordinate gridpoint
           \return the value of the [i,j,k]th element of the data grid
      */     
      real value( int i, int j, int k ) const;
      
      /// returns a reference to a single data value
      /*! This method returns a reference to a single data value.
           \param i the index of the x-coordinate gridpoint
           \param j the index of the y-coordinate gridpoint
           \param k the index of the z-coordinate gridpoint
           \return a reference to the [i,j,k]th element of the data grid
      */     
      real& valueref( int i, int j, int k );
      
      /// returns a single data value
      /*! This method returns a single data value, indexed directly into the data array.
           \param indx the index directly into the data array
           \return the value of the [idx]th element of the data grid
      */     
      real value( int indx ) const;
      
      /// returns an array of data values
      /*! This method fills an array of data values.
      
      
           \param n the number of data values to return
           \param vals a pointer to an n-element array of real values to hold the data.
                       If NULLPTR, then a new array is created, and the calling
                       routine has the responsibility of deleting it.
           \param indices a pointer to a (3*n)-element array of indices into the grid, that locate each data gridpoint
                         Got the ith element of vals, coords[3*i] is the x-coordinate index,
                         coords[3*i+1] is the y-coordinate index, and coords[3*i+2] is the z-coodinate index.
           \return a pointer to the vals array. If the input vals is not NULLPTR, then this is the same
                   as the input vals. otherwise, it points to the newly-created array of values.                          
      */     
      real* values( int n, real* vals, int* indices ) const;
      

      /// returns the vector of longitudes
      /*! This method returns the set of gridpoint longitudes.
         
          \return a vector of longitudes
      */      
      std::vector<real> longitudes() const;

      /// returns the vector of latitudes
      /*! This method returns the set of gridpoint latitudes.
         
          \return a vector of latitudes
      */      
      std::vector<real> latitudes() const;

      /// returns a single longitude gridpoint value
      /*! This method returns a single longitude gridpoint value.
      
          \param i index of longitude to be returned
      
          \return the [i]th longitude
      */
      real longitude( int i ) const;
         
      /// returns a single latitude gridpoint value
      /*! this method returns a single latitude gridpoint value.

          \param j index of latitude to be returned

          \return the [j]th latitude
      */
      real latitude( int j ) const;
      
      /// restricts a given longitude to be within the range of this object's longitudes
      /*! This method returns a longitude which has had 360 degrees added or subtracted
          to make it lie within the range of longitudes in this object's grid,
          or between the (n-1)st and the oth grid points.
          
          Note that this method will wrap a longitude even if the grid does not cover all
          longitudes.
            
            \param lon the input longitude
            
            \return the "wrapped" longitude
      */
      real wrap( real lon ) const;

      /// "wrap" a longitude index 
      /*! Given an integer index into the grid longitudes, which might or
          might not actually exist in the set of grid longitudes, this method
          will "wrap" the index until is fits into the range of
          grid longitude indices. This is equivalent to adding and subtracting
          360 degrees to the longitude value itself until it lies
          within the range of grid longitudes.
          
          Note that this method will throw a badindex exception if the
          grid longitudes do not wrap (i.e., if they do not cover
          the whole globe).
         
            \param i the input longitude index
            
            \return the "wrapped" longitude index
      */
      int iwrap( int i ) const;

      /// returns the two indices which straddle a given longitude value
      /*! This method returns the two indices which straddle a given longitude value.
      
           \param lon the input longitude value
           \param i1 returns the index that lies to the west of the longitude
           \param i2 returns the index that lies to the east of the longitude
      
      */     
      void lonindex( real lon, int* i1, int* i2 ) const;

      /// returns the two indices which straddle a given latitude value
      /*! This method returns the two indices which straddle a given latitude value.
      
           \param lat the input latitude value
           \param i1 returns the index that lies to the south of the latitude
           \param i2 returns the index that lies to the north of the latitude
      */     
      void latindex( real lat, int* i1, int* i2 ) const;


      ///  loads data and dimensions into this object
      /*!  This method loads data and dimensions into this object.

           \param inlons vector of longitudes
           \param inlats vector of latitudes
           \param inlevels vector of vertical levels
           \param indata vector of data values, in row-major order
           \param loadFlags bitwise flags:
                   - GFL_WRAP if longitude wrapping shoud be on (i.e., global data grid)
                   - GFL_NOWRAP if longitude wrapping should not be on
                   - GFL_PREFILL if the data grid is to be pre-filled with bad-data flags
                                     (the default is to leave the data undefined)
      */
      void load( const realvec& inlons, const realvec& inlats, const realvec& inlevels, const realvec& indata, const int loadFlags=0 );

      ///  loads dimensions only into this object
      /*!  This method loads dimensions only into this object.
           \param inlons vector of longitudes
           \param inlats vector of latitudes
           \param inlevels vector of vertical levels
           \param loadFlags bitwise flags:
                   - GFL_WRAP if longitude wrapping shoud be on (i.e., global data grid)
                   - GFL_NOWRAP if longitude wrapping should not be on
                   - GFL_PREFILL if the data grid is to be pre-filled with bad-data flags
                                     (the default is to leave the data undefined)
      */
      void load( const realvec& inlons, const realvec& inlats, const realvec& inlevels, const int loadFlags=0 );

      ///  loads data only into this object
      /*!  This method loads data only into this object
           \param indata vector of data values, in row-major order
           \param loadFlags bitwise flags:
      */
      void load( const realvec& indata, const int loadFlags=0 );


      /// checks whether the dimensions and time of a given GridField3D object are compatible with this object.
      /*! This method checks another GridField3D object for compatibility with this one.
          
          \param obj the GridField3D object with which this object is to be compared
          \param compatFlags bitwise flags:
                - METCOMPAT_STRICT if strict compatibility in all aspects is required (default)
                - METCOMPAT_HORIZ if horizontal compatibility is required
                - METCOMPAT_VERT if vertical compatibility is required
                - METCOMPAT_TIME if time compatibility is required 

          \return true if the dimensions and the times match, false otherwise
      */
      bool compatible( const GridField3D&  obj, int compatFlags = METCOMPAT_STRICT ) const;

      /// checks whether the dimensions and time of a given GridFieldSfc  object are compatible with this object.
      /*! This method checks a GridFieldSfc object for compatibility with this one.
          
          \param obj the GridFieldSfc object with which this object is to be compared
          \param compatFlags bitwise flags:
                - METCOMPAT_STRICT if strict compatibility in all aspects is required (default)
                - METCOMPAT_HORIZ if horizontal compatibility is required
                - METCOMPAT_VERT if vertical compatibility is required (guaranteed not to be compatible!)
                - METCOMPAT_TIME if time compatibility is required 
          \return true if the dimensions and the times match, false otherwise
      */
      bool compatible( const GridFieldSfc& obj, int compatFlags = METCOMPAT_HORIZ && METCOMPAT_TIME ) const;

      /// checks for metadata compatibility with a given GridLatLonField3D object
      /*! This method checks whether another GridLatLonField3D object has the same metadata as this one.
          Metadata include the quantity name and units, and the vertical coordinate
          name and units.
          
          \param obj the GridLatLonField3D object with which this object is to be compared

          \return true if the metadata are the same, false otherwise
      */
      bool match( const GridLatLonField3D& obj ) const;

      /// indicates whether the grid longitudes cover the entire globe
      /*! This method determines whether the grid longiutudes wrap around--that ius,
          whether they cover the whole globe. The gap between the first longiutude and the
          last one, modulo 360 degrees, should not be must different than the
          difference between the first and second longitudes, to be considered global.
      
           \return 1 if the grid longitudes wrap around. 0 otherwise
      
      */
      int doeswrap() const;
      
       /// extracts a 2D surface corresponding to a vertical level.
      /*! This method extracts a 2D surface corresponding to a vertical level.

          \param k the index of the vertical level to be extracted
          
          \return a pointer to the new surface object.  The calling routine is responsible
          for deleting the new object.
      */
      GridLatLonFieldSfc* extractSfc( int k ) const;    

      /// extracts a 2D surface corresponding to a vertical level.
      /*! This method extracts a 2D surface corresponding to a vertical level.
          It differs from the extractSfc() method in that it returns a pointer
          to a generic GridFieldSfc object instead of GridLatLonField, so
          that the result may be used by routines that do not care what grid 
          the data lie on.

          \param k the index of the vertical level to be extracted
          
          \return a pointer to the new surface object.  The calling routine is responsible
          for deleting the new object.
      */
      GridFieldSfc* extractSurface( int k ) const;    

      /// replaces a vertical level with the contents of the given 2D surface.
      /*! This method replaces a vertical level with the contents of the given 2D surface.
          The physical quantity should be the same.  Units will be adjusted automatically
          by converting the surface units to MKS values, and then from MKS to 
          the values being used in the object.
          But note that the physical quantity and the vertical coordinate quantity
          are not checked against the physical quantity and surface quantity
          of the input surface.
          
          \param sfc the surface to be inserted
          \param k the level to be replaced
      */
      void replaceLevel( GridFieldSfc& sfc, int k);    


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
      GridField3D* duplicate() const;

      /// returns the number of possible data points in the grid.
      /*! This method returns the number of possible data points in the grid.
          
          \return the total number of gridpoints, or zero
          if the grid has been specified but no data have been loaded.
          
      */
      int dataSize() const;   


      /// returns a set of gridpoint coordinates
      /*! This method returns the longitude, latitude, and vertical coordinate values
          of a given set of gridpooints
      
      
          \param n the number of gridpoint coordinates desired
           \param is n-element array of first-indices into the data array
           \param js n-element array of second-indices into the data array
           \param ks n-element array of third-indices into the data array
           \param olons n-element array of reals to hold the longitudes
           \param olats n-element array of reals to hold the latitudes
           \param olevs n-element array of reals to hold the vertical coordinates
      */
      void gridcoords( int n, int* is, int* js, int* ks, real* olons, real* olats, real* olevs, int flags=0) const;

      ///  returns a set of gridpoint values
      /*!  Returns a set of gridpoint values. 

           Warning: if you are getting the values from a centralized met processors,
           you must perform some kind of handshaking with the processor 
           (perhaps by using the ask_for_data() method of the GridField class)
           before calling this method.
           
           \param n the number of gridpoint values desired
           \param is n-element array of longitude indices
           \param js n-element array of latitude indices
           \param ks n-element array of vertical level indices
           \param vals n-element array of reals to hold the results
           \param flags a set of bitwise flags that control the behavior of the method:
                   0x01 = ignore any multiprocessing and fetch the gridpoints locally
                   0x02 = call svr_done once the gridpoints have been obtained.
      */
      void gridpoints( int n, int* is, int* js, int* ks, real* vals, int flags=0) const;


      ///  returns a set of gridpoint values
      /*!  Returns a set of gridpoint values. 

           Warning: if you are getting the values from a centralized met processors,
           you must perform some kind of handshaking with the processor 
           (perhaps by using the ask_for_data() method of the GridField class)
           before calling this method.
           
           \param n the number of gridpoint values desired
           \param indices an n-element array of indices directly into the data array
           \param vals n-element array of reals to hold the results
           \param flags a set of bitwise flags that control the behavior of the method:
                   0x01 = ignore any multiprocessing and fetch the gridpoints locally
                   0x02 = call svr_done once the gridpoints have been obtained.
      */
      void gridpoints( int n, int* indices, real* vals, int flags=0) const;

      /// (parallel processing) receives metadata from a central met processor
      /*! Receives metadata from a central met processor, if
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
      void receive_meta();
      
      
      /// (parallel processing) handles the server side of the gridpoints transaction
      /*! This method handles the server side of the gridpoints transaction, for multi-processing
         with a dedicated meteorological data processor.
         For serial processing or when there is no dedicated met processor,
         this routine does nothing.
         
         \param id the ID of the processor whose request is to be satisfied
      */   
      void svr_send_meta(int id) const;

      /// the type of object this is
      static const string iam;

      /// returns a string with the specific type of object this is
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


      /// returns the normalized area of a grid cell, given two grid indices
      /*! This method returns the normalized (i.e., unit-sphere) horizontal area associated with a grid cell centered
                on a given gridpoint
                
          \param i the first (longitudinal) index of the gridpoint
          \param j the second (latitudinal) index of the gridpoint
          
          \return the area of the [i,j]th gridpoint
      */         
      real area(int i, int j) const;

      /// returns the normalized area of a grid cell, given a single grid index
      /*! This method returns the normalized (unit-sphere) horizontal area associated with a grid cell centered
                on a given gridpoint

          \param i the index for the gridpoint
          
          \return the area of the [i]th gridpoint
      */         
      real area(int i) const;

      /// returns a surface of horizontal grid cell areas
      /*! This method returns a surface object containing the normalized areas of each grid point cell.
          (Normalized here means that the sum of the areas over the globe is 2*pi.)
          
          \return a pointer to the new surface object.  The calling reoutine is responsible
          for deleting the new object.
      */
      GridFieldSfc* areas() const;
      

      /// takes the provided arrays of dimensional values and data values as its own
      /*! This method takes arrays of longitude values and
      latitude value,s as well as 
          an array of data values; and it
          makes them its own. That is, this GridLatLonField3D object 
         is therafter responsible for deleting the arrays. The calling routine
         must not delete the arrays or change any of heir elements.
       
          /param nlons the number of longitudes
          /param nlats the number of latitudes
          /param nzs the number of vertical levels
          /param vals the nlons*nlats-length array of data values to be "absorbed"
          /param lonvals if non-null, the array of longitude values to be "absorbed"
          /param latvals if non-null, the array of latitude values to be "absorbed"
          /param zvals if non-null, the array of vertical level values to be "absorbed"
            
     */    
      void absorb( int nlons, int nlats, int nzs, real* vals , real* lonvals=NULLPTR, real* latvals=NULLPTR, real* zvals=NULLPTR);

      /// takes the provided array of longitude values as its own
      /*! This method takes an array of longitude values and
          makes them its own. That is, this GridLatLonField3D object 
         is therafter responsible for deleting the array. The calling routine
         must not delete the array or change any of its elements.
       
          /param n the length of the array
          /param lonvals the array of longitudes to be "absorbed"
            
     */    
      void absorbLons( int n, real* lonvals );

      /// takes the provided array of latitude values as its own
      /*! This method takes an array of latitude values and
          makes them its own. That is, this GridLatLonField3D object 
         is therafter responsible for deleting the array. The calling routine
         must not delete the array or change any of its elements.
       
          /param n the length of the array
          /param latvals the array of latitudes to be "absorbed"
            
     */    
      void absorbLats( int n, real* latvals );

      /// (parallel processing) sets the process group and met processor 
      /*! This method sets the process group and met processor for parallel processing.
      
           \param pg a pointer to the process group being used for parallel processing
      
           \param met the ID of the processor within pg that is dedicated to handling met 
                  data (or -1 if there is none)
      */
      void setPgroup( ProcessGrp* pg, int met);





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

     /// returns the beginning of just one layer of the Grid
     /*! This method returns the beginning of just one layer of the Grid.
     
        \param k the index of the grid layer over which we will iterate
        
        \return an iterator that points to the beginning of the selected grid layer
     */   
     iterator begin(int k);

     /// returns (just past) the end of just one layer of the Grid
     /*! This method returns the end of just one layer of the Grid, or more
         precisely one past the last element of the grid layer.
     
        \param k the index of the grid layer over which we will iterate
        
        \return an iterator that points to the end of the selected grid layer
     */   
     iterator end(int k);

     /// returns the beginning of the Grid
     /*! This method initializes an immutable iterator to the beginning of a gridded field
         
         \return an immutable iterator that points to the beginning of a grid
     */
     const_iterator begin() const;

     /// returns (just past) the end of the Grid
     /*! This method initializes an immutable iterator to the end of a gridded field, or more
         precisely one past the last element of the grid.
         
         \return an immutable iterator that points to the end of a grid
     */
     const_iterator end() const;

     /// returns the beginning of just one layer of the Grid
     /*! This method returns the beginning of just one layer of the Grid.
     
        \param k the index of the grid layer over which we will iterate
        
        \return an immutable iterator that points to the beginning of the selected grid layer
     */   
     const_iterator begin(int k) const;

     /// returns (just past) the end of just one layer of the Grid
     /*! This method returns the end of just one layer of the Grid, or more
         precisely one past the last element of the grid layer.
     
        \param k the index of the grid layer over which we will iterate
        
        \return an immutable iterator that points to the end of the selected grid layer
     */   
     const_iterator end(int k) const;

     /// returns the first vertical profile of the horizontal grid
     /*! This method initializes a profileIterator to the first profile of the horizontal grid.
         
         \return a profileIterator that points to the vertical profile at the first horizontal gridpoint
     */
     profileIterator profileBegin();

     /// returns (just past) the last profile of the horizontal grid
     /*! This method initializes a profileIterator to one past the last profile of the horizontal grid
         
         \return a profileIterator that points to the vertical profile past the last horizontal gridpoint
     */
     profileIterator profileEnd();

     /// returns the first vertical profile of the horizontal grid
     /*! This method initializes an immutable profileIterator to the first profile of the horizontal grid.
         
         \return a reference to an immutable profileIterator that points to the vertical profile at the first horizontal gridpoint
     */
     const_profileIterator profileBegin() const;

     /// returns (just past) the last profile of the horizontal grid
     /*! This method initializes an immutable profileIterator to one past the last profile of the horizontal grid
         
         \return an immutable profileIterator that points to the vertical profile past the last horizontal gridpoint
     */
     const_profileIterator profileEnd() const;

     /// returns a specified vertical profile from a 3D grid
     /*! This method initializes a profileIterator so that it points to a specific vertical profile
         that corresponds to two given indices on the horizontal grud.

         \param i the longitude gridpoint index
         \param j the latitude gridpoint index

         \return a profileIterator that points to the vertical profile at the [i,j]th horizontal gridpoint
     */
     profileIterator profileBegin( int i, int j );

     /// returns a specified vertical profile from a 3D grid
     /*! This method initializes an immutable profileIterator so that it points to a specific vertical profile
         that corresponds to two given indices on the horizontal grud.

         \param i the longitude gridpoint index
         \param j the latitude gridpoint index

         \return an immutable profileIterator that points to the vertical profile at the [i,j]th horizontal gridpoint
     */
     const_profileIterator profileBegin( int i, int j ) const;

      /*! Given a simple index into the first level of data in the GridLatLonField3D object,
          this returns the two horizontal indices 
          and the vertical index into the data grid that 
          correspond to the simple index.
          
          \param index the simple index
          \param i a pointer to the longitude index
          \param j a pointer to the latitude index
          \param k = a pointer to the vertical coordinate index
      */
      void splitIndex( int index, int* i, int*j, int *k ) const;
      
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
      void splitIndex( int n, int *index, int* i, int* j, int* k ) const;
      
      /// joins three dimensional indices into a single index into the data array 
      /*! Given three dimensional indices, this routine returns a single simple index directly into a data array. 
      
      simple index into the data in the GridField3D object,
          this returns the two horizontal indices and the vertical index into the data grid that 
          correspond to the simple index.
          
          \return the simple direct index into a data array.
          \param i the first horizontal index
          \param j the second horizontal index
          \param k the vertical coordinate index
      */
      int joinIndex( int i, int j, int k ) const;
      
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
      int* joinIndex( int n, int *index, int* i, int* j, int* k ) const;


   protected:

      /// longitudes
      GridFieldDimLon lons;
      /// latitudes
      GridFieldDim lats;
      
      /*! sets the wrap flag based on flags or input data
      */
      void setWraps( const int loadFlags=0); 
      

      /// split a single index into longutude, latitude, and vertical indices
      /*! Given a simple index into the data in the GridLatLonField3D object,
          this returns the two horizontal and the vertical indices into the data grid that 
          correspond to the simple index.
          
          \param index the simple index
          \param i a pointer to the longitude index
          \param j a pointer to the latitude index
          \param k a pointer to the vertical index
      */
      void getindices( int index, int* i, int* j, int* k ) const;
      
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
