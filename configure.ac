AC_PREREQ([2.69])
AC_INIT([GigaTraj],[0.8],[Leslie.R.Lait@nasa.gov])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_SRCDIR([lib/Parcel.cc])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

AC_ARG_ENABLE([double],
[  --enable-double	Use double-precision instead of regular floating-point numbers ],
[case "${enableval}" in
 yes) do_double=true ;;
 no)  do_double=false ;;
 *) AC_MSG_ERROR([bad value ${enableval} for --enable-double]) ;;
 esac], [do_double=false])
AM_CONDITIONAL([DOUBLE], [test x$do_double = xtrue])
AC_SUBST([DO_DOUBLE],[0])
if test x$do_double = xtrue ; then
AC_SUBST([DO_DOUBLE], [1])
fi

AC_ARG_ENABLE([wrap0],
[  --enable-wrap0		by default, set longitudes to wrap at 0 degrees, making a range of 0 to 360 ],
[case "${enableval}" in
 yes) do_wrap0=true ;;
 no)  do_wrap0=false ;;
 *) AC_MSG_ERROR([bad value ${enableval} for --enable-wrap0]) ;;
 esac], [do_wrap0=false])
AM_CONDITIONAL([WRAP0], [test x$do_wrap0 = xtrue])
AC_SUBST([DO_WRAP0],[0])
if test x$do_wrap0 = xtrue ; then
AC_SUBST([DO_WRAP0], [1])
fi

AC_ARG_ENABLE([wrap180],
[  --enable-wrap180		by default, set longitudes to wrap at 180 degrees, making a range of -180 to 180 ],
[case "${enableval}" in
 yes) do_wrap180=true ;;
 no)  do_wrap180=false ;;
 *) AC_MSG_ERROR([bad value ${enableval} for --enable-wrap180]) ;;
 esac], [do_wrap180=false])
AM_CONDITIONAL([WRAP180], [test x$do_wrap180 = xtrue])
AC_SUBST([DO_WRAP180],[0])
if test x$do_wrap180 = xtrue ; then
AC_SUBST([DO_WRAP180], [1])
fi


AC_ARG_ENABLE([doxygen],
[  --enable-doxygen		Allows generation of documentation files ],
[case "${enableval}" in
 yes) do_doxygen=true ;;
 no)  do_doxygen=false ;;
 *) AC_MSG_ERROR([bad value ${enableval} for --enable-doxygen]) ;;
 esac], [do_doxygen=false])
AM_CONDITIONAL([DOXYGEN], [test x$do_doxygen = xtrue])

# the list of c andc++ compilers to look for--we will edit this if mpicc and mpicxx are available)
CLIST_CC="gcc cl cc"
CLIST_CXX="g++ c++"

#------- MPI ----------
#   where the MPI stuff can be found.
g_path_mpi_pfx=""
# are we going to try to use MPI?
AC_MSG_CHECKING(--with-mpi)
AC_ARG_WITH([mpi],
[  --with-mpi=path     Construct a version of the library that uses MPI for parallel processing ],
[case "${with_mpi}" in
 yes) do_mpi=true
      # do we get the MPI location from this environment variable? 
      if test "X${MPI_PREFIX}" != "X" ; then
         # sure, it's defined, but is it real?
         if test -d "${MPI_PREFIX}/" ; then
            g_path_mpi_pfx="${MPI_PREFIX}"
         else
            AC_MSG_ERROR([${MPI_PREFIX} is not a directory])   
         fi 
      fi ;;
 no)  do_mpi=false ;;
 *)   do_mpi=true
      # if the argument is not "yes" or "no", 
      # then this must be the MPI location
      if test -d "${withval}/" ; then
          g_path_mpi_pfx="${withval}"
      else
          AC_MSG_ERROR([Cannot find MPI prefix directory ${withval}])
      fi ;;
 esac], [do_mpi=false])
AC_MSG_RESULT($do_mpi)

