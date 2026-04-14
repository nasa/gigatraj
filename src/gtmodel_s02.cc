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


/*!

\page  gtmodel_s02 gtmodel_s02: simple trajectory model, using Swarm objects

gtmodel_s02 is a simple implementation of a parcel trajectory model,
based on the gigzatraj library toolkit. It reads parcel positions from
a text file and traces their histories. This model does not use
multiprocessing, but it can use cached data, if a cache directory is defined.


The calling sequence is:
\code
gtmodel_s02 [ --help|-h ] [ --list ] [ --verbose ] [--debug level] [ --config|-c configFile ] [ --rc|-r resourcefile ] \\
               [--cachedir|-d directory ] [ --source|-s metsource ] \\
               [ --begdate|-b yyyy-mm-ddThh:mm:ss ] [ --enddate|-e yyyy-mm-ddThh:mm:ss ] --zerodate|-z yymmddThh:mm:ss\\
               [ --tstep|-t timeDelta ] [ --frequency|-f outputfreq ]  \\
               [ --vertical|-v verticalCoord ] [ --iso|-i ] [ --parcelvertical pVerticalCoord ] \\
               [ --thin thinfactor [ --thinoff thinoffset ] ] \\
               [ --metbasehr metDataBaseHour] \\
               [ --metspacinghr metDataSpacingHour] \\
               [ --conformal adjvalue ] [ --integrator=integName ] \\
               [ --parcels|p parcelfile ] \\
               [--netcdf_out] [--inputnetcdf] \\
               [ --format fmt ] [ --noBadOutput ] [ --input_format fmt ] \\
               [--delay opentime ] [--mpi] [--met_server_ratio m ] \\
               [--save_to savefile ] [ --restore_from savefile ] [ --save_steps nsteps ] [[--keep-save]
\endcode
              
The command-line options are:

 \li \c help : prints out a description of options and then stops
 \li \c list : lists the available meteorological data sources (with brief descriptions) and then stops
 \li \c verbose : prints out messages to let the user know what progress is being made
 \li \c debug : prints out copious technical debugging messages from met field reading functions
 \li \c config : specifies the name of a configation file. The default is "fill_met_cache.config"
              Configuration files are plain text files. Blank lines
              and lines starting with "#" are comments and are ignored. Other lines
              are of the form "keyword=value", where the keyword is a long option name (e.g., "begdate", "source")
              and the value is what is assigned ot the keyword. For example, the line
\code
begdate=2007-10-23T13
\endcode
              specifies that the beginning date is to be set to "2007-10-23T13"
 \li \c rc : specifies the name of a resource file. The default is "$HOME/.gigatrajrc"
          Resource files have the same syntax as configuration files,
          but they are intended to be shared among different standalone programs. Thus, a resource file
          would contain settings that would apply to many different tools, such as the default meteorological
          source, while a configuration file would contain settings that are specific to a single tool
          or even a single run of a tool. 
 \li \c cachedir : specifies a directory into which the cached data files are to be placed. If omitted, then no disk caching is done.
 \li \c  source : specifies the meteorological data source. This may be one of:
               - SBROT = solid-body earth rotation, a test data set only
               - MERRA = the NASA Goddard GMAO MERRA reanalysis
               - MERRA2 = the NASA Goddard GMAO MERRA-2 reanalysis         
  \li \c begdate : the starting time for which parcel histories are to be traced, in "yyyy-mm-ddThh:mm:ss" format
  \li \c enddate : the ending timestamp for which parcel histories are to be traced, in "yyyy-mm-ddThh:mm:ss" format.
                   The enddate may be less than the begdate, in which case, back-trajectories will be calculated.
  \li \c zerodate : the timestamp that is to be used as the model's zero time. By default this will be the beginning time.

  \li \c vertical : the quantity that is to be used as the vertical coordinate of the cached data. The name of the quantity
                    and its default will depend on the data source. 

  \li \c parcelvertical : the quantity used by the input parcels as their vertical coordinate.
                    The default is the vertical coordinate used by the met source, as specified by the --vertical option.

  \li \c iso : parcels are to be traced on isosurfaces of the vertical coordinate, but fixing the vertical wind value 
               in the cooridnate system as zero. This is most useful when using potential temperature as
               the vertical coordinate, to trace parcel trajectories isentropically, which is a reasonable
               approximation in many cirsumstances. With other coordinate systems, speciifying iso will be
               physically meaningful only if a parcel is somehow actively manipulated to
               remain at a given vertical level (e.g., the trajectory of an isopycnic balloon).

  \li \c parcels : The name of a file from which the parcels are to be read.
                   If omitted or set to "-", stdin will be read.  
                   The input is a plain text file consisting of one parcel initial position per line.
                   Blank lines and comment lines (lines that begin with "#") are discarded.
                   Remaining lines are interpreted acording to a StreamREad-class format whihc
                   may be specified by the --input_format option. In the default input format, 
                   each parcel initialization consists of a longitude, a latitude, and a vertical coordinate value
                   (whichever vertical coordinate is being used); these are floating-point numbers separated by 
                   white space characters.

  \li \c format  : An output format specifier, as described in the StreamPrint class. Briefly, output fields
                   are specified with "%" escape codes: %o is longitude, %a is latitude, %v the vertical
                   coordinate, %t is the numerical model time, %T is a calendar-style time string 
                   (e.g. "2019-07-15T14:50:35"), %i is a parcel index, %f is the set of parcle flags,
                   %s is the parcel status, and %{field}m extracts the field value from the parcel's meteorological
                   source at the parcel's location. For exaple "%{T}m" prints the temeprature interpolated to
                   the parcel's longitude, latitude, vertical coordinate, and time. Note that the %v code
                   represents the vertical coordinate used by the model, which is not the same
                   as the input initialization vertical cooridnate, if the --parcelvertical option is used. 

  \li \c noBadOutput : prevents parcels which are not being traced (e.g., parcels which have hit 
                       the ground) from being output
  
  \li \c netcdf_out : sends output to 
  
  \li \c inputformat : an initialization input format specifier, as described in the StreamRead class.
                   The codes are basically the same as for the --format option. Any input fields
                   that match the the %i and %{field}m codes are discarded.
  
  \li \c input_netcdf : specifies that the parcel initialization file is a netcdf file.

  \li \c tstep : the time step to be used in the integration, in minutes.

  \li \c frequency : the (model) time interval, in hours, between output snapshots of the parcel collection. this 
                     should be a multiple of the model time step. If -1, then only the first and the final snapshots
                     are output.

  \li \c metbasehr : imposes a base hour--the first time snapshot in a day for which data are available. (The default is 00Z.)

  \li \c metspacinghr : imposes a spacing between successive time snapshots.
  
  \li \c thin : factor (default 1) by which the thin the horizontal data grid. For example,
                a thinfactor of 2 retains only every other longitude, and every other latitude.
  
  \li \c conformal : determines whether and how adjustments are to be made
                     to spatially-interpoated horizontal wind vectors,
                     to account for the sphericity of the earth near the poles.
                     Zero means apply no adjustment. A negative value means use the default
                     for the integrator. A positive value's effect depends on the integrator
                     being used.

  \li \c integrator  specifies the name of the integrator to be used. (e.g., "RK4", "RK4a").
                     (The default value, RK4a, is usually the best.)
     
  \li \c metoptions ; specifies a string of ";"-separated options for the met data source.
                      Each option consists of a keyword followed by an equals sign and a value;
                      (for example "optionA=value1;OptionB=value2") The values are passed as
                      strings to the Met data source's setOption() method; the object is free
                      to ignore any options that is does not recognize.
   
  \li \c thinoff : an offset (default 0) into the longitude index, used when thinning. If
                   thinfactor is 2 and thinoffset is 0, then the longitudes 0, 2, 4,... are
                   retained. If thinfactor is 2 and thinoffset is 1, the longitudes
                   1, 3, 5, ... are retained.

  \li \c format : provides a formatting string that determines how the parcle information is printed.
                  (See the StreamPrint class docucmentation for more information on codes to use in
                  the format string.) Note that a newline will be appended ot the end of this string.

   \li \c delay : sets a delay, in seconds, to used each time a URL is opened.
   
   \li \c mpi : runs in multiprossessing mode, using OpenMPI. Not that with this flag set,
                you will need to use something like mpirun to run the model, instead of
                running it directory from the command line. 
  
   \li \c met_server_ratio: sets the ratio of parcel-tracing processors to meteorological data servers 
                            in a multiprocessing environment. 
   
   \li \c save_to  after every N time steps, save the model state to the specified file, for later
                   restoration if the model run is interrupted.
   
   \li \c restore_from restore the model sate fomr the specified file first, then resume the model run from there.
   
   \li \c save_steps  the number of steps to trace until the model sate is saved to a savefile
                      The default is 500. 

   \li \c keep_save  Ordinarily the save file (if used) is deleted after a successful run. Use this option to keep it.
   
Note that the begdate and enddate settings are mandatory: they must be defined somewhere, usually in the command line options.

Settings are applied in this order: first, built-in default values are loaded. These defaults are:
      - verbose = off
      - help = off
      - list = off
      - config = gtmodel_s02.config
      - rc = $HOME/.gigatrajrc
      - parcels = "-"
      - format = "%t, %i, %o, %a, %v"
      - tstep = 15

Next, settings from the resource file are loaded, overwriting any previously-loaded settings.

After that, settings from the configuration file are loaded, overwriting any previously-loaded settings.

Finally, settings from the command-line options are loaded, overwriting any previously-loaded settings.



*/


