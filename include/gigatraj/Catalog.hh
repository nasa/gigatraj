
#ifndef GIGATRAJ_CATALOG_H
#define GIGATRAJ_CATALOG_H

#include "gigatraj/gigatraj.hh"
#include <gigatraj/CalGregorian.hh>

#include <vector>
#include <map>

namespace gigatraj {

/*!  \defgroup MetMisc Meteorological Miscellaneous
     \ingroup MetDat

\brief Miscellaneous classes for use with Meteorological Data Sources


*/

/*! 
\ingroup MetMisc
   \brief Catalog implements a catalog of files of meteorological data to be opened
   
   \details
   
The Catalog class provides a way for MetData objects which rely on reading files 
or URLs to map data requests to specific URLs.  A data request consists of 
the name of a physical quantity, a Gregorian calendar date-time string
(in 'yyyy-mm-ddThh:mm:ss' format, as in ISO8601) at which vallid data are desired, and (optionally) an
arbitrary confLocator string (typically used to indictate a forecast model run).
The Catalog's response to the request is a vector of objects, each of which
has the name of a file containing the data just before or at the date-time string,
the name of a file containing the data at the next time snapshot after the date-time string,
and various attributes and characteristics of the data that would be read from those
files: units, resolution, vertical coordinate, etc.

Why is a Catalog class needed? For data that are read from a central network server (such as an OPeNDAP), this
mapping can be hard-coded into a MetData object, with little need for a Catalog. 
But for data that are read from local files, we find that local sites often set up their own conventions for organizing
data sets, instead of following the conventions used by the original data source.
If gigatraj is to be useful, then some class is needed that can do the
mapping based on local site- or user-specific configurations.

In use, the Catalog will load a configuration from a file that 
essentially specifies which data are to be found in which files.
Then, as the MetData object queries the Catalog for a specific
physical quantity at a specific time in the Gregorian calendar, 
The "pre" and "post" filenames returned can be opened to read the data, their resepctive valid-at
times can be used for time interpolation, and the target attributes can be used to 
decide what other manipulations of the data should be applied.
And because the filenames are taken form a vector of objects, 
the MetData object can open each set of files in turn, until it successfully
reads the desired data. In this way one can for exmaple specify that
analysis data are to be read first, or forecast data if there are no analysis data yet.
Thus the Catalog class is of use to people who are implementing new MetData subclasses.
But understanding how Catalog works and how to configure it will be useful to
those who seek to adapt an existing MetData subclass to their own local file
organizational conventions.

The configuration file lists each physical quantity that should be readable by gigatraj.
Associated with ewach quantity is a list of "Targets" from which the data may be read.
Each Target represents a unique addressable entity such as a file. But of course file names
often contain such things as dates and quantity codes, so the 
Targets work by interpolating string values into the filename template, using varibale references.
For example, if the Target named "targ01" has the template "${QUANTITY}_$YEAR}${MONTH}${DOM}${HOUR}_${TABLE}.dat",
it will substitute values for QUANTITY, YEAR, MONTH, DOM (day-of-month), HOUr, and TABLE in the template,
returning a value such as "T_2021071518_MET.dat". In addition, each Target contains information
about the time spacing of the data a filCe contains, as well as a set of named attributes
that can be used for descriptions of such things as the file's horizontal resolution, vertical coordinate,
time averaging, and so on.

Where do the values of these variable references come from? Some of them are defined
automatically:

  * QUANTITY = the name of the quantity being requested
  * DATETIME = the full "yyyy-mm-ddThh:mm:ss" date string for the valid-at time snapshot
  * DATE = the "yyyy-mm-dd" date string for the valid-at time snapshot
  * TIME = the "hh:mm:ss" time string for the valid-at time snapshot
  * YEAR = the 4-digit year (e.g., 1997) from the valid-at time snapshot
  * CN = the 2-digit century (e.g., 19) from the valid-at time snapshot
  * YR = the 2-digit year of the century (e.g, 97) from the valid-at time snapshot
  * MONTH = the 2-digit number of the month (01-12) from the valid-at time snapshot
  * DOM = the 2-digit day of the month (01-31) from the valid-at time snapshot
  * DOY = the 3-digit day of the year (001-366) from the valid-at time snapshot
  * HOUR = the 2-digit hour from the valid-at time snapshot
  * MINUTE = the 2-digit minute from the valid-at time snapshot
  * SECOND = the 2-digit second  from the valid-at time snapshot
  * TAG = the optional tag supplied to the query() method

In addition to these, other variables may be defined within the
configuration file (see below).
But you should never attempt to define any of these automatically-defined variables in your configuration files, because
any such definitions will be overwritten by the definitions above.

If a variable is not defined either automatically or explicitly in the configuration,
then the Target's attributes are inspected. If an attribute is found with the same
name as the variable reference, then that Target attribute is used as the variable value.

If no variable or Target attribute is found that matches the variable reference, then the user's
runtime environment is consulted. A shell environment variable with the requested name
will have its value interpolated into the template.

Variables are values or expressions that can be referecned by name.
The Catalog configuration file allows for five types of variables:
strings, booleans, integers, floating-point numbers, and dates.
A variable's value may be a literal of the correct type, or it
may be an expression to be evaluated when a data query is made.
A variable may be defined conditionally, so that it is assigned
a specified value only if specified conditions apply. The conditions
are expressed as a boolean logical expression that evaluates to either
true or false. Multiple variable definitions may be defined (all of the same 
data type!), and when a data request is made the definitions will
be examined in the same order as they are found in the configuration file.
The first definition whose conditional expression evaluates as true, will
be the definition that is assigned to the variable.


The configuration file syntax:

The configuration file is a plain text file. Blank lines and comment lines (lines that consist of optional whitespace
followed by "#") will be ignored.

Identifiers consist of a leading alphabetic character ("A"-"Z" and "a"-"z"), followed
by zero or more alphabetic characters, numeric digits ("0"-"9"), or underscores ("_").

Aside from multiple definitons of the same variable, the lines of a configuration
file may be arranged in any order.

There should be exactly one line that names the attributes used by the Targets. That line
will begin with a ";" character, followed by one or more attribute identifiers.
All Targets must have the same number of attribute values as the number of attribute names given here.
an example of such a line might be:
\code
; horiz ; vert ; tavg 
\endcode

The MetData class that uses Catalog is responsible for
the symantics of these attributes; Catalog merely
handles the syntax.

For each physical quantity that gigatraj should be able to read,  there must be a line
of the form "quantity : standard_name : target1 | target2 | target3 | ..."; this lines tells the Catalog which
Targets can supply the desired quantity. 

Normally, "quantity" here has the form of a valid identifier: a leading uppercase or lowercase letter, followed
by zero or more letters, digits, or underscore ("_") characters.
Some data sets, however, may choose to use quantity identifiers that are not valid identifiers;
in that case, enclose the unusual identifier in parentheses. For example, "foo" can be used
as a quantity name without parentheses, but "cm.bar@q-10(m)" must be enclosed by parentheses:
"(cm.bar@q-10(m))". (Note that if the quantity name contains its own parentheses as it does here,
those parentheses must be balanced: for every "(" there must be a following ")".)

The standard_name is a standardized name for people who are unfamiliar with
the data set's own naming convention; UCAR's "CF" naming convention is a good one to use here.

Each Target in the "|"-separated list may be preceded
by one or two clauses enclosed in pairs of brackets ("[" and "]"). The first (or only) such
clause specifies information about units. Ideally, this information is read from the file,
but that is not always possible. The units information consists of a string giving the units
of the quantity, optionaly collowed by a ":" and a scale factor that can convert the quantity
as read from the file to MKS units, optionally followed by another ";" and an offset to be used in the
conversion to MKS units. Thus, "[K]" specifies the units of Kelvin (for temperature), while
"[F; 0.55556; 255.372]" specifies units of degrees Fahrenheit, which can be converted to Kelvin
by multiplying by 0.55556 and adding 255.372. The second pair of brackets, if given, specify
a description of that data variable: for example, "[air_temperature]".
The order of Targets in the "|"-separated list should be in their order of preference for
that physical quantity; typically this hsould be the order that requires less work
in units conversion, or horizontal regridding, or vertical regridding. The Target order can also
indicate the preference for analysis products over forecast model output. An example
might look like this:
\code
T : [K]targ1 | [C ; 1.0 ; 273.15]targ2
\endcode

If the scale factor is given without an offset,
then the offset is considered  to be zero. If no scale factor
is specified, then the conversion is undefined in the Catalog
and must be hard-coded in the MetData class that uses the Catalog.
           
Each Target that is referenced by a quantity line must be defined by a configuration line of the form:
"identifier := base-hour; inc-hour; attribute-value-1; attribute-value-2; ... ; filename-template".
the "identifier" is the name of the Target. The base-hour is the hour of the first time snapshot 
to be found in the file. The inc-hour is the spacing of time snapshots in the file.
These are followed by attribute values that correspond to the attribute names given
by the line that starts with ";". These are followed by the filename template. For example, 
a file that holds data for a single data, starting at 00Z and spaced 3 hours apart, with
a horizontal resolution of 1 by 1 degree, in pressure coordinates, and being non-averaged instantaneous
data, might look like this:
\code
 targ1 := 0.0 ; 3.0; 1X1 ; pres ; inst ; ${QUANTITY}_${YEAR}${MONTH}${DOM}_${FOO}.dat
\endcode


As mentioned above, some variables such as "QUANTITY" and "YEAR" are defined automatically
when a request is made. Others, such as "FOO" in our example, need to be defined in the configuration file.
A variable definition line may take the form "identifier/type = expression" or "identifier/type = logical-expression ? expression".
The identifier names the variable.
The type may be "S" (for string), "B" (for boolean), "I" (for integer),
"F" (for float), or "D" (for date).  If the variable is of type string, the "/S" may be omitted.
The expressions may be simple literal values, or they may be 
complicated arithmetic expressions. (If the type of the expression is not right, it will
be converted to the proper type when it is evaluated.)  
Multiple configuration lines may define a single variable. The definitions are evaluated
in order, and the first definition whose test expression evaluates to "true" will be used.
Examples of variable definitions might be:
\code
FOO/S = ${DATE} >= [2000-01-01] ? "New"
FOO/S = "Old"
EXAMPLE01/I = ( 125 + ${EXAMPLE02} ) *5 + ${DOY}
EXAMPLE02/I = 25
\endcode

In this example, FOO takes on the value "New" if the date of the data request is later than or equal to Jan. 1, 2000.
(Note that a date literal is enclosed in brackets, and a string literal is enclosed in quotes.); otherwise it takes
the value "Old". EXAMPLE01 is defined an an integer that depends on the separately-defined variable EXAMPLE02
and the automatically-defined DOY (the day of the year of the data request).

Note that circular definitions of variables is not permitted. You cannot define "FOO" with an expression involving "BAR" 
and then define "BAR" with an expression involving "FOO".

A variable reference may optionally include a format between the "$" and the opening brace before the name of the variable being referenced.
The formatting is applied after the variable reference has been evaluated, and as it is being interpolated ("printed") 
into a string or template. Booleans and integers take a single format code, a positive integer, which inicates the length of
the output. For example, if variable FOO is a boolean with a value of true, then "$3{FOO}" results in "tru" (the first
three characters of the string "true").  If Foo is an integer with the value 5, theh "$3{FOO}" results in "005"; leading
digits are zero-filled.  Floating point valaues, strings, and dates take two format codes
(both positive integers or zero), separated by a period (".").
For floats, the two format codes are the width of the ouptut and the number of decimal places.
For example, if floating variable FOO has the value 10.2375, then "$5.2{FOO}" results in "10.24".
For strings, the first format code is the width, and the second is the starting position.
If FOO is a string variable with value "ABCDEFG", then "$4.1{FOO}" becomes "BCDE".
For dates, the format codes have the same meaning as for strings; the date is converted
to an ISO 8601 format string first, and then the formatting is applied.

Note that format codes are useful only when interpolating variable references into string expressions.
Do not attempt to use them in straight variable reference, because you are unlikely to get the results
that you want.  For example,
\code
FOO/S = "ABCDEFG"
BAR/S = "$3.2{FOO}"
\endcode
will result in BAR evaluating to the string "CDE".

But 
\code
FOO/S = "ABCDEFG"
BAR/S = $3.2{FOO}
\endcode
will result in BAR evaluating to "ABCDEFG". The reason is that format specifiers are used
only when interpolating into strings, not when assigning values directly to other variables.
In this example, trying to use the BAR variable will  make the catalog look up the value of the 
FOO variable and assign that as BAR's evaluated value. And because FOO is not being interpolated 
into a string, the formatting codes here are ignored. The the previous example, 
BAR was assigned a string value, and inside the string was a reference to FOO. As the value of
FOO is interpo;lated into the string value (as opposed to merely being looked up), the
formatting codes are applied, and we get the BAR value we intended, "CDE".


The Catalog also permits defining Dimensions, to describe the dimensions of the data.
A Dimension definition line begins with an identifier, followed by a tilde ("~") character.
This is foillowed by several fields separated by semicolons (";"). The first field is
the name of the physical quantity used as the dimension. This is typically not one of the defined
Quantities in the Catalog, but it might be. More often, it is a quantity such as longitude, latitude, or pressure
(where the data set uses pressure as its vertical coordinate and consequently does not provide
an explicit pressure product). The next field after the quantity name is the quantity units.
After that is a code to specify how the values of the dimension are given: "LDN" (Low-Delta-Number) means
that the numeric values that follow give the low or starting vlaue, the delta between successive values, and the number of values.
"LHN" (Low-High-Number) means that the values the follow are the starting vlaue, the ending value, and the number of values.
"V" means that all of the values are given explicitly.
The final field in the line is a comma-separated list of floating-point numbers.

Note that variable references cannot be used in Dimension definitions.

One way to use Dimensions is to set up attributes correspondin to, say, longitude, latitude,
and the vertical. Then each Target definition could have a Dimension name as its respective
attribute value. 





For anyone interested in Extended Bakus-Naur form, the complete specification 
for a configuration line is given by:
\code
       
       <letter> = "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J"
                 | "K" | "L" | "M" | "N" } "O" | "P" | "Q" | "R" | "S" | "T"
                 | "U" | "V" | "W" | "X" | "Y" | "Z"
                 | "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j"
                 | "k" | "l" | "m" | "n" } "o" | "p" | "q" | "r" | "s" | "t"
                 | "u" | "v" | "w" | "x" | "y" | "z" ;
        
       <digit> = "0" | "1" | "2" | "3 " | "4" | "5" | "6" | "7" | "9" | "9" ;
        
       <quote> = "'" | '"';
       
       <otherChar> = "~" | "!" | "@" | "#" | "%" | "^" | "&" | "*" | "(" 
                    | ")" | "_" | "+" | "`" | "|" | "\" 
                    | "<" | ">" | "?" | "," | "." | "/" ;

       <whitespaceChar> = " " | "\t";
        
       <symbolChar> =  <otherchar> | ":" | "$" | "{" | "}" | "[" | "]" | ";" ;
       
       <noSemicolonChar> = <letter> | <digit> | <otherChar> | ":" | "$" | "{" | "}" | "[" | "]"  ;

       <noColonChar> = <letter> | <digit> | <otherChar> | ";" | "$" | "{" | "}" | "[" | "]"  ;

       <noBracketsChar> = <letter> | <digit> | <otherChar> | ":" | "$" | "{" | "}"  ;

       <noquoteChar>  = <letter> | <digit> | <whitespaceChar> | <symbolChar>;
        
       <anyChar> = <letter> | <digit> | <whitespaceChar> | <symbolChar> | <quote>;
       
       <idChar> = <letter> | <digit> | "_";
        
       
 
        
       <configLine> =  <commentLine>
                    | ( { <whitespace> }, <attributeDef>, { <whitespace> } ) 
                    | ( { <whitespace> }, <quantityDef>,  { <whitespace> } )
                    | ( { <whitespace> }, <targetDef>,    { <whitespace> } )
                    | ( { <whitespace> }, <variableDef>,  { <whitespace> } ) ;
                      
       <commentLine> = <emptyLine>
                     | ( { <whitespace>, }  "#",  { <anyChar> } );
       
       <whitespace> = <whiteSpaceChar>, { <whiteSpacechar> };

       <emptyLine> = "" | <whitespace>;
       

        
       
       <attributeDef> = ";",  <attributeList>;
        
       <attributeList> = [ <whitespace> ], <attributeID>, [ <whitespace> ] { ( ";",  [ <whitespace> ], <attributeID>, [ <whitespace> ] ) };

       <attributeID> = <identifier>;
       
       <identifier> = <letter> | ( <letter>, {<idChar>} );

       <weirdIdentifier> = <anyChar>, {<anyChar>} ;
       
       <quantityDef> = <quantityID>, [ <whitespace> ], ":", [ <whitespace> ], <stdname>, [<whitespace> ], [ <whitespace> ], <dims>, [<whitespace> ]  , ':', [ <whitespace> ], <targetList>;
       
       <quantityID> = <identifier> | ( "(", <weirdIdentifier>, ")" );
       
       <stdname> = <noColonChar>, { <noColonChar> };
       
       <dims> = <positiveInteger>
       
       <positiveInteger> = <digit>, {<digit>} 
              
       <targetList> = <targetElement>, { [ <whitespace> ], ";", [ <whitespace> ],  <targetElement> };
       
       <targetElement} = <targetID>, { <whitespace> }, [ <unitsSpec>, [ <descSpec> ] ];  
                    
       <targetID> = <identifer>;
       
       <unitsSpec> = "[", { <whitespace> }, <text>, { <whitespace> }, 
                    [ ";", { <whitespace> }, <scale>, { <whitespace> }, 
                    [ ";", { <whitespace> }, <offset>, { <whitespace> } ] ], "]";
       
       <descSpec> = "[", <text>, "]";
       
       <text> = <noBracketsChar>. {<noBracketsChar>};
       
       <scale> = <float>;
       
       <offset> = <float>;
       
       <float> = [ <sign> ], <digit>, { <digit> }, [ ".", [ <digit>, { <digit> } ] ];

        <sign> = "+" | "-";




       <targetDef> = <targetID>, [ <whitespace> ], ":=", [ <whitespace> ]
                     , <basedate>, [ <whitespace> ], ";", [ <whitespace> ]
                     , <urlSpacing>, [ <whitespace> ], ";", [ <whitespace> ]
                     , <inctime>, [ <whitespace> ]
                     , ";", [ <whitespace> ], <numtimes>, [ <whitespace> ]
                     , <attributeValueList>, [ <whitespace> ]
                     , ";", [ <whitespace> ], <template>, [ <whitespace> ];
        
       
       <basedate> = <datespec>;
       
       <urlSpacing> = <timePeriod>;
       
       <inctime> = <float>;
       
       <numtimes> = <integer>;
       
       <timePeriod> = "P", [ <integer>, "Y" ], [ <integer>, "M" ], [ <integer>, "D" ]
                     , [ "T", [ <integer>, "H" ], [ <integer>, "M" ], [ <integer>, "S" ] ] 
       
       <attributeValueList> = <attributeValue>, { [ <whitespace> ], ";", <attributeValue>, [ <whitespace> ] };
        
       <attributeValue> = { <attributeChar> };       

       <attributeChar> = <letter> | <digit> | <whitespace> | <quote> | <noSemicolonChar>;
       
       <template> = <anyChar> [ { <anyChar } ];
       
  
  
  
       

        <variableDef> = <variableID>, [ "/", <varTypeSpec> ], [ <whitespace> ]
                        , "=", [ <whitespace> ]
                        , [ <booleanExpression>, [ <whitespace> ], "?", [ <whitespace> ] ]
                        , <valueExpression>, [ <whitespace> ]; 
        

        <variableID> = <identifer>;
        
        <varTypespec> = "S" | "B" | "I" | "F" | "D";

        <booleanExpression> = <expression>;
        
        <valueExpression> = <expression>;

        <expression> = <literal>, [ <whitespace> ]
                      | <quasiLiteral>, [ <whitespace> ]
                      | <variableRef>, [ <whitespace> ]
                      | ( <unaryOperator>, [ <whitespace> ], <expression>, [ <whitespace> ] )
                      | ( <expression>, [ <whitespace> ], <binaryOperator>, [ <whitespace> ], <expression>, [ <whitespace> ] )
                      | ( "(", [ <whitespace> ], <expression>, [ <whitespace> ], ")", [ <whitespace> ] );

        <literal> = <stringLiteral>                  
                  | <booleanLiteral>
                  | <integerLiteral>
                  | <floatLiteral>
                  | <dateLiteral>;
        
        <quasiLiteral> = <stringWithRefs> | <dateWithRefs>;
        
        <stringLiteral> = ( '"', { <noQuoteChar> ], '"" )
                        | ( "'", { <noQuoteChar> ], "'" );
        
        <booleanLiteral> = "true" | "false";
       
        <integerLiteral> = <integer>;
        
        <integer> = [ <sign> ], <positiveInteger>;
        
        <floatLiteral> = <float>;
  
        <dateSpec> = <dateListeral> | <dateWithRefs>
  
        <dateLiteral> = "[", <year>, "-", <month>, "-", <dayOfMonth>,
                          [ "T", <hour> [ ":", <minute>, [ ":", <second> ] ] ]
        
        <dateWithRefs> = "[", { ( <variableRef> | <digit> | "-" )},   "]"
        
        <year> = <digit>, <digit>, <digit>, <digit>;
        
        <month> = <digit>, <digit>;
        
        <dayOfMonth> = <digit>, <digit>;
        
        <hour> = <digit>, <digit>;
        
        <minute> = <digit>, <digit>;
        
        <second> = <digit>, <digit> [ ".", { <digit> } ];
  
        <variableRef> = "$", [ <format1> [ ".", <format2> ] ], "{", variableID, "}" ;
        
        <stringWithRefs> = ( '"', { <noQuoteChar> | <variableRef> ], '"" )
                         | ( "'", { <noQuoteChar> | <variableRef> ], "'" );
        
        <format1> = <integer>;
        
        <format2> = <integer>;
        
        <unaryOperator> = "-" | "!";
        
        <binaryOperator> = "+" | "-" | "*" | "/" | "%" | "==" | "!=" | ">=" | ">" | "<=" | "<" | "&&" | "||";
        
        <DimensionDef> = <dimensionID>, { <whitespace> }, "~", { <whitespace> }, 
                          <quantity>, { <whitespace> }, ";", { <whitespace> }, 
                          <units>, { <whitespace> }, ";", { <whitespace> }, 
                          [ "LDN" | "LHN" | "V" ],  { <whitespace> }, ";", 
                          { <whitespace> },<value> { <whitespace> }, { ",", { <whitespace> }, <value>{ <whitespace> } }
        
        <dimensionID> = <identifier>;
        
        <quantity> =  <noSemicolonChar>, { <noSemicolonChar> };
        
        <units> = <noSemicolonChar>, { <noSemicolonChar> };
        
        <value> = <float>;

\endcode          


*/
class Catalog {


      
   protected:
   
