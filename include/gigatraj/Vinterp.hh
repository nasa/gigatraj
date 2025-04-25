#ifndef GIGATRAJ_VINTERP_H
#define GIGATRAJ_VINTERP_H

#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/Interpolator.hh"
#include "gigatraj/GridField3D.hh"
#include "gigatraj/GridFieldSfc.hh"


namespace gigatraj {


/*! \defgroup vinterpolators Vertical Interpolators

\ingroup interpolators
\brief interpolates from a vertical spatial profile

Vinterp class members provide a means of interpolating
gridded data to an arbitrary desired vertical coordinate.

*/

/*!
\ingroup vinterpolators
\brief abstract class for interpolating one or more vertical profiles to a specific vertical level

The Vinterp class is an abstract class.
An object of this class provides a means of interpolating a field on 
one or more vertical profiles
to specific vertical levels within those profiles.

The vertical profile at each horizontal gridpoint being interpolated 
should be monotonic.  This condition might
not be strictly satisfied at the lowest altitudes; only the topmost part of each profile
that is monotonic is used. 

The various interpolation methods of this class take an optional \p flags 
paremeter.  The flag constant values, which have names of the form GT_*, 
are defined in the gigatraj namespace.

*/

class Vinterp : public Interpolator {

   public:
      
      // constructor
      Vinterp() {};

      /// destructor
      virtual ~Vinterp() {};

      /// returns a copy of this Vinterp object
      virtual Vinterp* copy() const = 0;

      /// returns the kind of interpolator this is
      virtual std::string id() = 0; 

      /// interpolates to a particular level at a given horizontal grid point.
      /*! Given \p i and \p j indices into the horizontal grid of a GridField3D object,
          this function interpolates to a particular vertical level at the \p (i,j) horizontal grid point.
          
           \returns the interpolated value
           \param z the level to interpolate to
           \param grid a 3D data grid
           \param i the first index into the horizontal grid of the point which is to be interpolated
           \param j the second index into the horizontal grid of the point which is to be interpolated
           \param flags flag values affecting the interpolation
      */
      virtual real calc( real z, const GridField3D& grid, int i, int j, int flags=0 ) const = 0;      

      /// interpolates to a vector of vertical levels at a given horizontal grid point.
      /*! Given \p i and \p j indices into the horizontal grid of a GridField3D object,
          this function interpolates to a vector of vertical levels at the \p (i,j) horizontal grid point.
      
          \return a new vector containing the interpolated values.
                  It is the calling routine's responsibility to delete this returned
                  variable when it is no longer needed.
           \param z the level to interpolate to
           \param grid a 3D data grid
           \param i the first index into the horizontal grid of the point which is to be interpolated
           \param j the second index into the horizontal grid of the point which is to be interpolated
           \param flags flag values affecting the interpolation
      */
      virtual std::vector<real>*  calc( const std::vector<real>&  z, const GridField3D& grid, int i, int j, int flags=0 ) const = 0;      


      /// interpolates to an array of vertical levels at a given horizontal grid point.
      /*! Given \p i and \p j indices into the horizontal grid of a GridField3D object,
          this function interpolates to an array of vertical levels at the \p (i,j) horizontal grid point.
      
           \return a pointer to a newly-allocated array that contains the interpolated values.
                  It is the calling routine's responsibility to free this returned
                  variable when it is no longer needed.
           \param n the number of levels to interpolate to
           \param zs an array of levels to interpolate to
           \param grid a 3D data grid
           \param i the first index into the horizontal grid of the point which is to be interpolated
           \param j the second index into the horizontal grid of the point which is to be interpolated
           \param flags flag values affecting the interpolation
      */
      virtual real*  calc( int n, const real* zs, const GridField3D& grid, int i, int j, int flags=0 ) const = 0;      


      /// interpolates a 3D gridded field to a particular surface of the vertical coordinate
      /*! This function interpolates a 3D data grid to a given value of its vertical coordinate.
          The result is a 2D surface grid object that holds the vertically-interpolated
          values at each horizontal grid point of the 3D grid.
      
           \return a pointer to a new GridFieldSfc field that holds the interpolated values.  Note that the
                   calling routine will usually need to cast this to an appropriate
                   sub-class of GridFieldSfc.  The calling routine is responsible to dleeting
                   the new object.
           \param z the surface to interpolate to.  This will be the same physical quantity
                    as in the vertical coordinate of the \p grid parameter.
           \param grid the 3D grid to interpolate
           \param flags flag values affecting the interpolation
      */            
      virtual GridFieldSfc* surface( const real z, const GridField3D& grid, int flags=0 ) const = 0;


