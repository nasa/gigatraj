
#ifndef STREAMINIT_H
#define STREAMINIT_H

#include <istream>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/ParcelInitializer.hh"

namespace gigatraj {

/*!
\ingroup parcelinitializers

\brief loads Parcel state information from an istream

The StreamRead class reads information 
from an input stream and loads it into a Parcel
object or collection of Parcel objects.  Note that
the Parcel or collection of Parcels must already
exist.

StreamRead serves as a counterpart to the StreamPrint class.
It reads in the input stream, interpreting it 
according to a format specification.
Thus a high degrees of flexibility is allowed in how the information
in the input stream is arranged. Not all of the Parcel state has to
be specified, for example. That which is not specified is
inherited from a sample parcel.

*/

class StreamRead : public ParcelInitializer {

   private:
   
      // the stream from which parcel states are to be read
      std::istream *is;
      
      // holds format specifcations
      class FmtSpec {
      
         public:
         
          /* gives the type of thing to format
              "L" = literal; plug in the format as-is
              "T" = Calendar time string: extract characters start through ( start + len - 1 )
              "t" = model time: input model time, using len and float for total length and number-of-decimal-places
              "o" = longitude: input longitude, using len and float for total length and number-of-decimal-places
              "a" = latitude: input latitude, using len and float for total length and number-of-decimal-places
              "v" = vertical: input vertical coordinate, using len and float for total length and number-of-decimal-places
              "g" = tag value: input parcel tag, using len and float for total length and number-of-decimal-places
              "f" = flags: input the Parcel flags
              "s" = flags: input the Parcel state
              "i" = index: input an index number and discard it
              "{field}m" = input a floating=pint number and discard it
          */
          std::string type;
          // starting character for string insertion
          int start;
          // length of string insertion, or number of characters in numeric string
          int len;
          // number of decimal places to read in numeric string
          int fract;
          // (ignored; accepted only for compatibilit with StreamPrint)
          int align;
          // contains literal string
          std::string str;
          // contains the name of a meteorological field quantity
          std::string field;
          
          FmtSpec(const std::string type0="L", int start0=0, int len0=-1, int fract0=-1, std::string str0="", int align0=1 );

          /// Default destructor
          /*!
             This is the default destructor for the FmtSpec class.
          */
          ~FmtSpec();
      
          /// copy constructor
          /*!
             This is the copy contructor method for the FmtSpec class.
             
             \param src the source FmtSpec object to copy from
          */
          FmtSpec(const FmtSpec& src);

          /// copy assignment
          /*! 
              This is the copy assignment operator for the FmtSpec class.
          */
          FmtSpec& operator=(const FmtSpec& src);

          /// copies settings from a source object to this one
          /*! 
               This method copies settings from a source FmtSpec object
               to this one.
               
               \param src the source FmtSpec object
          */     
          void assign( const FmtSpec& src);
          
          
          void defaults(std::string type0="L", int start0=0, int len0=-1, int fract0=-1, std::string str0="", int align0=1 );
          
      };
   

      // specifies the format type of the output:
      std::vector<FmtSpec *> fmt;

      // number of format elements
      int nf;

      // clear the format
      void clearFormat();

      // converts a string to an integer
      int s2i( const std::string str );

      // converts an integer to a string
      std::string i2s( int i ) const;

      // ingests parcel data from an input string 
      void ingest( Parcel& p );
      
      // reads a double from the string
      int s2d( const std::string &str, double *result, int beg=0, int wid=-1, int dec=-1 ); 

      // reads an integer from the string
      int s2int( const std::string &str, int *result, int beg=0, int wid=-1 ); 

      // reads a sequence of characters to skip from the string
      int s2c( const std::string &str, int mode, string &sval, int beg=0, int wid=-1 ); 
      
   public:
   
      /// An exception for some kind of problem with the input
      class badinitstream: public ParcelInitializer::badinit {};

      /// An exception for some kind of problem with the format specification
      class badinitformat: public ParcelInitializer::badinit {};

      /// An exception for when the input does not match the format
      class badinitrejected: public ParcelInitializer::badinit {};
   
      /// Default constructor using cin
      /*! With the default StreamRead constructor, the standard input stream cin 
          is used as the source of the Parcel state data to be loaded.

          \param fmtstr the formatting that determines how the parcel information is to be
                 printed. See the format() method for a description of the syntax.
      
      */
      StreamRead( const std::string fmtstr="" );
      
