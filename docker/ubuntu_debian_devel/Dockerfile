ARG DOCKER_IMG="ubuntu:latest"

FROM $DOCKER_IMG

ARG QBDI_PLATFORM="linux-X86_64"

ENV USER="docker" \
    HOME="/home/docker" \
    PREFIX="/usr"

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
        libstdc++-8-dev \
        make \
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

WORKDIR $HOME/qbdi-deps/deps/gtest/$QBDI_PLATFORM

RUN sh build.sh prepare && \
    sh build.sh build && \
    sh build.sh package && \
    sh build.sh clean

WORKDIR $HOME/qbdi-deps/deps/llvm/

RUN python3 build.py prepare $QBDI_PLATFORM && \
    python3 build.py build $QBDI_PLATFORM && \
    python3 build.py package $QBDI_PLATFORM && \
    python3 build.py clean $QBDI_PLATFORM

# install validator dependency after build llvm to use the same cache between devel and release image
RUN sudo apt-get update && \
    sudo apt-get install -y --no-install-recommends \
        python3-yaml \
        imagemagick \
        zip \
        less \
        file \
        xxd \
        git && \
    sudo apt-get clean  && \
    sudo rm -rf /var/lib/apt/lists/*

# build / test / install QBDI
ARG CMAKE_ARGUMENT=""

# git archive -o qbdi.tar.gz --prefix=qbdi/ HEAD .
ADD qbdi.tar.gz $HOME/

WORKDIR $HOME/qbdi

RUN sudo chown -R $USER:$USER . && \
    # link llvm and gtest
    rm -rf deps && \
    ln -s $HOME/qbdi-deps/deps deps && \
    # build
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_CROSSCOMPILING=FALSE \
          -DPLATFORM=$QBDI_PLATFORM \
          -DCMAKE_INSTALL_PREFIX=$PREFIX \
          -DLOG_DEBUG=TRUE \
          -DEXAMPLES=TRUE \
          -DTOOLS_PYQBDI=TRUE \
          -DTOOLS_VALIDATOR=TRUE \
          $CMAKE_ARGUMENT \
          ../ && \
    make -j4 && \
    # test
    ./test/QBDITest && \
    # create package and install
    rm -f QBDI-*-$QBDI_PLATFORM.deb && \
    cpack -G DEB && \
    sudo dpkg -i QBDI-*-$QBDI_PLATFORM.deb

WORKDIR $HOME

CMD ["/bin/bash"]