#include <iostream>
#include <sstream>
#include <vector>

#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/FilePath.hh"
#include "gigatraj/Configuration.hh"

#include "gigatraj/MetSelector.hh"

#include "gigatraj/SerialGrp.hh"
#ifdef USE_MPI
#include "gigatraj/MPIGrp.hh"
#endif

#include "gigatraj/IntegRK4a.hh"
#include "gigatraj/IntegRK4.hh"

#include "gigatraj/Parcel.hh"
#include "gigatraj/Swarm.hh"
#include "gigatraj/trace.hh"
#include "gigatraj/PGenFile.hh"
#include "gigatraj/ChangeVertical.hh"

#include "gigatraj/StreamPrint.hh"
#include "gigatraj/StreamDump.hh"
#include "gigatraj/StreamLoad.hh"
#ifdef USE_NETCDF
#include "gigatraj/PGenNetcdf.hh"
#include "gigatraj/NetcdfOut.hh"
#endif

using namespace gigatraj;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;


/*------------------------------------------------------------------------------------------*/
/* This function sets up configuration parameters and gathers settings
   from configuration files and command line options
*/
int getconfig(int argc, char * const argv[], Configuration& conf, MetSelector &metPick ) 
{

    // the meteorological data source
    string src;
    // the starting date+time for tracing parcels
    string begdate;
    // the ending date+time for tracing parcels, in whatever calendar time system the meteorological data source is using
    string enddate;
    // the vertical coordinate system to be used
    string vertical;
    // the directory in which cached meteorological data files should be kept
    string cachedir;
    // where to read parcel initial locations from
    string parcelsource;
    // flag that indicates whether anything went wrong
    int status;
    // argv index, pointing to a command-line option
    int aidx;
    // print help text and quit?
    bool doHelp;
    // usage help string
    std::string usage;
    // list of met sources
    std::vector<std::string> srclist;
    
    
    usage = "gtmodel_s02 ";
    
    // start pout assuming all is well
    status = 0;



    //---------- define the settings
    /*
       This serves as an example of how to use the Configuration class.
       The following lines each define a configuration parameter.
       The calling sequence is: 
         conf.add( name, type, defaultValue, shortOpt, flags, description )
         
         If your system has long-style command-line options, then you
         can set a parameter by putting two hyphens in front of the 
         parameter name ("--verbose"). You can also use the short form
         ("-v") if you like or if your system does not have long-style command line
         options.

    */

    usage +=  "[ --help|-h ]";
    conf.add("help"      , cBoolean, "N"                , "h", 0, "print help" );
    usage +=  " [--list] ";
    conf.add("list"      , cBoolean, "N"                ,  "", 0, "list available met sources and quit" );
    
    // the config files from which settings may be read
    usage += " [--rc|-r resourcefile] ";
    conf.add("rc", cConfig, string(getenv("HOME")) + "/.gigatrajrc", "r" );
    usage +=  " [--config|-c configfile]";
    conf.add("config", cConfig, "gtmodel_s02.config", "c" );

    // and now the settings themselves
    usage +=  " [ --parcels parcelfile ]";
    conf.add("parcels"   , cString , "-"                , "p", 0, "parcel source file" );
    usage +=  " --begdate|-b datetime ";
    conf.add("begdate"   , cString , ""                 , "b", 0, "beginning date (e.g., 2008-12-05T10)" );
    usage +=  " --enddate|-e datetime ";
    conf.add("enddate"   , cString , ""                 , "e", 0, "ending date" );
    usage +=  " --source|-s metsource ";
    conf.add("zerodate"   , cString , ""                , "z", 0, "date equivalent ot the model's internal time 0.0" );
    usage +=  " --source|-s metsource ";
    conf.add("source"    , cString , ""                 , "s", 0, "data source" );
    usage +=  " [--vertical|-v verticalCoordQuantity]";
    conf.add("vertical"  , cString , ""                 , "v", 0, "model vertical coordinate" );
    usage +=  " [--parcelvertical ParcelVerticalCoordQuantity]";
    conf.add("parcelvertical"  , cString , ""           , "",  0, "parcel input/output vertical coordinate" );
    usage +=  " [--cachedir|-d dir]";
    conf.add("cachedir"  , cString , ""                 , "d", 0, "met data cache directory" );
    usage +=  " [--frequency|-f outputFreq]";
    conf.add("frequency" , cFloat  , "1.0"              , "f", 0, "output frequency, in hours" );
    usage +=  " [--tstep|-t timedelta ]";
    conf.add("tstep"     , cFloat  , "15.0"             , "t", 0, "integration time step, in minutes" );
    usage +=  " [--iso|-i ]";
    conf.add("iso"       , cBoolean, "N"                , "i", 0, "trace parcels on isosurfaces of the vertical coordinate" );
    usage +=  " [--verbose] ";
    conf.add("verbose"   , cBoolean, "N"                , "" , 0, "print detailed progress messages" );
    usage +=  " [--debug level] ";
    conf.add("debug"     , cInt    , "0"                , "" , 0, "print debugging messages for data access" );
    usage +=  " [--metbasehr metDataBaseHour]";
    conf.add("metbasehr" , cFloat, "-1.0"               , "" , 0, "impose this time base/offset, so that each day's data snapshots start with this hour" );
    usage +=  " [--metspacinghr metDataSpacingHour]";
    conf.add("metspacinghr" , cFloat, "0.0"             , "" , 0, "impose this time spacing, so that the met data snapshots are spaced this many hours apart" );
    usage +=  " [--thin thinfactor [ --thinoff thinoffset ] ]";
    conf.add("thin"      , cInt,  "1"                   , "" , 0, "horizontal thinning factor" );
    conf.add("thinoff"   , cInt,  "0"                   , "" , 0, "horizontal thinning offset" );
    usage +=  " [--metoptions 'optionA=value1;optionb=value2']";
    conf.add("metoptions"  , cString , ""                 , "", 0, "met data option settings" );
    usage +=  " [--conformal flagValue ]";
    conf.add("conformal" , cInt,  "-1"                  , "" , 0, "conformal wind vector adjustments" );
    usage +=  " [--integrator integratorName ]";
    conf.add("integrator"    , cString,"", "", 0, "Non-default Integrator to be used" );
    usage +=  " [--format fmt]";
    conf.add("format"    , cString,"%t, %i, %o, %a, %v", "", 0, "StreamPrint parcel output format string" );
#ifdef USE_NETCDF
    usage +=  " [--netcdf_out outputfile]";
    conf.add("netcdf_out"    , cString,""              , "", 0, "The output is to be sent to the netcdf file specified" );
    usage +=  " [--inputnetcdf]";
    conf.add("input_netcdf", cBoolean,"N"               , "", 0, "Parcel input file is a netcdf file" );
#endif
    usage +=  " [--noBadOutput] ";
    conf.add("noBadOutput", cBoolean,  "N"              , "" , 0, "prevent non-traced parcels from being output" );
    usage +=  " [--inputformat ifmt]";
    conf.add("inputformat", cString,""                 , "", 0, "StreamRead parcel input format string" );
    usage += " [ --delay opendelay ]";
    conf.add("delay"     , cInt,  "0"                   , "" , 0, "open delay in seconds" );
#ifdef USE_MPI
    usage +=  " [--mpi] ";
    conf.add("mpi"   , cBoolean, "N"                , "" , 0, "use OpenMPI multiprocessing" );
    usage +=  " [--met_server_ratio m ] ";
    conf.add("met_server_ratio", cInt, "5"          , "" , 0, "met client:server ratio (e.g., 3 means a 3:1 ratio)" );
#endif
    usage += " [ --save_to savefile ]";
    conf.add("save_to"     , cString,  ""                   , "" , 0, "file to save model state to" );
    usage += " [ --restore_from savefile ]";
    conf.add("restore_from", cString,  ""                   , "" , 0, "file from which to restore model state" );
    usage +=  " [--save_steps nsteps ] ";
    conf.add("save_steps", cInt, "500"          , "" , 0, "number of time steps bwtween which model state is to be saved" );
    usage +=  " [--keep_save] ";
    conf.add("keep_save"   , cBoolean, "N"                , "" , 0, "keep save_file after a successful run" );

    // Load the config setting values from any config files, as well as the command line
    // The defaults are loaded first
    // Then the config files, in the same order as they were defined.
    // Then the command-line options
    aidx = conf.load(argc,argv);
    
    
    // take care of the "help" case here
    // (This is also an example of how to retrieve a parameter's value.)
    conf.fetchParam("help", doHelp);
    if ( conf.get("help") == "Y" ) {
       conf.help(usage,"");
       exit(0);
    }
    
    // take care of the "list" case
    conf.fetchParam("list", doHelp);
    if ( conf.get("list") == "Y" ) {
       // not ethat we list only the met sources that are on a lat/lon grid
       srclist = metPick.list(METCAP_LATLON);
       for ( int i=0; i<srclist.size(); i++ ){
           cout << srclist[i] << ": " << metPick.description(srclist[i]) << endl;
       }
       exit(0);
    }
    
    //---------- do some simple checks before returning to the main program
    
    // User must specify source, beginning,  and ending,
    // and any tdelta must be positive nonzero.
    src = conf.get("source");
    begdate = conf.get("begdate");
    enddate = conf.get("enddate");
    parcelsource = conf.get("parcels");
    if ( src == "" || begdate == "" || enddate == "" || parcelsource == "" ) {
       // At least one was not defined. This is an error.
       status = 1;
    }
    
    // Any errors?  Print out a usage message.
    if ( status != 0 ) {   
       cerr << "Bad configuration." << endl;
       conf.help(usage,"");
    }
    
    return status;
}

