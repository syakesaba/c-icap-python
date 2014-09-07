INSTALL
=====

---

### c-icap 0.3.4
See: [official install documentation](http://c-icap.sourceforge.net/install.html)
#### compile (linux,mac)
```sh
./configure --enable-large-files --enable-ipv6 --enable-lib-compat --prefix=/usr/local/c-icap;
make install;
make doc;
```
#### c-icap.conf
append
```conf
Module service_handler srv_python.so
Service pyecho srv_echo.py
```

---

### squid
See: [official configuration documentation](http://wiki.squid-cache.org/Features/ICAP)
#### compile
##### *nix
```sh
./configure --prefix=/usr/local/squid --enable-icap-client --enable-ssl --enable-ssl-crtd --with-large-files;
make install;
```

##### MAC OS X(brew)
```sh
brew edit squid
```
and append options to args
```ruby
    args = %W[
      ***snip!***
      --prefix=/usr/local/squid
      --enable-ssl
      --enable-ssl-crtd
      --enable-icap-client
    ]
```conf

#### squid.conf
append
```conf
icap_enable on
icap_preview_enable on
icap_service echo reqmod_precache 1 icap://127.0.0.1:1344/echo
adaptation_service_set service_reqmod echo # echo2
adaptation_access service_reqmod allow all
icap_service echo respmod_precache 1 icap://127.0.0.1:1344/echo
adaptation_service_set service_respmod echo # echo2
adaptation_access service_respmod allow all
```

##### for SSL-BUMP
replace http_port config
```conf
http_port 3128 ssl-bump generate-host-certificates=on dynamic_cert_mem_cache_size=4MB cert=/usr/local/squid/ssl_cert/myCA.pem
```
append
```conf
always_direct allow all
ssl_bump client-first all
sslproxy_cert_error allow all
sslproxy_flags DONT_VERIFY_PEER
```
execute
```sh
mkdir  /usr/local/squid/ssl_cert
cd  /usr/local/squid/ssl_cert
openssl req -new -newkey rsa:1024 -days 3650 -nodes -x509 -keyout myCA.pem -out myCA.pem;
openssl x509 -in myCA.pem  -outform DER -out myCA.der
mkdir /usr/local/squid/ssl_db
/usr/local/squid/libexec/ssl_crtd -c -s /usr/local/squid/ssl_db
```

### c-icap-python
#### libpython(Python.h)
choose some...(depends your env)
```sh
apt-get install libpython-dev
yum install python-devel
pacman -S libpython-dev
ln -s /System/Library/Frameworks/Python.framework/Headers /usr/local/include/Python
```
#### libicapapi
```sh
ln -s /usr/local/c-icap/lib/libicapapi.{so,dylib,dll} /usr/local/lib/
ln -s /usr/local/c-icap/include/c_icap /usr/local/include/c_icap
```
#### compile
```sh
make all
```