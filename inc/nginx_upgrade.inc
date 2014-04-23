checkmap() {
MAPCHECK=$(grep '/usr/local/nginx/conf/fastcgi_param_https_map.conf' /usr/local/nginx/conf/nginx.conf)

if [[ -z "$MAPCHECK" ]]; then
	sed -i 's/http {/http { \ninclude \/usr\/local\/nginx\/conf\/fastcgi_param_https_map.conf;/g' /usr/local/nginx/conf/nginx.conf
fi

if [[ ! -f /usr/local/nginx/conf/fastcgi_param_https_map.conf ]]; then
	\cp $CUR_DIR/config/nginx/fastcgi_param_https_map.conf /usr/local/nginx/conf/fastcgi_param_https_map.conf
fi

if [[ -z "$(grep 'fastcgi_param HTTPS $server_https;' /usr/local/nginx/conf/php.conf)" ]]; then
	replace -s '#fastcgi_param HTTPS on;' 'fastcgi_param HTTPS $server_https;' -- /usr/local/nginx/conf/php.conf
fi
}

checknginxmodules() {

cecho "Check for missing nginx modules" $boldyellow
if [[ ! -f "${DIR_TMP}/ngx-fancyindex-0.3.1.tar.gz" || ! -f "${DIR_TMP}/ngx_cache_purge-2.1.tar.gz" || ! -f "${DIR_TMP}/Nginx-accesskey-2.0.3.tar.gz" || ! -f "${DIR_TMP}/nginx-http-concat-master.tar.gz" || ! -f "${DIR_TMP}/openssl-${OPENSSL_VERSION}.tar.gz" || ! -f "${DIR_TMP}/pcre-${NGINX_PCREVER}.tar.gz" || ! -f "${DIR_TMP}/nginx-dav-ext-module-${NGINX_EXTWEBDAVVER}.tar.gz" || ! -d "${DIR_TMP}/release-${NGXPGSPEED_VER}" ]]; then
ngxmoduletarball
openssldownload
fi

if [[ "$NGINX_PAGESPEEDGITMASTER" = [yY] ]]; then
    # if option to download official github based master ngx_pagespeed
    # remove old version downloaded & download master tarball instead
    cd $DIR_TMP
    rm -rf release-${NGXPGSPEED_VER}*
    nginxpgspeedtarball
fi

}

