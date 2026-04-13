## Process this file with automake to produce Makefile.in

lib_LIBRARIES = libgigatraj.a

GIGATRAJ_INCLUDE = -I$(top_srcdir)/include/ -I${top_builddir}/include

# We create this file when the library is made.
# Another makefile in the doc subdirectory uses
# this to decide whether to re-make the documentation
TRIGGER_DOCS_REMAKE = ../doc/.remake_trigger
noinst_DATA = $(TRIGGER_DOCS_REMAKE)

$(TRIGGER_DOCS_REMAKE): libgigatraj.a
	touch $@



## Everything needs the gigatraj library includes
AM_CPPFLAGS  = $(GIGATRAJ_INCLUDE)

GTCONFIGDIR = @datadir@/@PACKAGE@/

## add stuff for MPI as well

SUBDIRS = metsources filters

EXTRA_DIST = metsources/catalog_merra.py

if MPI
   AM_CPPFLAGS += @MPICFLAGS@
endif
if NETCDF
   AM_CPPFLAGS += @NCDFCFLAGS@
endif

libgigatraj_a_SOURCES = ../include/gigatraj/RandomSrc.hh        RandomSrc.cc \
                        ../include/gigatraj/FilePath.hh         FilePath.cc \
                        ../include/gigatraj/FileLock.hh         FileLock.cc \
                        ../include/gigatraj/Configuration.hh    Configuration.cc \
                        ../include/gigatraj/CalGregorian.hh     CalGregorian.cc \
                        ../include/gigatraj/PlanetNav.hh        PlanetNav.cc \
                        ../include/gigatraj/PlanetSphereNav.hh  PlanetSphereNav.cc \
                        ../include/gigatraj/Earth.hh            Earth.cc \
                        ../include/gigatraj/ProcessGrp.hh       ProcessGrp.cc \
                        ../include/gigatraj/SerialGrp.hh        SerialGrp.cc \
                        ../include/gigatraj/Catalog.hh          metsources/Catalog.cc \
                        ../include/gigatraj/GridField.hh        metsources/GridField.cc \
                        ../include/gigatraj/GridFieldDim.hh     metsources/GridFieldDim.cc \
                        ../include/gigatraj/GridFieldDimLon.hh     metsources/GridFieldDimLon.cc \
                        ../include/gigatraj/GridFieldProfile.hh     metsources/GridFieldProfile.cc \
                        ../include/gigatraj/GridFieldSfc.hh     metsources/GridFieldSfc.cc \
                        ../include/gigatraj/GridLatLonFieldSfc.hh   metsources/GridLatLonFieldSfc.cc \
                        ../include/gigatraj/GridField3D.hh      metsources/GridField3D.cc \
                        ../include/gigatraj/GridLatLonField3D.hh    metsources/GridLatLonField3D.cc \
                        ../include/gigatraj/Vinterp.hh          metsources/Vinterp.cc \
                        ../include/gigatraj/LinearVinterp.hh    metsources/LinearVinterp.cc \
                        ../include/gigatraj/LogLinearVinterp.hh metsources/LogLinearVinterp.cc \
                        ../include/gigatraj/Hinterp.hh          metsources/Hinterp.cc \
                        ../include/gigatraj/BilinearHinterp.hh  metsources/BilinearHinterp.cc \
                        ../include/gigatraj/MetOnTheFly.hh      metsources/MetOnTheFly.cc \
                        ../include/gigatraj/ThetaOTF.hh         metsources/ThetaOTF.cc \
                        ../include/gigatraj/TropOTF.hh          metsources/TropOTF.cc \
                        ../include/gigatraj/PressOTF.hh         metsources/PressOTF.cc \
                        ../include/gigatraj/PAltOTF.hh          metsources/PAltOTF.cc \
                        ../include/gigatraj/PAltDotOTF.hh       metsources/PAltDotOTF.cc \
                        ../include/gigatraj/DensOTF.hh          metsources/DensOTF.cc \
                        ../include/gigatraj/MPVOTF.hh           metsources/MPVOTF.cc \
                        ../include/gigatraj/SZAOTF.hh           metsources/SZAOTF.cc \
                        ../include/gigatraj/ThetaDotOTF.hh      metsources/ThetaDotOTF.cc \
                        ../include/gigatraj/BalanceThetaDot1OTF.hh      metsources/BalanceThetaDot1OTF.cc \
                        ../include/gigatraj/MetData.hh          metsources/MetData.cc\
                        ../include/gigatraj/MetGridData.hh      metsources/MetGridData.cc\
                        ../include/gigatraj/MetGridLatLonData.hh  metsources/MetGridLatLonData.cc \
                        ../include/gigatraj/MetSBRot.hh         metsources/MetSBRot.cc \
                        ../include/gigatraj/MetGridSBRot.hh     metsources/MetGridSBRot.cc 