Swarm* restore( Parcel &pcl, std::string &restore_file, ProcessGrp *pgrp, int mcsr, double *time, double *accumul_time )
{
    Swarm* result;
    std::ifstream input;
    int id;
    int np;
    StreamLoad* loadfrom;

    
    result = NULL;

    input.open( restore_file, std::ios::in | std::ios::binary );
    if ( input.is_open() ) {

        id = 999;
        input.read( reinterpret_cast<char *>(&id), static_cast<std::streamsize>( sizeof(int) ) );
        if ( id == 0 ) {
        
           input.read( reinterpret_cast<char *>(time), static_cast<std::streamsize>( sizeof(double) ) );
           input.read( reinterpret_cast<char *>(accumul_time), static_cast<std::streamsize>( sizeof(double) ) );
        
           input.read( reinterpret_cast<char *>(&np), static_cast<std::streamsize>( sizeof(int) ) );
           
           result =  new Swarm( pcl, pgrp, np, mcsr);
           
           loadfrom = new StreamLoad( 1 ); // binary parcel dump file
           loadfrom->stream( &input );
           loadfrom->apply( *result );
           delete loadfrom;
           
        }
        
        input.close();
    }
    
    return result;
}


void save( std::string &save_file, double time, double accumul_time, Swarm*  swarm )
{
    int id;
    int np;
    std::ofstream saveto;
    StreamDump *out;
    
    saveto.open( save_file, std::ios::out | std::ios::binary );
    if ( saveto.is_open() ) {
       
       id = 0;
       saveto.write( reinterpret_cast<char *>(&id), static_cast<std::streamsize>( sizeof(int) ) );
       
       saveto.write( reinterpret_cast<char *>(&time), static_cast<std::streamsize>( sizeof(double) ) );
       saveto.write( reinterpret_cast<char *>(&accumul_time), static_cast<std::streamsize>( sizeof(double) ) );
       
       np = swarm->size();
       saveto.write( reinterpret_cast<char *>(&np), static_cast<std::streamsize>( sizeof(int) ) );
       
       out = new StreamDump( saveto, 1 );
       out->apply( *swarm );       
       delete out;
       
       saveto.close();

    }
}


