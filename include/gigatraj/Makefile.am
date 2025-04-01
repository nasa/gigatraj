
# Note:
#   In the lists that follow, each header file is placed on its own line.
#   Classes are grouped together, and each class's header is
#   indented by its level of class inheritance.

pkginclude_HEADERS = gigatraj.hh \
                   RandomSrc.hh \
                   FilePath.hh \
                   FileLock.hh \
                   CalGregorian.hh \
                   PlanetNav.hh \
                    PlanetSphereNav.hh \
                     Earth.hh \
                   ProcessGrp.hh \
                    SerialGrp.hh \
                    MPIGrp.hh \
                   GridField.hh \
                    GridFieldDim.hh \
                    GridFieldDimLon.hh \
                    GridFieldProfile.hh \
                    GridFieldSfc.hh \
                    GridField3D.hh \
                   Interpolator.hh \
                    Vinterp.hh \
                     LinearVinterp.hh \
                     LogLinearVinterp.hh \
                    Hinterp.hh \
                     HLatLonInterp.hh \
                      BilinearHinterp.hh \
                   Integrator.hh \
                    IntegRK4.hh \
                    IntegRK4a.hh \
                   Catalog.hh \
                   MetData.hh \
                    MetSBRot.hh \
                    MetOnTheFly.hh \
                       ThetaOTF.hh \
                       TropOTF.hh \
                       PressOTF.hh \
                       PAltOTF.hh \
                       PAltDotOTF.hh \
                       DensOTF.hh \
                       ThetaDotOTF.hh \
                       SZAOTF.hh \
                    MetGridData.hh \
                       MetGridLatLonData.hh \
                          MetGridSBRot.hh 


# should be included in the list only once
# but for any of these conditionals.
if GEOSFP
   pkginclude_HEADERS +=  MetMyGEOS.hh
else
if MERRA2
   pkginclude_HEADERS +=  MetMyGEOS.hh
else
if MERRA
   pkginclude_HEADERS +=  MetMyGEOS.hh
endif
endif
endif

if MERRA
   pkginclude_HEADERS +=  MetMERRA.hh 
endif
if MERRA2
   pkginclude_HEADERS +=  MetMERRA2.hh 
endif
if GEOSFP
   pkginclude_HEADERS +=  MetGEOSfp.hh
   pkginclude_HEADERS +=  MetGEOSfpFcast.hh
   pkginclude_HEADERS +=  MetGEOSfpAssim.hh
endif
if MYGEOS
   pkginclude_HEADERS +=  MetMyGEOS.hh 
endif
   pkginclude_HEADERS +=  MetSelector.hh 

pkginclude_HEADERS += \
                   Parcel.hh \
                   Flock.hh \
                   Swarm.hh \
                   trace.hh \
                   ParcelGenerator.hh \
                    PGenRep.hh \
                    PGenRnd.hh \
                    PGenRndDisc.hh \
                    PGenDisc.hh \
                    PGenGrid.hh \
                    PGenLine.hh \
                    PGenRndLine.hh \
                    PGenFile.hh \
                   ParcelFilter.hh \
                    ParcelInitializer.hh \
                     StreamLoad.hh \
                     StreamRead.hh \
                    ParcelReporter.hh \
                     StreamDump.hh \
                     StreamPrint.hh 
