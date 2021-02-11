ARG DOCKER_IMG="ubuntu:latest"

FROM $DOCKER_IMG as builder

ARG QBDI_ARCH="X86_64"

ENV USER="docker" \
    HOME="/home/docker" \
    PREFIX="/usr" \
    QBDI_PLATFORM="linux"

# Get latest package list, upgrade packages, install required packages
# and cleanup to keep container as small as possible
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive \
    apt-get install -y --no-install-recommends \
        bash \
        sudo \
        build-essential \
        cmake \
        g++ \
        g++-multilib \
        git \
        libstdc++-8-dev \
        ninja-build \
        pkg-config \
        wget \
        ca-certificates \
        zlib1g-dev \
        python3 \
        python3-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# create a user
RUN adduser --disabled-password --gecos '' $USER && \
    adduser $USER sudo && \
    echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# switch to new user
USER $USER

# install QBDI compilation dependencies

# git archive -o qbdi-deps.tar.gz --prefix=qbdi-deps/ HEAD deps
ADD qbdi-deps.tar.gz $HOME/

WORKDIR $HOME

RUN sudo chown -R $USER:$USER .

WORKDIR $HOME/qbdi-deps/deps/llvm/

RUN python3 build.py prepare ${QBDI_PLATFORM}-${QBDI_ARCH} && \
    python3 build.py build ${QBDI_PLATFORM}-${QBDI_ARCH} OFF && \
    python3 build.py package ${QBDI_PLATFORM}-${QBDI_ARCH} && \
    python3 build.py clean ${QBDI_PLATFORM}-${QBDI_ARCH}

# build / test / install QBDI
ARG CMAKE_ARGUMENT=""

# git archive -o qbdi.tar.gz --prefix=qbdi/ HEAD .
ADD qbdi.tar.gz $HOME/

WORKDIR $HOME/qbdi

RUN sudo chown -R $USER:$USER . && \
    # link llvm
    rm -rf deps && \
    ln -s $HOME/qbdi-deps/deps deps && \
    # build
    mkdir build && \
    cd build && \
    cmake -G Ninja \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_CROSSCOMPILING=FALSE \
          -DQBDI_PLATFORM=$QBDI_PLATFORM \
          -DQBDI_ARCH=$QBDI_ARCH \
          -DCMAKE_INSTALL_PREFIX=$PREFIX \
          -DQBDI_TOOLS_PYQBDI=ON \
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
