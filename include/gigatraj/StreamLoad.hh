
#ifndef STREAMLOAD_H
#define STREAMLOAD_H

#include <istream>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/ParcelInitializer.hh"

namespace gigatraj {

/*!
\ingroup parcelinitializers

\brief loads Parcel state information from an istream

The StreamLoad class reads information 
from an input stream and loads it into a Parcel
object or collection of Parcel objects.  Note that
the Parcel or collection of Parcels must already
exist.

StreamLoad works as a counterpart to the StreamDump class,
to make it easy to save and restore Parcels.
It uses the Parcel class's ">>" operator
to pull state information from input stream. This means
that the information in the stream is limited to the time, longitude,
latitude, status, and flags; all these must be present.


*/

class StreamLoad : public ParcelInitializer {

   private:
   
      // the stream from which parcel states are to be read
      std::istream *is;
      
      // 0 for a load from text, 1 for a binary load
      int mode;
      
   public:
   
      /// An exception for some kind of problem with the input
      class badinitstream: public ParcelInitializer::badinit {};

      /// Default constructor using cin
      /*! With the default StreamLoad constructor, the standard input stream cin 
          is used as the source of the Parcel state data to be loaded.
          
          \param format determines the format of the input: 0=text (the default), 1=binary.

      */
      StreamLoad( int format=0 );
      
      /// Alternate constructor using a specified istream
      /*! With this alternate constructor, the input stream is specified.
      
          \param input the input stream 

          \param format determines the format of the input: 0=text (the default), 1=binary.
          
      */
      StreamLoad( std::istream& input, int format=0  );      
   
      /// destructor
      /*!
          This is the destructor for the StreamLoad class
      */
      ~StreamLoad();
      
      /// copy constructor
      /*!
         This is the copy contructor method for the StreamLoad class.
         
         \param src the source StreamLoad object to copy from
      */
      StreamLoad(const StreamLoad& src);

      /// copy assignment
      /*! 
          This is the copy assignment operator for the StreamLoad class.
      */
      StreamLoad& operator=(const StreamLoad& src);

      /// copies settings from a source object to this one
      /*! 
           This method copies settings from a source StreamLoad object
           to this one.
           
           \param src the source StreamLoad object
      */     
      void assign( const StreamLoad& src);




      /// Returns/set the load format
      /*! This method returns (and optionally sets) the format of the load from either text
          or binary.
          
          \param fmt Set this to 0 for text loads, 1 for binary loads.
                     If negative (the default), the format will not be set form
                     this parameter, anbd the currentl value will be returned.
          
          \return the current format (0=text, 1=binary)
      */
      int format( int fmt = -1 );

      /// sets/returns the input stream   
      /*! this methos sets and.or returns the input stream being used for the load.
      
          \param input a pointer ot the new stream. If NULL, then the current stream
                 is retained unchanged.
          
          \return a pointer ot the input stream
      */           
      std::istream* stream( std::istream* input=NULL );

      /// Initialize a single Parcel
      /*! This method initializes a single Parcel object.
      
          \param p the Parcel object to be initialized

      */
      void apply( Parcel& p ); 

      /// Initialize an array of Parcels
      /*! This method initializes an array of Parcels.
      
          \param p the array of Parcel objects to be initialized
           
          \param n the number of Parcel objects to be initialized

      */
      void apply( Parcel * p, const int n ); 

      /// Initialize a vector of parcels
      /*! This method initializes a vector of Parcels
      
          \param p the vector of Parcel objects to be initialized
           
      */
      void apply( std::vector<Parcel>& p ); 

      /// Initialize a list of parcels
      /*! This method initializes a list of Parcels
      
          \param p the list of Parcel objects to be initialized
           
      */
      void apply( std::list<Parcel>& p ); 

      /// Initialize a deque of parcels
      /*! This method initializes a deque of Parcels
      
          \param p the deque of Parcel objects to be initialized
           
      */
      void apply( std::deque<Parcel>& p ); 


      /// Initialize a Flock of parcels
      /*! This method initializes a Flock of Parcels
      
          \param p the Flock of Parcel objects to be initialized
           
      */
      void apply( Flock& p ); 


      /// Initialize a Swarm of parcels
      /*! This method initializes a Swarm of Parcels
      
          \param p the Swarm of Parcel objects to be initialized
           
      */
      void apply( Swarm& p ); 



      /*! \brief override >> operator for single Parcels
          
          This operator override permits using the 
          common "stream >> object" syntax with StreamLoad objects
          as is used with a standard input stream object. 
          
          \param p a single Parcel to be read
          
      */
      StreamLoad& operator>>( Parcel& p )
      {
         apply( p );
         return *this;  
      }

      /*! \brief override >> operator for vectors of Parcels
          
          This operator override permits using the 
          common "stream >> object" syntax with StreamLoad objects
          as is used with a standard input stream object. 
          
          \param p a vector of Parcels to be Read
          
      */
      StreamLoad& operator>>( std::vector<Parcel>& p )
      {
         apply( p );
         return *this;  
      }

      /*! \brief override >> operator for lists of Parcels
          
          This operator override permits using the 
          common "stream >> object" syntax with StreamLoad objects
          as is used with a standard input stream object. 
          
          \param p a list of Parcels to be Read
          
      */
      StreamLoad& operator>>( std::list<Parcel>& p )
      {
         apply( p );
         return *this;  
      }

      /*! \brief override >> operator for deques of Parcels
          
          This operator override permits using the 
          common "stream >> object" syntax with StreamLoad objects
          as is used with a standard input stream object. 
          
          \param p a deque of Parcels to be Read
          
      */
      StreamLoad& operator>>( std::deque<Parcel>& p )
      {
         apply( p );
         return *this;  
      }

      /*! \brief override >> operator for Flocks of Parcels
          
          This operator override permits using the 
          common "stream >> object" syntax with StreamLoad objects
          as is used with a standard input stream object. 
          
          \param p a Flock of Parcels to be Read
          
      */
      StreamLoad& operator>>( Flock& p )
      {
         apply( p );
         return *this;  
      }

      /*! \brief override >> operator for Swarms of Parcels
          
          This operator override permits using the 
          common "stream >> object" syntax with StreamLoad objects
          as is used with a standard input stream object. 
          
          \param p a Swarm of Parcels to be Read
          
      */
      StreamLoad& operator>>( Swarm& p )
      {
         apply( p );
         return *this;  
      }


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
