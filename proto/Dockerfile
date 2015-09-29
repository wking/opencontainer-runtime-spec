FROM fedora:latest

ENV PROTO_VERSION 3.0.0-beta-1
RUN dnf install -y wget tar autoconf automake make gcc gcc-c++ unzip libtool
RUN wget https://github.com/google/protobuf/archive/v${PROTO_VERSION}.tar.gz && \
    tar xf v${PROTO_VERSION}.tar.gz
WORKDIR protobuf-${PROTO_VERSION}
RUN ./autogen.sh && ./configure
RUN make -j$(grep ^proc /proc/cpuinfo | wc -l)
RUN make install -j$(grep ^proc /proc/cpuinfo | wc -l)
WORKDIR /
RUN dnf install -y golang git hg bzr
ENV GOPATH /gopath
ENV GOBIN /usr/bin
RUN go get -u github.com/golang/protobuf/{proto,protoc-gen-go}
WORKDIR protobuf-${PROTO_VERSION}
RUN protoc --go_out=${GOPATH} src/google/protobuf/any.proto
WORKDIR /
VOLUME /proto /output
CMD bash /proto/run.sh
