## Process this file with automake to produce Makefile.in


## Everything in the test suite needs the gigatraj library
LDADD = -lgigatraj
AM_LDFLAGS = -L ../lib/

GIGATRAJ_INCLUDE = -I$(top_srcdir)/include/ -I${top_builddir}/include

## Everything needs the gigatraj library includes
AM_CPPFLAGS  = $(GIGATRAJ_INCLUDE)

if MPI
   AM_CPPFLAGS += @MPICFLAGS@
   AM_LDFLAGS += @MPILIBS@ -lmpi
endif

if NETCDF
   AM_CPPFLAGS += @NCDFCFLAGS@
   AM_LDFLAGS += @NCDFLIBS@ -lnetcdf
endif

##### Very Basic stuff

TESTS = test_RandomSrc test_Configuration.sh \
        test_Catalog.sh \
        test_FilePath test_FileLock \
        test_CalGregorian \
        test_Earth
check_PROGRAMS = test_RandomSrc test_Configuration \
        test_Catalog \
        test_FilePath test_FileLock \
        test_CalGregorian \
        test_Earth 
EXTRA_DIST = test_Configuration.sh test_Catalog.sh test_Catalog.cat;


##### Process Group stuff

TESTS += test_SerialGrp 
check_PROGRAMS +=  test_SerialGrp

TESTS += test_FileLock_Serial
check_PROGRAMS +=  test_FileLock_Serial

if MPI
   TESTS += test_MPIGrp.sh  test_FileLock_MPI.sh
   check_PROGRAMS += test_MPIGrp test_FileLock_MPI
   test_FileLock_MPI_CPPFLAGS = $(AM_CPPFLAGS) -DUSING_MPI
endif   
EXTRA_DIST += test_MPI.sh \
              test_MPIGrp.sh \
              test_FileLock_MPI.sh 


##### Meteorological data stuff

TESTS += test_GridFieldDim test_GridFieldDim_serial \
         test_GridFieldDimLon test_GridFieldDimLon_serial \
         test_GridFieldProfile test_GridFieldProfile_serial \
         test_GridLatLonFieldSfc test_GridLatLonFieldSfc_serial \
         test_GridLatLonField3D  test_GridLatLonField3D_serial
check_PROGRAMS += test_GridFieldDim  test_GridFieldDim_serial \
         test_GridFieldDimLon  test_GridFieldDimLon_serial \
         test_GridFieldProfile  test_GridFieldProfile_serial \
         test_GridLatLonField3D test_GridLatLonField3D_serial \
         test_GridLatLonFieldSfc test_GridLatLonFieldSfc_serial 
if MPI
   TESTS += test_GridFieldDim_MPI.sh  
   check_PROGRAMS += test_GridFieldDim_MPI
   test_GridFieldDim_MPI_CPPFLAGS = $(AM_CPPFLAGS) -DUSING_MPI

   TESTS += test_GridFieldDimLon_MPI.sh  
   check_PROGRAMS += test_GridFieldDimLon_MPI
   test_GridFieldDimLon_MPI_CPPFLAGS = $(AM_CPPFLAGS) -DUSING_MPI

   TESTS += test_GridFieldProfile_MPI.sh  
   check_PROGRAMS += test_GridFieldProfile_MPI
   test_GridFieldProfile_MPI_CPPFLAGS = $(AM_CPPFLAGS) -DUSING_MPI

   TESTS += test_GridLatLonFieldSfc_MPI.sh  
   check_PROGRAMS += test_GridLatLonFieldSfc_MPI
   test_GridLatLonFieldSfc_MPI_CPPFLAGS = $(AM_CPPFLAGS) -DUSING_MPI

   TESTS += test_GridLatLonField3D_MPI.sh
   check_PROGRAMS += test_GridLatLonField3D_MPI
   test_GridLatLonField3D_MPI_CPPFLAGS = $(AM_CPPFLAGS) -DUSING_MPI
endif  
EXTRA_DIST += test_GridLatLonField3D_MPI.sh \
              test_GridLatLonFieldSfc_MPI.sh \
              test_GridFieldProfile_MPI.sh \
              test_GridFieldDimLon_MPI.sh \
              test_GridFieldDim_MPI.sh 

