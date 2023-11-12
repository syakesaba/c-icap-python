FROM ubuntu:22.04

RUN set -ex && apt install -y make gcc libicapapi-dev python3-dev libssl-dev graphviz doxygen
