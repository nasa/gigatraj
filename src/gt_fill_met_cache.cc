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

\page  gt_fill_met_cache gt_fill_met_cache: Fetch and cache meteorological data


The gt_fill_met_cache program runs through a set of times, reading data from
a meteorological data source, and thereby triggering the creation of cache
files.  The cache files can then be used in a trajectory model run.

Although it is not necessary to run this program before running
a trajectory model, is can be convenient to verify that all
the data that are needed for the trajectory run, are in fact available 
before the run begins.

The calling secuence is:
\code
gt_fill_met_cache [ --help|-h ] [ --list ] [ --verbose ] [--debug level]  [ --config|-c configFile ] [ --rc|-r resourcefile ] \\
               [--cachedir|-d directory ] [ --source|-s metsource ] \\
               [ --begdate|-b yyyy-mm-ddThh ] [ --enddate|-e yyyy-mm-ddThh ] [ --tdelta|-t timeDelta ] \\
               [ --quantities|-q quantityList ] [ --vertical|-v verticalCoord ] \\
               [ --thin thinfactor [ --thinoff thinoffset ] ] \\
               [--metbasehr metDataBaseHour] \\
               [--metspacinghr metDataSpacingHour] [--delay opentime ] 
\endcode
              
The command-line options are:

 \li \c help : prints out a description of options and then stops
 \li \c list : lists the available meteorological data sources (with brief descriptions) and then stops
 \li \c verbose : prints out messages to let the user know what progress is being made
 \li \c debug : prints out copious technical debugging messages from met field reading functions
 \li \c config : specifies the name of a configation file. The default is "gt_fill_met_cache.config"
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
 \li \c cachedir : specifies a directory into which the cached data files are to be placed.
 \li \c  source : specifies the meteorological data source. This may be one of:
               - SBROT = solid-body earth rotation, a test data set only
               - MERRA = the NASA Goddard GMAO MERRA reanalysis
               - MERRA2 = the NASA Goddard GMAO MERRA-2 reanalysis         
  \li \c begdate : the starting timestamp of the data that are to be cached, in "yyyy-mm-ddThh" format
  \li \c enddate : the ending timestamp of the data that are to be cached, in "yyyy-mm-ddThh" format
  
  \li \c tdelta : the time delta, in hours, between data timestamps.
  \li \c quantities : a comma-separated list of meteorologicsl data fields to be obtained from the source. The names
                      (and the defaults) depend on the source, but typically include the two components of the horizontal wind
                      as well as a quantity that is analogous to the vertical wind.
  \li \c vertical : the quantity that is to be used as the vertical coordinate of the cached data. The name of the quantity
                    and its default will depend on the data source. 

  \li \c thinfactor : fector (default 1) by which the thin the horizontal data grid. For example,
                      a thinfactor of 2 retains only every other longitude, and every other latitude.
   
  \li \c thinoffset : an offset (default 0) into the longitude index, used when thinning. If
                   thinfactor is 2 and thinoffset is 0, then the longitudes 0, 2, 4,... are
                   retained. If thinfactor is 2 and thinoffset is 1, the longitudes
                   1, 3, 5, ... are retained.
  \li \c metbasehr : imposes a base hour--the first time snapshot in a day for which data are available. (The default is 00Z.)

  \li \c metspacinghr : imposes a spacing between successive time snapshots.

  \li \c delay : sets a delay, in seconds, to used each time a URL is opened.


Note that the begdate, enddate, and source settings are mandatory: they must be defined somewhere, usually in the command line options.

Settings are applied in this order: first, built-in default values are loaded. These defaults are:
      - verbose = off
      - help = off
      - list = off
      - config = gt_fill_met_cache.config
      - rc = $HOME/.gigatrajrc
      - cachedir = "./"

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

#include "gigatraj/gigatraj.hh"
#include "gigatraj/FilePath.hh"
#include "gigatraj/Configuration.hh"

#include "gigatraj/MetSelector.hh"

using namespace gigatraj;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;