TESTS          += test_LinearVinterp test_LogLinearVinterp test_BilinearHinterp
check_PROGRAMS += test_LinearVinterp test_LogLinearVinterp test_BilinearHinterp
TESTS += test_BilinearHinterp_serial
check_PROGRAMS += test_BilinearHinterp_serial
if MPI
   TESTS += test_BilinearHinterp_MPI.sh
   check_PROGRAMS += test_BilinearHinterp_MPI
   test_BilinearHinterp_MPI_CPPFLAGS = $(AM_CPPFLAGS) -DUSING_MPI
endif  
EXTRA_DIST += test_BilinearHinterp_MPI.sh 

TESTS += test_ThetaOTF test_TropOTF test_PressOTF test_PAltOTF test_PAltDotOTF test_DensOTF test_ThetaDotOTF test_BalanceThetaDot1OTF test_SZAOTF
check_PROGRAMS += test_ThetaOTF test_TropOTF test_PressOTF test_PAltOTF test_PAltDotOTF test_DensOTF test_ThetaDotOTF test_BalanceThetaDot1OTF test_SZAOTF



TESTS += test_MetSBRot test_MetSBRot_serial
check_PROGRAMS += test_MetSBRot test_MetSBRot_serial
if MPI
   TESTS += test_MetSBRot_MPI.sh
   check_PROGRAMS += test_MetSBRot_MPI
   test_MetSBRot_MPI_CPPFLAGS = $(AM_CPPFLAGS) -DUSING_MPI
endif 
EXTRA_DIST += test_MetSBRot_MPI.sh 


TESTS += test_MetGridSBRot test_MetGridSBRot_serial
check_PROGRAMS += test_MetGridSBRot test_MetGridSBRot_serial
if MPI
   TESTS += test_MetGridSBRot_MPI.sh
   check_PROGRAMS += test_MetGridSBRot_MPI
   test_MetGridSBRot_MPI_CPPFLAGS = $(AM_CPPFLAGS) -DUSING_MPI
endif 
EXTRA_DIST += test_MetGridSBRot_MPI.sh test_MetGridSBRot.cfg

if MYGEOS
   TESTS += test_MetMyGEOS.sh
   check_PROGRAMS += test_MetMyGEOS
   test_MetMyGEOS_MPI_CPPFLAGS = $(AM_CPPFLAGS) -DUSING_MPI
   EXTRA_DIST += test_MetMyGEOS.sh test_MetMyGEOS.cat ;
   EXTRA_DIST += Data/gigatraj_merra2.geosgcm_prog.20000415_0300z.nc4  
   EXTRA_DIST += Data/gigatraj_merra2.geosgcm_prog.20000415_0400z.nc4
endif
if MERRA
   TESTS += test_MetMERRA.sh
   check_PROGRAMS += test_MetMERRA
   test_MetMERRA_MPI_CPPFLAGS = $(AM_CPPFLAGS) -DUSING_MPI
   EXTRA_DIST += test_MetMERRA.sh MERRA.cat ;
endif
if MERRA2
   TESTS += test_MetMERRA2.sh
   check_PROGRAMS += test_MetMERRA2
   test_MetMERRA2_MPI_CPPFLAGS = $(AM_CPPFLAGS) -DUSING_MPI
   EXTRA_DIST += test_MetMERRA2.sh MERRA2.cat ;
endif
if GEOSFP
   TESTS += test_MetGEOSfpFcast.sh test_MetGEOSfpAssim.sh test_MetGEOSfp.sh
   check_PROGRAMS += test_MetGEOSfpFcast test_MetGEOSfpAssim test_MetGEOSfp
   test_MetGEOSfpFcast_MPI_CPPFLAGS = $(AM_CPPFLAGS) -DUSING_MPI
   test_MetGEOSfpAssim_MPI_CPPFLAGS = $(AM_CPPFLAGS) -DUSING_MPI
   test_MetGEOSfp_MPI_CPPFLAGS = $(AM_CPPFLAGS) -DUSING_MPI
   EXTRA_DIST += test_MetGEOSfp.sh GEOSfp.cat ;
   EXTRA_DIST += test_MetGEOSfpAssim.sh GEOSfpAssim.cat ;
   EXTRA_DIST += test_MetGEOSfpFcast.sh GEOSfpFcast.cat ;
