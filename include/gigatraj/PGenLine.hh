#ifndef PGENLINE_H
#define PGENLINE_H


#include <deque>
#include <list>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/Earth.hh"
#include "gigatraj/Parcel.hh"
#include "gigatraj/ParcelGenerator.hh"

namespace gigatraj {


/*!
\ingroup parcelgenerators

\brief generates collections of parcels along a great-circle line

The PGenLine class generates collections of parcels whose positions
are initialized at regularly-spaced gridpoints along a great-circle line
between two locations.  Parcels are also spaced in the vertical, forming
a sort of ribbon along the great-circle line.
(All times are initialized to 0.0.)

Although Parcels are created in two dimensions (along-line and vertical), they are stored in
one-dimensional arrays or containers. They are stored in row-major
(i.e., Fortran) order: along-line first, then vertical
coordinates.

*/

class PGenLine : public ParcelGenerator {

   public:
      
      /// \brief default constructor
      /*! 
            This is the basic constructor for the PGenLine class.
      */
      PGenLine();

      /// \brief destructor
      /*! 
         This is the destructor method for the PGenLine class.
      */
      ~PGenLine(); 

      /// \brief Copy-constructor
      /*!
         This is the copy contructor method for the Filter_Null class.
         
         \param src the source Filter_Null object to copy from
      */
      PGenLine(const PGenLine& src);

      /// \brief copy assignment
      /*!
          This is the copy assignment operator for the PGenLine class.
      */
      PGenLine& operator=(const PGenLine& src);


      /// copies settings from a source object to this one
      /*! 
           This method copies settings from a source PGenLine object
           to this one.
           
           \param src the source PGenLine object
      */     
      void assign( const PGenLine& src);
      

      /// Create an array of Parcels on a grid
      /*! This method creates an array of Parcels on a grid, in row-major order.
      
         \param p the input parcel whose settings we are to copy
         \param np the number of parcels is returned in this variable
         \param beglon the beginning longitude value
         \param beglat the beginning latitude value
         \param endlon the ending longitude value
         \param endlat  the ending latitude value
         \param deltah the along-line horizontal increment
         \param begz the beginning vertical coordinate value
         \param endz the ending vertical coordinate value
         \param deltaz the vertical coordinate increment
         
         \return a Parcel pointer that points to an array of Parcel objects.
      */
      Parcel * create_array(const Parcel& p, int *np
                           , real beglon, real beglat
                           , real endlon, real endlat, real deltah
                           , real begz, real endz, real deltaz 
                           );


      /// Create a vector Container of Parcel objects 
      /*! This method creates a vector Container of Parcel objects. 
      
         \param p the input parcel whose settings we are to copy
         \param beglon the beginning longitude value
         \param beglat the beginning latitude value
         \param endlon the ending longitude value
         \param endlat  the ending latitude value
         \param deltah the along-line horizontal increment
         \param begz the beginning vertical coordinate value
         \param endz the ending vertical coordinate value
         \param deltaz the vertical coordinate increment
         
         \return a pointer to a vector of Parcel objects; the calling routine must delete this vector once it is no longer needed.
      */
       std::vector<Parcel>* create_vector(const Parcel& p
                           , real beglon, real beglat
                           , real endlon, real endlat, real deltah
                           , real begz, real endz, real deltaz 
                           );
      
      /// Create a list Container of Parcel objects 
      /*! This method create a List container of Parcel objects.
      
         \param p the input parcel whose settings we are to copy
         \param beglon the beginning longitude value
         \param beglat the beginning latitude value
         \param endlon the ending longitude value
         \param endlat  the ending latitude value
         \param deltah the along-line horizontal increment
         \param begz the beginning vertical coordinate value
         \param endz the ending vertical coordinate value
         \param deltaz the vertical coordinate increment
         
         \return a pointer to a list of Parcel obejcts; the calling routine must delete this list once it is no longer needed.
      */
       std::list<Parcel>* create_list(const Parcel& p
                           , real beglon, real beglat
                           , real endlon, real endlat, real deltah
                           , real begz, real endz, real deltaz 
                           );
      