/*------------------------------------------------------------------------------------------*/
int getconfig(int argc, char * const argv[], Configuration& conf, MetSelector &metPick ) 
{
    string src;
    string begdate;
    string enddate;
    string vertical;
    string outdir;
    string quantities;
    double tdelta;
    double tbase;
    double tspace;
    int thin;
    int thinoff;
    bool verbose; 

    int status;

    // argv index
    int aidx;
    // print help text and quit?
    bool doHelp;
    // usage help string
    std::string usage;
    // list of met sources
    std::vector<std::string> srclist;
    
    
    usage = "gt_fill_met_cache ";
    
    status = 0;

    usage += " [--help|-h] ";
    conf.add("help"      , cBoolean, "N"                , "h", 0, "print help and quit" );
    usage += " [--list] ";
    conf.add("list"      , cBoolean, "N"                ,  "", 0, "list available met sources and quit" );

    usage += " [--rc|-r resourcefile] ";
    conf.add("rc", cConfig, string(getenv("HOME")) + "/.gigatrajrc", "r" );
    usage += " [--config|-c configfile] ";
    conf.add("config", cConfig, "gt_fill_met_cache.config", "c" );

    // register these parameters with the configuration object
    usage += " [--verbose] ";
    conf.add("verbose"   , cBoolean, "N"                , "" , 0, "print detailed progress messages" );
    usage += " [--debug level] ";
    conf.add("debug"     , cInt    , "0"                , "" , 0, "print debugging messages for data access" );
    usage += " [--vertical|-v verticalCoordQuantity]";
    conf.add("vertical"  , cString , ""                 , "v", 0, "vertical coordinate" );
    usage += " [--quantities|-q quantitylist] ";
    conf.add("quantities", cString , ""                 , "q", 0, "(comma-separated) list of quantities" );
    usage += " --source|-s metsource ";
    conf.add("source"    , cString , ""                 , "s", 0, "data source" );
    usage += " --begdate|-b datetime ";
    conf.add("begdate"   , cString , ""                 , "b", 0, "beginning date (e.g., 2008-12-05T10)" );
    usage += " --enddate|-e datetime ";
    conf.add("enddate"   , cString , ""                 , "e", 0, "ending date" );
    usage += " [--tdelta|-t timedelta] ";
    conf.add("tdelta"    , cString , "0"                , "t", 0, "if > 0, time spacing between cached files, in hours (e.g.,6)" );
    usage += " [--cachedir|-d dir] ";
    conf.add("cachedir"  , cString , "./"               , "d", 0, "met data cache directory" );
    usage +=  " [--metbasehr metDataBaseHour]";
    conf.add("metbasehr" , cFloat, "-1.0"               , "" , 0, "if >= 0, impose this time base/offset, so that each day's data snapshots start with this hour" );
    usage +=  " [--metspacinghr metDataSpacingHour]";
    conf.add("metspacinghr" , cFloat, "0.0"             , "" , 0, "if > 0, impose this time spacing, so that the met data snapshots are spaced this many hours apart" );
    usage +=  " [--thin thinfactor [ --thinoff thinoffset ] ]";
    conf.add("thin"      , cInt,  "1"                   , "" , 0, "horizontal thinning factor" );
    conf.add("thinoff"   , cInt,  "0"                   , "" , 0, "horizontal thinning offset" );
    usage += " [ --delay opendelay ]";
    conf.add("delay"     , cInt,  "0"                   , "" , 0, "open delay in seconds" );

    // load the config values from any config files, as well as the command line
    aidx = conf.load(argc,argv);
    
    conf.fetchParam("help", doHelp);
    if ( conf.get("help") == "Y" ) {
       conf.help(usage,"");
       exit(0);
    }

    conf.fetchParam("list", doHelp);
    if ( conf.get("list") == "Y" ) {
       srclist = metPick.list(METCAP_LATLON);
       for ( int i=0; i<srclist.size(); i++ ){
           cout << srclist[i] << ": " << metPick.description(srclist[i]) << endl;
       }
       exit(0);
    }

    // now get the settings out of the config object
    src = conf.get("source");
    begdate = conf.get("begdate");
    enddate = conf.get("enddate");
    
    // User must specify source, beginning  and ending,
    // and any tdelta must be positive nonzero.
    if ( src == "" || begdate == "" || enddate == ""  ) {
       status = 1;
    }
    
    // Any errors?  Print out a usage message.
    if ( status != 0 ) {   
       cerr << "Bad configuration." << endl;
       conf.help(usage,"");
    }
    

    return status;
}


/*------------------------------------------------------------------------------------------*/