if MPI
   libgigatraj_a_SOURCES +=  ../include/gigatraj/MPIGrp.hh           MPIGrp.cc 
endif

if MYGEOS
   libgigatraj_a_SOURCES += ../include/gigatraj/MetMyGEOS.hh     metsources/MetMyGEOS.cc
endif
if MERRA
   libgigatraj_a_SOURCES += ../include/gigatraj/MetMyGEOS.hh     metsources/MetMyGEOS.cc
   metsources_MERRA_Directory_cc_CFLAGS = $(MDIRFLAG)
   libgigatraj_a_SOURCES += ../include/gigatraj/MetMERRA.hh     metsources/MetMERRA.cc
endif
if MERRA2
   libgigatraj_a_SOURCES += ../include/gigatraj/MetMyGEOS.hh     metsources/MetMyGEOS.cc
   libgigatraj_a_SOURCES += ../include/gigatraj/MetMERRA2.hh     metsources/MetMERRA2.cc
endif
if GEOSFP
   libgigatraj_a_SOURCES += ../include/gigatraj/MetMyGEOS.hh     metsources/MetMyGEOS.cc
   libgigatraj_a_SOURCES += ../include/gigatraj/MetGEOSfpFcast.hh     metsources/MetGEOSfpFcast.cc
   libgigatraj_a_SOURCES += ../include/gigatraj/MetGEOSfpAssim.hh     metsources/MetGEOSfpAssim.cc
   libgigatraj_a_SOURCES += ../include/gigatraj/MetGEOSfp.hh     metsources/MetGEOSfp.cc
endif
libgigatraj_a_SOURCES += ../include/gigatraj/MetSelector.hh     metsources/MetSelector.cc

libgigatraj_a_SOURCES += ../include/gigatraj/Parcel.hh           Parcel.cc \
                         ../include/gigatraj/Flock.hh            Flock.cc \
                         ../include/gigatraj/Swarm.hh            Swarm.cc \
                         ../include/gigatraj/trace.hh            trace.cc \
                         ../include/gigatraj/Integrator.hh       Integrator.cc \
                         ../include/gigatraj/IntegRK4.hh         IntegRK4.cc \
                         ../include/gigatraj/IntegRK4a.hh        IntegRK4a.cc \
                         ../include/gigatraj/ParcelGenerator.hh  ParcelGenerator.cc \
                         ../include/gigatraj/PGenRep.hh          PGenRep.cc \
                         ../include/gigatraj/PGenGrid.hh         PGenGrid.cc \
                         ../include/gigatraj/PGenFile.hh         PGenFile.cc \
                         ../include/gigatraj/PGenRnd.hh          PGenRnd.cc \
                         ../include/gigatraj/PGenRndDisc.hh      PGenRndDisc.cc \
                         ../include/gigatraj/PGenDisc.hh         PGenDisc.cc \
                         ../include/gigatraj/PGenLine.hh         PGenLine.cc \
                         ../include/gigatraj/PGenRndLine.hh      PGenRndLine.cc \
                         ../include/gigatraj/ParcelFilter.hh     filters/ParcelFilter.cc \
                         ../include/gigatraj/ParcelInitializer.hh ParcelInitializer.cc \
                         ../include/gigatraj/ParcelReporter.hh   filters/ParcelReporter.cc \
                         ../include/gigatraj/StreamDump.hh       filters/StreamDump.cc \
                         ../include/gigatraj/StreamLoad.hh       filters/StreamLoad.cc \
                         ../include/gigatraj/StreamPrint.hh      filters/StreamPrint.cc \
                         ../include/gigatraj/StreamRead.hh       filters/StreamRead.cc \
                         ../include/gigatraj/Filter_Trop.hh      filters/Filter_Trop.cc \
                         ../include/gigatraj/Filter_Null.hh      filters/Filter_Null.cc \
                         ../include/gigatraj/ChangeVertical.hh   filters/ChangeVertical.cc

if NETCDF
   libgigatraj_a_SOURCES += ../include/gigatraj/NetcdfIn.hh     filters/NetcdfIn.cc 
   libgigatraj_a_SOURCES += ../include/gigatraj/NetcdfOut.hh     filters/NetcdfOut.cc 
   libgigatraj_a_SOURCES += ../include/gigatraj/PGenNetcdf.hh   PGenNetcdf.cc 
endif