      /// Alternate constructor using a specified istream
      /*! With this alternate constructor, the input stream is specified.
      
          \param input the input stream 

          \param fmtstr the formatting that determines how the parcel information is to be
                 printed. See the format() method for a description of the syntax.
      */
      StreamRead( std::istream& input, const std::string fmtstr=""  );      
   
      /// destructor
      /*!
          This is the destructor for the StreamRead class
      */
      ~StreamRead();
      
      /// copy constructor
      /*!
         This is the copy contructor method for the StreamRead class.
         
         \param src the source StreamRead object to copy from
      */
      StreamRead(const StreamRead& src);

      /// copy assignment
      /*! 
          This is the copy assignment operator for the StreamRead class.
      */
      StreamRead& operator=(const StreamRead& src);

      /// copies settings from a source object to this one
      /*! 
           This method copies settings from a source StreamRead object
           to this one.
           
           \param src the source StreamRead object
      */     
      void assign( const StreamRead& src);


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

      /// set the format for how a parcel state is to be read in
      /*! This method allows the user to specify the format in which a parcel's information is to be read in.
          The specification takes the form of a string with the following format:
          
          * Any text up to a "%" character is read from the inut, matched against the format, and discarded.
            If the input does not match the format, then an exception is thrown.
          
          * "%%" reads a "%" from the input and discards it. If the input character is not "%", an exception is thrown.
          
          * "%T" read the parcel's time, in the form of a (MetData-source-specific) Calendar time string, from
                 the input. Just plain "%T" reads the entire time string (i.e., all characters until
                 whitespace is encountered). Alternatively, fragments of a time string may be read in,
                 and then substituted into the correspnding characters in the parcel's current time specification.
                 A number inserted before the "T" (e.g., "%4T") is used as the length of the input, 
                 starting with the zeroeth character by default. If two numbers, separated by a period, are inserted
                 before the "T", the first number is the starting character if the time string string in pull from the input,
                 and the second number is the number of characters to pull. For example, if the parcel's current time
                 (before the input is read) is equivalent to "2017-03-14T14:35:15", and the input is "05-23T09",
                 the the format code "%5.8T" will result in the parcel's time being set to the equivalent of
                 "2017-05-23T09:35:15". (Note that StreamRead knows nothing about the semantics
                 of the calendar string.) A minus sign may be inserted just after the "%", but it will be ignored.
          
         * "%t" reads the parcel's time, as internal model time (not as a calendar time string).
           A number can be inserted in front of the "t" to specify the total length of the input field.
           If two numbers separated by a period are inserted, the first is an overall length
           and the second is the number of decimal places. A "-" inserted just after the "%" will be ignored.
          
         * "%o" reads the parcel's longitude, in numeric form positive east.
           A number can be inserted in front of the "o" to specify the total length of the input field.
           If two numbers separated by a period are inserted, the first is an overall length
           and the second is the number of decimal places. A "-" inserted just after the "%" is ignored.
          
         * "%a" reads the parcel's latitude, in numeric form positive north.
           A number can be inserted in front of the "a" to specify the total length of the input field.
           If two numbers separated by a period are inserted, the first is an overall length
           and the second is the number of decimal places. A "-" inserted just after the "%" is ignoed.
          
         * "%v" reads the parcel's vertical coordinate, according to the coordinate system being
           used to trace the parcels.
           A number can be inserted in front of the "v" to specify the total length of the input field.
           If two numbers separated by a period are inserted, the first is an overall length
           and the second is the number of decimal places. A "-" inserted just after the "%" is ignored.
          
         * "%g" reads the parcel's tag value.
           A number can be inserted in front of the "g" to specify the total length of the input field.
           If two numbers separated by a period are inserted, the first is an overall length
           and the second is the number of decimal places. A "-" inserted just after the "%" is ignored.
         
         * "%f" reads the parcel's flags. A number can be inserted in front of the "f" to specify the 
            total length of the input field.
          
         * "%s" reads the parcel's status. A number can be inserted in front of the "s" to specify the 
            total length of the input field.
          
         * "%i" reads the parcel's index number. Because such an index is not part of an parcel's state     
           (but assigned and managed by the model that tracks the parcels), this index is discarded ater    
           reading. This code exists for compatibility with the StreamPrint class's format specifiers.      
           A number can be inserted in front of the "i" to specify the total length of the input field.    
                                                                                                            
         * "%{fieldname}m" reads the value of a meteorological field at the Parcel's location and time.     
           Since such a meteorological field value is not part of the Parcel's internal state, this value   
           is discarded. This code exists for compatibility with the StreamPrint class's format specifiers. 
           A number can be inserted in front of the "{" to specify the total length of the input field.     
           If two numbers separated by a period are inserted, the first is an overall length                
           and the second is the number of decimal places. A "-" inserted just after the "%" is ignored.    
          
          * "%c" reads and discards characters. 
             A number can be inserted in front of the "c" to specify the total length of the input field. 
             If the width is specified, then that many characters are read from the input and discarded.
             Otherwise, characters are read and discarded until either white space, a number, or the end of the line
             is encountered; numbers begin with a digit "0"-"9", a sign ("+" or "-"), or a decimal point (".").    
          
          * "%x" reads and discards characters. 
             A number can be inserted in front of the "x" to specify the total length of the input field. 
             If the width is specified, then that many characters are read from the input and discarded.
             Otherwise, characters (including white space) are read and discarded until a number
             or the end of line is encountered; numbers begin with a digit "0"-"9", a sign ("+" or "-"), or a decimal point (".").    
          
          For example, suppose a parcel to be traced isentropically is to be initialized from
          an input text string "Time: 2012-03-12T15:10:33 (       12.3455), Position (-40.56 ,24.335,457.00)".
          The format "Time: %T (%14.5t), Position (%-7.2o, %-6.2a, %v)" may be used, The Parcel's time
          will be set to the internal model time that corresponds to "2012-03-12T15:10:33" in the Gregorian
          calendar, then that time will be overwritten by the internal model time 12.3455.
          The Parcel's latitude is set to 24.335, its longitude to -40.56, and its vertical coordinate
          (presumably potential temperature) set to 457.00.
          
          Note that if the format string contains a newline character ("\n"), the format string is
          considered to end wth the previous character, after all subsequent characters are
          discarded.  Also, whitespace is not equivalent: spaces are considered different characters 
          from tabs.
          
          When a StreamRead object reads from the input stream using a format specification string,
          be aware that any parcel attributes not specified in the format string will be left
          unaltered. Also, the input stream is read in line by line, so that any portion of
          an input line beyond the last piece of information specified by the format string,
          is discarded. 
          
          \param fmtstr the format string. If the string is empty (""), then the default format is used.
          
      */
      void format( const std::string fmtstr );