      // (We use these classes in the Catalog, and in each other, but they should not be accessed by anything outside Catalog.)   
   
      /// This class holds the value of a variable
      /*! The VarVal class holds a value that can be assigned to a variable. All values associated with literals or 
          expressions are held in VarVal objects.
      
          ValVals do not have names, but they do have a type: either 'S' (string), 'B', (boolean), 'I' (integer), 'F' (float)
          or 'D' (date).
          
          A VarVal has two kinds of values associated with it. The "nominal" value is a string that holds a representation
          of the value. The "eval" value is a actual value of the correct type; this is either evalS, evalB, evalI, evalf, or evalD.
          Either the nominal value or the eval value may be set directly, and either may be converted to the other.
          There are also bitwise flags that are set to indicate which values have been set.
          If the nominal value has been set, then the ValidNominal flag is set. If an eval vlaue has been set, then the
          ValidEval flags is set.  
          
          Of course, an eval value can be set only if the value is a literal of the correct type. If the nominal
          value is set to a string that does not parse as a literal for the value's type, then the nominal
          value is not a literal and no eval value is set. Assuming the nominal value has noerrors, then 
          it will be a variable reference of the form "${name}" or (in the case of string values only) a string with
          variable references embedded within it (e.g., "red${stuff}blue".  In the former case, the evalS member
          will contain the name of the variable being referenced. In the second case, the evalS mjay be set to
          the nominal string with the variable reference values interpolated into the string.
          
          Note that the 'D' date type's evalD is a double-precision number that contains the 
          number of days elapsed since 1899-12-31T00:00:00.
          
          We use a single VarVal class definition to hold all variable types, instead of a VarVal subclass for each type,
          to make it easier to do type conversions, which we will be doing a lot.
          
          Also, the VarVal class has no facility for evaluating expressions, because it has no way
          to resolve veriable references.
      
      */
      class VarVal {
      
         public:
         
           /// flag: value contains a valid nominal value
           static const int ValidNominal = 0x01;
           /// flag: value contains a valid eval value
           static const int ValidEval    = 0x02;
           /// flag: value contains aliteral value, not a variable reference
           static const int IsLiteral    = 0x04;
           
      
           /// the nominal value: value as specified
           std::string nominal;
           /// the value type (I, F, B, D, S)
           char type;
           /// bitwise status flags
           int flags;
           /// eval for string type: actual working-value if type string
           std::string evalS;
           /// eval for integer type: actual working-value if type integer
           int evalI;
           /// eval for float type: actual working-value if type float
           float evalF;
           /// eval for boolean type: actual working-value if type boolean
           bool evalB;
           /// eval for date type: actual working-value if type date
           double evalD;
           
           /// formating parameter 1 (wid for I, F, and  D)
           int fmt1;
           /// formatting parameter 2 (dec for F, start for D)
           int fmt2;
           
           
           /// the basic constructor
           /*! This is the basic constructor for a value.
              
               The resulting VarVal has neither nominal nor eval values
               set, and the flags are all cleared.
                
               \param typ the type of the variable to be created
           */
           VarVal( char typ = 'B' );
           
           /// Constructor that creates a value of a given type and sets its nominal value
           /*! This VarVal constructor creates a value of any type and sets its nominal value.
               An attempt is also made to convert the nominal value to an eval value.
              
               \param val a string holding the value
               \param typ the vvalue type 
           */
           VarVal( const std::string& val, char typ = 'S' );
           /// boolean VarVal constuctor
           /*! This method creates a boolean VarVal and sets its value
               
                  \param val the boolean that the eval value is set to
           */
           VarVal( bool val );
           /// integer VarVal constuctor
           /*! This method creates an integer VarVal and sets its value
               
                  \param val the integer that the eval value is set to
           */
           VarVal( int val );
           /// float VarVal constuctor
           /*! This method creates a float VarVal and sets its value
               
                  \param val the float that the eval value is set to
           */
           VarVal( float val );
           /// date VarVal constuctor
           /*! This method creates a date VarVal and sets its value
               
                  \param val the numeric date that the eval value is set to
           */
           VarVal( double val );
           

           /// VarVal copy
           /*! This method creates a new complete copy of the VarVal object.
           
               \returns a pointer to the new VarVal. The calling routine must delet this object when done with it.
           */    
           VarVal* copy() const;
           
           /// convert type
           /*! This method converts a VarVal from one type to another.
           
               If the conversion is not possible, then a badValueConversion error is thrown.
           
                \param newtype the type to which the VarVal is to be converted.
           */     
           void convertType( char newtype );
           
           /// reconciles the nominal value with eval content
           /*! This method tries to reconcile the nominal with the eval value, 
               filling in the one if the other is present.
               This is possible, of course, only if the value is a literal.
           */
           void reconcile();
           
           /// returns whether the VarVal holds a valid nominal value
           /*! This method checks whether the VarVal has had its nominal value set.
               It does this by checking whther the ValidNominal flag is set. 
                
               \return true if the VarVal has a valid nominal value, false otherwise 
           */
           inline bool hasNominal() const {
                return ( flags & ValidNominal );
           }    
           /// returns whether the VarVal holds a valid eval value
           /*! This method checks whether the VarVal has had its eval value set.
               It does this by checking whther the ValidEval flag is set. 
                
               \return true if the VarVal has a valid eval value, false otherwise 
           */
           inline bool hasEval() const {
                return ( flags & ValidEval );
           }    
           /// returns whether the VarVal holds a literal value
           /*! This method checks whether the VarVal is a literal (and not a variable reference).
               It does this by checking whther the IsLiteral flag is set. 
                
               \return true if the VarVal is a literal, false otherwise 
           */
           inline bool hasLiteral() const {
                return ( flags & IsLiteral );
           }    

