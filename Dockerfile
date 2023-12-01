FROM ubuntu:22.04
ARG pv

WORKDIR /
COPY ./. /

RUN set -ex \
&& apt update -y \
&& apt install -y make gcc libicapapi-dev python${pv}-dev libssl-dev graphviz doxygen  c-icap squid-openssl \
&& make all doc flow

# Open squid port
EXPOSE 3128

# Run
RUN chmod +x /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]