      /// returns the format string, if any
      /*! This method returns the format string that is being used
          to interpret the input.
          
          \return the format string, as described in the void format() method. 
      */    
      std::string format() const;




      /*! \brief override >> operator for single Parcels
          
          This operator override permits using the 
          common "stream >> object" syntax with StreamRead objects
          as is used with a standard input stream object. 
          
          \param p a single Parcel to be read
          
      */
      StreamRead& operator>>( Parcel& p )
      {
         apply( p );
         return *this;  
      }

      /*! \brief override >> operator for vectors of Parcels
          
          This operator override permits using the 
          common "stream >> object" syntax with StreamRead objects
          as is used with a standard input stream object. 
          
          \param p a vector of Parcels to be Read
          
      */
      StreamRead& operator>>( std::vector<Parcel>& p )
      {
         apply( p );
         return *this;  
      }

      /*! \brief override >> operator for lists of Parcels
          
          This operator override permits using the 
          common "stream >> object" syntax with StreamRead objects
          as is used with a standard input stream object. 
          
          \param p a list of Parcels to be Read
          
      */
      StreamRead& operator>>( std::list<Parcel>& p )
      {
         apply( p );
         return *this;  
      }

      /*! \brief override >> operator for deques of Parcels
          
          This operator override permits using the 
          common "stream >> object" syntax with StreamRead objects
          as is used with a standard input stream object. 
          
          \param p a deque of Parcels to be Read
          
      */
      StreamRead& operator>>( std::deque<Parcel>& p )
      {
         apply( p );
         return *this;  
      }

      /*! \brief override >> operator for Flocks of Parcels
          
          This operator override permits using the 
          common "stream >> object" syntax with StreamRead objects
          as is used with a standard input stream object. 
          
          \param p a Flock of Parcels to be Read
          
      */
      StreamRead& operator>>( Flock& p )
      {
         apply( p );
         return *this;  
      }

      /*! \brief override >> operator for Swarms of Parcels
          
          This operator override permits using the 
          common "stream >> object" syntax with StreamRead objects
          as is used with a standard input stream object. 
          
          \param p a Swarm of Parcels to be Read
          
      */
      StreamRead& operator>>( Swarm& p )
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