           /// determines whether the VarVal really holds a literal value
           /*! This method examines a string to determine whether it holds a literal value 
               that is compatible with the VarVal's type.
               
               \param str the string to be examined
               \return true is the string holds a literal value fo rthis type, false otherwise
           */    
           bool isLiteral( std::string& str );
           
           /// generates a string that corresponds the the VarVal's value.
           /*! This methid takes the value of the VarVal and prints it to a string.
               The VarVal must be a literal, andit must have a valid eval value.
               (Conversion from the nominal value is not done automatically.)
               
               The printing is done with formatting, as follows:
               
               * String: fmt1 is the size, fmt2 the starting position of an extracted substring
               * Boolean:  fmt1 is the size of the extracted "true" or "false", starting with character 0
               * Integer: fmt1 is the size of the printed string, padded with zeroes if necessary.
               * Float: fmt1 is the size of the printed stgring, and fmt2 is the number of digits past the decimal point
               * Date: fmt1 is the size, fmt2 the starting position of an extracted substring
               
               \param output the output string
               \return true if the conversion succeeded, false otherwise 
           */
           bool print( std::string& output ) const;
           
           /// put the "nominal" value into the "eval" values
           /*! The method takes a valid nominal value and converts it tan eval 
               value of the VarVal's type.
           */    
           void n2e();
           
           /// put the eval value of the right type into the nominal value
           /*! This method takes a valid eval vlau eof the right type and
               uses the print() method to produce a string that is then
               made the nominal value.
           */    
           void e2n();
        
           /// diagnostic dump of the VarVal
           /*! This method writes the contents of the VarVal to std::cerr
               in a human-readable text format.
               
               \param indent the number of spaces to indent each line of output.
                      This allows the VarVal dump to be nested within of other objects' dumps.
           */
           void dump( int indent = 0) const;
           
      };




      /// IDs for each of the kinds of Variable expression operators
      enum VarOperator {
           NOP,
           TestEQ,
           TestNE,
           TestGT,
           TestGE,
           TestLT,
           TestLE,
           TestOR,
           TestAND,
           TestNOT,
           Add,
           Subtract,
           Multiply,
           Divide,
           Remainder,
           Negate
      };
      
      /// variable operator
      /*!  The VarOp class implements operators that identify operations that are to be performed on VarVal operands.
      
           The VarOp merely identifies what is to be done; it performs no operations itself.

           Possible operators are:
            

           VarOperator |             operator | binary/unary | base priority  | allowed operands
           :------------: | :--------------------: | :------------: | :--------------: | :-----------:
           TestOR    |  <code>\|\|</code>   |     b        |     10        |  boolean
           TestAND   |  <code>&&</code>     |     b        |     10        | boolean
           TestEQ    |  <code>==</code>     |     b        |     20        | boolean, int, float, date, string
           TestNE    |  <code>!=</code>     |     b        |     20        | boolean, int, float, date, string
           TestGT    |  <code>></code>      |     b        |     20        | int, float, date
           TestGE    |  <code>>=</code>     |     b        |     20        | int, float, date
           TestLT    |  <code><</code>      |     b        |     20        | int, float, date
           TestLE    |  <code><=</code>     |     b        |     20        | int, float, date
           TestNOT   |  <code>!</code>      |     u        |     30        | boolean
           Add       |  <code>+</code>      |     b        |     50        | int, float, date, string
           Subtract  |  <code>-</code>      |     b        |     50        | int, float, date
           Multiply  |  <code>*</code>      |     b        |     60        | int, float
           Divide    |  <code>/</code>      |     b        |     60        | int, float
           Remainder |  <code>%</code>      |     b        |     60        | int, float
           Negate    |  <code>-</code>      |     u        |     80        | int, float
      
      */
      class VarOp {
          public:
          
             /// Identifies what kind of operator this is
             VarOperator id;
             /*! Holds the operator's priority or precedence, which is determined by the kind of operators it is 
                and by the level of nested parentheses.
             */   
             int priority;
             /// This is the number of operands the operator needs, either 1 or 2.
             int nops;
             /// This is the VarVal type that the operator results in.
             char typ;
             
             /// The basic VarOp constructor
             /*! This is the base VarOp constructor.
             */
             VarOp();
             /// Fancy constructor
             /*! This is a more advanced VarOp constructor, which
                 allows for specifying the operator.
                 
                 \param op  The VarOperator id
                 \param pri the base priority to be added to the operator's own native piroirty
             */    
             VarOp( VarOperator op, int pri=0 );
             
             /// copies a VarOp
             /*! This method creates a copy of the VarOp.
             
                 \return a pointer to the new VarOp. The calling roujtine is repsonsible for deleting the new VarOp 
                         once it is no longer needed.
             */            
             VarOp* copy() const;

             /// diagnostic dump of the VarOp
             /*! This method writes the contents of the VarOp to std::cerr
                 in a human-readable text format.
                 
                 \param indent the number of spaces to indent each line of output.
                        This allows the VarOp dump to be nested within of other objects' dumps.
             */
              void dump( int indent=0 ) const;
           
      };
          
 
      /// Element of a variable expression queue, holding a VarVal or a VarOp 
      /*! The VarExpr class maintains a queue of operands and operators that it ues
          to implement an expression. But operators and operands, being of different classes,
          cannot exist in the same queue. The VarExpr Item class exists to remedy this problem;
          it holds either a VarVal or a VarOp object, so that a single class can
          be added to or taken from a VarExpr object's queue.
      */
      class VarExprItem {
      
           public:
             /// flag: true if this holds a VarOp
             bool isOp;
             /// flag: true if this holds a VarVal
             bool isVal;
             /// holds a pointer to the VarVal that is held here, if any
             VarVal* val;
             /// holds a pointer to the VarOp that is held here, if any
             VarOp* op;
      
             /// basic consturctor for the VarExprItem
             /*! This is the basic constructor for the VarExprItem.
                 The resulting object holds nothing.
             */    
             VarExprItem();
 
             /// VarExprItem constructor for holding a VarVal
             /*! This version of the VarExprItem constructor
                 initializes the new object to hold a VarVal.
                 
                 \param v a pointer to the VarVal object that is to be held. What is actually held
                          is a copy of the VarVal, so that the calling program is free to 
                          delete the v object after calling this constructor.
             */
             VarExprItem( VarVal* v );
 
             /// VarExprItem constructor for holding a VarOp
             /*! This version of the VarExprItem constructor
                 initializes the new object to hold a VarOp.
                 
                 \param o a pointer to the VarOp object that is to be held. What is actually held
                          is a copy of the VarOp, so that the calling program is free to 
                          delete the o object after calling this constructor.
             */
             VarExprItem( VarOp* o );
             
             /// desctructor
             /*! This is the destructor for the VarExprItem class.
                 It deletes any VarVal or VarOp object that is being held
             */    
             ~VarExprItem();
             
             /// clears a VarExprItem
             /*! This method resets a VarExprItem object so that it holds nothing.
                 Any VarVal or VarOp object that i sbeing held is deleted,
                 and all flags are cleared.
             */    
             void clear();
             
             /// sets a VarExprItem to hold a VarVal
             /*! This method loads a VarVal object into the VarExprItem.
                 First, the VarExprItem is cleared, then a copy of
                 the input VarVal is stored, and finally the isVal flag is set.

                 \param v a pointer to the VarVal object that is to be held. What is actually held
                          is a copy of the VarVal, so that the calling program is free to 
                          delete the v object after calling this constructor.
                 
             */
             void set( VarVal* v );
             
             /// sets a VarExprItem to hold a VarOp
             /*! This method loads a VarOp object into the VarExprItem.
                 First, the VarExprItem is cleared, then a copy of
                 the input VarOp is stored, and finally the isOp flag is set.

                 \param o a pointer to the VarOp object that is to be held. What is actually held
                          is a copy of the VarOp, so that the calling program is free to 
                          delete the o object after calling this constructor.
                 
             */
             void set( VarOp* o );
             
             /// diagnostic dump of the VarExprItem
             /*! This method writes the contents of the VarExprItem to std::cerr
                 in a human-readable text format. This includes any VarVal or VarOp
                 object that is being held.
                 
                 \param indent the number of spaces to indent each line of output.
                        This allows the VarOp dump to be nested within of other objects' dumps.
             */
             void dump( int indent ) const; 
             
      };
 
 

      /// holds parsed variable expressions
      /*! The VarExpr class implements arbitrary arithmetic, boolean, stroing, amd date expressions.
          An expression expression is represented as a queue or operators and operands in
          reverse Polish form.  For example, "(4 + 5)*2 - 10" would be implemented as
          the elements "4", "5", "+", "2", "*", "10", and "-" in the queue in that order.
      
          See the documentations for the VarVal and VarOp classes on how values (operands) and operators
          are implemented.
                    
          Note that the VarExpr has no evaluation methods. This is because variable references in the operands
          can be resolved only by a class that has access to the entire collection of variables.
          And that is at a higher level than VarExpr. See the Catalog::eval() methods for more information.
          
      */
      class VarExpr {
           
           public:
              
              /// the queue of VarVal operands and VarOp operators, each held within a VarExprItem object
              std::vector<VarExprItem*> items;
           
              /// destructor
              /*! This is the destructor for the VarExpr class
              */
              ~VarExpr();
              
              
              /// return how many items are in the queue
              /*! This method returns how many VarExprItem objects are
                  currently in the items qeueu.
                  
                  \return the number of items
              */    
              inline int size() 
              {
                  return items.size();
              }
              
              /// adds a VarVal operand to the items stack
              /*! This method adds a given VarVal to the queue.
              
                 \param v a pointer to the VarVal object that is to be held. What is actually held
                          is a copy of the VarVal, so that the calling program is free to 
                          delete the v object after calling this constructor.
               
              */
              void add( VarVal* v );

              /// adds an VarOp operator to the items stack
              /*! This method adds a given VarOp to the queue.
              
                 \param o a pointer to the VarOp object that is to be held. What is actually held
                          is a copy of the VarOp, so that the calling program is free to 
                          delete the o object after calling this constructor.
               
              */
              void add( VarOp* o );
              
              /// Returns an expression item fom the queue
              /*! This method retrieves a given item from the queue, whether VarVal operand
                  or VarOp operator.  The item is not removed from the queue.
              */ 
              inline VarExprItem* getItem( int i ) 
              {
                  return items[i];     
              }
              
             /// diagnostic dump of the VarExpr
             /*! This method writes the contents of the VarExpr to std::cerr
                 in a human-readable text format. This includes any VarExprItem
                 objects in the queue.
                 
                 \param indent the number of spaces to indent each line of output.
                        This allows the VarOp dump to be nested within of other objects' dumps.
             */
              void dump( int indent=0 ) const;
           
      };
      
      
      /// holds defined Catalog variables
      /*! Variables in a Catalog object are used, ultimately, to interpolate variable referecnes in a Target's template string,
          resulting in a filename or URL that correspsonds to requested conditions.
          
          Variables do not store simple values. Rather, a Variable stores a set of pairs of expressions;  one expression
          is for testing conditions and the other is for possible variable values. When a Variable is evaluated
          (by one of the Catalog's eval() methods), its test expressions are evaluated one by one, in order, until
          one such expression evaluates to the boolean value "true". If that happens, then the corresponding
          value expression is evaluated, and the resulting value is retunned as the Variable's value.
          (If no expression evaluates as true, then an error is thrown. In the configuration lines that
          define a variable, there should always be an unconditional definition at the end to ensure
          the variable is defined even if none of the other test conditions is true.)
          
                
      */
      class Variable {
          public:
          
            /// the name of the Catalog Variable. This identifer may be used in variable references elsewhere
            std::string name;
            /// the type of the variable. Must be 'S', "B', 'I', 'F', or 'D'.
            char type;
            /// true if the variable as at least one test-value pair of expressions to define it
            bool defined;
          
            /// constructor
            /*! This is the constructor for the Variable class.
                It can set the name and tupe of a variable, but the variable is still considered
                undefined until test-value expression pairs have been assigned to it.
                
                \param naym the name used to refer to the Variable. This identifer may be used in variable references elsewhere
                \param typ the type of the new Variable. Must be 'S', "B', 'I', 'F', or 'D'.
            */
            Variable( const std::string& naym="", char typ=0 );
            
            /// destructor
            /*! This is the destructor for the Variable class.
            */
            ~Variable();
          
            /// adds a variable value expressions and test expression pair to the variable
            /*! This method adds a new test-value expression pair to the Variable.
            
                Note that the VarExpr pointers are stored as pointers, instead of making copies of the objects
                to which they point. Thus, by calling addValue() the calling routine is turning
                over those object to the Variable for all subsequent management and should not
                attempt to dleete themm.
            
                \param value a pointer to a VarExpr expression object that determines the value of the variable
                \param test a pointer to the VarExpr expression object that determines whether the value parameter will be in affect
            */
            void addValue( VarExpr* value, VarExpr* test);
            
            /// returns the number of test/value pairs that define this variable
            /*! this method returns the number of test-value expression pairs that define the Variable.
            
                \return the number of expression pairs
            */    
            int size();

            /// returns the value and test expressions for element index in the variable definition list
            /*! This method retrieves a specified test-value expression pair from the Variable.
            
                \param index the index of the desire pair
                \param value a pointer to a pointer to a VarExpr for the value expression of the pair
                \param test a pointer to a pointer to a VarExpr for the test expression of the pair
            
            */
            void getExprs( size_t index, VarExpr** value, VarExpr** test);


             /// diagnostic dump of the Variable
             /*! This method writes the contents of the Variable to std::cerr
                 in a human-readable text format. This includes any test and value
                 expression pairs.
                 
                 \param indent the number of spaces to indent each line of output.
                        This allows the VarOp dump to be nested within of other objects' dumps.
             */
            void dump( int indent=0 ) const;
            
          private:
            
            /// hold the test expressions that define this Variable
            std::vector<VarExpr*> tests;
            
            /// holds the value expressions that define this Variable
            std::vector<VarExpr*> values;
       
      };
      
      
      /// this class holds a collection of variables
      /*! The VarSet class manages a collection of Variables. 
      
      */
      class VarSet {
          public:
          
