#ifndef PGENDISC_H
#define PGENDISC_H

#include <cstdlib>
#include <deque>
#include <list>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/Earth.hh"
#include "gigatraj/RandomSrc.hh"
#include "gigatraj/Parcel.hh"
#include "gigatraj/ParcelGenerator.hh"

namespace gigatraj {

/*!
\ingroup parcelgenerators

\brief generates collections of parcels regularly distributed within a disc

The PGenDisc class generates collections of parcels that are 
distributed horizontally within a disc of a given radius and thickness,
centered on a given longitude and latitude.
The distribution of points takes into account the convergence
of longitude lines at the pole.

Aside from the horizontal and vertical positions, all other characteristics
of the Parcels are copied from an input parcel. 

*/

class PGenDisc : public ParcelGenerator {

   public:
   
      /// default constructor
      /*! This is the default constructor for the PGenDisc class.
          The longitude and latitude are both initialized to zero.
          The radius is initialized to 50 km.
          The thickness is initialized to 1e-10.
      */    
      PGenDisc();
      
      /// constructor with specifications
      /*! This version of the constructor permits specifying the central latitude and
          longitude, radius, and thickness.
      
          \param lon the central longitude of the disc
          \param lat the central latitude of the disc
          \param level = the central vertical level of the disc
          \param r the radius of the disk, in km
          \param thickness the thickness of the disk, in current vertical coordinates
          
      */
      PGenDisc( const real lon, const real lat, const real level, const real r=50.0, const real thickness=1e-10 );

      /// \brief destructor
      /*! 
         This is the destructor method for the PGenDisc class.
      */
      ~PGenDisc(); 

      /// \brief Copy-constructor
      /*!
         This is the copy contructor method for the Filter_Null class.
         
         \param src the source Filter_Null object to copy from
      */
      PGenDisc(const PGenDisc& src);

      /// \brief copy assignment
      /*!
          This is the copy assignment operator for the PGenDisc class.
      */
      PGenDisc& operator=(const PGenDisc& src);


      /// copies settings from a source object to this one
      /*! 
           This method copies settings from a source PGenDisc object
           to this one.
           
           \param src the source PGenDisc object
      */     
      void assign( const PGenDisc& src);
      

      /// returns the central location of the disc
      /*! This method returns the central location of the disc.
      
          \param lon (output) the central longitude
          \param lat (output) the central latitude
          \param level (output) the central vertical level
      */
      const void center( real &lon, real &lat, real &level );    

      /// sets the central location of the disc
      /*! This method sets the central location of the disc.
      
          \param lon the central longitude
          \param lat the central latitude
          \param level the central vertical level
      */
      void set_center( const real lon, const real lat, const real level ); 

      /// returns the size of the disc
      /*! This method returns the radius and thickness of the disc
      
          \param r (output) the radius of the disk, in km
          \param thickness (output) the thickness of the disc, in current vertical coordinates
          
      */
      const void size( real &r, real &thickness );
      
      /// sets the size of the disc
      /*! This method sets the radius and thickness of the disc.
      
          \param r the radius of the disk, in km
          \param thickness the thickness of the disc, in current vertical coordinates
      */
      void set_size( const real r, const real thickness );
      
      
      /// Create an array of Parcels 
      /*! This method creates an array of randomly-distributed Parcels. 
      
         \param parcel copy this parcel
         \param n the number of parcels to create
         
         \return a Parcel pointer that points to an array of Parcel objects.
      */
      Parcel * create_array(Parcel parcel, int n );   


      /// Create an vector Container of Parcel objects 
      /*! This method creates a vector Container of randomly-distributed Parcel objects. 
      
        \param parcel copy this parcel
        \param n  the size of the vector object; the number of parcels to create
         
         \return a pointer to a vector of Parcel objects; the calling routine must delete this vector once it is no longer needed.
      */
       std::vector<Parcel>* create_vector(Parcel parcel, int n );
      
      /// Create an list Container of Parcel objects 
      /*! this method creates a list Container of randomly-distributed Parcel objects.
      
        \param parcel copy this parcel
        \param n  the size of the list object; the number of parcels to create
         
         \return a pointer to a list of Parcel obejcts; the calling routine must delete this list once it is no longer needed.
      */
       std::list<Parcel>* create_list(Parcel parcel, int n );
      
      /// Create an deque Container of Parcel objects 
      /*! This method creates a deque Container of randomly-distributed Parcel objects 
      
        \param parcel copy this parcel
        \param n  the size of the deque object; the number of parcels to create
         
         \return a pointer to a deque of Parcel objects; the calling routine must delete this deque once it is no longer needed.
      */
       std::deque<Parcel>* create_deque(Parcel parcel, int n );
         
      
      /// read locations randomly into a Flock container of Parcels
      /*! This method creates a Flock container of Parcels and initializes their locations randomly (horizontally) at a given level.
      
         \param parcel the input parcel whose settings we are to copy
         \param n  the size of the Flock object; the number of parcels to create
         \param pgrp a process-group object that is used for parallel processing
         \param r    the ratio of meteorological-data processors to parcel-tracing processors.
                     (For example, if r=3 then there will be one met processor for
                     every 3 parcel-tracing processors) 
         
         \return a pointer to a Flock of Parcel objects; the calling routine must delete this Flock once it is no longer needed.
      */
       Flock* create_Flock(const Parcel& parcel, int n
                           , ProcessGrp* pgrp=NULLPTR, int r=0
                           );
      
      /// read locations randomly into a Swarm pseudo-container of Parcels
      /*! This method creates a Swarm pseudo-container of Parcels and initializes their locations randomly (horizontally) at a given level.
      
         \param parcel the input parcel whose settings we are to copy
         \param n  the size of the Swarm object; the number of parcels to create
         \param pgrp a process-group object that is used for parallel processing
         \param r    the ratio of meteorological-data processors to parcel-tracing processors.
                     (For example, if r=3 then there will be one met processor for
                     every 3 parcel-tracing processors) 
         
         \return a pointer to a Swarm object; the calling routine must delete this Swarm once it is no longer needed.
      */
       Swarm* create_Swarm(const Parcel& parcel, int n
                           , ProcessGrp* pgrp=NULLPTR, int r=0
                           );
      
   private:
   
      /// the centrla longitude
      real lon0;
      /// the central latitude
      real lat0;
      /// the vertical level of the disk, in the current vertical coorindates
      real z0;
      /// the disc radius, in km
      real rad;
      /// the disc thickness, in vertical coordinates
      real thk;
      /// the preferred minimum seapration of parcels
      real min_sep;
      /// the absolute minimum separation of parcels
      real abs_min_sep;
      
      // list of circle info for each layer
      std::vector<int> layer_circle_spec;
      
      /// an internal table of how many points can fit in on any of a set of concentric circles
      std::vector<int> n_in_circ;
      /// an internal table of how many points can fit within a any of set of concentric circles
      std::vector<int> cumul_n_in_circ;
   
   
      /// initialize parcels positions in a sequence container
      /*! initialize parcels positions in a sequence container
      
         \param seq the sequence container that holds our parcels
         \param p the input parcel whose settings we are to copy
      */
      template< template<class U, class = std::allocator<U> > class Seq>
      void init( Seq<Parcel>* seq
                         );

      /// initializes internal tables
      void startup();
      
      /// calculates the number of points in each layer of the disk
      void calc_p( const int n );

      /// calculates the lat, lonb, and z of the next parcel
      void next_coords( real &lat, real &lon, real &z, int p_no, const int n, PlanetNav *nav );
      

   
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