# At this point, the location of the MPI stuff, g_path_mpi_pfx,
# may be defined. But some installations break out the binary
# executables into another, separate location.
# So check for that.
g_path_mpi_bin=""
AC_MSG_CHECKING(--with-mpi-bin)
AC_ARG_WITH([mpi-bin],
[  --with-mpi-bin=path     Specify the path of the MPI binaries (e.g., mpirun) ],
[ do_mpi=true 
  if test "x${withval}" != "x" ; then
     if test -d "${withval}/" ; then
        g_path_mpi_bin="${withval}"
     else
        AC_MSG_ERROR([cannot find MPI bin directory $withval])
     fi
  fi 
], )
# Now, if we are doing MPI...
if test "x${do_mpi}" = "xtrue" ; then
   # ... and if the binary path is undefined...
   if test "x${g_path_mpi_bin}" = "x" ; then
      # ... and if if MPI location is defined
      if test "x${g_path_mpi_pfx}" != "x" ; then
         # then try the bin subdir of the MPI location
         if test -d "${g_path_mpi_pfx}/bin/" ; then
            g_path_mpi_bin="${g_path_mpi_pfx}/bin"
         fi
      fi   
   fi
fi
AC_MSG_RESULT($g_path_mpi_bin)

# and some installations put the libraries in a different location
# than the test of the MPI stuff. 
# So check for that.
g_path_mpi_lib=""
AC_MSG_CHECKING(--with-mpi-lib)
AC_ARG_WITH([mpi-lib],
[  --with-mpi-lib=path     Specify the path of the MPI libraries ],
[ do_mpi=true 
  if test "x${withval}" != "x" ; then
     if test -d "${withval}/" ; then
        g_path_mpi_lib="${withval}"
     else
        AC_MSG_ERROR([cannot find MPI bin directory $withval])
     fi
  fi 
], )
# If we are using MPI...
if test "x${do_mpi}" = "xtrue" ; then
   # and if the library location is not defined
   if test "x${g_path_mpi_lib}" = "x" ; then
      # and if the MPI location is defined
      if test "x${g_path_mpi_pfx}" != "x" ; then
         # then define the library location as a subdir of that
         if test -d "${g_path_mpi_pfx}/lib/" ; then
            g_path_mpi_lib="${g_path_mpi_pfx}/lib"
         fi   
      fi   
   fi
fi
AC_MSG_RESULT($g_path_mpi_lib)

# aaaand some installations break the MPI include files out from
# the rest of the MPI stuff and put them in their own location.
# So check for that, too. (sigh)
g_path_mpi_incl=""
AC_MSG_CHECKING(--with-mpi-include)
AC_ARG_WITH([mpi-include],
[  --with-mpi-include=path     Specify the path of the MPI include files ],
[ do_mpi=true 
  if test "x${withval}" != "x" ; then
     if test -d "${withval}/" ; then
        g_path_mpi_incl="${withval}"
     else
        AC_MSG_ERROR([cannot find MPI bin directory $withval])
     fi
  fi 
], )
# If we are using MPI...
if test "x${do_mpi}" = "xtrue" ; then
   # and if the include files location is not defined
   if test "x${g_path_mpi_incl}" = "x" ; then
      # and if the MPI location is defined
      if test "x${g_path_mpi_pfx}" != "x" ; then
         # then define the includes location as a subdir of that
         if test -d "${g_path_mpi_pfx}/include/" ; then
            g_path_mpi_incl="${g_path_mpi_pfx}/include"
         fi   
      fi   
   fi
fi
AC_MSG_RESULT($g_path_mpi_incl)