             /// true if local variables have been added to this varset; false otherwise
             bool has_locals;
             /// quantity used to set up local variables
             std::string quant;
             /// valid-at time used to set up local variables
             double time;
             /// modle run used to set up local variables
             std::string tag;
           
             /// constructor 
             VarSet();
           
             /// \brief defines a new variable
             /*! This method adds a new Variable to the VarSet.
             
                \param name the name used to refer to the Variable. This identifer may be used in variable references elsewhere
                \param type the type of the new Variable. Must be 'S', "B', 'I', 'F', or 'D'.
             
             */
             void define( const std::string& name, const char type );
             
             /// \brief removes a variable, with all its definitions
             /*! This method removes a Variable form the VarSet and deletes it.
             
                 \param name the name of the Variable to be removed
             
             */
             void undefine( const std::string& name );

             /// \brief returns whether a given variable exists
             /*! This method lets a calling rojtine determine whether a Variable exists in the VarSet.
                 Note that the variable may exist (i.e., have a name and a type) without being
                 defined (i.e., having at least one test-value expression pair).
             
                 \param name the name of the Variable whose existence is to be determined
                 \return true of the variable exists; false otherwise.
                 
             */
             bool exists( const std::string& name ) const;

             /// \brief returns a pointer to a Variable
             /*! This method lets a calling routine access a variable in the VarSet.
             
                 \param name the name of the Variable to be returned
                 \return pointer to the Variable, if it exists, or NULLPTR otherwise
                  
             */
             Variable* getVariable( const std::string& name );

             /// \brief adds a new value to a variable, defining it if it does not already exist
             /*! This method adds a test-value expression pair to a specified Variable.
             
                Note that the VarExpr pointers are stored as pointers, instead of making copies of the objects
                to which they point. Thus, by calling addValue() the calling routine is turning
                over those object to the Variable for all subsequent management and should not
                attempt to dleete themm.
            
                 \param name the name of the Variable to be defined
                 \param type the type of the variable. This must match the type of other definitions of the same variable.
                 \param value a pointer to the VarExpr expression for the value
                 \param test a pointer to the VarExpr expression for the test                 
             
             */
             void addValue( const std::string& name, const char type, VarExpr* value, VarExpr* test); 

             /// destroys all variables
             /*! This method deletes all variables held by the VarSet.
             */
             void clear();
             
             /// diagnostic dump of the VarSet
             /*! This method writes the contents of the VarSet to std::cerr
                 in a human-readable text format. This includes any Variables
                 held within.
                 
                 \param indent the number of spaces to indent each line of output.
                        This allows the VarOp dump to be nested within of other objects' dumps.
             */
            void dump( int indent=0 ) const;

          private:
          
             /// the collection of Variables. the key is the Variable name and the value is the Variable itself.
             std::map< std::string, Variable > vars;
             
      };


      /// \brief hold a dimension
      /*! A Dimension hold the specifications for a spatial dimension of gridded data.
          
      */
      class Dimension {
      
          public:
           
              /// the name of the dimension
              std::string name;
              /// the physical quantity represented by this dimension
              std::string quant;
              /// the units of the dimension
              std::string units;
              /*! specifies how the dimensional values are specified:
                  - 0 = uninitialzied
                  - 1 = values are explicitly given and stored in values
                  - 2 = start, delta, and n are specified
                  - 3 = start, end, and n are specified
              */
              int mode;
              /// the starting value of a set of regularly-spaced dimensional values
              float start;
              /// the ending value of a set of regularly-spaced dimensional values
              float end;
              /// the difference between successive values of a set of regularly-spaced dimensional values
              float delta;
              /// the number of successive values of a set of regularly-spaced dimensional values
              size_t n;
              /// contains the values of an explicitly-defined set of dimensional values
              std::vector<float> vals;
              
              /// the constructor
              Dimension();
              
              /// sets the start, number of values, and the delta between values
              /*! This method sets the dimension by specifiying
                  the starting value, the number of values, and the delta between values
                  
                  \param strt the starting value
                  \param num the number of values
                  \param delt the delta between successive values
              
              */    
              void set( float strt, size_t num, float delt);
              
              /// sets the start, the end, and the number of values
              /*! This method sets the dimension by specifiying
                  the starting value, the ending value, and the numkber of values
                  
                  \param strt the starting value
                  \param fin the ending value
                  \param num the number of values
              
              */    
              void set( float strt, float fin, size_t num);

              /// sets the values explicitly
              /*! This method sets the dimension by specifiying
                  them all.
                  
                  \param v a reference to a vector of values
              
              */    
              void set( std::vector<float>& v );
              
              /// returns the values of the dimension
              /*! This methoid returns a vector of dimensional values
                  computed if necessary from the start, end, n, and/or delta.
                  
                  \return a pointer to a vector of floats containing the gridpoint values of the dimension.
                          It is hte responsibility of the calling routine to delete this 
                          vector once it is no longer needed.
              */
              std::vector<float>* values();

             /// diagnostic dump of the Dimension
             /*! This method writes the contents of the Dimensionj to std::cerr
                 in a human-readable text format. 
                 
                 \param indent the number of spaces to indent each line of output.
                        This allows the Dimension dump to be nested within of other objects' dumps.
             */
             void dump( int indent=0 ) const;
              
      }; 
      
      
      /// The collection of Dimensions
      /*! The DimensionSet class manages the Dimensions of a Catalog.
          This makes it possible to easily add Dimensions, retrieve them, and 
          query for their existence.
      */
      class DimensionSet {
          public:
          
             /// defines a new Dimension in the DimensionSet
             /*! This method adds a new Dimension to the Dimensionset.
             
                 \param dim a pointer to the new Dimension. 
             
             */
             void define( const Dimension* dim );

             /// returns whether a given Dimension exists
             /*! This method is used for query the DimensionSet to see whather a Dimension
                 with a given name exists within.
                 
                 \param name the name of the Dimension whose existence is to be determined
                 \return true if the Dimension is defined in this DimensionSet, false otherwise
             */    
             bool exists( const std::string& name ) const;

             /// retrives a named Dimension
             /*! This method retrieves a Dimension from the DimensionSet.
             
                 \param name the name of the Dimension to be retrievd
                 \return a pointer to the Dimension, or NULLPTR if no Dimension by that name is defined.
                 
             */
             Dimension* getDimension( const std::string& name );

             /// destroys all Dimensions
             /*! This method clears all definitoins of all Dimensions in the DimensionSet.
             */
             void clear();
          
             /// diagnostic dump of the DimensionSet
             /*! This method writes the contents of the DimensionSet to std::cerr
                 in a human-readable text format. 
                 
                 \param indent the number of spaces to indent each line of output.
                        This allows the DimensionSet dump to be nested within of other objects' dumps.
             */
             void dump( int indent=0 ) const;

         private:
         
             /// the collection of Dimensions
             std::map< std::string, Dimension > dims;
      };

      

      /// \brief holds a time interval
      /*! A TimeInterval holds an arbitrary time interval
          that is split up into years, months, days, and seconds.
          The splitting is needed because not every year is of the same lengths,
          and neither does every month.
          
          The idea is to be able to apply a TimeInterval to a date and 
          obtain a new date as a result. 
           
      */
      class TimeInterval {
      
           public:
           
              /// the number of years
              int years;
              /// the number of months
              int months;
              /// the number of days
              int days;
              /// the number of seconds
              double secs;
              /// true if all timestamps are in one URL
              bool allHere;
              
              /// the constructor
              TimeInterval();
              
              /// Parses a string, extracting the parts
              /*! This method parsea an ISO time period string, populating
                  its members fomr the parts of the string.
              
                  The time period string
                  starts with a 'P', 
                  optionally followed by an integer and a'Y' (denoting the number of years),
                  optionally followed by an integer and a 'M' (denoting the number of months), 
                  optionally followed by an integer and a 'D' (denoting the number of days),
                  optionally followed by a time interval specification.
                  
                  The time interval specification begins with a 'T',
                  optionally followed by an integer and a 'H' (denoting the number of hours), 
                  optionally followed by an integer and a 'M' (denoting the number of minutes),
                  optionally followed by an integer and a 'S' (denoting the number of seconds).
                  
                  for example, "P1Y6" is a period of one year and six months.
                  "P1M" is a period of one month.
                  "PT1M" is a period of one minute.
                  "PT1H" is a period of one hour.
                                  
              
                  \param interval the string that specifies the interval
                  \return returns the index o fthe final character parsed, 0 if parsing failed
              
              */
              size_t parse( const std::string& interval );
              
              
              /// Parses a time period string, extracting the parts
              /*! This method parsea an ISO time period string, populating
                  its members from the parts of the string.
              
                  The time period string
                  starts with a 'P', 
                  optionally followed by an integer and a'Y' (denoting the number of years),
                  optionally followed by an integer and a 'M' (denoting the number of months), 
                  optionally followed by an integer and a 'D' (denoting the number of days),
                  optionally followed by a time interval specification.
                  
                  The time interval specification begins with a 'T',
                  optionally followed by an integer and a 'H' (denoting the number of hours), 
                  optionally followed by an integer and a 'M' (denoting the number of minutes),
                  optionally followed by an integer and a 'S' (denoting the number of seconds).
                  
                  for example, "P1Y6" is a period of one year and six months.
                  "P1M" is a period of one month.
                  "PT1M" is a period of one minute.
                  "PT1H" is a period of one hour.
                                  
              
                  \param interval a character array that specifies the interval
                  \param len the number of characters in the character array to parse
                  \param start the starting position in the character array at which parsing is ot begin
                  \return returns the index o fthe final character parsed, 0 if parsing failed
              
              */
              size_t parse( const char* interval, size_t len, size_t start=0 );
              
      };

      /// \brief entity for determining filenames and URLs
      /*! A Target describes an entity that can be opened for reading data. Instead of a string that contains
         an actual filename or URL, though, the Target contains a template from which such a filename or URL
         can be constructed, based on the data being requested.  A request for data will cause the Target to 
         "resolve" to an appropriate filename or URL, which is returns in a DataSource object along with 
         some descriptive attributes.
         
         A file to which the filename or URL refers is assumed to consist of one or more
         time snapshots of one or more gridded physical quantities. (The type of grid does not
         matter to Catalog, except as it may be indicated in the filename or URL; presumably the MetData 
         object that uses the Catalog will concern itself
         with that.).  Because a data request generally will refer to a specific time at which valid data
         are desired, A Target will resolve to two filenmes or URLs that straddle the desired time.
         To aid in determining which file hold which times, the Target has a base time and a time increment.
         
      */
      class Target {

          public:
             
             /// the name of the target
             std::string name;
             
             /*! the attributes. Attirbute names are defined in the Catalog configuration and apply to all defined Targets.
                 A Target's attributes are the values that correspond to those names
             */    
             std::map< std::string, std::string > attrs;
             
             /*! the template for a filename or URL. The template string is transformed into the filename or URL
                 by interpolating variable references contained in the template
             */    
             std::string templayt;
             
             /// the base time (in hours). This is the hour of the day of the first time snapshot contained in the file.
             double basetime;

             /*! holds the temporal spacing between successive URLs, as years, months, days, and seconds
             */
             TimeInterval urlSep;
             
             /*! a base date related true first time in the snapshot in the file. 
                (the VarVal nominal is set to the date string (format of a date literal, but may contain 
                embedded variable refs)
             */
             VarVal dbase;
             
             /// the time separation (in hours) between successive time snapshots within a single URL
             double inctime;
             
             /// the number of time snapshots in a single URL (value <=0 are valid and mean indeterminant)
             int nt;
             
             /// retrieve an attribute of this Target
             /*! This method returns an attribute value of the Target.
             
                  \param attr the name of the attribute to be retirved
             */
             std::string getAttr( const std::string& attr);
             
             /// diagnostic dump of the Target
             /*! This method writes the contents of the Target to std::cerr
                 in a human-readable text format. 
                 
                 \param indent the number of spaces to indent each line of output.
                        This allows the Target dump to be nested within of other objects' dumps.
             */
             void dump( int indent=0 ) const;

      };
      
      /// The collection of Targets
      /*! The TargetSet class manages the Targets of a Catalog.
          This makes it possible to easily add Targets, retrieve them, and 
          query for their existence.
      */
      class TargetSet {
          public:
          
             /// defines a new Target in the TargetSet
             /*! This method adds a new Target to the Targetset.
             
                 \param tgt a pointer to the new Target. Once handed to the TargetSet, the TargetSet assumes all responsibility 
                        for managing the Target. In particular, the calling routine should not delete the Target pointed to by
                        tgt.
             
             */
             void define( const Target* tgt );

             /// returns whether a given Target exists
             /*! This method is used for query the TargetSet to see whather a Target
                 with a given name exists within.
                 
                 \param name the name of the Target whose existence is to be determined
                 \return true if the Target is defined in this TargetSet, false otherwise
             */    
             bool exists( const std::string& name ) const;

             /// retrives a named Target
             /*! This method retrieves a Target from the TargetSet.
             
                 \param name the name of the Target to be retrievd
                 \return a pointer to the Target, or NULLPTR if no Target by that name is defined.
                 
             */
             Target* getTarget( const std::string& name );

             /// destroys all Targets
             /*! This method clears all definitoins of all Targets in the TargetSet.
             */
             void clear();
          
             /// diagnostic dump of the TargetSet
             /*! This method writes the contents of the TargetSet to std::cerr
                 in a human-readable text format. 
                 
                 \param indent the number of spaces to indent each line of output.
                        This allows the TargetSet dump to be nested within of other objects' dumps.
             */
             void dump( int indent=0 ) const;

         private:
         
             /// the collection of Targets
             std::map< std::string, Target > targets;
      };

      /// a Class for referencing Targest from Quantities
      /*!  When a data request is made toa Catalog, the physical quantity desired is looked up
           and a list of Targets is returned which could be used to access that quantity.
           
           Hopefully, the files that the Target points the calling routine to will include all
           of the necessary metadata to make sense ot the data that is read from it.
           Unfortunately, that hope is not always justified. Thus, one may need to specify
           such things as units in the Catalog configuration. And because the same physical quantity
           with the same name might have different units when accessed via different Targets,
           those will need to be associated with each Target for each quantity.
           
           The TargetRef class supplies a way of making that association. It holds the name
           of a specific Target, and it holds optional metadata about the quantity.
      
      */
      class TargetRef {
          public:
             
