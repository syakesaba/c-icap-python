#!/bin/bash
# encoding: utf-8

# c-icap

C_ICAP_CONF="/etc/c-icap/c-icap.conf"
C_ICAP_MODULES_DIR="/usr/lib/x86_64-linux-gnu/c_icap"

cp -f libc-icap-python.so $C_ICAP_MODULES_DIR/libc-icap-python.so
cp -f srv_echo.py $C_ICAP_MODULES_DIR/srv_echo.py

echo "#=== begin of added config ===" >> $C_ICAP_CONF
echo "Module service_handler libc-icap-python.so" >> $C_ICAP_CONF
echo "Service srv_python srv_echo.py" >> $C_ICAP_CONF
echo "#=== end of added config ===" >> $C_ICAP_CONF
cat $C_ICAP_CONF | grep "#=== end of added config== =" -A1000 > /dev/null #fflush()
c-icap -D -d 10 -f $C_ICAP_CONF

# squid

SQUID_CONF="/etc/squid/squid.conf"
SQUID_SSL_DIR="/etc/squid/ssl"
SQUID_LIB_DIR="/usr/lib/squid"

mkdir -p $SQUID_SSL_DIR
openssl req -new -newkey rsa:2048 -nodes -days 3650 -x509 -keyout $SQUID_SSL_DIR/myCA.pem -out $SQUID_SSL_DIR/myCA.crt -subj "/C=JP/ST=Ikebukuro/L=Tokyo/O=Dollers/OU=Dollers Co.,Ltd./CN=squid.local"
openssl x509 -in $SQUID_SSL_DIR/myCA.crt -outform DER -out $SQUID_SSL_DIR/myCA.der
$SQUID_LIB_DIR/security_file_certgen -c -s $SQUID_SSL_DIR/ssl_db -M 4MB
echo "#==== begin of added config ===" >> $SQUID_CONF
echo "always_direct allow all" >> $SQUID_CONF
echo "icap_service_failure_limit -1" >> $SQUID_CONF
echo "ssl_bump bump all" >> $SQUID_CONF
echo "sslproxy_cert_error allow all" >> $SQUID_CONF
sed "/^http_port 3128$/d" -i $SQUID_CONF
sed "s/^http_access allow localhost$/http_access allow all/" -i $SQUID_CONF
echo "http_port 3128 ssl-bump generate-host-certificates=on dynamic_cert_mem_cache_size=4MB cert=$SQUID_SSL_DIR/myCA.crt key=$SQUID_SSL_DIR/myCA.pem" >> $SQUID_CONF
echo "sslcrtd_program $SQUID_LIB_DIR/security_file_certgen -s $SQUID_SSL_DIR/ssl_db -M 4MB" >> $SQUID_CONF
echo "icap_enable on" >> $SQUID_CONF
echo "icap_preview_enable on" >> $SQUID_CONF
echo "icap_preview_size 1024" >> $SQUID_CONF
echo "icap_206_enable on" >> $SQUID_CONF
echo "icap_persistent_connections on" >> $SQUID_CONF
echo "adaptation_send_client_ip off" >> $SQUID_CONF
echo "adaptation_send_username off" >> $SQUID_CONF
echo "icap_service srv_echo_req reqmod_precache icap://127.0.0.1:1344/echo"  >> $SQUID_CONF
echo "icap_service srv_echo_resp respmod_precache icap://127.0.0.1:1344/echo" >> $SQUID_CONF
#echo "adaptation_service_set svc_echo_req_set srv_echo_req" >> $SQUID_CONF
#echo "adaptation_service_set svc_echo_resp_set srv_echo_resp">> $SQUID_CONF
echo "adaptation_service_chain svc_echo_req_chain srv_echo_req" >> $SQUID_CONF
echo "adaptation_service_chain svc_echo_resp_chain srv_echo_resp">> $SQUID_CONF
echo "adaptation_access svc_echo_req_chain allow all">> $SQUID_CONF
echo "adaptation_access svc_echo_resp_chain allow all">> $SQUID_CONF
echo "#==== end of added config ===" >> $SQUID_CONF
cat $SQUID_CONF | grep "#==== end of added config ===" -A1000 > /dev/null #fflush()
squid -d 10 -f $SQUID_CONF

bash