endif
TESTS += test_MetSelector
check_PROGRAMS += test_MetSelector

##### Parcel stuff

TESTS += test_Parcel \
        test_Filter_Null \
        test_StreamDump_Load \
        test_StreamRead \
        test_StreamPrint \
        test_FlockSerial \
        test_SwarmSerial \
        test_ChangeVertical
check_PROGRAMS += test_Parcel \
        test_Filter_Null \
        test_StreamDump_Load \
        test_StreamRead \
        test_StreamPrint \
        test_FlockSerial \
        test_SwarmSerial \
        test_ChangeVertical
EXTRA_DIST += parcels_test00.dat 

if NETCDF
   TESTS += test_NetcdfIn test_NetcdfOut
   check_PROGRAMS += test_NetcdfIn test_NetcdfOut 
   EXTRA_DIST += test_netcdfIn_data_01.nc4
endif

if MPI
   TESTS += test_FlockMPI.sh 
   check_PROGRAMS += test_FlockMPI  
   TESTS += test_SwarmMPI.sh 
   check_PROGRAMS += test_SwarmMPI  
   TESTS += test_StreamPrintMPI.sh
   check_PROGRAMS += test_StreamPrintMPI 
   TESTS += test_StreamReadMPI.sh
   check_PROGRAMS += test_StreamReadMPI 
if NETCDF
   TESTS += test_NetcdfInMPI.sh
   check_PROGRAMS += test_NetcdfInMPI 
   TESTS += test_NetcdfOutMPI.sh
   check_PROGRAMS += test_NetcdfOutMPI 
endif
endif   
EXTRA_DIST += test_FlockMPI.sh \
              test_SwarmMPI.sh \
              test_StreamPrintMPI.sh \
              test_StreamReadMPI.sh \
              test_NetcdfInMPI.sh \
              test_NetcdfOutMPI.sh 

TESTS += test_PGenRep test_PGenGrid test_PGenRnd \
         test_PGenRndDisc test_PGenDisc test_PGenFile
check_PROGRAMS += test_PGenRep test_PGenGrid test_PGenRnd \
        test_PGenRndDisc test_PGenDisc test_PGenFile 
EXTRA_DIST += test_PGenFile.dat \
              test_PGenFile_fmt.dat
if NETCDF
   TESTS += test_PGenNetcdf
   check_PROGRAMS += test_PGenNetcdf
   # EXTRA_DIST += test_netcdfIn_data_01.nc4
endif


TESTS += test_Filter_Trop
check_PROGRAMS += test_Filter_Trop


TESTS += test_traj000 test_traj001 test_traj010
check_PROGRAMS += test_traj000 test_traj001 test_traj010
EXTRA_DIST += test_traj001.dat

TESTS += test_gtmodel_s01.sh
check_PROGRAMS += check_traj_diffs
EXTRA_DIST += test_gtmodel_s01.sh \
              gtmodel_s01_input.txt \
              gtmodel_s01_original.txt

###########  Sources

test_RandomSrc_SOURCES = test_RandomSrc.cc test_utils.cc test_utils.hh
test_RandomSrc_DEPENDENCIES = ../lib/libgigatraj.a

test_Configuration_SOURCES = test_Configuration.cc test_utils.cc test_utils.hh
test_Configuration_DEPENDENCIES = ../lib/libgigatraj.a

test_Catalog_SOURCES = test_Catalog.cc test_utils.cc test_utils.hh
test_Catalog_DEPENDENCIES = ../lib/libgigatraj.a

test_FilePath_SOURCES = test_FilePath.cc test_utils.cc test_utils.hh
test_FilePath_DEPENDENCIES = ../lib/libgigatraj.a

test_FileLock_SOURCES = test_FileLock.cc test_utils.cc test_utils.hh
test_FileLock_DEPENDENCIES = ../lib/libgigatraj.a

test_FileLock_Serial_SOURCES = test_FileLock_PGrp.cc test_utils.cc test_utils.hh
test_FileLock_Serial_DEPENDENCIES = ../lib/libgigatraj.a