             /// the name of the Target
             std::string tname;
             
             /// the units of the quantity, in whatever convention is sued by the MetData class that uses this Catalog
             std::string units;
             /// a scale factor that will take the data read form its native unoits to MKS units
             float scale;
             /// an offset that will be used to take the data from its native units to MKS units
             float offset;
             /// a human-readable string for describing the data
             std::string description;
             
             /// the base constructor for the TargetRef
             /*1 This is the base contructor for the TargetRef class.
             */
             TargetRef();
             
             /// The fancy constructor for a TargetRef
             /*! this is the full-blown constructor for the TargetRef class, permitting
                 full specification of all the information it holds.
             
             \param tget the name of the Target to be used to acquire a given quantity
             \param uu the units of the quantity
             \param scl the scale factor for transforming the quantity to MKS units
             \param offst the offset used for transforming the quantity to MKS units
             \param dd a string that describes the data as read via the Target
             
             */
             TargetRef( const std::string& tget, const std::string& uu="", float scl=1.0, float offst=0.0, const std::string& dd="" );
          
             /// diagnostic dump of the TargetRef
             /*! This method writes the contents of the TargetRef to std::cerr
                 in a human-readable text format. 
                 
                 \param indent the number of spaces to indent each line of output.
                        This allows the TargetRef dump to be nested within of other objects' dumps.
             */
             void dump( int indent=0 ) const;
      };


  
      /// Defines how a desired physical quantity may be accessed
      /*! The Quantity class defines how a givenphysical quantity may be accessed by 
          the MetData object that uses the Catalog.
          
          A Quantity has a name (this should be the name by which it is referenced in the data set itself),
          as well as a list of TargetRef objects that are used to access the data.
      
      */
      class Quantity {
          public:
             
             /// the name of the quantity
             std::string name;
             
             /// a standardized name fo rthe quantity
             std::string stdname;
             
             /// the number of spatial dimensions a single time snapshot of the data has
             int dims;
             
             /// the set of TargetRefs that describe the locations of that quantity
             std::vector<TargetRef> tlist;
             
             /// The constructor for the Quantity class
             /*! This is the constructor for the Quantity class.
                  
                  \param nayme the name of the quantity
                  \param stdnayme a standardized name for  the quantity (e.g., acccording to CF naming conventions)
             */
             Quantity( const std::string& nayme, const std::string& stdnayme="" );
             
             /// clears all TargetRef specifications
             /*! This method clears out the list of TargetRef objects.
             */
             void clear();
             
             /// returns the number of TargetRef objects held by this Quantity
             /*! This method returns how many Targets are associated with this Quantity.
             
                 \return the numberof TargetRef objects for this Quantity
             */    
             int size() const;
             
             /// diagnostic dump of the Quantity
             /*! This method writes the contents of the Quantity to std::cerr
                 in a human-readable text format. This includes all of its TargetRef objects.
                 
                 \param indent the number of spaces to indent each line of output.
                        This allows the Quantity dump to be nested within of other objects' dumps.
             */
             void dump( int indent=0 ) const;
             
      };


      /// Manages a Catalog's Quantities
      /*! The QuantitySet class provides a way for a Catalog to manage the Quantity objects that it has defined.
      */
      class QuantitySet {
          public:
          
             /// defines a new Quantity
             /*! This method adds a new Quantity to eh QuantitySet.
             
                 \param quant the Quantity to be added. The QuantitySet assumes all responsibility for managing
                              the Quantity, so the calling routine should not attempt to delete it.
             */
             void define( const Quantity* quant );

             /// returns whether a given Quantity exists
             /*! This method returns whether a given quantity has been defined in this QuantitySet.
             
                 \param name the name of the Quantity whose existence is to be defined
                 \return true of the Quantity is defined, false otherwise
             */
             bool exists( const std::string& name ) const;

             /// retrieves a named Quantity
             /*! This method retrieves a named Quantity fomr the QuantitySet.
             
                 \param name the name of the desired Quantity
                 \return a pointer to the Quantity requested. This Quantity must not be deleted by the calling routine. 
             */
             Quantity* getQuantity( const std::string& name );
             
             /// destroys all Quantities
             /*! this method deletes all of the Quantities from the QuantitySet
             */
             void clear();
             
             /// returns the number of quantities defined
             /*! This method returns the number of Quantity objects held within.
             
                 \return the number of Quantity objects
             */
             int size() const;

             /// diagnostic dump of the QuantitySet
             /*! This method writes the contents of the QuantitySet to std::cerr
                 in a human-readable text format. This includes all of its Quantity objects.
                 
                 \param indent the number of spaces to indent each line of output.
                        This allows the QuantitySet dump to be nested within of other objects' dumps.
             */
             void dump( int indent=0 ) const;
             
             
             /// maps between data set quantity names and standard quantity names
             std::map< std::string, std::string > stdnames;

          private:
             
             // the collection of quantity objects. The key is the name of the Quantity, and the value is the Quanity itself.
             std::map< std::string, Quantity > quants;
          
      };


      // (These are accessible by various classes defined within the Catalog class:)

      /// returns true is the input character is whitespace
      static bool isWhitespace( char cc );

      /// returns true if the input character is a letter
      /*! This method tests whether a given character is a letter.
          A letter is in the range 'A' through 'Z', or 'a' through 'z';
      
          \param cc the char to be tested
          \return true if the char is a letter, false otherwise
      */     
      static bool isLetter( char cc );
      
      /// returns true if the input character is a digit
      /*! This method tests whether a given character is a digit.
          A digit is in the range '0' through '9'';
      
          \param cc the char to be tested
          \return true if the char is a digit, false otherwise
      */     
      static bool isDigit( char cc );

      /// \brief checks whether the next part of a char array is a string literal
      /*! This method scans ahead in an array of chars, looking to see if it contains
          a string literal.
          
          A string literal is a sequence of characters with no variables references or quotes in it.
          If a strin gis not a string ltieral, then it is some kind of string expression.
        
          \param str the char array that contains the string to be scanned
          \param len the number of characters in str    
          \param start the index into str at which scanning is to start
          \return the number of chars that comprise the string literal. If there is no string literal, this is 0.
      */
      static size_t isStringLiteral( const char* str, size_t len, size_t start = 0); 


      /// \brief scans a character array to see whether it holds a boolean literal or expression
      /*! This method scans ahead in an array of chars, looking to see if it contains
          a boolean literal.
          
          A boolean literal consists of the words "true" or "false" (quotes not included).
      
          \param str the char array that contains the string to be scanned
          \param len the number of characters in str    
          \param start the index into str at which scanning is to start
          \return the number of chars that comprise the boolean literal. If there is no boolean literal, this is 0.
      */    
      static size_t isBoolLiteral( const char* str, size_t len, size_t start = 0); 

      /// \brief scans a character array to see whether it holds a int literal or expression
      /*! This method scans ahead in an array of chars, looking to see if it contains
          an integer literal.
          
          An literal consists of an optional sign ("+" or "-"_) followed by one
          or more digits.
      
          \param str the char array that contains the string to be scanned
          \param len the number of characters in str    
          \param start the index into str at which scanning is to start
          \return the number of chars that comprise the integer literal. If there is no integer literal, this is 0.
      */    
      static size_t isIntLiteral( const char* str, size_t len, size_t start = 0); 

      /// \brief scans a character array to see whether it holds a float literal or expression
      /*! This method scans ahead in an array of chars, looking to see if it contains
          a float literal.
          
          A float literal consists of an optional sign ("+" or "-"_, followed by one or more
          digits, optionally followed by a decimal point ("."), followed
          by zero or more digits.
      
          \param str the char array that contains the string to be scanned
          \param len the number of characters in str    
          \param start the index into str at which scanning is to start
          \return the number of chars that comprise the float literal. If there is no float literal, this is 0.
      */    
      static size_t isFloatLiteral( const char* str, size_t len, size_t start = 0); 

      /// \brief scans a character array to see whether it holds a Date literal or expression
      /*! This method scans ahead in an array of chars, looking to see if it contains
          a date literal.
          
          A date literal consists of a four-digit year, followed by "-", followed
          by a two-digit month, followed by a two-digit day-of-month, optionally
          followed by a "T" and a two-digit hour, optionally followed by a ":" and
          a two-digit minute, optionally followed by a ":" and a two digit second
          optionally followed by a "." and a decimal frsaction of a second.
          For example: "2013-07-30T23:45:23.4567".
      
          \param str the char array that contains the string to be scanned
          \param len the number of characters in str    
          \param start the index into str at which scanning is to start
          \return the number of chars that comprise the date literal. If there is no date literal, this is 0.
      */    
      static size_t isDateLiteral( const char* str, size_t len, size_t start = 0); 


      /// \brief convert a string to a string (interpreting escaped characters)
      /*! This method goes through an input string and copies it to an output
          string, detecting and converting escaped characters
          (e.g., "\'") to single characters as it does so.
          
          The escape character is '\\'. "\\\\" becomes '\\'.
          "\\t" becomes a single tab character. 
          Escaped double- and single-quotes become those quote characters.
          
          \param str the input string, which may contain escaped characters
          \param result the output string, which will not contain escaped characters
          \return always returns true
      
      */
      static bool s2uString( const std::string& str, std::string& result);

      /// \brief convert a string with escapable characters to a string (with those characters escaped 
      /*! This method copies an input string to an output string,
          escaping certain characters with the '\\" character.
          Characters escaped include tab, single-quote/apostrophe, ans double quote.
          (This is essentially the inverse of the s2uString() method.)

          \param str the input string
          \param result the output string, which may contain escaped characters
          \param wid if > 0, this is the number of characters to copy to the output. 
                     If <= 0, the entire stgringfrom start to the end will be copied.
          \param start the first character of str that will be copied to the ooutput
          \return always returns true
      
      */
      static bool s2eString( const std::string& str, std::string& result, int wid=0, int start=0);
         
      /// \brief convert a string to a boolean
      /*! This method converts a string to a boolean.
           The input string must be either "true" or "false".
           
           \param str the input string
           \param result the output boolean
           \return returns true if the conversion was successful, false otherwise
      
      */
      static bool s2Bool( const std::string& str, bool& result);
         

      /// \brief convert a boolean to a string
      /*! This method converts a boolean value to a string.
      
           \param val the input boolean
           \param result the output string, either "true" or "false"
           \param wid output width. If less than or equal to zero, the entire string is returned in result. Otherwise, the 
                      result is truncated to the first wid characters.
           \return always returns true

      */
      static bool b2String( bool val, std::string& result, int wid=0 );

      /// \brief convert a string to an integer
      /*! This method converts s string to an integer.
      
          \param str the input string
          \param result = the output integer
          \returns true if the conversion was successful, false otherwise
      
      */
      static bool s2Int( const std::string& str, int& result );
   
         
      /// \brief convert an integer to a string
      /*! This method converts an integer value to a string.
      
           \param val the input integer
           \param result the output string
           \param wid output width. If less than or equal to zero, the entire string is returned in result. Otherwise, the 
                      result is padded with leading zeroes to the given widdth. IF the width is too small to hold
                      the string, the entire string isoutput.
           \return true if the conversion was successful,

      */
      static bool i2String( int val, std::string& result, int wid=0 );

      /// \brief convert a string to a float
      /*! This method converts a string to a floating-point number.
      
          \param str the input string
          \param result the output floating-point number
          \return true if the conversion was successful, false otherwise
      
      */
      static bool s2Float( const std::string& str, float& result );   
         
      /// \brief convert a float to a string
      /*! This method converts an float value to a string.
      
           \param val the input float
           \param result the output string
           \param wid output width. If less than or equal to zero, the entire string is returned in result. Otherwise, the 
                      result is formatted with this width.
           \param dec the number of digits to include past the decimal point, if >=0. Otherwise the default precision is used.           
           \return true if the conversion was successful,
      
      */
      static bool f2String( float val, std::string& result, int wid=0, int dec=-1 );

      /// an array of month lengths, used in date calculations
      static const int monthlengths[13];
      
      /// an array of day-of-year numbers for each month, used in date calculations
      static const int daynums[13];

      /// \brief convert a string to a numeric date
      /*! Converts a string to a double precision number the holds
          the number of days elapsed since 1899-12-31T00:00:00.
          
          The input string must be one of the forms 
          "yyyy-mm-ddThh:mm:ss.sss", "yyyy-mm-ddThh:mm:ss",
          "yyyy-mm-ddThh:mm", "yyyy-mm-ddThh", or "yyyy-mm-dd".
          
          \param str the input string. 
          \param result the output double-precision numeric date
          \return true if the conversion was successful, false otherwise
      
      */
      static bool s2Date( const std::string& str, double& result );
         
      /// \brief returns whether a given year is a leap year
      /*! This method returns whether a given year is a leap year or not.
      
          \param year the year to be examined
          \return true if the year is a leap year, false otherwise
      */
      static bool isLeap( int year );

      /// \brief convert a numeric date into parts of a date: year, month, day, etc.
      /*! This method converts a numeric date and calculated the various parts that make up a date.
      
           \param val the input double-precision numberic date ( the number of elapsed days since 1899-12-31T00:00:00)
           \param year the output integer year  (e.g., 2021) 
           \param month the output integer month of the year (1-12)
           \param dayofmonth the output integer day of the month (1-31)
           \param dayofyear the output integer day of the year (1-366)
           \param hours the output integer hour of the day (0-23)
           \param minutes the output integer minutes past the hour (0-59)
           \param seconds the output float seconds past the minute (0-59.99999)
      
      */
      static void d2parts( double val, int& year, int& month, int& dayofmonth, int& dayofyear, int& hours, int& minutes, float& seconds ); 
         