function funct_nginxupgrade {

checkmap

cecho "**********************************************************************" $boldyellow
cecho "* Nginx Update script - Included in Centmin Extras" $boldgreen
cecho "* Version: $SCRIPT_VERSION - Date: $SCRIPT_DATE - $COPYRIGHT" $boldgreen
cecho "**********************************************************************" $boldyellow
echo " "
cecho "This software comes with no warranty of any kind. You are free to use" $boldyellow
cecho "it for both personal and commercial use as licensed under the GPL." $boldyellow
echo " "
read -ep "Nginx Upgrade - Would you like to continue? [y/n] " nukey

if [[ "$nukey" = [nN] ]];
then
    exit 0
fi

DIR_TMP="/svr-setup"
if [ ! -d "$DIR_TMP" ]; then
mkdir /svr-setup
fi

funct_mktempfile

if [ ! -f /etc/init.d/nginx ]; then
    cp $CUR_DIR/init/nginx /etc/init.d/nginx
    chmod +x /etc/init.d/nginx
    chkconfig --levels 235 nginx on
fi

echo ""
read -ep "Install which version of Nginx? (version i.e. $NGINX_VERSION}): " ngver

# Backup Nginx CONF
if [ "$NGINXBACKUP" == 'y' ]; then
	nginxbackup
fi

# Backup ngx_pagespeed pagespeed.conf
if [[ "$NGINX_PAGESPEED" = [yY] ]]; then
	if [[ -f /usr/local/nginx/conf/pagespeed.conf ]]; then
		pagespeedbackup
	fi
fi

# tasks for updated ngx_pagespeed module parity
pagespeeduptasks

    echo "*************************************************"
    cecho "* Updating nginx" $boldgreen
    echo "*************************************************"

    cd $DIR_TMP

    # nginx Modules / Prerequisites
	cecho "Installing nginx Modules / Prerequisites..." $boldgreen

checknginxmodules

if [[ "$GPERFTOOLS_SOURCEINSTALL" = [yY] ]]; 
then
    echo "*************************************************"
    cecho "* Source Upgrade Google Perftools" $boldgreen
    echo "*************************************************"

    # Install libunwind
    echo "Compiling libunwind..."
    if [ -s libunwind-${LIBUNWIND_VERSION}.tar.gz ]; then
        cecho "libunwind ${LIBUNWIND_VERSION} Archive found, skipping download..." $boldgreen 
    else
        $DOWNLOADAPP http://download.savannah.gnu.org/releases/libunwind/libunwind-${LIBUNWIND_VERSION}.tar.gz $WGETRETRY
    fi

    tar xvzf libunwind-${LIBUNWIND_VERSION}.tar.gz
    cd libunwind-${LIBUNWIND_VERSION}
    make clean
    ./configure
    make$MAKETHREADS
    make install

    # Install google-perftools
    cd $DIR_TMP

    echo "Compiling google-perftools..."
    if [ -s google-perftools-${GPERFTOOLS_VERSION}.tar.gz ]; then
        cecho "google-perftools ${GPERFTOOLS_VERSION} Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://google-perftools.googlecode.com/files/google-perftools-${GPERFTOOLS_VERSION}.tar.gz $WGETRETRY
    fi

    tar xvzf google-perftools-${GPERFTOOLS_VERSION}.tar.gz
    cd google-perftools-${GPERFTOOLS_VERSION}
    make clean
    ./configure --enable-frame-pointers
    make$MAKETHREADS
    make install
    #echo "/usr/local/lib" > /etc/ld.so.conf.d/usr_local_lib.conf
    #/sbin/ldconfig

fi # GPERFTOOL_SOURCEINSTALL

echo ""
read -ep "Do you want to recompile OpenSSL ? Only needed if you updated OpenSSL [y/n]: " recompileopenssl 
echo ""

if [ "$recompileopenssl" == 'y' ]; then

if [[ "$CENTOSVER" = '5.3' || "$CENTOSVER" = '5.4' || "$CENTOSVER" = '5.5' || "$CENTOSVER" = '5.6' || "$CENTOSVER" = '5.7' || "$CENTOSVER" == '5.8' || "$CENTOSVER" == '5.9' ]]; then
    cd $DIR_TMP

    echo "Compiling OpenSSL..."
    if [ -s openssl-${OPENSSL_VERSION}.tar.gz ]; then
        cecho "openssl ${OPENSSL_VERSION} Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://www.openssl.org/source/openssl-${OPENSSL_VERSION}.tar.gz $WGETRETRY
    fi

    tar xvzf openssl-${OPENSSL_VERSION}.tar.gz
fi # for nginx openssl

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' ]]; then
    # Install OpenSSL
    cd $DIR_TMP
    echo "Compiling OpenSSL..."
    if [ -s openssl-${OPENSSL_VERSION}.tar.gz ]; then
        cecho "openssl ${OPENSSL_VERSION} Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP http://www.openssl.org/source/openssl-${OPENSSL_VERSION}.tar.gz $WGETRETRY
    fi

    tar xvzf openssl-${OPENSSL_VERSION}.tar.gz
    cd openssl-${OPENSSL_VERSION}
    make clean
    ./config --prefix=/usr/local --openssldir=/usr/local/ssl
    make
    make install

    ./config shared enable-tlsext --prefix=/usr/local --openssldir=/usr/local/ssl
    make clean
    make
    make install
fi # openssl centos 6

fi # recompileopenssl

if [[ "$PCRE_SOURCEINSTALL" = [yY] ]]; 
then
    echo "*************************************************"
    cecho "* Source Install PCRE" $boldgreen
    echo "*************************************************"

    # Install PCRE
    cd $DIR_TMP

    echo "Compiling PCRE..."
    if [ -s pcre-${PCRE_VERSION}.tar.gz ]; then
        cecho "pcre ${PCRE_VERSION} Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre-${PCRE_VERSION}.tar.gz $WGETRETRY
    fi

    tar xvzf pcre-${PCRE_VERSION}.tar.gz
    cd pcre-${PCRE_VERSION}
    make clean
    ./configure
    make$MAKETHREADS
    make install

fi

if [ "$NGINX_OPENRESTY" == 'y' ]; then
funct_nginxmodules_openresty
else
funct_nginxmodules
fi

    # Install nginx
    cd $DIR_TMP

    echo "Compiling nginx..."
    if [ -s nginx-${ngver}.tar.gz ]; then
        cecho "nginx ${ngver} Archive found, skipping download..." $boldgreen
    else
        $DOWNLOADAPP "http://nginx.org/download/nginx-${ngver}.tar.gz" $WGETRETRY
    fi

    tar xvfz nginx-${ngver}.tar.gz
    cd nginx-${ngver}
    make clean

if [[ "$NGINXPATCH" = [yY] ]]; then

    echo "*************************************************"
    cecho "Nginx Patch Time - 60 second delay" $boldgreen
    cecho "to allow you to patch files" $boldgreen
    echo "*************************************************"
    sleep 60
fi

if [ "$NGINX_OPENRESTY" == 'y' ]; then
funct_nginxconfigure_openresty
else
funct_nginxconfigure
fi

################
# error check

	ERR=$?
	if [ $ERR != 0 ]; then
	NOTIFY_MESSAGE="\n`date`\nError: $ERR, Nginx configure failed\n"
	echo -e "$NOTIFY_MESSAGE" >> "$TMP_MSGFILE"

funct_showtempfile
	cleanup_msg

	exit
	
	else

	NOTIFY_MESSAGE="\n`date`\nSuccess: Nginx configure ok\n"
	echo -e "$NOTIFY_MESSAGE" >> "$TMP_MSGFILE"

funct_showtempfile
	cleanup_msg

	fi

# error check
################

    make

################
# error check

	ERR=$?
	if [ $ERR != 0 ]; then
	NOTIFY_MESSAGE="\n`date`\nError: $ERR, Nginx make failed\n"
	echo -e "$NOTIFY_MESSAGE" >> "$TMP_MSGFILE"

funct_showtempfile
	cleanup_msg

	exit
	
	else

	NOTIFY_MESSAGE="\n`date`\nSuccess: Nginx make ok\n"
	echo -e "$NOTIFY_MESSAGE" >> "$TMP_MSGFILE"

funct_showtempfile
	cleanup_msg

	fi

# error check
################

/etc/init.d/nginx stop
sleep $NGINXUPGRADESLEEP

NGINXPSCHECK=`ps --no-heading -C nginx`

if [ ! -z "$NGINXPSCHECK" ]; then
echo ""
echo "nginx seems to be still running, trying to stop it again..."
echo ""
/etc/init.d/nginx stop
sleep $NGINXUPGRADESLEEP
fi

    make install

################
# error check

	ERR=$?
	if [ $ERR != 0 ]; then
	NOTIFY_MESSAGE="\n`date`\nError: $ERR, Nginx wasn't installed properly\n"
	echo -e "$NOTIFY_MESSAGE" >> "$TMP_MSGFILE"

funct_showtempfile
	cleanup_msg

	exit
	
	else

	NOTIFY_MESSAGE="\n`date`\nSuccess: Nginx was installed properly\n"
	echo -e "$NOTIFY_MESSAGE" >> "$TMP_MSGFILE"

    /etc/init.d/nginx start

funct_showtempfile
	cleanup_msg

CBODYCHECK=`grep 'client_body_in_file_only on' /usr/local/nginx/conf/nginx.conf`
if [ $CBODYCHECK ]; then
sed -i 's/client_body_in_file_only on/client_body_in_file_only off/g' /usr/local/nginx/conf/nginx.conf
fi

    echo "*************************************************"
    cecho "* nginx updated" $boldgreen
    echo "*************************************************"

	fi

# error check
################

echo " "

if [[ "$ENABLE_MENU" != [yY] ]]; then

ASK "Do would you like to run script cleanup (Highly recommended) ? [y/n] "
if [[ "$key" = [yY] ]];
then
    rm -rf /svr-setup
    echo "Temporary files/folders removed"
fi

ASK "Do you want to delete this script ? [y/n] "
if [[ "$key" = [yY] ]];
then
    echo "*************************************************"
    cecho "* Deleting Centmin script... " $boldgreen
    echo "*************************************************"
    echo "Removing..."

rm -f $0

    echo "*************************************************"
    cecho "* Centmin script deleted" $boldgreen
    echo "*************************************************"
fi

fi

}