#------- NETCDF ----------
g_path_ncdf_pfx=""
AC_MSG_CHECKING(--with-netcdf)
AC_ARG_WITH([netcdf],
[  --with-netcdf=path     Construct a version of the library that uses NetCDF for parallel processing ],
[case "${with_netcdf}" in
 yes) do_ncdf=true 
      g_path_ncdf_pfx="/usr"
      if test "X${NCDF_PREFIX}" != "X" ; then
         if test -d "${NCDF_PREFIX}/" ; then
            g_path_ncdf_pfx="${NCDF_PREFIX}"
         else
            AC_MSG_ERROR([${NCDF_PREFIX} is not a directory])   
         fi 
      fi ;;
 no)  do_ncdf=false ;;
 *)   do_ncdf=true
      if test -d "${withval}/" ; then
          g_path_ncdf_pfx="${withval}"
      else
          AC_MSG_ERROR([Cannot find NCDF prefix directory ${withval}])
      fi ;;
 esac], [do_ncdf=false])
AC_MSG_RESULT($do_ncdf)

g_path_ncdf_lib=""
AC_MSG_CHECKING(--with-netcdf-lib)
AC_ARG_WITH([netcdf-lib],
[  --with-netcdf-lib=path     Specify the path of the NetCDF libraries ],
[ do_ncdf=true 
  if test "x$g_path_ncdf_pfx}" = "x" ; then
     g_path_ncdf_pfx="/usr"
  fi
  if test "x${withval}" != "x" ; then
     if test -d "${withval}/" ; then
        g_path_ncdf_lib="${withval}"
     else
        AC_MSG_ERROR([cannot find NCDF bin directory $withval])
     fi
  fi 
], )
if test "x${do_ncdf}" = "xtrue" ; then
   if test "x${g_path_ncdf_lib}" = "x" ; then
      if test -d "${g_path_ncdf_pfx}/lib/" ; then
         g_path_ncdf_lib="${g_path_ncdf_pfx}/lib"
      fi   
   fi
fi
AC_MSG_RESULT($g_path_ncdf_lib)

g_path_ncdf_incl=""
AC_MSG_CHECKING(--with-netcdf-include)
AC_ARG_WITH([netcdf-include],
[  --with-netcdf-include=path     Specify the path of the NetCDF include files ],
[ do_ncdf=true 
  if test "x$g_path_ncdf_pfx}" = "x" ; then
     g_path_ncdf_pfx="/usr"
  fi
  if test "x${withval}" != "x" ; then
     if test -d "${withval}/" ; then
        g_path_ncdf_incl="${withval}"
     else
        AC_MSG_ERROR([cannot find NCDF bin directory $withval])
     fi
  fi 
], )
if test "x${do_ncdf}" = "xtrue" ; then
   if test "x${g_path_ncdf_incl}" = "x" ; then
      if test -d "${g_path_ncdf_pfx}/include/" ; then
         g_path_ncdf_incl="${g_path_ncdf_pfx}/include"
      fi   
   fi
fi
AC_MSG_RESULT($g_path_ncdf_incl)


save_libs="${LIBS}"
save_cflags="${CFLAGS}"
save_path="${PATH}"

if test "X${do_mpi}" = "Xtrue" ; then

   # We may have the MPI binaries location undefined.
   # Or it may be wrong. This is the time to find out.
   g_need_mpi_bin=1
   if test "x${g_path_mpi_bin}" != "x" ; then
      # this has to be there
      if test -x "${g_path_mpi_bin}/mpirun" ; then
         g_need_mpi_bin=0
      fi
   fi
   
   if test $g_need_mpi_bin = 1 ; then
      # Look through the PATH for it.
      as_save_IFS=$IFS; IFS=$PATH_SEPARATOR
      for as_dir in $PATH
      do
          if test -x $as_dir/mpirun ; then
             g_path_mpi_bin=$as_dir
             # and derive the MPI path from it if we need to
             if test "x$g_path_mpi_pfx}" = "x" ; then
                g_path_mpi_pfx=`dirname $g_path_mpi_bin`
             fi
             break
          fi
      done
      IFS=$as_save_IFS
   fi
   
   if test "x${g_path_mpi_bin}" != "x" ; then
      MPI_PATH="${g_path_mpi_bin}/"
      # this holds only for the rest of this script,
      # but it is useful
      PATH="${PATH}:${MPI_PATH}"
   fi

