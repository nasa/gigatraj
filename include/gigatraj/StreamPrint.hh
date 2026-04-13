#ifndef STREAMPRINT_H
#define STREAMPRINT_H

#include <ostream>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/ParcelReporter.hh"
#include "gigatraj/MetData.hh"

namespace gigatraj {

/*!
\ingroup parcelreporters

\brief prints Parcel state information to an ostream

The StreamPrint class prints out parcel information, in human-readable form, to an ostream.

It has a counterpart for input, the StreamRead class.


*/

class StreamPrint : public ParcelReporter {

   private:
   
      // the stream to which parcel states are to be printed
      std::ostream *os;
      
   public:
   

      /// An exception for some kind of problem with the output
      class badstreamprint: public ParcelReporter::badreport {};

      /// An exception for a bad print format
      class badprintformat{};


      /// Default constructor using cout
      /*! With the default constructor, the standard output stream cout
          is used as the destination of the Parcel state data being printed.
          
          \param fmtstr the formatting that determines how the parcel information is to be
                 printed. See the format() method for a description of the syntax.
      
      */
      StreamPrint( const std::string fmtstr="%T: %o, %a, %v\n" );
      
      /// Alternate constructor using a specified ostream
      /*! With this alternate constructor, the output stream is specified.
      
         \param output the output stream 
         \param fmtstr the format specification string
      */
      StreamPrint( std::ostream& output, const std::string fmtstr="%T: %o, %a, %v\n"  );      
   
      /// destructor
      /*! This is the destructor method for the StreamPrint object
      */
      ~StreamPrint();         
      
      /// copy constructor
      /*!
         This is the copy contructor method for the StreamPrint class.
         
         \param src the source StreamPrint object to copy from
      */
      StreamPrint(const StreamPrint& src);

      /// copy assignment
      /*! 
          This is the copy assignment operator for the StreamPrint class.
      */
      StreamPrint& operator=(const StreamPrint& src);

      /// copies settings from a source object to this one
      /*! 
           This method copies settings from a source StreamPrint object
           to this one.
           
           \param src the source StreamPrint object
      */     
      void assign( const StreamPrint& src);




   
      /// omitNonTraced
      /*! This method sets a flag that prevents parcels that are not being traced from being output.
          The default is to print out all parcels, regardless of whether they
          are being traced. But with met data (%m) included in the output,
          this can become very, very time-consuming.
      */    
      void omitNonTraced();

      /// includeNonTraced
      /*! This method clears the flag that prevents parcels that are not being traced from being output.
          This sets the flag back to the default, which is to print out all parcels, regardless of whether they
          are being traced. But with met data (%m) included in the output,
          this can become very, very time-consuming.
      */    
      void includeNonTraced();

      /// nonTraced
      /*! This method returns the flag that determines whether  
          parcels that are not being traced will be output.
          
          \return 0 if all parcels will be output
                  1 if only traced parcels will be output
      */
      int nonTraced();

      /// Print a single Parcel
      /*! This method prints a single Parcel object.
      
          \param p the Parcel object to be printed

      */
      void apply( Parcel& p ); 

      /// Print an array of Parcels
      /*! This method prints an array of Parcels.
      
          \param p the array of Parcel objects to be printed
    
          \param n the number of Parcel objects to be operated on

      */
      void apply( Parcel * const p, const int n ); 


      /// print a vector of parcels
      /*! This method prints a vector of Parcels
      
          \param p the vector of Parcel objects to be printd
           
      */
      void apply( std::vector<Parcel>& p ); 

      /// print a list of parcels
      /*! This method prints a list of Parcels
      
          \param p the list of Parcel objects to be printed
           
      */
      void apply( std::list<Parcel>& p ); 

      /// print a deque of parcels
      /*! This method prints a deque of Parcels
      
          \param p the deque of Parcel objects to be printed
           
      */
      void apply( std::deque<Parcel>& p ); 

      /// print a Flock of parcels
      /*! This method prints a Flock of Parcels
      
          \param p the Flock of Parcel objects to be printed
           
      */
      void apply( Flock& p ); 


      /// print a Swarm of parcels
      /*! This method prints a Swarm of Parcels
      
          \param p the Swarm of Parcel objects to be printed
           
      */
      void apply( Swarm& p ); 