int main( int argc, char * const argv[] ) 
{
    // return status. 0 = all went well.
    int status;
    // configuration object
    Configuration config;
    // flag for printing information about what the program is doing
    bool verbose;
    // the name of the meteorological data source
    string src;
    // the name of the vertical coordinate to be used by the meteorological data source
    string vertical;
    // a comma-separated list (no spaces!) of quantities to be read and cached
    // by the meteorological data source
    string quantities;
    // the starting date for which data are to be obtained.
    // (Note that the format is whatever the meteorological data source uses.)
    string begdate;
    // the ending date for which data are to be obtained.
    // (Note that the format is whatever the meteorological data source uses.)
    string enddate;
    // the time interval used to sweep through from the starting time
    // to the ending time, in hours.
    double tdelta;
    // met data time base/offset
    double tbase;
    // met data time spacing
    double tspace;
    // the output directory into which the cache files should be written
    string outdir;
    // bracketing calendar times
    string begbrack, endbrack;
    // generates the meteorological data source object
    MetSelector metPick;
    // the meteorological data source object
    MetGridLatLonData *metsource;
    // the current, starting, and ending times, in internal model time
    double time, begtime, endtime;
    // the list of quantities to be read (i.e, the quantities string broken 
    // apart by commas into substrings)
    vector<string> quantlist;
    // a throwaway data point (thrown away before we care only about the
    // side effect of causing the cache files to be created)
    real junk;
    // the wuantity currently being read
    string quant;
    // the units of the vertical cooridnte of the meteorological data source
    string vertunits;
    // flag to be set if we are still searching for commas in the
    // quantitiues string, as we break it apart by commas
    int searching;
    // positions in the quantities string, used while breaking apart
    // by commas
    size_t begpos, endpos;
    // File path for output cache directory
    FilePath filepath;
    // resource file name
    string rcfile;
    // thinning specs
    int thin, thinoff;
    // met source capaability flags
    int metcaps;
    // met srouce debugging level
    int debug;
    // delay before each url open
    int delay = 0;
    // reasonably near-surface value of the vertical coordinate
    real sfc_val;
    // names of the pressure and potnetial temperature quantities in the met data set
    std::string p_name, theta_name;
    
    // we assume all will go well (until it doesn't)    
    status = 0;

    status = status | getconfig( argc, argv, config, metPick );
    config.fetchParam("verbose", verbose);
    vertical = config.get("vertical");
    src = config.get("source");
    quantities = config.get("quantities");
    begdate = config.get("begdate");
    enddate = config.get("enddate");
    tdelta = config.str2dbl( config.get("tdelta") );
    outdir = config.get("cachedir");
    tbase = config.str2dbl( config.get("metbasehr") );
    tspace = config.str2dbl( config.get("metspacinghr") );
    thin = config.str2int( config.get("thin") );
    thinoff = config.str2int( config.get("thinoff") );
    debug = config.str2int( config.get("debug") );
    delay = config.str2dbl( config.get("delay") );
       
    if (verbose) {
       cerr << "verbose = " << verbose << endl;
       cerr << "source = " << src << endl;
       cerr << "vertical = " << vertical << endl;
       cerr << "debug = " << debug << endl;
       cerr << "quantities = " << quantities << endl;
       cerr << "begdate = " << begdate << endl;
       cerr << "enddate = " << enddate << endl;
       cerr << "tdelta = " << tdelta << endl;
       cerr << "output = " << outdir << endl;
       cerr << "status = " << status << endl;
       cerr << "metbasehr = " << tbase << endl;
       cerr << "metspacinghr = " << tspace << endl;
       cerr << "thin = " << thin << endl;
       cerr << "thinoff = " << thinoff << endl;
       cerr << "delay = " << delay << endl;
    }


    
    
    // check that we have a legal met source
    if ( status == 0 ) {

        try {            
            metcaps = metPick.characterization( src );
            if ( ! ( metcaps & METCAP_LATLON ) ) {
               std::cerr << "Meteorological data source is not on a lat/lon grid: " << src << std::endl;
               status = 4;
            }
        } catch (MetSelector::badUnknownSource) {
            std::cerr << "Unknown meteorological data source: " << src << std::endl;
            status = 3;
        }
       
    }

       
    if ( status == 0 ) {

       // create a met source of the desired type
       metsource = dynamic_cast<MetGridLatLonData*>(metPick.source( src ));


       // set a native time spacing for the data 
       metsource->impose_times( tbase, tspace );
       
       // set a pre-open wait time
       if ( delay > 0 ) {
          metsource->setOption("Delay", delay );
       }    
    
       // Set up the output cache directory
       filepath.setTop(outdir);
       filepath.makedir();
       // Tell the data source to use disk caching
       metsource->setCacheDir( outdir );
       
       // thin out the horizontal grid?
       // set any horizontal thinning (not all sources use this)
       if ( thin > 1 && thinoff < thin ) {
          metsource->setOption("HorizontalGridThinning", thin);
          metsource->setOption("HorizontalGridOffset", thinoff );
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
       
       metsource->getOption("PressureName", p_name );
       metsource->getOption("PotentialTemperatureName", theta_name );
       
       if ( vertical == p_name ) {
          sfc_val = 1000.0;
       } else if ( vertical == theta_name ) {
          sfc_val = 280.0;
       } else {
          sfc_val = 0.0;
       }

       if (verbose) {
          cerr << " using " << src << " for met data" << endl;
          cerr << " with vertical coord " << vertical << "[" << vertunits << "]" << endl;
       }

    }



    // Proceed only if there have been no errors so far
    if ( status == 0 ) {


       // Come up with a default quantities list, if we need one.
       // The names of the quantities depend on which
       // data source we are using, and the quantities
       // themselves depend on which vertical coordinate is
       // being used.  The default list is basically
       // the three components of the wind in whatever
       // coordinate system is in use.
       if ( quantities == "" ) {
          quantities = metsource->u_wind();
          quantities = quantities + "," + metsource->v_wind();
          quantities = quantities + "," + metsource->w_wind();     
       }

       if ( status == 0 ) {
          // Break the quantities string, which is a comma-separated
          // list of quantities, into its compoents, which
          // are stored in a vector.
          begpos = 0;
          searching = 1;
          while (searching) {
              // Search for the next comma.
              endpos = quantities.find(',', begpos);
              if ( endpos != string::npos ) {
                 // Found it.
                 // Extract the quantity...
                 quant = quantities.substr(begpos,endpos-begpos);
                 // ...and set up for the next search
                 begpos = endpos + 1;
              } else {
                 // No comma found; end of string.
                 // Extract the quantity that we have.
                 quant = quantities.substr(begpos);
                 // Clear the flag so that we can leave.
                 searching = 0;
              }   
              if (verbose) {
                 cerr << "adding " << quant << " to quants list" << endl;
              }
              // add the quantity to the vector
              quantlist.push_back(quant);
          } 
          if ( quantlist.size() < 1 ) {
             cerr << "Empty quantity list!" << endl;
             status = 6;
          }   
       }
       

       if ( status == 0 ) {
          // Loop over the quantities to check if they are all legal for this data source
          for ( vector<string>::iterator q = quantlist.begin(); q != quantlist.end(); q++ ) {
              
              if (verbose) {
                 cerr << " checking legality of <<" << *q << ">>" << endl;
              }
              if ( ! metsource->legalQuantity(*q) ) {
                 cerr << *q << " is unknown to this data source" << endl;
                 status = 1;
              }   
          }

         // Find the default delta between the times we want to retrieve
         // (This is not necessarily the same as tspace)
         if ( tdelta <= 0 ) {
            tdelta = metsource->get_default_timedelta( quantlist[0], begdate );
         }



       }
       
       
    
       if ( status == 0 ) {
          
          // Translate the starting and ending times from
          // the data source's calendar strings (usually ISO8601)
          // to internal model times
          begtime = metsource->cal2Time(begdate);
          endtime = metsource->cal2Time(enddate);
              
          metsource->dbug = debug;
          
          // Loop over all the times
          for ( time=begtime; time<=endtime; time += tdelta/24.0 ) {

              if (verbose) {
                 cout << "Time = " << metsource->time2Cal(time) << " : ";
              }

              // Loop over the quantities to be read from the source for this time
              for ( vector<string>::iterator q = quantlist.begin(); q != quantlist.end(); q++ ) {
                  if (verbose) {
                     cout <<  "  " << *q ;
                  }

                  // read a single data point of this quantity at this time,
                  // at the equator, prime meridian and surface.
                  junk = metsource->getData( *q, time, 0.0, 0.0, sfc_val );
              }
              if (verbose) {
                 cout << endl;
                 //cerr << "time, begtime, endtime, tdelta=" << time << ", " << begtime << ", " << endtime << ", " << tdelta << endl;
              }
          }
          
       }

       // All done.  Destroy the data source object.
       delete metsource;
    }   
    
    // Leave.
    exit(status);
    
}