std::string trim_string( std::string& str )
{
     std::string result;
     size_t istart;
     size_t iend;
     size_t slen;
     char cc;
     
     result = "";

     slen = str.size(); 
     if ( slen > 0 ) {
        istart = 0;
     
        cc = str[istart];
        while ( ( istart < slen) && (cc == ' ' || cc == '\t' || cc == '\n' ) ) {
           istart++;
           cc = str[istart];
        }
        if ( istart < slen ) {
           iend = slen - 1;
           while ( ( iend >= 0 ) && (cc == ' ' || cc == '\t' || cc == '\n' ) ) {
              iend--;
              cc = str[iend];
           }
           if ( iend >= istart ) {
              result = str.substr( istart, iend - istart + 1 );
           }
           
        }
     }
     
     return result;
}

int parse_met_options( const std::string& options, std::vector<std::string>& keys, std::vector<std::string>& values )
{
      int result = 0;
      bool more;
      size_t istart;
      size_t iend;
      size_t optslen;
      std::string pair;
      std::string key;
      std::string val;
      size_t equals;
      
      keys.clear();
      values.clear();
      
      optslen = options.size();
      if ( optslen == 0 ) {
         return result;
      }
      
     // std::cerr << "parse_met_options: <<" << options << ">>" << std::endl; 
      
      more = true;
      istart = 0;
      while (more) {
          // std::cerr << "   istart=" << istart << std::endl;
          // search for the delimieter between key=value pairs.
          iend = options.find( ";", istart );
          if ( iend == std::string::npos ) {
             iend = optslen;
          }
          // we end on the character just before the ';'
          // or on the string's ;last character
          iend --;
          //std::cerr << "   istart=" << istart <<", iend=" << iend << std::endl;
          
          if ( istart < iend ) {
             // extact this key=value pair form the string of options
             pair = options.substr( istart, iend - istart + 1 );
             //std::cerr << "     keyval pair: <<" << pair << ">>" << std::endl; 
             
             // now look for the equals sign
             equals = pair.find( "=" );
             if ( equals != std::string::npos ) {
                //std::cerr << "     found equals at " << equals << std::endl;
                
                if ( (equals > 0) && ( equals < (pair.size()) - 1) ) { 
                   key = pair.substr(0, equals );
                   val = pair.substr(equals + 1, pair.size() - equals );
                   //std::cerr << "     key=<<" << key << ">>, value=<<" << val << ">>" << std::endl; 
                   
                   key = trim_string( key );
                   val = trim_string( val );
                   
                   //std::cerr << "    *key=<<" << key << ">>, value=<<" << val << ">>" << std::endl; 
                   
                   keys.push_back(key);
                   values.push_back( val );
                   
                   istart = iend + 2;
                   if ( iend >= optslen ) {
                      more = false;
                   } 
                } else {
                   // "+" is the first or the last charectsr? Error!
                   result = 1;
                   more = false;
                }           
             } else {
                 // no "="? that's an error
                 result = 1;
                 more = false;
             }
          } else {
              more = false;
          }
      
      }
      

      return result;
}


/*------------------------------------------------------------------------------------------*/