test_FileLock_MPI_SOURCES = test_FileLock_PGrp.cc test_utils.cc test_utils.hh
test_FileLock_MPI_DEPENDENCIES = ../lib/libgigatraj.a

test_CalGregorian_SOURCES = test_CalGregorian.cc test_utils.cc test_utils.hh
test_CalGregorian_DEPENDENCIES = ../lib/libgigatraj.a

test_Earth_SOURCES = test_Earth.cc test_utils.cc test_utils.hh
test_Earth_DEPENDENCIES = ../lib/libgigatraj.a

test_SerialGrp_SOURCES = test_SerialGrp.cc test_utils.cc test_utils.hh
test_SerialGrp_DEPENDENCIES = ../lib/libgigatraj.a

test_MPIGrp_SOURCES = test_MPIGrp.cc test_utils.cc test_utils.hh
test_MPIGrp_DEPENDENCIES = ../lib/libgigatraj.a

test_GridFieldDim_SOURCES = test_GridFieldDim.cc test_utils.cc test_utils.hh
test_GridFieldDim_DEPENDENCIES = ../lib/libgigatraj.a

test_GridFieldDimLon_SOURCES = test_GridFieldDimLon.cc test_utils.cc test_utils.hh
test_GridFieldDimLon_DEPENDENCIES = ../lib/libgigatraj.a

test_GridFieldProfile_SOURCES = test_GridFieldProfile.cc test_utils.cc test_utils.hh
test_GridFieldProfile_DEPENDENCIES = ../lib/libgigatraj.a

test_GridLatLonFieldSfc_SOURCES = test_GridLatLonFieldSfc.cc test_utils.cc test_utils.hh
test_GridLatLonFieldSfc_DEPENDENCIES = ../lib/libgigatraj.a

test_GridLatLonField3D_SOURCES = test_GridLatLonField3D.cc test_utils.cc test_utils.hh
test_GridLatLonField3D_DEPENDENCIES = ../lib/libgigatraj.a

test_GridFieldDim_serial_SOURCES = test_GridFieldDim_PGrp.cc test_utils.cc
test_GridFieldDim_serial_DEPENDENCIES = ../lib/libgigatraj.a

test_GridFieldDimLon_serial_SOURCES = test_GridFieldDimLon_PGrp.cc test_utils.cc
test_GridFieldDimLon_serial_DEPENDENCIES = ../lib/libgigatraj.a

test_GridFieldProfile_serial_SOURCES = test_GridFieldProfile_PGrp.cc test_utils.cc
test_GridFieldProfile_serial_DEPENDENCIES = ../lib/libgigatraj.a

test_GridFieldDim_MPI_SOURCES = test_GridFieldDim_PGrp.cc test_utils.cc
test_GridFieldDim_MPI_DEPENDENCIES = ../lib/libgigatraj.a

test_GridFieldDimLon_MPI_SOURCES = test_GridFieldDimLon_PGrp.cc test_utils.cc
test_GridFieldDimLon_MPI_DEPENDENCIES = ../lib/libgigatraj.a

test_GridFieldProfile_MPI_SOURCES = test_GridFieldProfile_PGrp.cc test_utils.cc
test_GridFieldProfile_MPI_DEPENDENCIES = ../lib/libgigatraj.a

test_GridLatLonFieldSfc_serial_SOURCES = test_GridLatLonFieldSfc_PGrp.cc test_utils.cc
test_GridLatLonFieldSfc_serial_DEPENDENCIES = ../lib/libgigatraj.a

test_GridLatLonFieldSfc_MPI_SOURCES = test_GridLatLonFieldSfc_PGrp.cc test_utils.cc
test_GridLatLonFieldSfc_MPI_DEPENDENCIES = ../lib/libgigatraj.a

test_GridLatLonField3D_serial_SOURCES = test_GridLatLonField3D_PGrp.cc test_utils.cc
test_GridLatLonField3D_serial_DEPENDENCIES = ../lib/libgigatraj.a

test_GridLatLonField3D_MPI_SOURCES = test_GridLatLonField3D_PGrp.cc test_utils.cc
test_GridLatLonField3D_MPI_DEPENDENCIES = ../lib/libgigatraj.a