#   # We may have the MPI libraries location undefined.
#   if test "x${g_path_mpi_lib" = "x" ; then
#      # Look through the LD_LIBRARY list for it.
#      as_save_IFS=$IFS; IFS=$PATH_SEPARATOR
#      for as_dir in $LD_LIBRARY
#      do
#          # ugly, but the file names may vary depending on the OS.
#          g_anything_matches=`ls -1 $as_dir/libmpi*`
#          if test "x${g_anything_matches}" != "x" ; then
#             g_path_mpi_lib=$as_dir
#             # and derive the MPI path from it if we need to
#             if test "x$g_path_mpi_pfx}" = "x" ; then
#                g_path_mpi_pfx=`dirname $g_path_mpi_lib`
#             fi
#             break
#          fi
#      done
#      IFS=$as_save_IFS
#   fi

#   # We may have the MPI includes location undefined.
#   # or it may be wrong.
#   g_need_mpi_incl=1
#   if test "x${g_path_mpi_incl}" != "x" ; then
#      # this has to be there
#      if test -s "${g_path_mpi_incl}/mpi.h" ; then
#         g_need_mpi_incl=0
#      fi
#   fi
#   
#   if test $g_need_mpi_incl = 1 ; then
#      # Look through the INCLUDE list for it.
#      as_save_IFS=$IFS; IFS=$PATH_SEPARATOR
#      for as_dir in $INCLUDE
#      do
#          if test -s "${as_dir}/mpi.h" ; then
#             g_path_mpi_incl=$as_dir
#             # and derive the MPI path from it if we need to
#             if test "x$g_path_mpi_pfx}" = "x" ; then
#                g_path_mpi_pfx=`dirname $g_path_mpi_incl`
#             fi
#             break
#          fi
#      done
#      IFS=$as_save_IFS
#   fi
   
   # now add the MPI library location to our library
   if test "X${g_path_mpi_lib}" != "X" ; then
      MPI_LIBS="-L${g_path_mpi_lib}/"
      LIBS="${LIBS} ${MPI_LIBS} "
   fi

   # We are using MPI C bindings in gigatraj, even
   # though gigatraj is written in C++.
   # (So sue us.)
   # Anyway, we need to turn off C++ bindings for gigatraj
   MPI_CFLAGS="-DOMPI_SKIP_MPICXX  "

   # now add the MPI include files to our CFLAGS
   if test "X${g_path_mpi_incl}" != "X" ; then
      MPI_CFLAGS="${MPI_CFLAGS} -I${g_path_mpi_incl}/"
   fi
   CFLAGS="${CFLAGS} ${MPI_CFLAGS} "

fi

if test "X${do_ncdf}" = "Xtrue" ; then

   # the NCDF library
   if test "X${g_path_ncdf_lib}" != "X" ; then
      NCDF_LIBS="-L${g_path_ncdf_lib}/"
      LIBS="${LIBS} ${NCDF_LIBS} "
   fi

   NCDF_CFLAGS=""
   # now the NCDF include files
   if test "X${g_path_ncdf_incl}" != "X" ; then
      NCDF_CFLAGS="${NCDF_CFLAGS} -I${g_path_ncdf_incl}/"
   fi
   CFLAGS="${CFLAGS} ${NCDF_CFLAGS} "
fi

# (for debugging)
#AC_MSG_WARN([MPI_PATH=${MPI_PATH}])
#AC_MSG_WARN([MPI_LIBS=${MPI_LIBS}])
#AC_MSG_WARN([MPI_CFLAGS=${MPI_CFLAGS}])
#AC_MSG_WARN([CLIST_CC=${CLIST_CC}])
#AC_MSG_WARN([CLIST_CXX=${CLIST_CXX}])
#AC_MSG_WARN([NCDF_LIBS=${NCDF_LIBS}])
#AC_MSG_WARN([NCDF_CFLAGS=${NCDF_CFLAGS}])

