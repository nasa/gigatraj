

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

#include "config.h"

#include "gigatraj/FilePath.hh"

using namespace gigatraj;

#if OSTYPE == UNIX
const std::string FilePath::sep = "/";
#include <cerrno>
#include <cstdio>
#include <sys/stat.h>
#include <sys/types.h>
extern "C" int mkdir(const char *pathname, mode_t mode);
#endif

FilePath::FilePath()
{
   topmost = ".";
}

FilePath::FilePath( const std::string top )
{
   //checkLegal(top);
   topmost = top;
}


FilePath::~FilePath()
{
   parts.clear();
}

void FilePath::clear()
{
   topmost = ".";
   parts.clear();
}


FilePath::FilePath(const FilePath& src)
{
   topmost = src.topmost;
   parts = src.parts;
}


FilePath& FilePath::operator=(const FilePath& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    topmost.assign( src.topmost );
    parts = src.parts;
    
    return *this;
}

bool FilePath::operator==(const FilePath& two) const
{
    // the result to be returned
    bool result;
    // the number of components in each file path
    int np, np2;
    
    np = parts.size();
    np2 = two.parts.size();
    
    // topmost parts the same and the same number of components?
    result = (topmost == two.topmost) 
             && ( np == np2 );
    if ( result ) { 
       // ... then compare each component
       for ( int i=0; i < np && i < np2; i++ ) {
           result = result && (parts[i] == two.parts[i] );
           if ( ! result ) {
              // if one component failed to match, there is no need to continue
              break;
           }   
       }
    }       

    return result;
}

bool FilePath::operator!=(const FilePath& two) const
{
    // the result to be returned
    bool result;
    // the number of components in each file path
    int np, np2;
    
    np = parts.size();
    np2 = two.parts.size();
    
    // topmost parts different or different numbers of components?
    result = (topmost != two.topmost) 
             ||  ( np != np2 );
    if ( ! result ) {      
       // ... then compare each component
       for ( int i=0; i < np && i < np2; i++ ) {
           result = result || (parts[i] != two.parts[i] );
           if ( result ) {
              // if one component failed to match, there is no need to continue
              break;
           }   
       }
    }       

    return result;
}

std::string FilePath::fullpath() const
{
    // the result to be returned
    std::string result;
    int beg;
    // number of components in the file path
    int n;

    // begin with a new, empty string
    result = "";
    
    // how many components in this file path?
    n = parts.size();

#if OSTYPE == UNIX
    beg = 0;
    if ( topmost == "." && n > 0 ) {
       // relative file path with components
       // So we start the string with the first component
       result = parts[beg];
       beg++;
    } else {
       // absolute file path, or relative with no components
       // either way, start the string with the 
       // topmost part of the file path
       result = topmost;
    }         
    // now append the rest of the file path, component
    // by component, separated by the separater character
    for (int i=beg; i<n; i++ ) {
        result = result + sep + parts[i];
    }        
#endif

    return result;
}