      /// \brief convert a date to a string
      /*! This method converts a numeric date to a string. The output string
          is of the form "yyyy-mm-ddThh:mm:ss.sss", althouyh it can be shortened
          with the wid and start parameters. If the output string truncactes some portion of
          the time, the smaller time parts may be rounded up into the larger ones.
          For example if the full date is "2021-07-23T23:30:59.9999" is truncated
          to the "yyyy-mm-ddThh:mm" form, then the result is "2021-07-23T23:31".
          But theis rounding up is confined to the time and never affects the date.
          "2021-07-23T23:59:59.9999" truncated to the "yyyy-mm-ddThh" form yields
          "2021-07-23T24".
      
           \param val the input double-precision numberic date ( the number of elapsed days since 1899-12-31T00:00:00)
           \param result the output string
           \param wid output width. If less than or equal to zero, the entire string is returned in result. Otherwise, the 
                      result is set to be this wide.
           \param start the starting point of the full "yyyy-mm-ddThh:mm:ss" string to be sent to the output.       
           \return true if the conversion was successful,
      */
      static bool d2String( double val, std::string& result, int wid=0, int start=0 );


      /// \brief find the next occurrance of a character
      /*! This method finds the next occurrance of a given character
          in a character array.
          
          \param ch the character being searched for
          \param str the array of characters being searched
          \param start the starting index position in the character array to start the search
          \param stop if >=start, do not search past this position. if <start, serach to 0-byte stirng temrinator
          \return the index of the character if found, -1 if not;
      */
      static int findNext( char ch, const char* str, size_t start= 0, int stop=-1  );    

      /// scan for a given (unescaped) character
      /*! This method is like the findNext() method, except that characters
          that are escaped (i.e., preceded by a "\") or inside quopted ("aaa" or 'bbbb')
          are not considered to match the given character.
          
          \param ch the character being searched for
          \param str the array of characters being searched
          \param start the starting index position in the character array to start the search
          \param stop if >=start, do not search past this position. if < start, search to 0-byte string terminator
          \return the index of the character if found, -1 if not;
      */
      static int scanFor( char ch, const char* str, size_t start= 0, int stop=-1 );    
          
   
      /// scans for a string
      /*! This method scans a char array for an exact match to a given string
      
          \param match the string to be matched against  
          \param str the array of characters being searched
          \param start the starting index position in the character array to start the search
          \param stop if >=start, do not search past this position. if < start, search to 0-byte string terminator
  
          \return the index of the end fo the match, it a match was found, -1 otherwise
      */
      static int matchAgainst( const std::string& match, const char* str, size_t start=0, int stop=-1 );

  
      /// \brief parses a veriable reference
      /*! This method scans a char array, parsing and extracting a variable reference
         
          A variable referecne consists of a leading "$", optionally followed
          by one or more digits for a format code, optionally followed by a "." and
          opne or more digits for a second format code, follwoed by "{",
          followed by an identfier, and terminated by a "}".
          For example, "${foo}", "$4{foo}", and "$8.2{foo}" are all valid 
          variable references.
         
          \param str the array of characters being searched
          \param idx a reference to an index into the str at which the scanning of str is to start. If a variable
                     reference is found, idx is left at the character just after the scanned portion.
                     (This might leave idx pointing at the null terminator byte.)
         \param name if a variable reference is found, this string will contain the entire variable reference, from "$" to "}".            
         \param ref if a variable reference is found, this string will contain the name of the varibale referenced.  
         \param fmt1 if a variable reference is found and a format code is included, fmt1 is set to that integer. (Otherwise it is set to -1).          
         \param fmt2 if a variable reference is found and a second format code is included, fmt2 is set to that integer. (Otherwise it is set to -1).          
           
      */ 
      static void extractVarRef( const char* str, size_t& idx, std::string& name, std::string& ref, int* fmt1, int* fmt2 );

      /// \brief get a regular identifier
      /*! This method advances through a character array,
          constructing an string that contains an identifier.
          
          \param str the character array being scanned.
          \param idx the index into the character array. On return, this
                     is the index of the next character beyond the identifier,
                     or the terminating null.
          \return a string containing the identifier name
      */
      static std::string getIdentifier( const char* str, size_t& idx );   

      /// \brief get a weird identifier
      /*! This method advances through a character array,
          constructing an string that contains a non-conforming identifier
          enclosed in parentheses. 
          
          This is used for finding quantity names. In most cases, the quantity name
          is a legitimate Catalog identifier, and the getIdentifier() methos is used.
          But some data sets may have some quantoities whose names are not 
          legal Catalog identifiers. In those cases, enclosing the quantity
          name in parentheses lets the Catalog parser know to take everything
          from the opening parenthesis up to the matching closing parentheses,
          and use that as the quantity name.
          
          \param str the character array being scanned.
          \param idx the index into the character array. On return, this
                     is the index of the next character beyond the identifier,
                     or the terminating null.
          \return a string containing the identifier name
      */
      static std::string getWeirdIdentifier( const char* str, size_t& idx );   


      
   public:

      /// \brief Holds information about possible sources of data
      /*! The DataSource class holds the information returned from a data query.
      
          This include not only the filenames or URLs preceeeding and following
          the desired valid-at time of the data, but also certain characteristics 
          associated with those data sources.
          
      */   
      class DataSource {
      
          public:
          
              /// the name of the quantity
              std::string name;
              /// the validAt time for which the data set was queried
              std::string time;
              /// the number of spatial dimensions of a single time snapshot of data
              int dims;
              /// the name of the target that produced this destination
              std::string target;
              /// the units of the data
              std::string units;
              /// a scale factor used to convert the data as read to MKS units
              float scale;
              /// an offset used to concert the data as read into MKS units
              float offset;
              /// a brief description of the data
              std::string description;
              /*! the type of thing that the pre and post members are:
                 
                   * -1: unknown
                   * 0: filename (starts with "/" or "file://")
                   * 1: URL (starts with "http://" or "https://")
                   * 2: OTF: e.g., "OTF://quant1,quant2", to indicate quantities needed to do on-the-fly calculations
              */
              int type;




              /// the number of time snapshots in the pre URL
              int tN;


              /// the time delta within the pre/post URL, in days
              double tDelta;
              /// the time just preceeding or at the valid-at time
              std::string t0;
              /// the first time snapshot in the pre URL, in days
              double preStart_t;
              /// the first time snapshot in the pre URL
              std::string preStart;
              /// the index of the time snapshot in the pre URL
              int preN;
              /// a filename or URL for the source of the data just preceeding or at the valid-at time
              std::string pre;
              
              /// the time just following the valid-at time
              std::string t1;
              /// the first time snapshot in the pre URL, in days
              double postStart_t;
              /// the first time snapshot in the post URL
              std::string postStart;
              /// the index of the time snapshot in the pre URL
              int postN;
              /// a filename or URL for the source of the data at or just following the valid-at time
              std::string post;
              
              /// the class constructor
              /*! This is the constructor for the DataSource class.
              */
              DataSource();

             /// diagnostic dump of the DataSource
             /*! This method writes the contents of the DataSource to std::cerr
                 in a human-readable text format. 
                 
                 \param indent the number of spaces to indent each line of output.
                        This allows the DataSource dump to be nested within of other objects' dumps.
                        
             */
             void dump( int indent ) const;
              
      };





   // Errors 

   /// \brief An exception for a missing configuration file
   class badNoFile {};
   
   /// \brief An exception for a configuration syntax error
   class badConfigSyntax {};
   
   /// \brief An exception for mismatching Variable types
   class badVarType {};
   
   /// \brief string to data type conversion error
   class badDataConversion {};

   /// \brief multiply-defined target error
   class badMultiplyDefinedTarget {};

   /// \brief multiply-defined dimension error
   class badMultiplyDefinedDimension {};

   /// \brief multiply-defined quanity error
   class badMultiplyDefinedQuantity {};
   
   /// \brief recursively-defined variable
   class badRecursiveVar {};
   
   /// \brief expression evaluation failure
   class badExpression {};

   /// \brief undefined target error
   class badUnknownTarget {};

   /// \brief bad date spec
   class badDateString {};

   /// \brief bad value conversion
   class badValueConversion {};
   


   
      /// \brief the basic constructor
      /*! This is the basic constructor for the Catalog class.
   
          Theis class expects to be inititalized by reading a configuration
          that sets up rules on how to map a meteorological data request to
          a URL. The question is how to find the configuration file?
          The Catalog class does with via a "taG": a string that is associated
          with a particular data source.
  
          If the confLocator is specified in the constructor, then the Catalog object
          will look for its configuration according as described 
          in the deswcription of the findConfig() method.
          
          If a confLocator is given and a configuration file is found, then 
          it is read automatically.
          
          If a confLocator is given and a configuration files is not found, then an error is 
          thrown.
          
          If no confLocator is given, then the Catalog if left unconfigured. 
          Note, however, that some kind of configuration for a data set
          must be laoded into the Catalog object before it can be put
          to any practical use.
          
          \param tagg the confLocator that specifies the meteorological dataset whose
                     configuration is to be read.
   
      */
      Catalog( const std::string& tagg = "" ); 
   
      /// \brief sets the debug level
      /*! This method sets the debug level, which determines how verbose
          messages are printed to std::cerr as the Catalog object is sued.
          
          \param level 0 means no beginning messages. The higher the number, the higher the verbosility
      */
      void debug( int level);
      
      /// \brief returns the debug level
      /*! This method returns the current setting of the debugigng level.
      
          \return the current debug level
      */
      int debug();        
   
      /// \brief sets the confLocator used to find the catalog configuration
      /*! This method sets the confLocator that is used to specify where
          the Catalog's configuration file is to be found, as described
          in the class basic constructor..
          
          Note that, unlike the constructor, this method does not find
          or load any configuration. It merely sets the confLocator value.
          
          \param tagg the confLocator that specifies the meteorological dataset whose
                     configuration is to be read.
      */
      void confLocator( const std::string& tagg );
   
      /// returns the confLocator setting
      /*! This method returns the currently value of the confLocator.
   
          \return the confLocator that specifies the meteorological dataset whose
                     configuration is to be read.        
      */
      std::string confLocator();
   
   
      /// \brief sets a desired time increment and offset
      /*! This method sets a desired time increment and offset for data snapshots.
      
          Each data sets has its own native succession of snapshots, with its own
          starting time offset within a file and its own time increment between
          successive snapshots in the file.  Generally, a user should use
          the data set's native time characteristics. For some purposes, however, it may
          be desirable to use a subset of those snapshots. for exampe, if data grids
          are given at every hour, starting at 00Z, the user might want to use
          data at every six hours.
          
          This method permits specifying such alternate incrments, as well as a time offset.
          (Tho offset can be used, for example, to select data every 24 hours, starting at 12Z.)
          
          Note that the increment specified here must be an integral multiple of the native data 
          set's time increment. Similarly, the time offset, if specified, must be set such that
          the difference between it and the native offset must be an integral multiple of the native time increment.
          
          \param tinc the desried time incrment, in hours
          \param toff (optional) the desired time offset, in hours
          
      */
      void timeSpacing( double tinc, double toff=0.0 );
      
      /// \brief returns the special time spacing settings
      /*! This method returns the desired time increment and offset.
      
           \param toff (optional) a pointer to a double in which the time offset, in hours, will be returned
           \return the desired time increment between successive data snapshots, in hours
      */
      double timeSpacing( double* toff=NULLPTR );      
   
      /// \brief sets a desired attribute by which data sources are to be selected
      /*! This method specifies a value that a Target attribute should match
          as data soruces are evaluated.
          
          \param attr the name of the attribute to be specified
          \param value the value for thet attribute
          \param priority a priority assigned to this attribute. The higher the priority the greater the preference
                          given to this value of this attribute, and a priority of 9999 makes the value mandatory. 
                          Negative priorities make aq vlaue ldess desirable, and a priority of -9999 makes
                          the value forbidden.
      */
      void desired( const std::string& attr, const std::string& value, int priority=1 );    
   
      /// \brief returns a specification for a desired attribute
      /*! If a desired value has been set for a given Target attribute, then this 
          method returns it
      
          \param attr the name of the attribute whose desired value is to be returned
          \param strict a pointer to a boolean that indicates whether this attribute value will be strictly enforced
          \return the value of the attribute
      */ 
      std::string desired( std::string& attr, int* strict=NULLPTR ) const;
   
      /// finds the Catalog's configuration file, using the confLocator
      /*! This method uses the Catalog's confLocator setting to find the
          object's configuration file.

          1. It appends the suffix ".cat" to the Catalog object's 
             confLocator setting and looks for
             a file by that name in these places, in order (stopping
             successfully when it first finds such a file):
                
                1a. the current working directory.
                
                1b. the directory referred to by the environment variable 
                    GIGATRAJ_CATALOGS, if it is defined
                
                1c. the directory /usr/share/gigatraj/, if it exists.
   
          2. If no file has been found, then the confLocator is treated as an
             environment variable whose value is the name of the file.
          
          \return the string containing the file name, if it exist. If no configuration
                  file is found, then an empty string is returned.
             
      */
      std::string findConfig() const;
   
      /// \brief loads configuration information from a file
      /*! This method loads configuration information from a file.
   
          Basically, each text line from the file is read in,
          leading and trailing whitespace is removed,
          and the line is passed to the addRule() method to be 
          parsed and added to eh Catalog configuration.
          (See that method for a description of the format of each line.)
         
         \param filename the name of the file that contains the Catalog's configuration information
      */
      void load( const std::string& filename="" );
      

      /// \brief adds a configuration line
      /*! This method adds a new configuration line to the Catalog's configuration.
   
          \param cfg a string containing a configuration line
          
      */
      void addRule( std::string& cfg );    

      /// \brief resets the catalog to its empty, unconfigured state
      /*! This method resets the Catalog, clearing out all Quantity definitions,
          Target definitions, Variable definitions, and attribute names.
      */
      void clear();

      
      /// the first date for which there may be data
      std::string first_date;
      
      /// the final data for which there m may be data
      std::string final_date;


      ///  \brief retrieve filenames, given a quantity and timestamp
      /*! This method provides the querying mechanism of the Catalog class.
          Given the name of a physical quantity and a date+time at which the
          data are desired, fetchTarget() will consult its configuration and 
          return a set of DataSource objects, each describing a candidate
          source of the data. 
          
          This method cannot guarantee that the source of the data is actually available.
          (The server might be offline, for example.) But it does construct a valid
          filename or URL that can be used to access the data file, if it is available.
          
          \param quantity the name of the physical quantity desired
          \param validAt a date string of the form "yyyy-mm-ddThH:mm:ss" for which data is desired.
          \param dests a vector of output DataSource objects, each of which describes a possible source of the data
          \param tag an arbitrary string, intended to signify a forecast model run, which optoinally may be used in creating the DataSources
          
          \return true if the source, validAt, and tag resolved to a valid data source, false otherwise
      */
      bool query( const std::string& quantity, const std::string& validAt, std::vector<DataSource>& dests, const std::string& tag="" ); 