# define stuff for the Makefiles 
AM_CONDITIONAL([MPI], [test x$do_mpi = xtrue])
AC_SUBST([DO_MPI],[0])
if test x$do_mpi = xtrue ; then
AC_SUBST([DO_MPI], [1])
AC_SUBST([MPIPATH], [${MPI_PATH}])
AC_SUBST([MPICFLAGS], [${MPI_CFLAGS}])
AC_SUBST([MPILIBS], [${MPI_LIBS}])

# openMPI wants us to use mpicc instead of cc or gcc, so we test for its presence
AC_CHECK_PROG([use_mpicc], [mpicc], [yes], [no])
# this is actually more of a check for whether the MPI C++ bindings 
# are present in this site's openmpi installation
AC_CHECK_PROG([use_mpicxx], [mpicxx], [yes], [no])
if test x$use_mpicc = xyes ; then
   CLIST_CC="mpicc ${CLIST_CC}"
fi
if test x$use_mpicxx = xyes ; then
   CLIST_CXX="mpicxx ${CLIST_CXX}"
fi
fi

##### optional MetData subclasses 

AC_ARG_ENABLE([allmet],
[  --enable-allmet		Add all meteorological data classes ],
[case "${enableval}" in
 yes) do_allmet=true ;;
 no)  do_allmet=false ;;
 *) AC_MSG_ERROR([bad value ${enableval} for --enable-allmet]) ;;
 esac], [do_allmet=false])

AC_ARG_ENABLE([merra],
[  --enable-merra		Add class for reading NASA's GMAO MERRA meteorological data ],
[case "${enableval}" in
 yes) do_merra=true ;;
 no)  do_merra=false ;;
 *) AC_MSG_ERROR([bad value ${enableval} for --enable-merra]) ;;
 esac], [do_merra=false])
AM_CONDITIONAL([MERRA], [test x$do_merra = xtrue])
AC_SUBST([DO_MERRA],[0])
if test x$do_merra = xtrue ; then
do_ncdf=true
do_mygeos=true
AC_SUBST([DO_MERRA], [1])
fi

AC_ARG_ENABLE([merra2],
[  --enable-merra2		Add class for reading NASA's GMAO MERRA2 meteorological data ],
[case "${enableval}" in
 yes) do_merra2=true ;;
 no)  do_merra2=false ;;
 *) AC_MSG_ERROR([bad value ${enableval} for --enable-merra2]) ;;
 esac], [do_merra2=false])
if test x$do_allmet = xtrue ; then
   do_merra2=true
fi 
AM_CONDITIONAL([MERRA2], [test x$do_merra2 = xtrue])
AC_SUBST([DO_MERRA2],[0])
if test x$do_merra2 = xtrue ; then
do_ncdf=true
do_mygeos=true
AC_SUBST([DO_MERRA2], [1])
fi

AC_ARG_ENABLE([geosfp],
[  --enable-geosfp		Add class for reading NASA's GMAO GEOS fp meteorological data ],
[case "${enableval}" in
 yes) do_geosfp=true ;;
 no)  do_geosfp=false ;;
 *) AC_MSG_ERROR([bad value ${enableval} for --enable-geosfp]) ;;
 esac], [do_geosfp=false])
if test x$do_allmet = xtrue ; then
   do_geosfp=true
fi 
AM_CONDITIONAL([GEOSFP], [test x$do_geosfp = xtrue])
AC_SUBST([DO_GEOSFP],[0])
if test x$do_geosfp = xtrue ; then
do_ncdf=true
do_mygeos=true
AC_SUBST([DO_GEOSFP], [1])
fi

AC_ARG_ENABLE([mygeos],
[  --enable-mygeos		Add class for reading local output files from NASA's GMAO GEOS ],
[case "${enableval}" in
 yes) do_mygeos=true ;;
 no)  do_mygeos=false ;;
 *) AC_MSG_ERROR([bad value ${enableval} for --enable-mygeos]) ;;
 esac], [do_mygeos=false])