int main( int argc, char * argv[] ) 
{
    // return status. 0 = all went well.
    int status;
    // configuration object
    Configuration config;
    // flag for printing information about what the program is doing
    bool verbose;                      
    // the name of the vertical coordinate to be used by the meteorological data source
    string vertical;
    // the name of the parcel vertical coordinate to be used for input and output
    string parcelVertical;
    // the name of the meteorological data source
    string metsrc_name;
    // the file contaiing parcle initial locations
    string parcelsource;
    // the starting date for which data are to be obtained.
    // (Note that the format is whatever the meteorological data source uses.)
    string begdate;
    // the ending date for which data are to be obtained.
    // (Note that the format is whatever the meteorological data source uses.)
    string enddate;
    // the model zero time
    string zerodate;
    // the meteorological data cache directory 
    string cachedir;
    // the output frequency, in seconds
    float outfreq;
    // flag to indicate whether we are to trace on isosurfaces of the vertical coordinate
    bool isosfc;
    // flag to indicate whether to continue to iterate
    bool tracing;
    // met data time base/offset
    float tbase;
    // met data time spacing
    float tspace;
    // met source debugging level
    int debug;
    // output format string (StreamPrint style)
    std::string fmt;
    // input format string (StreamRead style)
    std::string ifmt;
    // time delta, in minutes
    double tstep;
    // use MPI?
    bool use_mpi;
    // met client-to-server ratio
    int mcsr;
    // bad-parcel output flag
    bool nobad;
    // save/restore
    bool do_restore;
    bool do_save;
    bool keep_save;
    // save-file
    std::string save_file;
    // restore-file
    std::string restore_file;
    // save-interval
    int sinterval;
    // save-count
    int scount;
    // we dump parcels states every accumul_time interval    
    double accumul_time;
#ifdef USE_NETCDF
    // flag indicating whether to use netcdf for input
    bool inNetcdf;
    // flag indicating whether to use netcdf for output
    bool outNetcdf;
    // the name of the output netcdf file
    std::string outNetcdfFile;
    // for reading a netcdf file
    PGenNetcdf* in_netcdf;
    // of rwriting to a netcdf file
    NetcdfOut* out_netcdf;
#endif

    // a comma-separated list (no spaces!) of quantities to be read and cached
    // by the meteorological data source
    string quantities;
    // generates the meteorological data source object
    MetSelector metPick;
    // the meteorological data source object
    MetGridData *metsource;
    // a met data source for parcel vert coord conversion
    MetGridData *altmetsource;
    // the current, starting, and ending times, in internal model time
    double time, finaltime, begtime, endtime;
    double tdelta;
    // a throwaway data point (thrown away before we care only about the
    // side effect of causing the cache files to be created)
    real junk;
    // the quantity currently being read
    string quant;
    // the units of the vertical cooridnte of the meteorological data source
    string vertunits;
    // flag to be set if we are still searching for commas in the
    // quantities string, as we break it apart by commas
    int searching;
    // positions in the quantities string, used while breaking apart
    // by commas
    size_t begpos, endpos;
    // File path for output cache directory
    FilePath filepath;
    // resource file name
    string rcfile;
    // Parcels some from here
    PGenFile parcelFactory;
    // source of parcels
    std::ifstream infile;
    // parcel source that we actually use
    std::istream *input;
    // The Swarm of Parcels to be traced
    Swarm *swarm;
    // Lets us iterate over the Parcels in the swarm
    Swarm::iterator iter;
    // process group for serial (no multiprocessing here)
    // process group for MPI multiprocessing
    ProcessGrp *pgrp;
    // sample Parcel
    Parcel pcl;
    // To print parcel info to stdout
    StreamPrint Out;
    // thinning specs
    int thin, thinoffset;
    // met source capaability flags
    int metcaps;
    // produce output for a time step
    bool do_output;
    // Parcel filter for converting their vertical coordinates
    ChangeVertical* inVChange;
    // iterator for output
    Swarm::iterator fiter;
    // special Parcel for input/output
    Parcel fpcl;
    // delay before each url open
    int delay = 0;
    // wind vector adjustment for spatial interpolation
    int confml;
    // met data source options
    std::string metoptions;
    std::vector<std::string> metopt_keys, metopt_vals;
    // indicates whether cooridnate conversions are needed
    bool conv_parcel_coords;
    // was the vertical coord set by the user?
    bool spec_vertical;
    // was the parcel vertical coordinate set by the user?
    bool spec_parcelvertical;
    // the name of a non-default integrator
    std::string integratorName;
    // A non-default integrator
    Integrator* integrator;


    // we assume all will go well (until it doesn't)    
    status = 0;

    // configure the model
    status = status | getconfig( argc, argv, config, metPick );

    // Proceed only if there have been no errors so far
    if ( status == 0 ) {

       // get the configuration settings
       config.fetchParam("verbose", verbose);
       vertical = config.get("vertical");
       spec_vertical = ( vertical != "" );
       parcelVertical = config.get("parcelvertical");
       spec_parcelvertical = ( parcelVertical != "" );
       metsrc_name = config.get("source");
       parcelsource = config.get("parcels");
       begdate = config.get("begdate");
       enddate = config.get("enddate");
       zerodate = config.get("zerodate");
       cachedir = config.get("cachedir");
       config.fetchParam("frequency", outfreq);
       config.fetchParam("iso", isosfc);
       config.fetchParam("metbasehr", tbase);
       config.fetchParam("metspacinghr", tspace);
       config.fetchParam("thin", thin);
       config.fetchParam("thinoff", thinoffset);
       config.fetchParam("conformal", confml);
       config.fetchParam("integrator", integratorName );
       config.fetchParam("metoptions", metoptions);
       debug = config.str2int( config.get("debug") );
       fmt = config.get("format");
       config.fetchParam("noBadOutput", nobad);
       ifmt = config.get("inputformat");
       tstep = config.str2dbl( config.get("tstep") );
       delay = config.str2dbl( config.get("delay") );
#ifdef USE_MPI
       config.fetchParam("mpi", use_mpi);
       config.fetchParam("met_server_ratio", mcsr);
#else
       use_mpi = false;
       mcsr = 0;
#endif
       config.fetchParam("save_to", save_file );
       config.fetchParam("restore_from", restore_file );
       sinterval = config.str2int( config.get("save_steps") );
       config.fetchParam("keep_save", keep_save);
#ifdef USE_NETCDF
       config.fetchParam("input_netcdf", inNetcdf );
       outNetcdfFile = config.get("netcdf_out");
       outNetcdf = ( outNetcdfFile != "" );
#endif
       
       do_save = ( save_file != "" ) && ( sinterval > 0 );
       do_restore = ( restore_file != "" );
       
       // Create the process group
#ifdef USE_MPI
       if ( use_mpi ) {
          // create a process group (MPI, of course)
          // This will call MPI_Init().
          pgrp = new MPIGrp(argc, argv);
          if (verbose) {
             std::cerr << "Doing MPI process groups" << std::endl;
          }
       } else {
#else
       if ( 1 ) {
#endif
          // we use a serial group--no multiprocessing.
          pgrp = new SerialGrp();
          if (verbose) {
             std::cerr << "Doing a serial process group" << std::endl;
          }
       }

       if ( zerodate == "" ) {
          zerodate = begdate;
       }
                 
       if (verbose) {
          cerr << "verbose = " << verbose << endl;
          cerr << "parcelsource = " << parcelsource << endl;
          cerr << "source = " << metsrc_name << endl;
          cerr << "vertical = " << vertical << endl;
          cerr << "parcelvertical = " << parcelVertical << endl;
          cerr << "debug = " << debug << endl;
          cerr << "isosurface = " << isosfc << endl;
          cerr << "begdate = " << begdate << endl;
          cerr << "enddate = " << enddate << endl;
          cerr << "zerodate = " << zerodate << endl;
          cerr << "tstep = " << tstep << endl;
          cerr << "cachedir = " << cachedir << endl;
          cerr << "frequency = " << outfreq << endl;
          cerr << "metbasehr = " << tbase << endl;
          cerr << "metspacinghr = " << tspace << endl;
          cerr << "thin = " << thin << endl;
          cerr << "thinoff = " << thinoffset << endl;
          cerr << "conformal = " << confml << endl;
          cerr << "integrator = " << integratorName << endl;
          cerr << "metoptions = " << metoptions << endl;
          cerr << "format = " << fmt << endl;
          cerr << "noBadOutput = " << nobad << endl;
          cerr << "inputformat = " << ifmt << endl;
          cerr << "status = " << status << endl;
          cerr << "delay = " << delay << endl;
          cerr << "save_to = " << save_file << endl;
          cerr << "restore_from = " << restore_file << endl;
          cerr << "save_steps = " << sinterval << endl;
#ifdef USE_NETCDF
          cerr << "inNetcdf = " << inNetcdf << endl;
          cerr << "outNetcdf = " << outNetcdf << endl;
          cerr << "outNetcdfFile = " << outNetcdfFile << endl;
#endif
       }

       // convert from hours to day fraction (i.e., model time)
       if ( outfreq != -1.0 ) {
          outfreq = outfreq / 24.0;
       }

       if ( tstep <= 0.0 ) {
          cerr << "tstep must be >= 0" << endl;
          status = 1;
       }

    }    
    
    // parse any met options into keywords and values
    metopt_keys.clear();
    metopt_vals.clear();
    if ( parse_met_options( metoptions, metopt_keys, metopt_vals ) ) {  
       std::cerr << "Met Options '" << metoptions << "' has a syntax error." << std::endl;
       status = 1;
    }
    
    // check that we have a legal met source
    if ( status == 0 ) {

        try {            
            metcaps = metPick.characterization( metsrc_name );
            if ( ! ( metcaps & METCAP_LATLON ) ) {
               std::cerr << "Meteorological data source is not on a lat/lon grid: " << metsrc_name << std::endl;
               status = 4;
            }
        } catch (MetSelector::badUnknownSource) {
            std::cerr << "Unknown meteorological data source: " << metsrc_name << std::endl;
            status = 3;
        }
       
    }

#ifdef USE_NETCDF
    if ( status == 0 ) {
       if ( inNetcdf ) {
          in_netcdf = new PGenNetcdf();
          if ( ifmt != "" ) {
             in_netcdf->format( ifmt );
          }
          if ( spec_parcelvertical ) {
             in_netcdf->vertical( parcelVertical );
          } else if ( spec_vertical ) {
             in_netcdf->vertical( vertical );
          }
          
          in_netcdf->open(parcelsource);

          if ( ! spec_parcelvertical ) {
             parcelVertical = in_netcdf->vertical();
          } else {
          
              
          
             if ( parcelVertical == in_netcdf->vertical() ) {
                // even if this is not specified in the cmd line,
                // it is specified in the file
                spec_parcelvertical = true;
                
             } else {
                // should never get here
                std::cerr << "Parcel vertical coord " << in_netcdf->vertical() 
                          << " from file does not match that specified " << parcelVertical << std::endl;
                status = 100;
             }
          }

       }
    }
#endif

    conv_parcel_coords = false;

    if ( status == 0 ) {

       if ( spec_parcelvertical && ( ! spec_vertical ) ) {
          vertical = parcelVertical;
       }

       conv_parcel_coords = ( spec_parcelvertical 
                            && spec_vertical 
                            && (parcelVertical != vertical) );


       // create a met source of the desired type
       metsource = dynamic_cast<MetGridLatLonData*>(metPick.source( metsrc_name ));

       // set any options that we need
       if ( metoptions != "" ) {
          for ( int i=0; i < metopt_keys.size(); i++ ) {
             metsource->setOption( metopt_keys[i], metopt_vals[i] );
          }
       }

       // set a native time spacing for the data 
       metsource->impose_times( tbase, tspace );
       
       // set a pre-open wait time
       if ( delay != 0 ) {
          metsource->setOption("Delay", delay );
       }
       // set the prozessing group
       metsource->setPgroup( pgrp );

        
       // Set up the output cache directory
       filepath.setTop(cachedir);
       filepath.makedir();
       // Tell the data source to use disk caching
       metsource->setCacheDir( cachedir );
       
       // thin out the horizontal grid?
       // set any horizontal thinning (not all sources use this)
       if ( thin > 1 && thinoffset < thin ) {
          metsource->setOption("HorizontalGridThinning", thin);
          metsource->setOption("HorizontalGridOffset", thinoffset );
       }

       // Set the vertical coordinates and units
       if ( vertical != "" ) {
          if ( metsource->legalQuantity(vertical) ) {
             try {
                metsource->set_vertical(vertical, "");  // we want the default vertical coord units, so specify ""
             } catch (gigatraj::MetData::badquantity()) {
                cerr << vertical << " !! is unknown to this data source" << endl;
                status = 1;
             }
          } else {
             cerr << vertical << " is Unknown to this data source" << endl;
             status = 1;
          }
       } else {
          // using the default vert coord
          vertical = metsource->vertical();
       }
       vertunits = metsource->vunits();

       if ( conv_parcel_coords ) {
          // create the new source
          altmetsource = dynamic_cast<MetGridLatLonData*>(metPick.source( metsrc_name ));
          // copy settings from the main source
          *altmetsource = *metsource;
          // Change the vertical coordinate
          if ( altmetsource->legalQuantity(parcelVertical) ) {
             try {
                altmetsource->set_vertical(parcelVertical, "");  // we want the default vertical coord units, so specify ""
                altmetsource->dbug = debug;
             } catch (gigatraj::MetData::badquantity()) {
                cerr << parcelVertical << " !! is unknown to this data source" << endl;
                status = 1;
             }
          } else {
             cerr << parcelVertical << " is Unknown to this data source" << endl;
             status = 1;
          }
       } else {
          altmetsource = NULLPTR;
       }

       // We might be doing iso-surface trajectories
       if ( isosfc ) {
          metsource->setIsoVertical();
       }       

       // Translate the starting and ending times from
       // the data source's calendar strings (usually ISO8601)
       // to internal model times
       metsource->defineCal( zerodate, 0.0 );
       begtime = metsource->cal2Time(begdate); 
       endtime = metsource->cal2Time(enddate);
       if ( altmetsource != NULLPTR ) {
          altmetsource->defineCal( zerodate, 0.0 );
          altmetsource->setIsoVertical(); // used only for corod conversion, so no vert velocity
       }
       // initialize the prototype parcel to the model starting time
       pcl.setTime( begtime );
       
       pcl.setMet( *metsource);
       
       if ( integratorName == "RK4" ) {
          integrator = new IntegRK4;
          pcl.integrator( integrator );
       } else if ( integratorName == "RK4a" ) {
          integrator = new IntegRK4a;
          pcl.integrator( integrator );
       } else { 
          integrator = NULLPTR;
       }
       
       // the default time step for the integration is 15 minutes
       //tdelta = 15.0/60.0/24.0 ; 
       tdelta = tstep/60.0/24.0 ; 
       // If the start time is later than the end time, we are
       // doing back-trajectories, and tdelta needs to be negative.
       if ( begtime > endtime ) {
          tdelta = - tdelta;
       }   
       
       if (verbose) {
          cerr << " using " << metsrc_name << " for met data" << endl;
          cerr << " with vertical coord " << vertical << "[" << vertunits << "]" << endl;
       }
       
       
    }


    
    // set up the Swarm of Parcels
    if ( status == 0 ) {
       
       if ( do_restore == 0 ) {  
              
#ifdef USE_NETCDF
          if ( ! inNetcdf ) {
#endif
             // reading the parcels in from a text file or stdin
                 
             // read in the parcels                                        
             if ( parcelsource != "-" ) {
                // from a file
                infile.open(parcelsource);
                input = &infile;
             } else {
                // from stdin
                input = &std::cin;
             }
             if ( ifmt != "" ) {
                parcelFactory.format( ifmt );
             }
          
             swarm = parcelFactory.create_Swarm( pcl, input, pgrp, mcsr );
          
             if ( parcelsource != "-" ) {
                // close the input file if we are not using stdin.
                infile.close();
             }
#ifdef USE_NETCDF
          } else {  
             // read the parcels in from a netcdf file
             
             if ( verbose ) {
                std::cerr << " netcdf input: using " << vertical << " for vert and " 
                         << parcelVertical << " for parcel vert" << std::endl; 
             }
             
             swarm = in_netcdf->create_Swarm( pcl, parcelsource, pgrp, mcsr ); 
             delete in_netcdf;
             // The Parcels read in from the file have the time from the file.
             // We need to change the times to out starting time.
             for ( Swarm::iterator fi=swarm->begin(); fi != swarm->end(); fi++ ) {
                 fi->setTime( begtime );
             }
          }   
#endif
          if ( verbose ) {
             std::cerr << "Parcels have been read from input " << std::endl; 
          }
          
          // Note: pcl was initialized w/ time 0.0.
          // and so every Parcel in the swarm has its time set to 0.0
          // which is fine, since we just equivalenced the calendar starting time
          // to model time 0.0 with the deficeCal() call above.
       
          // The Swarm of Parcels should use the met source that we chose
          //swarm->setMet( *metsource );
       
          time = begtime;
          
          // we dump parcels states every accumul_time interval 
          double accumul_time = 0.0;

       } else {
       
          // restore the Swarm from a previous, interrupted run
          if ( verbose ) {
             cerr << "Restoring parcels from file " << restore_file << ":";
          }
          swarm = restore( pcl, restore_file, pgrp, mcsr, &time, &accumul_time ); 
          if ( verbose ) {
             cerr << " resuming from model time " << time << std::endl;
          }
       }   
       
       // set wind vector conformal adjustments
       if ( confml >= 0 ) {
          iter = swarm->begin();
          iter->conformal( confml );
       }
       

       if ( do_restore == 0 ) {  
          // but we may need to convert the parcels' vertical coordinates
          // (If we have restored from a previous run, there is no need
          if ( conv_parcel_coords ) {
              inVChange = new ChangeVertical( vertical, parcelVertical, altmetsource );
              
              // convert the parcels' input positions to the vertical
              // coordinate we will use to actually trace them
              if ( verbose ) {
                 cerr << "Converting parcel vertical coordinates from " << parcelVertical
                      << " to  " << vertical << endl;
              }
              for ( iter = swarm->begin(); iter != swarm->end(); iter++ ) {
                  fpcl = *iter;
                  // the value in the parcel is not really "Vertical" coodinate, 
                  // but in the "ParcelVertical" coordinate.
                  // For the conversion to work, this must be noted.
                  fpcl.setNonVert();
    
                  // Do the conversion for this parcel           
                  inVChange->apply( fpcl );

                  // the parcel's nonVert status has been changes by the apply() above
                  
                  // put the new parcel's values in the Swarm element
                  *iter = fpcl;

              }
              
              delete inVChange;
              delete altmetsource;
              
          }
       }   
    }
    
    // Set up for printing: model-time, parcel-index, longitude, latitude, vertical-coordinate
    //Out.format("%t, %i, %o, %a, %v\n");
    Out.format(fmt + "\n");
    if ( nobad ) {
       Out.omitNonTraced();
    }


    //-------------- trace parcel trajectories
    
    // We only do this if we have a valid met source and parcels
    if ( status == 0 ) {
    
       // set this to 1 if you want copious output for debugging
       // a new met source
       //metsource->dbug = 0; // no debug messages
       //metsource->dbug = 1; // only basic data access and regridding messages
       //metsource->dbug = 2; // disk or memory cache usage
       //metsource->dbug = 3; // ???
       //metsource->dbug = 4; // data read details
       //metsource->dbug = 5; // sample data values on read
       //metsource->dbug = 10;
       metsource->dbug = debug;
       
       if ( verbose ) {
          swarm->dump();
       }

       if ( verbose ) {
          cerr << "initial printout : " << endl;
       }    

       // output initial parcel data
#ifdef USE_NETCDF
       if ( ! outNetcdf ) {
#endif
          Out << *swarm;
#ifdef USE_NETCDF
       } else {
          out_netcdf = new NetcdfOut();
          out_netcdf->filename( outNetcdfFile );
          out_netcdf->contents("gigatraj trajectories");
          if ( vertical != "" ) {
             out_netcdf->vertical( vertical );
          }
          if ( conv_parcel_coords ) {
             out_netcdf->addQuantity( parcelVertical );
          }
          out_netcdf->format( fmt );
          out_netcdf->init( &pcl, swarm->size() );
          out_netcdf->open();
          out_netcdf->apply( *swarm );
       }
#endif

       if ( verbose ) {
          cerr << "STARTING : " << endl;
       }    
    
       // Trace the Parcel trajectories
       // forwards or backwards
       tracing = true;
       
       // initialize the save-state counter
       scount = 0;
       
       swarm->metDelay();
       
       while ( tracing ) {
           if ( debug ) {         
              std::cerr << "@@@@@@@@@@@@@@@@@@@@ start of tracing loop" << std::endl;
           }
       
           // We would like to end up exactly on our ending time.
           // But tdelta can push us over.
           // Account for this.
           if ( tdelta > 0 ) {
              if ( (time + tdelta) > endtime ) {
                 tdelta = endtime - time;
              }
           } else {
              if ( (time + tdelta) < endtime ) {
                 tdelta = endtime - time;
              }
           }
           
           
           // Trace all the Parcels through this time step
           // (This is the heart of the trajecfory model.
           // All of the details of getting met data, managing caches,
           // etc., are hidden in this one line.)

           //cout << "iteration: " << endl;
           //cout << "      before time: " << ((*swarm)[0]).getTime() << endl;
           //- std::cerr << "~~~~~~~~~~~ about to trace swarm through one step" << std::endl;
           //trace( swarm, tdelta );
           swarm->advance( tdelta );
           //- std::cerr << "~~~~~~~~~~~ done tracing swarm through one step" << std::endl;
           //cout << "      after  time: "  << ((*swarm)[0]).getTime() << endl;
           
           scount++;
           
           // by default, we will not produce output for this time step
           do_output = false;
           
           // the parcels now have this time, so we may as well, too.
           time += tdelta; 
           // will we be still be tracing after this step?
           if ( tdelta > 0 ) {
              tracing = ( endtime - time ) > 0.001;
           } else {
              tracing = ( time - endtime ) > 0.001;
           }
           
           // One step closer to printing out
           accumul_time += abs(tdelta);
           
           //tracing = (tdelta > 0)? (time < endtime) : (time > endtime);
           
           if ( verbose ) {
              cerr << "Time = " << metsource->time2Cal(time) << " : ";
              //cerr << " = " << time << " : " << ", delta=" << tdelta << " : ";
              //cerr << endl;
           }

           // save the current model state?
           if ( do_save && (scount >= sinterval) ) {
              if ( verbose ) {
                 cerr << " (saving state) ";
              
              }
              save( save_file, time, accumul_time, swarm );
              scount = 0;
           }
       

           
           //cerr << "accumul_time=" << accumul_time << ", tdelta=" << tdelta << endl;
           // print out if it's time, or if we are at the end
           //if ( verbose ) {
           //   cerr << " "  << outfreq << ", " << accumul_time;
           //}
           
           // we will do output if we are set to produce output, and it is now time to do so
           if ( outfreq > 0 ) {
              if ( (outfreq - accumul_time) < abs(tdelta)/2.0 ) {   
                 do_output = true;
              }   
           }
           // we always produce output at the end
           if ( ! tracing ) do_output = true;

           if ( do_output ) {
              if ( verbose ) {
                 cerr << " Output ";
              }
              
              // do printout here
#ifdef USE_NETCDF
              if ( ! outNetcdf ) {
#endif
                 Out << *swarm ;
#ifdef USE_NETCDF
              } else {
                 out_netcdf->apply( *swarm );
              }
#endif

              accumul_time = 0.0;

              swarm->metDelay();
           }
           if ( verbose ) {
              cerr << endl;
           }
  
           if ( debug ) {         
              std::cerr << "@@@@@@@@@@@@@@@@@@@@ end of tracing loop" << std::endl;
           }
       }
       
       swarm->sync();
       
       // All done.  Destroy the things we created
#ifdef USE_NETCDF
       if ( outNetcdf ) {
          out_netcdf->close();
          delete out_netcdf;
       }
#endif
       delete swarm;
       delete metsource;
       if ( integrator != NULLPTR ) {
          delete integrator;
       }
    }

    /* Shut down any multiprocesing */
    pgrp->shutdown();
    
    /* since we go there without crashing, remove any savefiles */
    if ( ! keep_save ) {
       remove( save_file.c_str() );
    }
    
    // Leave.
    exit(status);
    
}