std::string FilePath::directory( int n ) const
{
    // re result to be returned
    std::string result;
    // the first directory component
    int beg;  
    // the last directory component (not including the base file name)
    int end;  
    // True if the file path is absolute
    bool usetop;
    int nn;
    // the number of components in the file path
    int psize;
    
    // hwo many components?
    psize = parts.size();
    
    // is this path absolute?
    usetop = ( topmost != "." );
    
    // create a brand new empty string
    result = "";

#if OSTYPE == UNIX
    if ( n == 0 ) {
       // take the whole directory path

       if ( psize > 1 ) {
          // both directory components and base name are in parts
          beg = 0;
          // (psize-1 is the last component, which is the base file name)
          end = psize - 2;
       } else {
          // parts contains at most the base file name
          // We set beg and end to values that will ensure
          // no components will be added in the loop below.
          beg = -1;
          end = -3;
       }   
       if ( usetop ) {
          // absolute file path
          result = topmost;
          if ( psize <= 1 ) {
             // no directory components
             // so stop here with a terminating separator
             result = result + sep;
          }   
       } else {
          // relative file path
          if ( beg >= 0 ) {
             // start the string with the first component
             result = parts[beg];
             beg++;
          } else {
             // start with the current directory
             result = "." + sep;
          }
       }
    } else if ( n > 0 ) {
       // take only the n highest components

       // are there components?
       if ( psize > 1 ) {
          // both directory components and base name are in parts
          beg = 0;
          end = n-1;
          // do not append anything past component # (psize-2)
          if ( end > (psize - 2) ) {
             end = psize - 2;
          }   
       } else {
          // parts contains at most the base file name
          // We set beg and end to values that will ensure
          // no components will be added in the loop below.
          beg = -1;
          end = -3;
       }   
       if ( usetop ) {
          // absolute path
          result = topmost;
          if ( psize <= 1 ) {
             // no directory components
             result = result + sep;
          }   
       } else {
          // relative path
          if ( beg >= 0 ) {
             result = parts[beg];
             beg++;
          } else {
             result = "." + sep;
          }
       }

    } else {
       // take only the -n lowest components

       if ( psize > 1 ) {
          // both directory components and base name are in parts
          
          // end with the last non-base-name component
          end = psize-2;
          // and start with (abs(n)-1) components before that
          beg = end + n + 1;
          // but do not overshoot the beginning
          if ( beg < -1 ) {
             beg = -1;
          }
       } else {
          // parts contains at most the base file name
          // We set beg and end to values that will ensure
          // no components will be added in the loop below.
          beg = -1;
          end = -3;
       }
        
       if ( beg == -1 ) {
          // no component to begin, so use the topmost 
          result = topmost;
          // and we will begin with the first path component
          beg++;
          if ( psize <= 1 ) {
             // no directory components
             // terminate here
             result = result + sep;
          }   
       } else {
          // beg >= 0
          if ( beg == 0 && usetop ) {
             result = topmost + sep + parts[beg];
          } else {
             result = parts[beg];
          }
          beg++;
       }

    }
             
    // now that we have the beginning of the string right,
    // append subsequent components of the path to it.
    // If we have component to add, that is.
    if ( beg >= 0 && end < (psize-1) && beg <= end ) {
       for ( int i=beg; i<=end; i++ ) {
          result = result + sep + parts[i];
       }
    }      
       

#endif

    return result;
}


std::string FilePath::basename() const
{
    std::string result;
    
    
    result = "";

    // if we have file path components, use the last one
    // as the base name 
    if ( parts.size() > 0 ) {
        result = parts[ parts.size() - 1 ];
    }
    
    return result;    
}



void FilePath::setTop( const std::string top )   
{
    // check that the file name is acceptable to the OS,
    // noting that separators are OK here.
    checkLegal(top, SepOK);
    topmost = top; 
}



void FilePath::append( const std::string item )
{
    // holds the part of the input string we have not yet processed
    std::string tmp;
    // a file path component that has been extracted from the input string
    std::string xtract;
    // the position of the next separator character in the input string
    size_t pos;
    
    // start with the entire input string
    tmp = item;
    
    // try to find the next separator character
    while ( (pos = tmp.find(sep)) != std::string::npos ) {
    
       // found it
       
       // extract the string up until that separator
       xtract = tmp.substr(0,pos);
    
       // check that it is acceptable to the OS
       checkLegal(xtract);
       // and store it
       parts.push_back(xtract);
    
       // drop the part of the input that we have just extracted
       tmp = tmp.substr(pos + 1);
       
    }
    // is anything left of the input after our scanning
    // and extraction?
    if ( tmp != "" ) {
       // yes.  check it first
       checkLegal(tmp);    
       // and then add it to the components list
       parts.push_back(tmp);
    }
       
}


void FilePath::append( const FilePath& item )
{
     // add each input component to what we already have.
     for (int i=0; i<item.parts.size(); i++ ) {
         parts.push_back( item.parts[i] );
     }    
    
}


void FilePath::prepend( const std::string item )
{
    // vector of input file path components
    std::vector<std::string> newparts;
    // holds the part of the input string we have not yet processed
    std::string tmp;
    // a file path component that has been extracted from the input string
    std::string xtract;
    // the position of the next separator character in the input string
    size_t pos;
    
    // begin
    tmp = item;

    // start by reserving space for what we already know we will need
    newparts.reserve( parts.size() + 1 ); 

    
    // try to find the next separator character
    while ( (pos = tmp.find(sep)) != std::string::npos ) {
    
       // found it
       
       // extract the string up until that separator
       xtract = tmp.substr(0,pos);
    
       // chck it and then push it into the input list
       checkLegal(xtract);    
       newparts.push_back(xtract);
    
     
       // drop the part of the inpout that we have just extracted
       tmp = tmp.substr(pos+1);
    }
    // is anything left of the input after our scanning
    // and extraction?
    if ( tmp != "" ) {
       // yes.  check it and store it, too
       checkLegal(tmp);    
       newparts.push_back(tmp);
    }

    // now add the file path components that we already have
    for (int i=0; i<parts.size(); i++ ) {
        newparts.push_back( parts[i] );
    }    
    
    // and replace the old with the new
    parts = newparts;
}


