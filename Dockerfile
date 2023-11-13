FROM ubuntu:22.04

WORKDIR /opt/app
COPY ./. /opt/app

RUN set -ex && apt update -y && apt install -y make gcc libicapapi-dev python3-dev libssl-dev graphviz doxygen  c-icap squid-openssl && make all doc flow

# Open squid port
EXPOSE 3128

# Run
RUN chmod +x /opt/app/entrypoint.sh
ENTRYPOINT ["/opt/app/entrypoint.sh"]