if test x$do_allmet = xtrue ; then
   do_mygeos=true
fi 
AM_CONDITIONAL([MYGEOS], [test x$do_mygeos = xtrue])
AC_SUBST([DO_MYGEOS],[0])
if test x$do_mygeos = xtrue ; then
do_ncdf=true
AC_SUBST([DO_MYGEOS], [1])
fi



AM_CONDITIONAL([NETCDF], [test x$do_ncdf = xtrue])
AC_SUBST([DO_NETCDF],[0])
if test x$do_ncdf = xtrue ; then
AC_SUBST([DO_NETCDF], [1])
AC_SUBST([NCDFCFLAGS], [${NCDF_CFLAGS}])
AC_SUBST([NCDFLIBS], [${NCDF_LIBS}])
fi


###### Enough with specifications. Let's start checking for stuff.


#AC_MSG_WARN([CLIST_CC=${CLIST_CC}])
#AC_MSG_WARN([CLIST_CXX=${CLIST_CXX}])
 
AC_PROG_CC( [ ${CLIST_CC} ] )
if test x$CC = xmpicc ; then
   if test x$MPI_PATH != x ; then
      if test -x "$MPI_PATH$CC" ; then
         CC="$MPI_PATH$CC"
      fi
   fi
fi 
AC_PROG_CXX( [ ${CLIST_CXX} ] )
if test x$CXX = xmpicxx ; then
   if test x$MPI_PATH != x ; then
      if test -x "$MPI_PATH$CXX" ; then
         CXX="$MPI_PATH$CXX"
      fi
   fi
fi 

AC_PROG_RANLIB
AM_PROG_AR([ar])
# Can we run MPI programs for parallel programming?
if test x$do_mpi = xtrue ; then
   AC_ARG_VAR([MPIRUN], [The program used to run MPI programs (used to run the test suite)])
   AC_PATH_PROG([MPIRUN], [mpirun] )
fi
# Can we run the Doxygen documentation generator software?
if test x$do_doxygen = xtrue ; then

   AC_ARG_VAR([DOXYGENRUN], [The program used to run doxygen])
   AC_PATH_PROG([DOXYGENRUN], [doxygen])
   AS_IF([test "x$DOXYGENRUN" = x],
     [AC_MSG_ERROR([doxygen is enabled but the doxygen command is missing])] )

   # do we have the components for making LaTeX documentation?
   AC_ARG_VAR([DOX_RUN_LATEX], [The program used to run latex])
   AC_PATH_PROG([DOX_RUN_LATEX], [latex] )

   AC_ARG_VAR([DOX_RUN_MAKEINDEX], [The program to make latex indices])
   AC_PATH_PROG([DOX_RUN_MAKEINDEX], [makeindex] )

   AC_CHECK_PROG([DOX_PDFLATEX], [pdflatex], [YES], [NO] )


fi
# Define which forms of documentation Doxygen should generate
AM_CONDITIONAL([DOX_DOES_HTML], [true])
AM_CONDITIONAL([DOX_DOES_LATEX], [test x${DOX_RUN_LATEX} != x ])
#AM_CONDITIONAL([DOX_DOES_LATEX], [false])
AM_CONDITIONAL([DOX_DOES_MAN], [true])
AM_CONDITIONAL([DOX_DOES_RTF], [true])
AM_CONDITIONAL([DOX_DOES_XML], [false])
AM_CONDITIONAL([DOX_DOES_PERLMOD], [false])
AM_CONDITIONAL([DOX_DOES_AUTOGEN], [false])
AM_CONDITIONAL([DOX_DOES_DOCBOOK], [false])
# The base doxygen config file, doc/gigatraj_base.dconf.in, 
# refers to certain C Macro definitions to determine which
# sections of the documentation are to be generated.
# We set some definitions here to assist:
AS_IF([test "x${do_double}" = xtrue],
   [AC_SUBST([DOXDEF_DOUBLE], [USE_DOUBLE])],
   [AC_SUBST([DOXDEF_DOUBLE], [ ])])