      /// extracts a 2D iso-surface from a 3D data grid
      /*! This function
         interpolates a 3D gridded field in one vertical coordinate to a surface using a different
         3D gridded met field having the same vertical coordinate.  This is useful for 
         changing from a native vertical coordinate to a user-preferred vertical coordinate.
         
         That is, given physical quantities \c A, \c B, and \c C:
\verbatim
             A(on C) = Vinterp_object.surface( A(on B), C(on B) )     
\endverbatim
         
         \return a pointer to a new GridFieldSfc object of the physical quantity from \p grid that has been
                 interpolated to the value \p z of \p vgrid. The calling routine is responsible to dleeting
                   the new object. 
         \param z the value (of the physical quantity in \p vgrid) of the iso-surface desired
         \param grid the 3D grid to interpolate
         \param vgrid the 3D grid, having the same grid and valid-at time as the \p grid parameter, of the quantity
                to be used as the vertical coordinate for the interpolation. 
                Note that the horizontal grids of \p grid and \p vgrid
                must be compatible.
         \param flags flag values affecting the interpolation
      */                 
      GridFieldSfc* surface( const real z, const GridField3D& grid, const GridField3D& vgrid, int flags=0 ) const;
 
      /// extracts a 2D iso-surface from a 3D data grid
      /*! This function interpolates a 3D gridded field having one vertical coordinate to a surface 
          specified in terms of that coordinate
         
         That is, given physical quantities \c A and \c B, and a surface \c B=b (that is, 
         an isosurface of quantity \c B with value \c b):
\verbatim
             A(on B=b) = Vinterp_object.surface( A(on B), B=b ) 
\endverbatim
         
         \return a pointer to a new GridFieldSfc object of the physical quantity from \p grid that has been
                 interpolated to the surface \p vsfc.  The calling routine is responsible to dleeting
                   the new object.
         \param grid the 3D grid to interpolate
         \param vsfc a 2D aurface containing the same physical quantity that is the vertical
                coordinate of \p grid.  The data values of \p grid will be interpolated to this
                surface. Note that that horizontal grids of \p grid and \p vsfc must be compatible, and
                their valid-at times must be the same.
         \param flags flag values affecting the interpolation
      */                 
      GridFieldSfc* surface( const GridField3D& grid, const GridFieldSfc& vsfc, int flags=0 ) const;
 
      /// interpolates a 3D grid to a new vertical coordinate
      /*! This function interpolates a 3D gridded field in one vertical coordinate to a set of surfaces using a different
         3D gridded met field.  
         
         That is, given physical quantities \c A, \c B, and \c C:
\verbatim
             A(on C) = Vinterp_object.reProfile( A(on B), C(on B) ) 
\endverbatim

           \return a pointer to a new GridField3D object containing the interpolated values. The calling routine is responsible to dleeting
                   the new object.        
           \param zs a vector containing the isosurfaces of \p vgrid's physical quantity to interpolate to
           \param grid the 3D grid to interpolate
           \param vgrid a 3D grid, having the same horizontal grid, vertical grid, and valid-at time as 
                the \p grid parameter. \p vgrid's physical quantity is 
                used as the vertical coordinate for the interpolation.  
           \param flags flag values affecting the interpolation
      */                 
      GridField3D* reProfile( const std::vector<real>& zs, const GridField3D& grid, const GridField3D& vgrid, int flags=0 ) const;

      /// interpolates a 3D grid to a new vertical coordinate
      /*! This function interpolates a 3D gridded field in one vertical coordinate to a set of surfaces 
          using a different 3D gridded met field.  
         
         Given physical quantities \c A, \c B, and \c C:
\verbatim
             A(on C) = Vinterp_object.reProfile( A(on B), C(on B) ) 
\endverbatim
         
           \return a pointer to a new GridField3D object containing the interpolated values. The calling routine is responsible to dleeting
                   the new object.                 
           \param n the number of levels to interpolate to (i.e., the length of \p zs)
           \param zs an array of isosurfaces of \p vgrid's physical quantity to interpolate to
           \param grid the 3D grid to interpolate
           \param vgrid a 3D grid, having the same horizontal grid, vertical grid, and valid-at time as 
                the \p grid parameter. \p vgrid's physical quantity is 
                used as the vertical coordinate for the interpolation.  
           \param flags flag values affecting the interpolation
      */                 
      GridField3D* reProfile( int n, const real* zs, const GridField3D& grid, const GridField3D& vgrid, int flags=0 ) const;
   
 
      /// interpolates a 3D grid to a new vertical coordinate
      /*! This function interpolates a 3D gridded field in one vertical coordinate to a 
         set of surfaces using a different 3D gridded met field.  
         
         Given physical quantities \c A, \c B, and \c C:
\verbatim
             A(on C) = Vinterp_object.reProfile( A(on B), B(on C) ) 
\endverbatim
         
           \return a pointer to a new GridField3D object containing the interpolated values.  The return value will have
                the same physical quantity as \grid, but the same vertical coordinate
                as \vgrid. The calling routine is responsible to dleeting
                   the new object.                  
           \param grid the 3D grid to interpolate
           \param vgrid a 3D grid, having the same horizontal grid and valid-at time as 
                the \p grid parameter.  \p vgrid's physical quantity is the same
                as \p grid's vertical coordinate.  
           \param flags flag values affecting the interpolation
      */                 
      GridField3D* reProfile( const GridField3D& grid, const GridField3D& vgrid, int flags=0 ) const;
 