      /// finds a quantity name, given a standard name
      /*! This method looks up the name of a quantity in a data set that
          corresponds to a standard name. for example, if CF naming conventions
          are the standard being used, then a query for "air_temperature"
          might return a quantity name such as "T".
          
          \param stdname the standard name of the desired quantity, 
          \return the data set's name for the standard-named quantity
      */
      std::string stdLookup( std::string& stdname );
             

      /// returns a Target attribute name, given its index
      /*! This method returns the name of an attribute, given
          its index number.
          
          \param index the number of the Target atttribute
          \return the name of the attribute
      */
      std::string attrName( int index );     

      /// returns the index of Target attribute name
      /*! This method returns the index of an attribute, given
          its name.
          
          \param name the name of the Target atttribute
          \return the index of the attribute, -1 if not found
      */
      int attrIndex( const std::string& name );
      
      /// returns the value of a given attribute of a given Target
      /*! This method returns the value of an attribute of a given Target
      
          \param target the name of the Target being queried
          \param index the index of the attribute whose value is wanted
          \return the value of the attribute 
      
      */
      std::string getAttr( const std::string& target, int index );
      
      /// returns the value of a given attribute of a given Target
      /*! This method returns the value of an attribute of a given Target
      
          \param target the name of the Target being queried
          \param attr the name of the attribute whose value is wanted
          \return the value of the attribute 
      
      */
      std::string getAttr( const std::string& target, const std::string& attr );
      
      /// returns the value of a given attribute of a given Target
      /*! This method returns the value of an attribute of a given Target
      
          \param dest the DataSource from the Target being queried
          \param attr the name of the attribute whose value is wanted
          \return the value of the attribute 
      
      */
      std::string getAttr( const DataSource& dest, const std::string& attr );

      /// \brief obtains the time increment and base time (offset) for a DataSource
      /*! This method obtains the base time of a Datasource and the time increment
          between successive time snapshots within the file.
          
          \param dest the Data source to be queried
          \param tinc (output) a pointer to a float that will receive the time increment
          \param toff (output) a reference to a string that will receive the base time
          \param n (output) a pointer to an integer with the number of time snapshots in the file. <=0 if indeterminant.
      */    
      void get_timeSpacing( const DataSource& dest, double* tinc,std::string& toff, int* n ); 


      /// \brief looks up the named variable and returns its value
      /*!  Given the name of a variable, this method evaluates its value
           and returns that value converted to a string.
           
           Note that unlike the query() method, variableValue() does not
           define a quantity or a date, so the automatically-defined
           variables QUANTITY, DATETIME, and so on are undefined.
           This in turn means that many variables cannot be evaluated,
           and the return value will reflect this. Thus, this method is
           intended mainly for debugging. 
           
           \param name the namke of the defined Variable whose value is sought
           \param output the string value of the evaluated Variable, output
           \return true if the variable was successfully evaluated, false otherwise.
      
      */
      bool variableValue( const std::string& name, std::string& output );


      /// returns dimensional values
      /*! This method looks up a dimension and returns information about it.
      
          \param name a referecne ot a string that contains the name of the desired dimension
          \param quantity a reference to a string that contains the name of the physical quantity (e.g. "lon") of the dimension
          \param units the units of the dimensional values. "" if the dimension is not defined.
          \param values a points to a pointer to a vector of floats contaiing the set of values along the dimension.
                        *values is set to NULLPTR if the dimension is not defined.
                        If values is itself set to NULLPTR on input, then no values will be returned.
           \return true if the dimension exists, falseotherwise
       */
       bool dimensionValues( const std::string& name, std::string& quantity, std::string& units, std::vector<float>** values );    

      /// converts a date string to a catalog numeric time
      /*! This method concerts a date string to the numeric time (in days since 1899-12-31T00:00:00) used by Catalog.
      
           \param time the date string, in "yyyy-mm-ddThh:mm:ss" format
           \return the nuneric time
      */
      double date2number( const std::string& time );

      /// diagnostic dump of the Catalog
      /*! This method writes the contents of the Catalog to std::cerr
          in a human-readable text format.
          
          \param indent the number of spaces to indent each line of output.
                 This allows the VarVal dump to be nested within of other objects' dumps.
      */
      void dump( int indent = 0) const;


          
   private:

      /// debugging level
      int dbug;
      
      /// the confLocator setting for this Catalog   
      std::string my_confLocator;
   
      /// the set of attribute names for this Catalog
      std::vector<std::string> attrNames;
   
      /// the set of variables
      VarSet varset;
      
      /// the set of targets
      TargetSet targetset;
      
      /// the set of dimensions
      DimensionSet dimset;
      
      /// ppinter to a target being evaluated
      Target* currentTarget;
   
      /// the set of quantities
      QuantitySet quantityset;
      
      /// The number of rules that have been loaded
      int nrules;

      // flags indicating whether a given variable is being referenced during some variable's evaluation
      // (used to detect circular variable definitions)
      std::map< std::string, bool > ref;
             
   
      /// The desired time increment, in hours, between successive snapshots to be used. Default is 0 (uses the native time inc)
      double des_tinc;
      /// the desired time offset, in hours, from 0 of the first time snapshot in a data source. default is 0.
      double des_toff;
      
      /// a set of desired attributes, which if set will prioritize one matching ddata source over another
      std::map<std::string, std::string> des_attrs;
      /// a ste of strictness flags, indicating whether the desired attributes *must* match the Targets'
      std::map<std::string, int> des_priorities;
      
   
      /// \brief tests to see whether a file exists
      /*! This method tests whether a file exists.
      
          \param fname the name of the file
          \return true if the file exists, false otherwise
      */    
      bool fileExists( const std::string& fname ) const;

      /// \brief obtains the value of an environment variable
      /*! This method reads the value of an environment variable.
      
          \param varname the name of the environment variable
          \return the value of the environment variable, if it exists. 
                  Otherwise, an empty string is returned.
      */     
      std::string getEnv( const std::string& varname ) const;

      /// \brief strips off leading and trailing whitespace from a string
      /*! This method removes leading and trailing whitespace from a string.
      
           \param line the string to be trimmed.
      
      */
      void trim( std::string& line ) const;
      
      ///  \brief skip whitespace
      /*! This method advances through a character array
          until it encounters non-whitewhape or the end of the string.
          
          \param str the character array being scanned.
          \param idx the index into the character array. On return, this
                     is the index of the next non-whitespace character,
                     or of the terminating null.
      */               
      void skipwhite( const char* str, size_t& idx ) const;

      /// \brief returns true if the input character is a variable type specifier
      /*! This method checks whether the inoput char signficies a
         Variable type.
         
         \param char the charactyer to be eamined
         \returns true if char is an allowed Variable type, false otherwise
      */   
      bool isVarType( char cc ) const;

      /// get attribute names
      /*! This method parses an Attribute Names configuration line.
          The line consists of a ";", followed by a ";"-separated
          list of attribute names.
          
          If the input string does not match the attribute-names line format,
          then a badConfigSyntax error is thrown.
          
          Each Target must have exactly the sane number of attribute values
          as there are attribute names defined here.
          
          \param str the character array being scanned.
          \param idx the index into the character array. On return, this is the index 
                     of the character array's terminating null.
      */
      void parseAttrNames( const char* str, size_t& idx );

      /// parse a dimensional specification
      /*! This method parses an array of characters as a dimensional definition line.
      
          A dimension definition line consust of a dimension identifier, wolloed by "~",
          followed by a string contaiing the units of the dimensional vlaues,
          followed by ";", followed by a format specfifier (eithe r"LDN", "LHN", or "V"),
          followed by ":", followe dby a comma-separated list of floating point values.
      
          \param str the character array being scanned.
          \param idx the index into the character array. On return, this
                     is the index of the next character that no longer fits the dimension-definition format,
                     or of the terminating null.
          \return a pointer to a new Dimension object. If an error was encountered parsing the string,
                  then NULLPTR is returned. Note that it is the responsibility of the calling routine to delete
                  the new dimension once it is no longer needed. 
       */
       Dimension* parseDimDef( const char* str, size_t& idx );

      /// parse a variable value (and test)
      /*! This method parsea sn array of characters as a Variable defintion line.
          
          A Variable definitions line begins with a Variable identifier, followe dby a "=",
          optionally followed by a test expression and a "?", followed by 
          a value expression.
          
          \param var a pointer to the Var object to be pupulated by the string's specifications
          \param str the character array being scanned.
          \param idx the index into the character array. On return, this
                     is the index of the next character that no longer fits the variable-definition format,
                     or of the terminating null.
      */
      void parseVarDef( Variable* var, const char* str, size_t& idx );
      
      /// \brief parses a string literal
      /*! This method scans an input char array, looking for a string literal.
          A string literal begins with an unescaped single or double quote
          and ends with the same quote, with a sequence of zero or more characters
          between them.
          
          \param str the character array being scanned.
          \param idx the index into the character array. On return, this
                     is the index of the next character beyond the string literal,
                     (which may be the terminating null).
          \return a pointer to a new VarVal object of type string, that holds the string literal.
                  It is the responsibility of the calling routine to delete this object once it is no longer needed.
      */    
      VarVal* parseString(  const char* str, size_t& idx ) const;

      /// \brief parses a date literal
      /*! This method scans an input char array, looking for a date iteral.
          A date literal begins with a "[", followed by an ISO date and (optioanlly) time,
          followed by "]".  "[2003-05-27]", "[2021-12-10T23:56:45.475]", and "[1997-04-15T12]"
          are all valid date literals.
          
          \param str the character array being scanned.
          \param idx the index into the character array. On return, this
                     is the index of the next character beyond the date literal,
                     (which may be the terminating null).
          \return a pointer to a new VarVal object of type date, that holds the date literal.
                  It is the responsibility of the calling routine to delete this object once it is no longer needed.
      */    
      VarVal* parseDate(  const char* str, size_t& idx ) const;

      /// \brief parses a boolean literal
      /*! This method scans an input char array, looking for a boolean literal.
          A boolean literal is either the string "true" or "false".
          
          \param str the character array being scanned.
          \param idx the index into the character array. On return, this
                     is the index of the next character beyond the boolean literal,
                     (which may be the terminating null).
          \return a pointer to a new VarVal object of type boolean, that holds the boolean literal.
                  It is the responsibility of the calling routine to delete this object once it is no longer needed.
      */    
      VarVal* parseBool(  const char* str, size_t& idx ) const;

      /// \brief parses a numeric literal
      /*! This method scans an input char array, looking for a integer or float literal.
          A numeric literal consists of an optional sign ("+" or "-"), followed by
          a sequence of one or more digits ("0"-"9"), optionally follwoed
          by a "." and more digits. If there is no ".", then the result value
          is an integer literal; if there is a".", then it is a float literal.
          
          \param str the character array being scanned.
          \param idx the index into the character array. On return, this
                     is the index of the next character beyond the numeric literal,
                     (which may be the terminating null).
          \return a pointer to a new VarVal object of type integer or float, that holds the numeric literal.
                  It is the responsibility of the calling routine to delete this object once it is no longer needed.
      */    
      VarVal* parseNumber(  const char* str, size_t& idx ) const;
      
      /// \brief parses a variable reference
      /*! This method scans an input char array, looking for a variable reference.
          A variable referencestarts with a "$", optionally followed by a seuqnce of digits,
          optionally follwoed by a "." and another seuqence of digits, followed by 
          a "{", followed by the name of the referecned variable, followed by "}".
          Eamples are "${foo}", "$10{foo}", and "$10.3{foo}".      
          
          \param str the character array being scanned.
          \param idx the index into the character array. On return, this
                     is the index of the next character beyond the variable referecne,
                     (which may be the terminating null).
          \return a pointer to a new VarVal object of type string, that holds the variable reference.
                  It is the responsibility of the calling routine to delete this object once it is no longer needed.
      */    
      VarVal* parseVarRef(  const char* str, size_t& idx ) const;
      
      /// \brief parse an operator
      /*! This method scans an input char array, looking for a varioable operator.
           An operator can be any of: "-", "+", "*", "/", "%", "==", "!=", ">", ">=", "<", "<=", 
           "||", "&&", and "!".
          
          \param str the character array being scanned.
          \param idx the index into the character array. On return, this
                     is the index of the next character beyond the operator,
                     (which may be the terminating null).
         \return a pointer to a new VarOp object that holds the operator.
                  It is the responsibility of the calling routine to delete this object once it is no longer needed.
      */
      VarOp* parseOp( const char* str, size_t& idx, int pri=0 ) const;

      /// \brief parse an expression
      /*! This method scans an input char array, looking for a varioable expression.
          A varibale expresion consist of at least one literal or variable reference,
          possibly combined with other literals or variable references by meansof operators.
          Sub-expressions enclosed by parentheses are also allowed.
          Examples include "5", "5 + 3", "${foo}", and " 3*( ${foo} - 10) == 27"
          
          As the character array is scanned the expression is converted into
          a sequence of VarExprItem objects in Reverse Polish order. 
          
          \param str the character array being scanned.
          \param idx the index into the character array. On return, this
                     is the index of the next character beyond the expression,
                     (which may be the terminating null).
         \return a pointer to a new VarExpr objetc that holds the expression.
                  It is the responsibility of the calling routine to delete this object once it is no longer needed.
      */
      VarExpr* parseVarExpr( const char* str, size_t& idx, int stop=-1 ) const;
      
     /// \brief parse a Target
      /*! This method parses an Target definition configuration line.
          The line consists of a Target identifier, followe dby ":=", followed
          by a base time (in hours), ";", a time increment (also in hours),
          ";" a ";"-separted list of attribute values, followed by ";"
          and a template for a filename or URL.
          
          Each Target must have exactly the sane number of attribute values
          as there are attribute names defined.
          
          If the input string does not match the Target definition line format,
          then a badConfigSyntax error is thrown.
          
          \param str the character array being scanned.
          \param idx the index into the character array. On return, this is the index 
                     of the character array's terminating null.
     */
     Target* parseTarget( const char* str, size_t& idx ) const;

