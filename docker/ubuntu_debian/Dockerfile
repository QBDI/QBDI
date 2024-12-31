ARG DOCKER_IMG="ubuntu:latest"

FROM $DOCKER_IMG AS builder

ARG QBDI_ARCH="X86_64"

ENV USER="docker" \
    HOME="/home/docker" \
    PREFIX="/usr" \
    QBDI_PLATFORM="linux"

# setup backport to use cmake >= 3.28
RUN ( cat /etc/debian_version | grep -v -q 12 ) || \
    ( echo 'deb http://deb.debian.org/debian bookworm-backports main contrib non-free' > /etc/apt/sources.list.d/backports.list )

# Get latest package list, upgrade packages, install required packages
# and cleanup to keep container as small as possible
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive \
    apt-get install -y --no-install-recommends \
        adduser \
        bash \
        build-essential \
        cmake \
        g++ \
        git \
        libstdc++-11-dev \
        ninja-build \
        pkg-config \
        wget \
        ca-certificates \
        python3 \
        python3-dev && \
    ( ( cat /etc/debian_version | grep -v -q 12 ) || ( apt-get install -y --no-install-recommends cmake/bookworm-backports ) ) && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# create a user
RUN adduser --disabled-password --gecos '' --home "$HOME" "$USER"

# build / test / install QBDI
ARG CMAKE_ARGUMENT=""

# git archive -o qbdi.tar.gz --prefix=qbdi/ HEAD .
ADD qbdi.tar.gz $HOME/

WORKDIR $HOME/qbdi

RUN chown -R $USER:$USER .

# switch to new user
USER $USER

RUN mkdir build && \
    cd build && \
    cmake -G Ninja \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_CROSSCOMPILING=FALSE \
          -DQBDI_PLATFORM=$QBDI_PLATFORM \
          -DQBDI_ARCH=$QBDI_ARCH \
          -DCMAKE_INSTALL_PREFIX=$PREFIX \
          -DQBDI_TOOLS_PYQBDI=OFF \
          -DQBDI_CCACHE=OFF \
          $CMAKE_ARGUMENT \
          ../ && \
    ninja && \
    # test
    ./test/QBDITest && \
    # create package and install
    rm -f QBDI-*-$QBDI_PLATFORM.deb && \
    cpack -G DEB


FROM $DOCKER_IMG

WORKDIR /root

COPY --from=builder /home/docker/qbdi/build/*.deb .
RUN apt-get update && \
    apt-get install -y ./*.deb && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

CMD ["/bin/bash"]