      /// Create a deque Container of Parcel objects 
      /*! This method creates a deque Container of Parcel objects.
      
         \param p the input parcel whose settings we are to copy
         \param beglon the beginning longitude value
         \param beglat the beginning latitude value
         \param endlon the ending longitude value
         \param endlat  the ending latitude value
         \param deltah the along-line horizontal increment
         \param begz the beginning vertical coordinate value
         \param endz the ending vertical coordinate value
         \param deltaz the vertical coordinate increment
         
         \return a pointer to a deque of Parcel objects; the calling routine must delete this deque once it is no longer needed.
      */
       std::deque<Parcel>* create_deque(const Parcel& p
                           , real beglon, real beglat
                           , real endlon, real endlat, real deltah
                           , real begz, real endz, real deltaz 
                           );
      
      
      /// create a Flock container of Parcels
      /*! This method creates a Flock container of Parcels and initializes their locations from the givne Parcel
      
         \param parcel the input parcel whose settings we are to copy
         \param beglon the beginning longitude value
         \param beglat the beginning latitude value
         \param endlon the ending longitude value
         \param endlat  the ending latitude value
         \param deltah the along-line horizontal increment
         \param begz the beginning vertical coordinate value
         \param endz the ending vertical coordinate value
         \param deltaz the vertical coordinate increment
         \param pgrp a process-group object that is used for parallel processing
         \param r    the ratio of meteorological-data processors to parcel-tracing processors.
                     (For example, if r=3 then there will be one met processor for
                     every 3 parcel-tracing processors) 
         
         \return a pointer to a Flock of Parcel objects; the calling routine must delete this Flock once it is no longer needed.
      */
       Flock* create_Flock(const Parcel& parcel
                           , real beglon, real beglat
                           , real endlon, real endlat, real deltah
                           , real begz, real endz, real deltaz 
                           , ProcessGrp* pgrp=NULLPTR, int r=0
                           );
      
      
      /// create a Swarm pseudo-container of Parcels
      /*! This method creates a Swarm pseudo-container of Parcels and initializes their locations from the givne Parcel
      
         \param parcel the input parcel whose settings we are to copy
         \param beglon the beginning longitude value
         \param beglat the beginning latitude value
         \param endlon the ending longitude value
         \param endlat  the ending latitude value
         \param deltah the along-line horizontal increment
         \param begz the beginning vertical coordinate value
         \param endz the ending vertical coordinate value
         \param deltaz the vertical coordinate increment
         \param pgrp a process-group object that is used for parallel processing
         \param r    the ratio of meteorological-data processors to parcel-tracing processors.
                     (For example, if r=3 then there will be one met processor for
                     every 3 parcel-tracing processors) 
         
         \return a pointer to a Swarm of Parcel objects; the calling routine must delete this Swarm once it is no longer needed.
      */
       Swarm* create_Swarm(const Parcel& parcel
                           , real beglon, real beglat
                           , real endlon, real endlat, real deltah
                           , real begz, real endz, real deltaz 
                           , ProcessGrp* pgrp=NULLPTR, int r=0
                           );

   

   private:
      
      /// calculate the number of gridpoints needed
      /*! This method calculates the number of gridpoints needed.
      
         \param beglon the beginning longitude value
         \param beglat the beginning latitude value
         \param endlon the ending longitude value
         \param endlat  the ending latitude value
         \param deltah the along-line horizontal increment
         \param begz the beginning vertical coordinate value
         \param endz the ending vertical coordinate value
         \param deltaz the vertical coordinate increment
         
         \return the number of gridpoints
      */
      int count_gridpoints( const Parcel& p, real beglon, real beglat
                           , real endlon, real endlat, real datah
                           , real begz, real endz, real deltaz 
                           );
      

      /*! initialize the gridpoints in a sequence container
      
         \param seq the sequence container that holds our parcels
         \param p the input parcel whose settings we are to copy
         \param beglon the beginning longitude value
         \param beglat the beginning latitude value
         \param endlon the ending longitude value
         \param endlat  the ending latitude value
         \param deltah the along-line horizontal increment
         \param begz the beginning vertical coordinate value
         \param endz the ending vertical coordinate value
         \param deltaz the vertical coordinate increment
      */
      template< template<class U, class = std::allocator<U> > class Seq>
      void initgrid( Seq<Parcel>* seq, const Parcel& p
                         , real beglon, real beglat
                         , real endlon, real endlat, real deltah
                         , real begz, real endz, real deltaz  
                         );


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
