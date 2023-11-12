INSTALL
=====

# libc-icap-python.so

```sh
apt install make gcc python3-dev libicap-api libssl-dev
make all
```

# c-icap.conf

```conf
Module service_handler srv_python.so
Service pyecho srv_echo.py
```

DOC
=====

# doc

```sh
apt install doxygen
make doc
# ls ./doc
```

# flow

```sh
apt install graphviz
make flow
# ls ./flow.jpg
```