test_LinearVinterp_SOURCES = test_LinearVinterp.cc test_utils.cc test_utils.hh
test_LinearVinterp_DEPENDENCIES = ../lib/libgigatraj.a

test_LogLinearVinterp_SOURCES = test_LogLinearVinterp.cc test_utils.cc test_utils.hh
test_LogLinearVinterp_DEPENDENCIES = ../lib/libgigatraj.a

test_BilinearHinterp_SOURCES = test_BilinearHinterp.cc test_utils.cc test_utils.hh
test_BilinearHinterp_DEPENDENCIES = ../lib/libgigatraj.a

test_BilinearHinterp_serial_SOURCES = test_BilinearHinterp_PGrp.cc test_utils.cc test_utils.hh
test_BilinearHinterp_serial_DEPENDENCIES = ../lib/libgigatraj.a

test_BilinearHinterp_MPI_SOURCES = test_BilinearHinterp_PGrp.cc test_utils.cc test_utils.hh
test_BilinearHinterp_MPI_DEPENDENCIES = ../lib/libgigatraj.a

test_ThetaOTF_SOURCES = test_ThetaOTF.cc test_utils.cc test_utils.hh
test_ThetaOTF_DEPENDENCIES = ../lib/libgigatraj.a

test_TropOTF_SOURCES = test_TropOTF.cc test_utils.cc test_utils.hh
test_TropOTF_DEPENDENCIES = ../lib/libgigatraj.a

test_PressOTF_SOURCES = test_PressOTF.cc test_utils.cc test_utils.hh
test_PressOTF_DEPENDENCIES = ../lib/libgigatraj.a

test_PAltOTF_SOURCES = test_PAltOTF.cc test_utils.cc test_utils.hh
test_PAltOTF_DEPENDENCIES = ../lib/libgigatraj.a

test_PAltDotOTF_SOURCES = test_PAltDotOTF.cc test_utils.cc test_utils.hh
test_PAltDotOTF_DEPENDENCIES = ../lib/libgigatraj.a

test_DensOTF_SOURCES = test_DensOTF.cc test_utils.cc test_utils.hh
test_DensOTF_DEPENDENCIES = ../lib/libgigatraj.a

test_ThetaDotOTF_SOURCES = test_ThetaDotOTF.cc test_utils.cc test_utils.hh
test_ThetaDotOTF_DEPENDENCIES = ../lib/libgigatraj.a

test_BalanceThetaDot1OTF_SOURCES = test_BalanceThetaDot1OTF.cc test_utils.cc  test_utils.hh
test_BalanceThetaDot1OTF_DEPENDENCIES = ../lib/libgigatraj.a

test_SZAOTF_SOURCES = test_SZAOTF.cc test_utils.cc test_utils.hh
test_SZAOTF_DEPENDENCIES = ../lib/libgigatraj.a

test_MetSBRot_SOURCES = test_MetSBRot.cc test_utils.cc test_utils.hh
test_MetSBRot_DEPENDENCIES = ../lib/libgigatraj.a

test_MetSBRot_serial_SOURCES = test_MetSBRot_PGrp.cc test_utils.cc test_utils.hh
test_MetSBRot_serial_DEPENDENCIES = ../lib/libgigatraj.a

test_MetSBRot_MPI_SOURCES = test_MetSBRot_PGrp.cc test_utils.cc test_utils.hh
test_MetSBRot_MPI_DEPENDENCIES = ../lib/libgigatraj.a

test_MetGridSBRot_SOURCES = test_MetGridSBRot.cc test_utils.cc test_utils.hh
test_MetGridSBRot_DEPENDENCIES = ../lib/libgigatraj.a

test_MetGridSBRot_serial_SOURCES = test_MetGridSBRot_PGrp.cc test_utils.cc test_utils.hh
test_MetGridSBRot_serial_DEPENDENCIES = ../lib/libgigatraj.a

test_MetGridSBRot_MPI_SOURCES = test_MetGridSBRot_PGrp.cc test_utils.cc test_utils.hh
test_MetGridSBRot_MPI_DEPENDENCIES = ../lib/libgigatraj.a

test_MetMERRA_SOURCES = test_MetMERRA.cc test_utils.cc test_utils.hh
test_MetMERRA_DEPENDENCIES = ../lib/libgigatraj.a

