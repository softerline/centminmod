###############################################
yumcentosfivedownload() {

if [[ "$CENTOSVER" = '5.3' || "$CENTOSVER" = '5.4' || "$CENTOSVER" = '5.5' || "$CENTOSVER" = '5.6' || "$CENTOSVER" = '5.7' || "$CENTOSVER" == '5.8' || "$CENTOSVER" == '5.9' || "$CENTOSVER" == '5.10' ]]; then
{
    cd $DIR_TMP

if [ -s "${CENTOSFIVEEPELFILE}" ]; then
  echo "${CENTOSFIVEEPELFILE} [found]"
  else
  echo "Error: ${CENTOSFIVEEPELFILE} not found !!! Download now......"
  wget ${WGETOPT} ${CENTOSFIVEEPEL} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${CENTOSFIVEEPELFILE} download failed." $boldgreen
FAILEPEL='y'
checklogdetails
	exit #$ERROR
else 
	cecho "Download done." $boldyellow
	rpm -i ${CENTOSFIVEEPELFILE}
	yum${CACHESKIP} -q list ccache 2>&1 >/dev/null
	ERR=$?
	CCEPELCHECK="$ERR"
		if [ "$CCEPELCHECK" == '0' ]; then
			yumpriorepel
			echo ""
		else
			cecho "Error: problem with ${CENTOSFIVEEPELFILE} install." $boldgreen
			exit $ERR
		fi
	fi

fi

if [[ -f /etc/yum.repos.d/rpmforge.repo || -f /etc/yum.repos.d/rpmforge.OLD ]]; then
	rm -rf /etc/yum.repos.d/rpmforge.repo
	rm -rf /etc/yum.repos.d/rpmforge.OLD
	rm -rf ${DIR_TMP}/rpmforge-release*

	FORGERPM=$(rpm -qa | grep rpmforge-release)
	rpm -e $FORGERPM
fi

if [ -s "${CENTOSFIVERPMFORGEFILE}" ]; then
  echo "${CENTOSFIVERPMFORGEFILE} [found]"
  else
  echo "Error: ${CENTOSFIVERPMFORGEFILE} not found !!! Download now......"
  wget ${WGETOPT} ${CENTOSFIVERPMFORGE} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${CENTOSFIVERPMFORGEFILE} download failed." $boldgreen
FAILRPMFORGE='y'
checklogdetails
	#exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm --import http://apt.sw.be/RPM-GPG-KEY.dag.txt
rpm -i ${CENTOSFIVERPMFORGEFILE}
	yum${CACHESKIP} -q list axel 2>&1 >/dev/null
	ERR=$?
	CCRPMFORGECHECK="$ERR"
		if [ "$CCRPMFORGECHECK" == '0' ]; then
			yumpriorrpmforge
			echo ""
		else
			cecho "Error: problem with ${CENTOSFIVERPMFORGEFILE} install." $boldgreen
			#exit $ERR
		fi
	fi

fi


if [[ "$CENTALTREPO_DISABLE" = [nN] ]]; then
if [ -s ${CENTOSFIVECENTALTFILE} ]; then
  echo "${CENTOSFIVECENTALTFILE} [found]"
  else
  echo "Error: ${CENTOSFIVECENTALTFILE} not found !!! Download now......"
  wget ${WGETOPT} ${CENTOSFIVECENTALT} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${CENTOSFIVECENTALTFILE} download failed." $boldgreen
FAILCENTALT='y'
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm --import http://centos.alt.ru/pub/repository/centos/RPM-GPG-KEY-CentALT
rpm -i ${CENTOSFIVECENTALTFILE}
yumpriorcentalt
echo ""
	fi

fi
fi # CENTALTREPO_DISABLE

if [ -s "${CENTOSFIVEPERCONARELEASEFILE}" ]; then
  echo "${CENTOSFIVEPERCONARELEASEFILE} [found]"
  else
  echo "Error: ${CENTOSFIVEPERCONARELEASEFILE} not found !!! Download now......"
  wget ${WGETOPT} ${CENTOSFIVEPERCONARELEASE} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${CENTOSFIVEPERCONARELEASEFILE} download failed." $boldgreen
FAILPERCONA='y'
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm --import http://www.percona.com/downloads/RPM-GPG-KEY-percona
rpm -i ${CENTOSFIVEPERCONARELEASEFILE}
	#yum -q list axel 2>&1 >/dev/null
	ERR=$?
	CCPERCONACHECK="$ERR"
		if [ "$CCPERCONACHECK" == '0' ]; then
			yumpriorpercona
			echo ""
		else
			cecho "Error: problem with ${CENTOSFIVEPERCONARELEASEFILE} install." $boldgreen
			exit $ERR
		fi
	fi

fi

if [ -s "${CENTOSFIVEVARNISHFILE}" ]; then
  echo "${CENTOSFIVEVARNISHFILE} [found]"
  else
  echo "Error: ${CENTOSFIVEVARNISHFILE} not found !!! Download now......"
  wget ${WGETOPT} ${CENTOSFIVEVARNISH} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${CENTOSFIVEVARNISHFILE} download failed." $boldgreen
FAILVARNISH='y'
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow

rpm --nosignature -i ${CENTOSFIVEVARNISHFILE}
	#yum -q list axel 2>&1 >/dev/null
	ERR=$?
	CCVARNISHCHECK="$ERR"
		if [ "$CCVARNISHCHECK" == '0' ]; then
			yumpriorvarnish
			echo ""
		else
			cecho "Error: problem with ${CENTOSFIVEVARNISHFILE} install." $boldgreen
			exit $ERR
		fi
	fi

fi

####

} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_yuminstall_centos5.log
fi # CENTOS 5.x

}