AS_IF([test "x${do_mpi}" = xtrue],
   [AC_SUBST([DOXDEF_MPI], [USE_MPI])],
   [AC_SUBST([DOXDEF_MPI], [ ])])


# If we are using the g++ compiler, then we should turn off
# variable tracking when compiling MERRA_Directory.cc, because
# of the huge number of entries in the MERRA variable set.
# same applies for MERRA2 and GEOSFP
MDIRFLAG=""
if test x$do_merra = xtrue || test x$do_merra2 = xtrue || test x$do_geosfp = xtrue ; then
   # (Should we do something about mpicxx here?)
   case $CXX in
   *g++*) MDIRFLAG="-fno-var-tracking";;
   *);;
   esac
fi

# Checks for header files.
AC_CHECK_HEADER([math.h])
AC_CHECK_HEADERS([stdio.h])
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([time.h])
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([sys/types.h])
AC_CHECK_HEADERS([sys/stat.h])
AC_CHECK_HEADERS([string.h])

# check for long arguments
AC_CHECK_HEADERS([getopt.h],[do_longargs=true], [do_longargs=false] )
AM_CONDITIONAL([LONGARGS], [test x$do_longargs = xtrue])
AC_SUBST([DO_LONGARGS], [0])
if test x$do_longargs = xtrue ; then
AC_SUBST([DO_LONGARGS], [1])
fi

#    only check for MPI if we are using MPI
if test x$do_mpi = xtrue ; then
# we will need the header file
#AC_CHECK_HEADERS([mpi.h], [], [AC_MSG_ERROR(no MPI header file was  found)])
AC_CHECK_HEADERS([mpi.h  openmpi-x86_64/mpi.h]
  , [gigatraj_found_mpi_headers=yes; break;])
AS_IF([test "x$gigatraj_found_mpi_headers" != "xyes"], [AC_MSG_ERROR([no MPI header file was  found])])
if test x$use_mpicc = xno ; then
# we will need the MPI C library 
#AC_LIB_LINKFLAGS(mpi)
AC_SEARCH_LIBS([MPI_Init],[mpi], [], [AC_MSG_ERROR(no MPI library file was found)] )
fi
if test x$test_mpicxx = xyes ; then
# MPI C++ bindings are present, so the header file contains C++ definisiotns,
# so we will need this library as well.
AC_CHECK_LIB([mpi_cxx], [MPI_Init], [], [] )
fi
fi

#    check for netcdf v4 if we are using MERRA, MERRAS2, or GEOSFP
#if test x$do_merra = xtrue || test x$do_merra2 = xtrue || test x$do_geosfp = xtrue ; then
if test x$do_ncdf = xtrue ; then
AC_MSG_NOTICE([Doing netcdf!!!!])
AC_CHECK_HEADERS([netcdf.h], [], [AC_MSG_ERROR(no NetCDF header file was  found)])
AC_SEARCH_LIBS([nc_put_var1_uint], [netcdf], [], [AC_MSG_ERROR([You need to install netcdf v4 or greater (with dap enabled), or the libdap and libnc-dap libraries from http://opendap.org/download/index.html])])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_FUNC_ALLOCA
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

AC_CONFIG_FILES([Makefile
                 include/gigatraj/Makefile
                 include/gigatraj/gigatraj.hh
                 lib/Makefile
                 lib/filters/Makefile
                 lib/metsources/Makefile
                 src/Makefile
                 src/gtmodel_m01 src/gtmodel_m02
                 confiles/Makefile
                 test/Makefile
                 test/test_MPI.sh
                 doc/gigatraj_base.dconf:doc/gigatraj_base.dconf.in
                 doc/latex_yes.dconf:doc/latex_yes.dconf.in
                 doc/Makefile])
AC_OUTPUT

chmod 755 src/gtmodel_m01 src/gtmodel_m02 test/test_MPI.sh