test_MetMERRA2_SOURCES = test_MetMERRA2.cc test_utils.cc test_utils.hh
test_MetMERRA2_DEPENDENCIES = ../lib/libgigatraj.a

test_MetGEOSfpFcast_SOURCES = test_MetGEOSfpFcast.cc test_utils.cc test_utils.hh
test_MetGEOSfpFcast_DEPENDENCIES = ../lib/libgigatraj.a

test_MetGEOSfpAssim_SOURCES = test_MetGEOSfpAssim.cc test_utils.cc test_utils.hh
test_MetGEOSfpAssim_DEPENDENCIES = ../lib/libgigatraj.a

test_MetGEOSfp_SOURCES = test_MetGEOSfp.cc test_utils.cc test_utils.hh
test_MetGEOSfp_DEPENDENCIES = ../lib/libgigatraj.a

test_MetMyGEOS_SOURCES = test_MetMyGEOS.cc test_utils.cc test_utils.hh
test_MetMyGEOS_DEPENDENCIES = ../lib/libgigatraj.a

test_MetSelector_SOURCES = test_MetSelector.cc test_utils.cc test_utils.hh
test_MetSelector_DEPENDENCIES = ../lib/libgigatraj.a

test_Parcel_SOURCES = test_Parcel.cc test_utils.cc test_utils.hh
test_Parcel_DEPENDENCIES = ../lib/libgigatraj.a

test_ChangeVertical_SOURCES = test_ChangeVertical.cc test_utils.cc test_utils.hh
test_ChangeVertical_DEPENDENCIES = ../lib/libgigatraj.a

test_StreamDump_Load_SOURCES = test_StreamDump_Load.cc test_utils.cc test_utils.hh
test_StreamDump_Load_DEPENDENCIES = ../lib/libgigatraj.a

test_StreamRead_SOURCES = test_StreamRead.cc test_utils.cc test_utils.hh
test_StreamRead_DEPENDENCIES = ../lib/libgigatraj.a

test_NetcdfIn_SOURCES = test_NetcdfIn.cc test_utils.cc test_utils.hh
test_NetcdfIn_DEPENDENCIES = ../lib/libgigatraj.a

test_NetcdfOut_SOURCES = test_NetcdfOut.cc test_utils.cc test_utils.hh
test_NetcdfOut_DEPENDENCIES = ../lib/libgigatraj.a

test_NetcdfInMPI_SOURCES = test_NetcdfInMPI.cc test_utils.cc test_utils.hh
test_NetcdfInMPI_DEPENDENCIES = ../lib/libgigatraj.a

test_NetcdfOutMPI_SOURCES = test_NetcdfOutMPI.cc test_utils.cc test_utils.hh
test_NetcdfOutMPI_DEPENDENCIES = ../lib/libgigatraj.a

test_PGenNetcdf_SOURCES = test_PGenNetcdf.cc test_utils.cc test_utils.hh
test_PGenNetcdf_DEPENDENCIES = ../lib/libgigatraj.a

test_StreamPrint_SOURCES = test_StreamPrint.cc test_utils.cc test_utils.hh
test_StreamPrint_DEPENDENCIES = ../lib/libgigatraj.a

test_FlockSerial_SOURCES = test_FlockSerial.cc test_utils.cc test_utils.hh
test_FlockSerial_DEPENDENCIES = ../lib/libgigatraj.a

test_SwarmSerial_SOURCES = test_SwarmSerial.cc test_utils.cc test_utils.hh
test_SwarmSerial_DEPENDENCIES = ../lib/libgigatraj.a

test_FlockMPI_SOURCES = test_FlockMPI.cc test_utils.cc test_utils.hh
test_FlockMPI_DEPENDENCIES = ../lib/libgigatraj.a

test_SwarmMPI_SOURCES = test_SwarmMPI.cc test_utils.cc test_utils.hh
test_SwarmMPI_DEPENDENCIES = ../lib/libgigatraj.a

test_StreamPrintMPI_SOURCES = test_StreamPrintMPI.cc test_utils.cc test_utils.hh
test_StreamPrintMPI_DEPENDENCIES = ../lib/libgigatraj.a

