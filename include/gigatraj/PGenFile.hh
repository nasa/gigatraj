#ifndef PGENFILE_H
#define PGENFILE_H

#include <iostream>
#include <istream>
#include <fstream>
#include <sstream>
#include <deque>
#include <list>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/Parcel.hh"
#include "gigatraj/ParcelGenerator.hh"
#include "gigatraj/StreamRead.hh"


namespace gigatraj {

/*!
\ingroup parcelgenerators

\brief generates collections of parcels from a file of positions

The PGenFile class generates collections of parcels whose positions
are initialized from a file.  

The input file is a plain text file with one line for each parcel
to be initialized.  The pound sign ("#") marks the beginning
of a comment; it and subsequent chacatcers are discarded.
Blank lines and lines consisting of white space, are discarded.
Any lines remaining are interpreted according to a format specification.

The default input format is three floating-point numbers, separated
by white space: longitude, latitude, and the vertical coordinate, 
in that order.

Alternate formats may be specified with the format() method.
This allows not only rearranging the attributes of the parcel,
but reading parcle state, flags, and a tag; as well as skipping
over extraneous numbers and strings.

It is perhaps worth noting that 
the main difference between this class and the StreamRead class is
that PGenFile is a parcel generator, which creates collections
of Parcels (arrays, vectors, lists, dequeues, and Flocks)
and initializes them from a file or an istream.
The StreamRead class reads from an istream and loads
the data into collections of Parcels that already exist.
Also, the StreamRead class requires every input line to have parcel data,
with no comments or blank lines.

*/

class PGenFile : public ParcelGenerator {

   public:
   
      /// the constrcutor
      /*! this is the constructor for the PGenFile class
      
          \param fmt an optional format specifier that describes what the input looks like
                     (See the format() method of the StreamRead class for more informtion.)
   
      */
      PGenFile( string fmt="" );
      
      /// the destructor
      /*! This is the destructor for the PGenFile class
      */
      ~PGenFile();

      /// \brief Copy-constructor
      /*!
         This is the copy contructor method for the Filter_Null class.
         
         Note that the source's input stream is not copied.
         
         \param src the source Filter_Null object to copy from
      */
      PGenFile(const PGenFile& src);

      /// \brief copy assignment
      /*!
          This is the copy assignment operator for the PGenFile class.
         
         Note that the source's input stream is not copied.
      */
      PGenFile& operator=(const PGenFile& src);


      /// copies settings from a source object to this one
      /*! 
           This method copies settings from a source PGenFile object
           to this one.
         
         Note that the source's input stream is not copied.
           
           \param src the source PGenFile object
      */     
      void assign( const PGenFile& src);
   
      /// read locations from a file into an array of Parcels
      /*! This method creates an array of Parcels and initializes their locations from a file.
      
         \param p the input parcel whose settings we are to copy
         \param np the number of parcels is returned in this variable
         \param file the name of the file from which parcel locations are to be read
         
         \return a Parcel pointer that points to an array of Parcel objects.
      */
      Parcel * create_array(const Parcel& p, int *np
                           , const std::string &file
                           ); 

      /// read locations from a file into an array of Parcels
      /*! This method creates an array of Parcels and initializes their locations from a file.
          This version uses an open istream and is thus suitable for use with stdin.
          Thus it can read the input only once, instead of reading the file to count
          parcels, then closing and re-opening the file to read the parcel positions.
          Consequently, it reads the parcels into a vector container first, then copies
          those parcels into a Flock. For large numbers of parcels, this can be inefficient.
      
         \param p the input parcel whose settings we are to copy
         \param np the number of parcels is returned in this variable
         \param file an already-open std::istream from which parcels are to be read.
         
         \return a Parcel pointer that points to an array of Parcel objects.
      */
      Parcel * create_array(const Parcel& p, int *np
                           , std::istream* file
                           ); 




