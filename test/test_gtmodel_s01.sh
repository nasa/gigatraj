#!/bin/sh

# Runs parcels forwarda and backwards for five days,

VALGRIND=""
while getopts v: ARG
do
    case "${ARG}" in
    v) VALGRIND="valgrind ${OPTARG}";;
    \?) "Bad Option ${ARG}" >&2
        exit 1;;
    esac
done
shift `expr ${OPTIND} - 1`



#METSRC="MERRA"
#METSRC="MERRA2"
#METSRC="GEOSfp"
METSRC="GridSBRot"

GVERT="Theta"

#GCACHE="/misc/lrl02/lait/metcache/"
GCACHE="${TMPDIR}gigatraj_metcache/"

case "${METSRC}" in
"MERRA")
    BEGDATE="2014-03-20T06:30:00"
    ENDDATE="2014-03-25T06:30:00"
    ;;
"MERRA2")    
    BEGDATE="2018-03-20T06:30:00"
    ENDDATE="2018-03-25T06:30:00"
    ;;
"GEOSfp")    
    BEGDATE="2018-03-20T06:30:00"
    ENDDATE="2018-03-25T06:30:00"
    ;;
"GridSBRot")    
    BEGDATE="2018-03-20T06:30:00"
    ENDDATE="2018-03-25T06:30:00"
    ;;
esac

IFILE="gtmodel_s01_input.txt"

FMT="%t, %i, %10.2o, %10.2a, %v"

OFILE="gtmodel_s01_output.txt"

CFILE="gtmodel_s01_original.txt"

if [ "x${srcdir}" != "x" ] ; then
   IFILE="${srcdir}/${IFILE}"
   CFILE="${srcdir}/${CFILE}"
fi

BDAY=`echo "${BEGDATE}" | cut -c1-10`T00:00
EDAY=`echo "${ENDDATE}" | cut -c1-10`T21:00


${VALGRIND}   ../src/gtmodel_s01 \
      --begdate ${BEGDATE} \
      --enddate ${ENDDATE} \
      --zerodate "${BEGDATE}" \
      --source Met${METSRC} \
      --iso \
      --metspacinghr 6 \
      --format "${FMT}" \
      --verbose \
      -f 0.24 \
      --parcels ${IFILE} \
      > ${OFILE}
    OK=$?

if [ ${OK} != 0 ] ; then
   echo "gtmodel_s01 failed: $OK" >&2
   exit 1
fi

#if cmp ${CFILE} ${OFILE} ; then
if ./check_traj_diffs ${CFILE} ${OFILE} ; then
   echo "Files match" >&2
   /bin/rm -f "${OFILE}"
   exit 0
else
   echo "Files do NOT match" >&2
   exit 1
fi