      /// transforms from a grid of data A with vertical coordinate B, to a grid of data B on vertical coordinate A.
      /*! This function inverts the relationship between a physical quantity and its vertical coordinates.
          Generally, this make sense only for two quantities that vary monotonically with
          each other.
      
         Given physical quantities \c A, and \c B:
\verbatim
             A(on B) = Vinterp_object.invert( B(on A) ) 
\endverbatim

             \return a pointer to a new GridField3D object holding the interpolated data. The calling routine is responsible to dleeting
                   the new object.      
             \param newlevels a vector of the isosurfaces of \grid's physical quantity 
                    to which its vertical coordinate values are to be interpolated
             \param grid the input data grid to be interpolated
      */
      GridField3D* invert( const std::vector<real>& newlevels, const GridField3D& grid ) const; 

 
      /// interpolates from a vector
      /*!  Given a vertical profile, this function interpolates to a desired level within that vector
      
          \return the interpolated value
          \param zs a vector containing the vertical levels of the input data
          \param vals a vector containing the input data
          \param z the vertical level to interpolate to
          \param bad the value that marks bad or missing data in the vals vector
          \param flags flag values affecting the interpolation
          
      */
      real profile( const std::vector<real>& zs, const std::vector<real>& vals, real z, real bad, int flags=0 ) const;


   protected:

      /// returns whether to do interpolations locally
      /*! This function is used for interpreting the flags value passed to the various calculation routines.
      
          \return if interpolation is to be done locally (ignoring any
                  multiprocessing), then 1 is returns.  Otherwise, 0 is returned.
      */    
      inline int do_local( int flags ) const
      {
          
          return (flags & ~GT_INTRP_LOCAL)? 1 : 0;
      
      }

      /// returns the direction in which a vector of values increases
      /*! This function determines the direction of elements of the "profile" vector.
         
         
         \return +1 if the elements increase with index, and -1 if they decrease.
                    Note that the elements should increase or decrease monotonically, but
                    they do not have to.  If they are not monotonic, then only those
                    elements are used which constitute the longest monotonic segment
                    at the top (highest-index) of the vector.
         \param profile the vector of elements whose direction is to be determined
         \param vbad the bad-or-missing-data fill value.  Any element of \p profile which is equal to \p vbad
                is ignored.
      */
      inline int getDirection( const std::vector<real>* profile, real vbad ) const
      {
          int dir;
          int k;
          int nzs;
          real upperz;
          int upperk;
          real lowerz;
          int lowerk;
          
          dir = 0;

          nzs = profile->size();

          // find the uppermost point in the profile for which the
          // vertical data is not a bad point
          upperz = vbad;
          for ( k=nzs-1; k >= 1 && upperz == vbad ; k-- ) {
              upperz   = (*profile)[k];
              upperk   = k;
          }
          // was there such a point?
          if ( upperz != vbad ) {
         
             lowerz = vbad;
             // find the lowest non-bad point down from the upperk point
             for ( k=upperk-1; k >= 0 && lowerz == vbad && lowerz != upperz; k-- ) {
                 lowerz   = (*profile)[k];
                 lowerk   = k;
             }
             // was there such a point?
             if ( lowerz != vbad ) {
                
                (lowerz < upperz ) ? dir=1 : dir=-1;
                
             }
          }    

          return dir;
          
      }
      

      /// returns the direction in which a vector of values increases
      /*! This function determines the direction of elements of the "profile" vector.
         
         
         \return +1 if the elements increase with index, and -1 if they decrease.
                    Note that the elements should increase or decrease monotonically, but
                    they do not have to.  If they are not monotonic, then only those
                    elements are used which constitute the longest monotonic segment
                    at the top (highest-index) of the vector.         
         \param profile the vector of elements whose direction is to be determined.  There are
               no bad-or-missing data points in this vector.

      */
      inline int getDirection( const std::vector<real>* profile ) const
      {
          int dir;
          int k;
          int nzs;
          
          dir = 0;

          nzs = profile->size();

          
          ((*profile)[0] < (*profile)[nzs-1]) ? dir = 1 : dir=-1;


          return dir;
          
      }
      
      /// interpolates a vector of data and a coordinate to a single coordinate value
      /*! This function performs an interpolation, given a vector of dependent and independent
          variables.  It takes into account bad or missing data in both variables.
          
          \return the interpolated value
          \param griddata the input dependent variable
          \param vgriddata the inupt independent variable
          \param z the independent varibale value to interpolate to
          \param bad the bad-or-missing-data fill value used in \p griddata
          \param vbad the bad-or-missing-data fill value used in \p vgriddata
          \param dir specified the direction of the independent variable: +1 if increasing, -1 if decreasing
          \param debug set to 0 for quiet output, otherwide for debugging output.
      */    
      virtual real interp( const std::vector<real>* griddata, const std::vector<real>* vgriddata, real z, real bad, real vbad, int dir, int debug=0 ) const = 0; 



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