      /// read locations from a file into a vector container of Parcels
      /*! This method creates a vector container of Parcels and initializes their locations from a file.
      
         \param p the input parcel whose settings we are to copy
         \param file the name of the file from which parcel locations are to be read
         
         \return a pointer to a vector of Parcel objects; the calling routine must delete this vector once it is no longer needed.
      */
       std::vector<Parcel>* create_vector(const Parcel& p
                           , const std::string &file
                           );
   
      /// read locations from a file into a vector container of Parcels
      /*! This method creates a vector container of Parcels and initializes their locations from a file.
          This version uses an open istream and is thus suitable for use with stdin.
      
         \param p the input parcel whose settings we are to copy
         \param file an already-open std::istream from which parcels are to be read.
         
         \return a pointer to a vector of Parcel objects; the calling routine must delete this vector once it is no longer needed.
      */
       std::vector<Parcel>* create_vector(const Parcel& p
                           , std::istream* file
                           );
      




      /// read locations from a file into a list container of Parcels
      /*! This method creates a list container of Parcels and initializes their locations from a file.
            
         \param p the input parcel whose settings we are to copy
         \param file the name of the file from which parcel locations are to be read
         
         \return a pointer to a list of Parcel obejcts; the calling routine must delete this list once it is no longer needed.
      */
       std::list<Parcel>* create_list(const Parcel& p
                           , const std::string &file 
                           );
      
      /// read locations from a file into a list container of Parcels
      /*! This method creates a list container of Parcels and initializes their locations from a file.
          This version uses an open istream and is thus suitable for use with stdin.
            
         \param p the input parcel whose settings we are to copy
         \param file an already-open std::istream from which parcels are to be read.
         
         \return a list of Parcel objects; the calling routine must delete this list once it is no longer needed.
      */
       std::list<Parcel>* create_list(const Parcel& p
                           , std::istream* file
                           );
      



      /// read locations from a file into a deque container of Parcels
      /*! This method creates a deque container of Parcels and initializes their locations from a file.
      
         \param p the input parcel whose settings we are to copy
         \param file the name of the file from which parcel locations are to be read
         
         \return a pointer to a deque of Parcel objects; the calling routine must delete this deque once it is no longer needed.
      */
       std::deque<Parcel>* create_deque(const Parcel& p
                           , const std::string &file
                           );
      
      
      /// read locations from a file into a deque container of Parcels
      /*! This method creates a deque container of Parcels and initializes their locations from a file.
      
         \param p the input parcel whose settings we are to copy
         \param file an already-open std::istream from which parcels are to be read.
         
         \return a pointer to a deque of Parcel objects; the calling routine must delete this deque once it is no longer needed.
      */
       std::deque<Parcel>* create_deque(const Parcel& p
                           , std::istream* file
                           );
      



      /// read locations from a file into a Flock container of Parcels
      /*! This method creates a Flock container of Parcels and initializes their locations from a file.
          This version uses an open istream and is thus suitable for use with stdin.
      
         \param p the input parcel whose settings we are to copy
         \param file the name of the file from which parcel locations are to be read
         \param pgrp a process-group object that is used for parallel processing
         \param r    the ratio of meteorological-data processors to parcel-tracing processors.
                     (For example, if r=3 then there will be one met processor for
                     every 3 parcel-tracing processors) 
         
         \return a pointer to a Flock of Parcel objects; the calling routine must delete this Flock once it is no longer needed.
      */
       Flock* create_Flock(const Parcel& p
                           , const std::string &file, ProcessGrp* pgrp=NULLPTR, int r=0
                           );
      
      
      /// read locations from a file into a Flock container of Parcels
      /*! This method creates a Flock container of Parcels and initializes their locations from a file.
          This version uses an open istream and is thus suitable for use with stdin.
          Thus it can read the input only once, instead of reading the file to count
          parcels, then closing and re-opening the file to read the parcel positions.
          Consequently, it reads the parcels into a vector container first, then copies
          those parcels into a Flock. For large numbers of parcels, this can be inefficient.
      
         \param p the input parcel whose settings we are to copy
         \param file an already-open std::istream from which parcels are to be read.
         \param pgrp a process-group object that is used for parallel processing
         \param r    the ratio of meteorological-data processors to parcel-tracing processors.
                     (For example, if r=3 then there will be one met processor for
                     every 3 parcel-tracing processors) 
         
         \return a pointer to a Flock of Parcel objects; the calling routine must delete this Flock once it is no longer needed.
      */
       Flock* create_Flock(const Parcel& p
                           , std::istream* file, ProcessGrp* pgrp=NULLPTR, int r=0
                           );

      