     /// parse a quantity line
      /*! This method parses an Quantity definition configuration line.
          The line consists of a Quantity identifier, followed by ":", followed
          by a "|"-separted list of Target references. Each Target reference in turn
          consists of an optional units specifier encloses in "{" and"]", 
          optionally followed by a description enclosed in "[" and "]", followed by the name
          of the Target being referenced. A units specifier consist of
          a string giving the human-readbale units, optionally followed by ";" and a 
          floating-point scale factor, optionally followed
          by another ";" and a floating-point offset.
                
          If the input string does not match the Quantity definition line format,
          then a badConfigSyntax error is thrown.
          
          \param str the character array being scanned.
          \param idx the index into the character array. On return, this is the index 
                     of the character array's terminating null.
     */
     void parseQuantity( Quantity* quant, const char* str, size_t& idx ) const;
     
      /// \brief parse a configuration line
      /*! Given a line of a configuration file, this method determines
          what kind of configuration likne it is and calls theapprioriate method
          to parse it.
        
          \param conf the configurationline to be parsed
      */    
      void parse( const std::string& conf );

     /// \brief set up variables for quantity and time
     /*! This method takes a quantity name, a date string, and an optional forecaqst run string,
         and it used them to define a set of automatically-defined variables. 
         
         This is called whan the Catalog is queried for data, just before the queried quantites'
         Target are examined, which usually triggers a number of variable lookups.
     
         \param quantity the name of the quantity
         \param tyme the ISO-8601 date format ("yyyy-mm-ddThh:mm:ss")
         \param tag a tag such as the forecast run label
         
     */    
     void setup_vars(  const std::string& quantity, double tyme, const std::string& tag );
     
     /// \brief removes variables for quantity and time
     /*! This method removes the automatically-defined variables' definitions.
     */
     void takedown_vars();
     
     /// \brief applies the TestEQ operation to two variable values
     /*! This method applies the TestEQ operation (==) to two variable values.
     
         \param v1 a pointer to the first operand
         \param v2 a pointer to the second operand
         \return a pointer to a new VarVal the holds the result of the operation. It is the 
                 responsibility of the calling routine to delete this object once it is no longer needed. 
     */
     VarVal* opTestEQ( VarVal* v1, VarVal*v2 );

     /// \brief applies the TestNE operation to two variable values
     /*! This method applies the TestNE operation (!=) to two variable values.
     
         \param v1 a pointer to the first operand
         \param v2 a pointer to the second operand
         \return a pointer to a new VarVal the holds the result of the operation. It is the 
                 responsibility of the calling routine to delete this object once it is no longer needed. 
     */
     VarVal* opTestNE( VarVal* v1, VarVal*v2 );

     /// \brief applies the TestGT operation to two variable values
     /*! This method applies the TestGT operation (>) to two variable values.
     
         \param v1 a pointer to the first operand
         \param v2 a pointer to the second operand
         \return a pointer to a new VarVal the holds the result of the operation. It is the 
                 responsibility of the calling routine to delete this object once it is no longer needed. 
     */
     VarVal* opTestGT( VarVal* v1, VarVal*v2 );

     /// \brief applies the TestGE operation to two variable values
     /*! This method applies the TestGE operation (>=) to two variable values.
     
         \param v1 a pointer to the first operand
         \param v2 a pointer to the second operand
         \return a pointer to a new VarVal the holds the result of the operation. It is the 
                 responsibility of the calling routine to delete this object once it is no longer needed. 
     */
     VarVal* opTestGE( VarVal* v1, VarVal*v2 );

     /// \brief applies the TestLT operation to two variable values
     /*! This method applies the TestLT operation (<) to two variable values.
     
         \param v1 a pointer to the first operand
         \param v2 a pointer to the second operand
         \return a pointer to a new VarVal the holds the result of the operation. It is the 
                 responsibility of the calling routine to delete this object once it is no longer needed. 
     */
     VarVal* opTestLT( VarVal* v1, VarVal*v2 );

     /// \brief applies the TestLE operation to two variable values
     /*! This method applies the TestLE operation (<=) to two variable values.
     
         \param v1 a pointer to the first operand
         \param v2 a pointer to the second operand
         \return a pointer to a new VarVal the holds the result of the operation. It is the 
                 responsibility of the calling routine to delete this object once it is no longer needed. 
     */
     VarVal* opTestLE( VarVal* v1, VarVal*v2 );

     /// \brief applies the TestOR operation to two variable values
     /*! This method applies the TestOR operation (||) to two variable values.
     
         \param v1 a pointer to the first operand
         \param v2 a pointer to the second operand
         \return a pointer to a new VarVal the holds the result of the operation. It is the 
                 responsibility of the calling routine to delete this object once it is no longer needed. 
     */
     VarVal* opTestOR( VarVal* v1, VarVal*v2 );

     /// \brief applies the TestAND operation to two variable values
     /*! This method applies the TestAND operation (&&) to two variable values.
     
         \param v1 a pointer to the first operand
         \param v2 a pointer to the second operand
         \return a pointer to a new VarVal the holds the result of the operation. It is the 
                 responsibility of the calling routine to delete this object once it is no longer needed. 
     */
     VarVal* opTestAND( VarVal* v1, VarVal*v2 );

     /// \brief applies the Add operation to two variable values
     /*! This method applies the Add operation (+) to two variable values.
     
         \param v1 a pointer to the first operand
         \param v2 a pointer to the second operand
         \return a pointer to a new VarVal the holds the result of the operation. It is the 
                 responsibility of the calling routine to delete this object once it is no longer needed. 
     */
     VarVal* opAdd( VarVal* v1, VarVal*v2 );

     /// \brief applies the Subtract operation to two variable values
     /*! This method applies the Subtract operation (-) to two variable values.
     
         \param v1 a pointer to the first operand
         \param v2 a pointer to the second operand
         \return a pointer to a new VarVal the holds the result of the operation. It is the 
                 responsibility of the calling routine to delete this object once it is no longer needed. 
     */
     VarVal* opSubtract( VarVal* v1, VarVal*v2 );

     /// \brief applies the Multiply operation to two variable values
     /*! This method applies the Multiply operation (*) to two variable values.
     
         \param v1 a pointer to the first operand
         \param v2 a pointer to the second operand
         \return a pointer to a new VarVal the holds the result of the operation. It is the 
                 responsibility of the calling routine to delete this object once it is no longer needed. 
     */
     VarVal* opMultiply( VarVal* v1, VarVal*v2 );

     /// \brief applies the Divide operation to two variable values
     /*! This method applies the Divide operation (/) to two variable values.
     
         \param v1 a pointer to the first operand
         \param v2 a pointer to the second operand
         \return a pointer to a new VarVal the holds the result of the operation. It is the 
                 responsibility of the calling routine to delete this object once it is no longer needed. 
     */
     VarVal* opDivide( VarVal* v1, VarVal*v2 );

     /// \brief applies the Remainder operation to two variable values
     /*! This method applies the Remainder operation (%) to two variable values.
     
         \param v1 a pointer to the first operand
         \param v2 a pointer to the second operand
         \return a pointer to a new VarVal the holds the result of the operation. It is the 
                 responsibility of the calling routine to delete this object once it is no longer needed. 
     */
     VarVal* opRemainder( VarVal* v1, VarVal*v2 );

     /// \brief applies the TestNOT operation to a variable value
     /*! This method applies the TestNOT operation (!) to a variable value.
     
         \param v1 a pointer to the operand
         \return a pointer to a new VarVal the holds the result of the operation. It is the 
                 responsibility of the calling routine to delete this object once it is no longer needed. 
     */
     VarVal* opTestNOT( VarVal* v1 );

     /// \brief applies the NOP operation to a variable value
     /*! This method applies the null operation (i.e., no operation) to a variable value.
         The result is an unaltered copy of the operand.
     
         \param v1 a pointer to the operand
         \return a pointer to a new VarVal the holds the result of the operation. It is the 
                 responsibility of the calling routine to delete this object once it is no longer needed. 
     */
     VarVal* opNOP( VarVal* v1 );

     /// \brief applies the Negate operation to a variable value
     /*! This method applies the Negate operation (unary -) to a variable value.
     
         \param v1 a pointer to the operand
         \return a pointer to a new VarVal the holds the result of the operation. It is the 
                 responsibility of the calling routine to delete this object once it is no longer needed. 
     */
     VarVal* opNegate( VarVal* v1);
      
      /// \brief applies a unary operator to an operand
      /*! This method applies a unary operator to an operand.
      
          \param op a poiner to the operator's VarOp object
          \param v1 a pointer to the operand's VarVal object
          \return a pointer to a new VarVal the holds the result of the operation. It is the 
                 responsibility of the calling routine to delete this object once it is no longer needed. 
      */     
      VarVal* operate( VarOp* op, VarVal* v1 );
      
      /// \brief applies a binary operator to two operands
      /*! This method applies a binary operator to two operands.
      
          \param op a poiner to the operator's VarOp object
          \param v1 a pointer to the first operand's VarVal object
          \param v2 a pointer to the second operand's VarVal object
          \return a pointer to a new VarVal the holds the result of the operation. It is the 
                 responsibility of the calling routine to delete this object once it is no longer needed. 
      */     
      VarVal* operate( VarOp* op, VarVal* v1, VarVal* v2 );

      /// \brief looks up the named variable and returns its value
      /*! This method looks up a variable definition and evbaluates it value.
      
          \param name the name of the variable to be evaluated
          \return a pointer to a new VarVal object that holds the result
                  of the evaluation. For each definitoin of this variable, the test expression
                  is evaluated, and if it is true, then the value expression is evaluated.
      */                   
      VarVal* getValue( const std::string& name );
      
      /// \brief evaluates a variable expression
      /*! This method evaluates a given variable expression. This includes
          resolvoing all variable referecnes in operands and applying all operators.  
      
          When applying unary operators to operands, the 
          the type of the operand must make sense for the operator.
          The negation operator (-) is applicable only to integers and floats.
          The not operator (!) is applicable only to boolean values. If the operand does not fit the operator,
          then an attempt is made to convert the operand to the appropriate type. If the conversion fails,
          an error is thrown.
          
          \param expr a pointer to the VarEXpr expression that is to be evaluated
          \return a pointer to a new VarVal object that holds the result
                  of the evaluation. For each definitoin of this variable, the test expression
                  is evaluated, and if it is true, then the value expression is evaluated.
      */
      VarVal* eval( VarExpr* expr );
      
      /// \brief evaluates a variable value
      /*! This method converts a variable value that may be a literal or a variable reference,
          and it turns it into a literal VarVal.
          
          If the VarVal is a string, then it may contain multiple variable references.
          all of these are resolved--recursively, if needed.
          
          /param val a pointer to a VarVal value. If this holds a literal, no action is taken.
                     If it is a variable reference, then the reference is resolved
                     and the VarVal is made a literal.
          
          \return true if the evaluation succeeded, false otherwise
                     
      */
      bool eval( VarVal* val );
      
      /// interpolates variable references into a string
      /*! This methoid take sa string which can incorporate variable references,
          and it resolves those references and substitutes them into the string.
          
          \param refstr the input string, which may contain variables references that start witth "$" and end with "}".
          \return a copy of the input string, with all variable references resolved.
      
      */
      std::string interpVarRefs( const std::string refstr );


     /// \brief looks up a variable reference
     /*! This method looks up a variablke reference
     */
     VarVal* lookup( const std::string& name );
     
     /// \brief ensures two vlaues are compatible for an operation
     /*!  When applying a binry operator to two operands, it is important
          to ensure that both operands are of the same type.
          This method does that.
          
          \param v1 a pointer to the first operand's VarVal object
          \param v2 a pointer to the second operand's VarVal object
          \return true if the reconciliation succeeded, false otherwise
     */     
     bool reconcileVals( VarVal* v1, VarVal* v2 ); 
     
     /// find the time after a base time is advanced by a TimeInterval
     /*! This method uses a TimeInterval object to advance a base time
         to determine a future time.
         
         \param base the starting time
         \param tinc a reference to the TimeInterval object that specifes how far in the future to advance
         \return the new time
     */    
     double advanceTime( double base, TimeInterval& tinc );
     
     
     /// find the time after a base time is retreated by a TimeInterval
     /*! This method uses a TimeInterval object to move backwards from a base time
         to determine a past time.
         
         \param base the starting time
         \param tinc a reference to the TimeInterval object that specifes how far in the past to retreat
         \return the new time
     */    
     double retreatTime( double base, TimeInterval& tinc );
             
     /// \brief finds two times that bracket a given time
     /*! This method finds two dsta snapshot times that bracket a given time
     
         \param tyme the time at which data are desired, in days elapsed since 1899-12-31T00 UTC
         \param pre_t a reference to the data time snapshot that preceeeds the desired time
         \param pre_url_t a reference to the data starting time of the URL or file that preceeds or contains the desired time
         \param pre_n a reference to the index of the time snapshot within the pre_url_t URL or file, that immediately preceeds or falls on the desired time
         \param post_t a reference to the data time snapshot that follows the desired time
         \param post_url_t a reference to the data starting time of the URL or file that follows the desired time
         \param post_n a reference to the index of the time snapshot within the post_url_t URL or file, that immediately follows the desired time
         \param ntot a reference to the number of time snapshots within the "pre" URL
     */
     void bracket( double tyme, double& pre_t, double& pre_url_t, int& pre_n, double& post_t, double& post_url_t, int& post_n, int& ntot );

     /// \brief filters candidate data sources according how how well the desired attributes match the actual
     /*! This method inspects a vector of Datasource object, re-ordering them or deleting them
         according to how well their Target's attributes match a set of attributes specified
         by the desired() method calls.
         
         \param ds the vector of DataSource objects. This may be modified as this method runs.
          
     */
     void filter_DataSource( std::vector<Catalog::DataSource>& da );

      /// \brief obtains the time increment, starting time, and number of times for a Target
      /*! Given a Target, this method returns the time spacing info for the target
          
          Note that setup_vars() must already have been called, with a quantity and a valid-at time,
          so that the necessary local variables have been defined. 
          
          \param targ the Target to be queried
          \param tinc (output) a pointer to a float that will receive the time increment
          \param toff (output) a reference to a string that will receive the starting time
          \param n (output) a pointer to an integer with the number of time snapshots in the file. <=0 if indeterminant.
      */    
      void get_targetTimeInfo( Target& targ, double* tinc,std::string& toff, int* n ); 


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