test_StreamReadMPI_SOURCES = test_StreamReadMPI.cc test_utils.cc test_utils.hh
test_StreamReadMPI_DEPENDENCIES = ../lib/libgigatraj.a

test_PGenRep_SOURCES = test_PGenRep.cc test_utils.cc test_utils.hh
test_PGenRep_DEPENDENCIES = ../lib/libgigatraj.a

test_PGenGrid_SOURCES = test_PGenGrid.cc test_utils.cc test_utils.hh
test_PGenGrid_DEPENDENCIES = ../lib/libgigatraj.a

test_PGenRnd_SOURCES = test_PGenRnd.cc test_utils.cc test_utils.hh
test_PGenRnd_DEPENDENCIES = ../lib/libgigatraj.a

test_PGenRndDisc_SOURCES = test_PGenRndDisc.cc test_utils.cc test_utils.hh
test_PGenRndDisc_DEPENDENCIES = ../lib/libgigatraj.a

test_PGenDisc_SOURCES = test_PGenDisc.cc test_utils.cc test_utils.hh
test_PGenDisc_DEPENDENCIES = ../lib/libgigatraj.a

test_PGenFile_SOURCES = test_PGenFile.cc test_utils.cc test_utils.hh
test_PGenFile_DEPENDENCIES = ../lib/libgigatraj.a

test_Filter_Null_SOURCES = test_Filter_Null.cc test_utils.cc test_utils.hh
test_Filter_Null_DEPENDENCIES = ../lib/libgigatraj.a

test_Filter_Trop_SOURCES = test_Filter_Trop.cc test_utils.cc test_utils.hh
test_Filter_Trop_DEPENDENCIES = ../lib/libgigatraj.a

test_traj000_SOURCES = test_traj000.cc test_utils.cc test_utils.hh
test_traj000_DEPENDENCIES = ../lib/libgigatraj.a

test_traj001_SOURCES = test_traj001.cc test_utils.cc test_utils.hh
test_traj001_DEPENDENCIES = ../lib/libgigatraj.a

test_traj010_SOURCES = test_traj010.cc test_utils.cc test_utils.hh
test_traj010_DEPENDENCIES = ../lib/libgigatraj.a

check_traj_diffs_SOURCES = check_traj_diffs.cc test_utils.cc test_utils.hh
check_traj_diffs_DEPENDENCIES = ../lib/libgigatraj.a

# We need the following if the build directory is 
# not the same asthe source directory.
# (If they are the same, then the link fails and true is executed.)

test_MPIGrp.sh: test_MPI.sh
	ln -s test_MPI.sh $@  || true

test_BilinearHinterp_MPI.sh: test_MPI.sh
	ln -s test_MPI.sh $@  || true

test_FileLock_MPI.sh: test_MPI.sh
	ln -s test_MPI.sh $@  || true

test_FlockMPI.sh: test_MPI.sh
	ln -s test_MPI.sh $@  || true

test_GridLatLonField3D_MPI.sh: test_MPI.sh
	ln -s test_MPI.sh $@  || true

test_GridFieldProfile_MPI.sh: test_MPI.sh
	ln -s test_MPI.sh $@  || true

test_GridLatLonFieldSfc_MPI.sh: test_MPI.sh
	ln -s test_MPI.sh $@  || true

test_GridFieldDim_MPI.sh: test_MPI.sh
	ln -s test_MPI.sh $@  || true

test_GridFieldDimLon_MPI.sh: test_MPI.sh
	ln -s test_MPI.sh $@  || true

test_MetGridSBRot_MPI.sh: test_MPI.sh
	ln -s test_MPI.sh $@  || true

test_MetSBRot_MPI.sh: test_MPI.sh
	ln -s test_MPI.sh $@  || true

test_NetcdfInMPI.sh: test_MPI.sh
	ln -s test_MPI.sh $@  || true

test_NetcdfOutMPI.sh: test_MPI.sh
	ln -s test_MPI.sh $@  || true

test_StreamPrintMPI.sh: test_MPI.sh
	ln -s test_MPI.sh $@  || true

test_StreamReadMPI.sh: test_MPI.sh
	ln -s test_MPI.sh $@  || true

test_SwarmMPI.sh: test_MPI.sh
	ln -s test_MPI.sh $@  || true

        