void FilePath::prepend( const FilePath& item )
{
     // vector of input file path components
     std::vector<std::string> newparts;

     topmost = item.topmost;

     // start by reserving space for what we already know we will need
     newparts.reserve( parts.size() + item.parts.size() ); 
    
     // copy the input components
     for (int i=0; i<item.parts.size(); i++ ) {
         newparts.push_back( item.parts[i] );
     }
     // copy the components that we already had
     for (int i=0; i<parts.size(); i++ ) {
         newparts.push_back( parts[i] );
     }    

     // and replace the old with the new
     parts = newparts;
}

void FilePath::checkLegal( const std::string str, FilePathFlag flags ) const 
{
    std::string test;

    test = sep;
#if OSTYPE == UNIX    
    test.append(1,0x00);
#endif   

    // we check to make sure there are no spearator characters in there,
    // unless we specirfically say that is OK 
    if ( (flags & ~SepOK) && (str.find_first_of(test) != string::npos) ) {
       throw (badFileName());
    }
       
}



int FilePath::count() const
{
   return parts.size();
}


int FilePath::find( std::string srch, int start )
{
    int result;

    // assume we won't find it    
    result = -1;
    
    if ( start >= 0 && start < parts.size() ) {
       for (int i=start; i < parts.size(); i++ ) {
           if ( parts[i] == srch ) {
               // got it!
               result = i;
               break;
           }
       }        
    }        

    return result;

}

int FilePath::find( FilePath& srch, int start )
{
    // result: the index of our path components where the match begins
    int result;
    // have we matched all components so far?
    bool found;
    // loop variables
    int i,i2,j;
    // number of components that we have
    int np;

    // assume we won't find it    
    result = -1;
    
    np = parts.size();
    
    // do we have anything to look for?
    if ( start >= 0 && start < parts.size() ) {
       // start searching, component by component
       // We will quit if we find a string of our components that match
       // all of the search path's components.
       for ( i=start; i < np; i++ ) {
           // look though each of the search path's components,
           // comparing them with ours
           found = true;
           for ( j=0, i2=i; j < srch.parts.size() && i2 < np  ; j++, i2++ ) {
               if ( parts[i2] != srch.parts[j] ) {
                  // it only takes one mismatch to determine the result
                  found = false;
                  break; // break out fo the j loop
               }
           }
           if ( found ) {
              result = i;
              break; // break out of the i loop
           }   
       }        
    }        

    return result;




}



void FilePath::dump(const std::string& msg ) const
{
    std::cerr << " Dumping: " << msg << std::endl;
    std::cerr << "      top=" << topmost << std::endl;
    std::cerr << "      # parts=" << parts.size() << std::endl;
    for ( int i=0; i<parts.size(); i++ ) {
        std::cerr << "        " << i << ": " << parts[i] << std::endl;
    }

}


void FilePath::makedir()
{
     // name of a directory to create
     std::string dir;
     // the number of directories we are nested down into
     int dirlevel;
     // dir creation erro code
     int err;
     // have we made everything yet?
     bool done;     
#if OSTYPE == UNIX    
     // everything created will be world writable,
     // subject to the user's umask, but
     // the sticky bit will be turned on, like /tmp
     int mode = 01777;
#endif
     
     // start at the top
     dirlevel = 0;
     // and continue until we are done.
     done = false;
     while ( ! done ) {
         // fetch the directory name
         dir = directory( count() - dirlevel );
#if OSTYPE == UNIX 
         // try to create it
         err = mkdir(dir.c_str(), mode );
         if ( err < 0 ) {
            // That did not work.
            // Why not?
            if ( errno == EEXIST ) {
               // The directory already exists,
               // which is OK.
               // pretend there was no error at all
               err = 0;
               // This directory was "created",
               // but we might need to move down
               // to the next directory level
               dirlevel--;
               if ( dirlevel < 0 ) {
                  // we have made all the directories
                  done = true;
               }   
            } else if ( errno == ENOENT ) {
               // Directory component does not eixst (impossible?)
               // or is a danlging symlink.
               
               // trim the directory level
               // and keep going
               dirlevel++;
            } else {
               // some other error
               // give up 
               done = true;
            }
         } else {
            // the creation succeeded
            // this directory was created,
            // but we might need to move down
            // to the next directory level
            dirlevel--;
            if ( dirlevel < 0 ) {
               // we have made all the directories
               done = true;
            }   
         }
#endif
     }

     if ( err < 0 ) {
        throw (badMkdirFailed());
     }
        
}