      /// set the format for how a parcel is to be printed
      /*! This method allows the user to specify the format in which a parcel's information is to be printed.
          The specification takes the form of a string with the following format:
          
          * Any text up to a "%" character is copied to the output verbatim.
          
          * "%%" appears as "%" in the output
          
          * "%T" inserts the  parcel's time, after conversion to a (MetData-source-specific) Calendar time string,
            into the output. A number inserted before the "T" (e.g., "%4T") is used as the length of the output, 
            starting with the zeroeth character by default. If two numbers, separated by a period, are inserted
            before the "T", the first number is the starting character to extract from the character string,
            and the second number is the number of characters to extract. for example, if the calendar string is 
            "2017-03-14T14:35:15", then "%5.8T" will results in "03-14T14" being inserted into the output.
            (Note that the "14:35:15" is not rounded to "15". StreamPrint knows nothing about the semantics
            of the calendar string.) If a minus sign is inserted just after the "%", then the output
            is left-aligned; the default is right alignment (i.e., leading spaces).
      
         * "%t" inserts the parcel's time, as internal model time (not converted to a calendar time).
           A number can be inserted in front of the "t" to specify the total length of the output.
           If two numbers separated by a period are inserted, the first is an overall length
           and the second is the number of decimal places. A "-" inserted just after the "%" indicates
           left-alignment.
          
         * "%o" inserts the parcel's longitude, in numeric form positive east.
           A number can be inserted in front of the "o" to specify the total length of the output.
           If two numbers separated by a period are inserted, the first is an overall length
           and the second is the number of decimal places. A "-" inserted just after the "%" indicates
           left-alignment.
          
         * "%a" inserts the parcel's latitude, in numeric form positive north.
           A number can be inserted in front of the "a" to specify the total length of the output.
           If two numbers separated by a period are inserted, the first is an overall length
           and the second is the number of decimal places. A "-" inserted just after the "%" indicates
           left-alignment.
          
         * "%v" inserts the parcel's vertical coordinate, according to the coordinate system being
           used to trace the parcels.
           A number can be inserted in front of the "v" to specify the total length of the output.
           If two numbers separated by a period are inserted, the first is an overall length
           and the second is the number of decimal places. A "-" inserted just after the "%" indicates
           left-alignment.
          
         * "%g" inserts the parcel's tag value.
           A number can be inserted in front of the "g" to specify the total length of the output.
           If two numbers separated by a period are inserted, the first is an overall length
           and the second is the number of decimal places. A "-" inserted just after the "%" indicates
           left-alignment.
          
          * "%f" inserts the parcel's flags. A number can be inserted in front of the "f" to
            specify the total length of the output.
          
          * "%s" inserts the parcel's status code. A number can be inserted in front of the "s" to
            specify the total length of the output.
          
          * "%i" inserts the parcel's index number. For single parcels, the index is zero. For Arrays, 
            vectors, lists, deques, and flocks of parcels, this is the element number within
            the collection of parcels. (Note that as these collections are manipulated, the index
            number of an individual parcel may change.) A number can be inserted in front of the "i" to
            specify the total length of the output.
          
          * "%{fieldname}m" inserts the value of a meteorological field at the Parcel's location and time.
            By default, the Parcel's meteorological data source is sued to obtain the field value,
            but the setMetSRC() method may be used to specify an alternate data source.
            Note that the process of reading and interpolating data is likely to slow the model down
            significantly; you will probably want to use this specifier sparingly.
            A number can be inserted in front of the "{" to specify the total length of the output.
            If two numbers separated by a period are inserted, the first is an overall length
            and the second is the number of decimal places. A "-" inserted just after the "%" indicates
            left-alignment.

          * "%c" prints out a sequence of "c" characters. 
             A number can be inserted in front of the "c" to specify the total length of the output field. 
             Otherwise just one "c" is printed. This code is included for compatibility with the StreamRead class's
             format specifications.    
          
          * "%x" prints out a sequence of "x" characters. 
             A number can be inserted in front of the "x" to specify the total length of the output field. 
             Otherwise just one "x" is printed. This code is included for compatibility with the StreamRead class's
             format specifications.    
          
          
          For example, suppose a parcel being traced isentropically 
          is at internal model time 12.3455, corresponding to calendar
          time "2012-03-12T15:10:33" i the Gregorian calendar, and is at position 40.56 west longitude,
          24.334546 north latitude, at 457 K potential temperature.
          The format "Time: %T (%14.5t), Position (%-7.2o, %-6.2a, %v)" will
          insert into the stream the string 
          "Time: 2012-03-12T15:10:33 (       12.3455), Position (-40.56 ,24.335,457.00)"
          
          \param fmtstr the format string. Most often, the format string should end with "\n", so that
                     each parcel's information will appear on a separate line from the others.
          
      */
      void format( const std::string fmtstr );