      /// read locations from a file into a Swarm pseudo-container of Parcels
      /*! This method creates a Swarm pseudo-container of Parcels and initializes their locations from a file.
          This version uses an open istream and is thus suitable for use with stdin.
      
         \param p the input parcel whose settings we are to copy
         \param file the name of the file from which parcel locations are to be read
         \param pgrp a process-group object that is used for parallel processing
         \param r    the ratio of meteorological-data processors to parcel-tracing processors.
                     (For example, if r=3 then there will be one met processor for
                     every 3 parcel-tracing processors) 
         
         \return a pointer to a Swarm of Parcel objects; the calling routine must delete this Swarm once it is no longer needed.
      */
       Swarm* create_Swarm(const Parcel& p
                           , const std::string &file, ProcessGrp* pgrp=NULLPTR, int r=0
                           );
      
      /// read locations from a file into a Swarm pseudo-container of Parcels
      /*! This method creates a Swarm pseudo-container of Parcels and initializes their locations from a file.
          This version uses an open istream and is thus suitable for use with stdin.
          Thus it can read the input only once, instead of reading the file to count
          parcels, then closing and re-opening the file to read the parcel positions.
          Consequently, it reads the parcels into a vector pseudo-container first, then copies
          those parcels into a Swarm. For large numbers of parcels, this can be inefficient.
      
         \param p the input parcel whose settings we are to copy
         \param file an already-open std::istream from which parcels are to be read.
         \param pgrp a process-group object that is used for parallel processing
         \param r    the ratio of meteorological-data processors to parcel-tracing processors.
                     (For example, if r=3 then there will be one met processor for
                     every 3 parcel-tracing processors) 
         
         \return a pointer to a Swarm of Parcel objects; the calling routine must delete this Swarm once it is no longer needed.
      */
       Swarm* create_Swarm(const Parcel& p
                           , std::istream* file, ProcessGrp* pgrp=NULLPTR, int r=0
                           );


      /// sets the format of the input.
      /*! This method specifies what the input should look like.
          (See the format() method of the StreamRead class for more information.)
          
          \param fmt the format specifier string. If empty (""), then the default is used.

      */
      void format( const string fmt );

      /// returns the input format specification
      /*! This method returns the input format specifiation.
      
          \return a string containing the current format.
      */
      std::string format() const;


   private:
   
      /// read the next parcel location from a file
      /*! This method reads precisely one parcel location from an open istream.
      
         
         \param stream a pointer to the istream from which the parcel is to be read
         \param p a pointer to a Parcel to receive the input information
         
         \return  a status code:
            *   0 = a parcel was read
            *   1 = a blank or comment line was read
            *  -1 = the end of the file was encountered
            *  -2 = a formatting or other input error was encountered
      */
       int readparcel( std::istream* input, Parcel *p );


      /// read locations from a file into a sequence container of Parcels
      /*! This method reads locations from an open istream into Parcels
          which are stored in some kind of sequence container object.
      
         \param seq the sequence container that holds the parcels
         \param p the input parcel whose settings we are to copy
         \param np the number of parcels read is returned in this variable
         \param input the input stream from which we are to read
      */
      template< template<class U, class = std::allocator<U> > class Seq>
      void initbunch( Seq<Parcel>* seq, const Parcel& p, int *np, std::istream* input
                    );                        

      // a StreamRead object used for formatted input
      StreamRead *interpretor;

      // a string input stream, for use with the StreamRead interpretor
      std::istringstream instring;
      

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
