#ifndef GIGATRAJ_FILTER_TROP_H
#define GIGATRAJ_FILTER_TROP_H


#include "gigatraj/gigatraj.hh"
#include "gigatraj/ParcelFilter.hh"
#include "gigatraj/PAltOTF.hh"

namespace gigatraj {

/*!

\ingroup parcelfilters

\brief filters parcels that cross the tropopause

The Filter_Trop class provides a means for disabling parcels
that cross the tropopause. The filtering takes the form of setting 
a Parcel's NoTrace flag.  Parcels that already have their NoTrace 
flag set are left unaffected.


*/

class Filter_Trop {

   public:
   
      /// the constructor
      /*! This is the constructor for Filter_Trop objects.
           
          \param direction  sets the direction of the parcels to be filtered:
                            -1 filters out parcels above the tropopause, leaving those above it
                            +1 filters out parcels below the tropopause, leaving those below it
                            0 filters out parcels away from the tropopause in either direction, leaving those close toit
          
          \param tolerance the distance from the tropopause 
                           (in whatever vertical coordinate is being used) within which 
                           a parcel must be in order to be filtered.
          
          \param quantity the name of the quantity in the parcels' meteorological data source                 
                          that corresponds to the tropopause to be used.
                          (This allows a choice for those data sets that have
                          multiple different kinds of tropopause products.)  
                                             
          \param field    the name of the quantity in the parcels' meteorological data source 
                          that corresponds to the three-dimensional field from
                          which the tropopause is extracted. For example, if
                          the tropopause pressure is "tropp" and the name of pressure
                          in the met data source is "p", then quantity would
                          be set to "tropp", and field would be set to "p".              

      */
      Filter_Trop( int direction=0, real tolerance=0.1, std::string quantity="", std::string field="");

      /// the destructor
      /*! This is the destructor for the Filter_Trop class
      */
      ~Filter_Trop();
      
      /// return the tropopause quantity
      /*! This method returns the name of the tropopause quantity
      
           returns the name
      */      
      std::string quantity() const;
      
      /// set the quantity
      /*! This method sets the name used by the meteorological data source
          to obtain its its tropopause values.
      */    
      void quantity( std::string q );

      /// return the tropopause field
      /*! This method returns the name of the vertical coordinate quantity field
          in which the tropopause is expressed.
      
           returns the name
      */      
      std::string field() const;
      
      /// set the field
      /*! This method sets the name used by the meteorological data source
          as the vertical coordinate in which the tropopause is expressed.
      */    
      void field( std::string q );

      /// return the direction
      /*! This method returns the direction in which to filter Parcels
      
           \return the direction: 
                * -1 = Parcels beyond the threshold distance below the tropopause are set to noTrace
                *  0 = Parcels beyond the threshold distance above or below the tropopause are set to NoTrace
                *  1 = PArcels beyond the threshold distance above the tropopause are set to NoTrace
      */      
      int direction() const;
      
      /// set the direction
      /*! This method sets the direction in which Parcels will be filtered.
           
           \param d the direction: 
                * -1 = Parcels beyond the threshold distance below the tropopause will be set to noTrace
                *  0 = Parcels beyond the threshold distance above or below the tropopause will be set to NoTrace
                *  1 = PArcels beyond the threshold distance above the tropopause will be set to NoTrace
                * (other values are ignored)

      */    
      void direction( int d );

      /// return the threshold distance
      /*! This method returns the threshold distance for the tropopause, beyond which
          Parcels are filtered.
      
           \return the threshold distance, in the Parcels' vertical coordinate.
      */      
      real threshold() const;
      
      /// set the threshold distance
      /*! This method sets the threshold distance for the tropopause, beyond which
          Parcels will be filtered.
           
           \param t the threshold distance, in the Parcels' vertical coordinate.

      */    
      void threshold( real t );

      /// return the negation flag
      /*! This method returns whether the filter is to be negated,
          so that Parcels which would be set to NoTrace are set to Trace,
          and vice versa.

          (Note that Parcels that were already set to NoTrace before being
          filtered remain at NoTrace.)
      
           \return the current value of the negation flag. True means that the 
           fitler test condition will be negated; false means that it will not be negated.
      */      
      bool negation() const;
      
      /// sets or clears the negation flag
      /*! This method sets whether the filter results will be negated,
          so that Parcels which would be set to NoTrace are set to Trace,
          and vice versa.
          
          (Note that Parcels that were already set to NoTrace before being
          filtered remain at NoTrace.)
           
           \param n the new value of the negation flag. True means that the 
           fitler test condition will be negated; false means that it will not be negated.

      */    
      void negation( bool n );


      /// method for applying applying the filter to a single Parcel
      /*! Apply the filter to a single parcel
      
      \param p the Parcel objects to which the filter is to be applied
    
      */
      void apply( Parcel& p ); 
      
      /// method for applying applying the filter to an array of Parcels
      /*! Apply the filter to an array of parcels
      
      \param p the array of Parcel objects to which the filter is to be applied
    
      \param n the number of Parcel objects in the array

      */
      void apply( Parcel * const p, const int n ); 
   
      /// method for applying applying the filter to a vector of Parcels
      /*! Apply the filter to a vector of Parcels
      
      \param p the vector of Parcel objects to which the filter is to be applied
    
      */
      void apply( std::vector<Parcel>& p ); 
   
      /// method for applying applying the filter to a list of Parcels
      /*! Apply the filter to a list of Parcels
      
      \param p the list of Parcel objects to which the filter is to be applied
    
      */
      void apply( std::list<Parcel>& p ); 
   
      /// method for applying applying the filter to a deque of Parcels
      /*! Apply the filter to a deque of Parcels
      
      \param p the deque of Parcel objects to which the filter is to be applied
    
      */
      void apply( std::deque<Parcel>& p ); 
   
      /// method for applying applying the filter to a Flock of Parcels
      /*! Apply the filter to a Flock of Parcels
      
      \param p the Flock of Parcel objects to which the filter is to be applied
    
      */
      void apply( Flock& p ); 
   
      /// method for applying applying the filter to a Swarm of Parcels
      /*! Apply the filter to a Swarm of Parcels
      
      \param p the Swarm of Parcel objects to which the filter is to be applied
    
      */
      void apply( Swarm& p ); 
      
    private:
    
       MetData* datsrc;
       
       /// the filter direction
       int dir;
       
       /// threshold distance from the tropopause
       real tol;

       // negate he filter before applying it tothe Parcels?       
       bool neg;
       
       
       /// the name of the tropopause quantity to request
       std::string tquant;
       
       /// the name of the physical quantity that tquant is
       std::string tkind;
      
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