      /// sets an alternate met data source
      /*! This method sets a meteorological data source to be used instead of
          the parcels' internal meteorological data source, for the "%{fieldname}m"
          format specifier.

          \param met a pointer to a MetData object.
      */
      void setMet( MetData* met );
      
      /// returns the alternate met data source  
      /*!! This method returns the alternate meteorological data  source
           used for the  "%{fieldname}m" format specifer.
           
           \return a pointer to the MetData object
      */
      MetData* getMet();     




      /*! \brief override << operator for single Parcels
          
          This operator override permits using the 
          common "stream << object" syntax with StreamPrint objects
          as is used with a standard output stream object. 
          
          \param p a single Parcel to be read
          
      */
      StreamPrint& operator<<( Parcel& p )
      {
         apply( p );
         return *this;  
      }
      /*! \brief override << operator for vectors of Parcels
          
          This operator override permits using the 
          common "stream << object" syntax with StreamPrint objects
          as is used with a standard output stream object. 
          
          \param p a vector of Parcels to be Read
          
      */
      StreamPrint& operator<<( std::vector<Parcel>& p )
      {
         apply( p );
         return *this;  
      }
      /*! \brief override << operator for lists of Parcels
          
          This operator override permits using the 
          common "stream << object" syntax with StreamPrint objects
          as is used with a standard output stream object. 
          
          \param p a list of Parcels to be Read
          
      */
      StreamPrint& operator<<( std::list<Parcel>& p )
      {
         apply( p );
         return *this;  
      }
      /*! \brief override << operator for deques of Parcels
          
          This operator override permits using the 
          common "stream << object" syntax with StreamPrint objects
          as is used with a standard output stream object. 
          
          \param p a deque of Parcels to be Read
          
      */
      StreamPrint& operator<<( std::deque<Parcel>& p )
      {
         apply( p );
         return *this;  
      }
      /*! \brief override << operator for Flocks of Parcels
          
          This operator override permits using the 
          common "stream << object" syntax with StreamPrint objects
          as is used with a standard output stream object. 
          
          \param p a Flock of Parcels to be Read
          
      */
      StreamPrint& operator<<( Flock& p )
      {
         apply( p );
         return *this;  
      }
      /*! \brief override << operator for Swarms of Parcels
          
          This operator override permits using the 
          common "stream << object" syntax with StreamPrint objects
          as is used with a standard output stream object. 
          
          \param p a Swarm of Parcels to be Read
          
      */
      StreamPrint& operator<<( Swarm& p )
      {
         apply( p );
         return *this;  
      }



   private:
   
      // holds format specifcations
      class FmtSpec {
      
         public:
         
          /* gives the type of thing to format
              "L" = literal; plug in the format as-is
              "T" = Calendar time string: extract characters start through ( start + len - 1 )
              "t" = model time: print model time, using len and float for total length and number-of-decimal-places
              "o" = longitude: print longitude, using len and float for total length and number-of-decimal-places
              "a" = latitude: print latitude, using len and float for total length and number-of-decimal-places
              "v" = vertical: print vertical coordinate, using len and float for total length and number-of-decimal-places
              "i" = index: print the parcel index (of a sequence or Flock of parcels)
              "{field}m" = print the field value at the Parcel location from the met source
          */
          std::string type;
          // starting character for string extraction
          int start;
          // length of string extraction, or number of characters in numeric string
          int len;
          // number of decimal places to print in numeric string
          int fract;
          // +1 for right-alignment, -1 for left alignment
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
      
      // alternate met source
      MetData* metsrc;
      
      // flag for omitting non-traced parcels from output
      int nobad;

      // clear the format
      void clearFormat();

      // converts a string to an integer
      int s2i( const std::string str );

      // creates an output string from parcel data
      std::string print( const Parcel& p, MetData *metsrc, int index=0 );
      
      
      // tell whether a parcel should be printed
      bool printThis( const Parcel& p ) const;
      // tell whether a parcel should be printed
      bool printThis( const Parcel* p ) const;
      
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
