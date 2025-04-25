#ifndef GIGATRAJ_LINEARVINTERP_H
#define GIGATRAJ_LINEARVINTERP_H

#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/Vinterp.hh"


namespace gigatraj {

/*!
\ingroup vinterpolators
\brief class for linearly interpolating one or more vertical profiles to a specific vertical level

An object of this class provides a means of linearly interpolating meteorological
fields in their vertical coordinates.

The various interpolation methods of this class take an optional \c flags 
paremeter.  The flag constant values, which have names of the form GT_*, 
are defined in the gigatraj namespace.


*/

class LinearVinterp : public Vinterp {

   public:
   
   
      /// constructor
      LinearVinterp();

      /// destructor
      ~LinearVinterp();

      /// returns a copy of this Vinterp object
      LinearVinterp* copy() const;

      /// returns the kind of interpolator this is
      std::string id(); 

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
      real calc( real z, const GridField3D& grid, int i, int j, int flags=0 ) const;      

      /// interpolates to a vector of vertical levels at a given horizontal grid point.
      /*! Given \p i and \p j indices into the horizontal grid of a GridField3D object,
          this function interpolates to a vector of vertical levels at the \p (i,j) horizontal grid point.
      
          \return a new vector containing the interpolated values.
                  It is the calling routine's responsibility to delete this returned
                  variable when it is no longer needed.
           \param zs the level to interpolate to
           \param grid a 3D data grid
           \param i the first index into the horizontal grid of the point which is to be interpolated
           \param j the second index into the horizontal grid of the point which is to be interpolated
           \param flags flag values affecting the interpolation
      */
      std::vector<real>*  calc( const std::vector<real>& zs, const GridField3D& grid, int i, int j, int flags=0 ) const;      

      /// interpolates to an array of vertical levels at a given horizontal grid point.
      /*! Given \p i and \p j indices into the horizontal grid of a GridField3D object,
          this function interpolates to an array of vertical levels at the \p (i,j) horizontal grid point.
      
           \return a pointer to a newly-allocated array that contains the interpolated values.
                  It is the calling routine's responsibility to delete this returned
                  variable when it is no longer needed.
           \param n the number of levels to interpolate to
           \param zs an array of levels to interpolate to
           \param grid a 3D data grid
           \param i the first index into the horizontal grid of the point which is to be interpolated
           \param j the second index into the horizontal grid of the point which is to be interpolated
           \param flags flag values affecting the interpolation
      */
      real* calc( int n, const real* zs, const GridField3D& grid, int i, int j, int flags=0 ) const;      

      /// interpolates a 3D gridded field to a particular surface of the vertical coordinate
      /*! This function interpolates a 3D data grid to a given value of its vertical coordinate.
          The result is a 2D surface grid object that holds the vertically-interpolated
          values at each horizontal grid point of the 3D grid.
      
           \return a pointer to a new GridFieldSfc field that holds the interpolated values.  Note that the
                   calling routine will usually need to cast this to an appropriate
                   sub-class of GridFieldSfc. The calling routine is responsible for deleting the
                   new object when it is no longer needed.
           \param z the surface to interpolate to.  This will be the same physical quantity
                    as in the vertical coordinate of the \p grid parameter.
           \param grid the 3D grid to interpolate
           \param flags flag values affecting the interpolation
      */            
      GridFieldSfc* surface( const real z, const GridField3D& grid, int flags=0 ) const;
  
   
   protected:
   
      /// inline function to do the weighted average for linear interpolation
      inline real minicalc( real z, real z1, real z2, real d1, real d2 ) const
      {
          real result;
          
          result = ( z - z1 ) / (z2 - z1) * (d2 - d1 ) + d1;
      
          return result;
      };
      
      /// interpolates a vector of data and a coordinate to a single coordinate value
      /*! This function performs an interpolation, given a vector of dependent and independent
          variables.  It takes into account bad or missing data in both variables.
          
          \return the interpolated value
          \param griddata the input dependent variable
          \param vgriddata the input independent variable
          \param z the independent varibale value to interpolate to
          \param bad the bad-or-missing-data fill value used in \p griddata
          \param vbad the bad-or-missing-data fill value used in \p vgriddata
          \param dir specified the direction of the independent variable: +1 if increasing, -1 if decreasing
          \param debug set to 0 for quiet output, otherwide for debugging output.
      */    
      real interp( const std::vector<real>* griddata, const std::vector<real>* vgriddata, real z, real bad, real vbad, int dir, int debug=0 ) const; 

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
