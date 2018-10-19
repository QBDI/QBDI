FROM ubuntu:16.04
MAINTAINER Charles Hubain <chubain@quarkslab.com>

ARG QBDI_BUILD_TYPE=Release

ENV USER docker
ENV HOME /home/$USER
ENV PREFIX /usr
ENV QBDI_PLATFORM linux-X86_64
ENV CMAKE_MAJOR 3.11
ENV CMAKE_VERSION 3.11.4

# Get latest package list, upgrade packages, install required packages 
# and cleanup to keep container as small as possible
RUN apt-get update \
    && apt-get install -y \
    bash \
    sudo \
    build-essential \
    cmake \
    g++ \
    g++-multilib \
    libncurses5-dev \
    libstdc++-4.9-dev \
    make \
    pkg-config \
    python \
    python-dev \
    wget \
    zlib1g-dev \
    vim \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR "/root"

RUN wget "https://cmake.org/files/v$CMAKE_MAJOR/cmake-$CMAKE_VERSION.tar.gz" && \
    tar xf cmake-$CMAKE_VERSION.tar.gz && \
    cd cmake-$CMAKE_VERSION/ && \
    ./configure --prefix=$PREFIX && \
    make && \
    make install && \
    cd .. && \
    rm -rf cmake-*

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

WORKDIR $HOME/qbdi-deps/deps/llvm/$QBDI_PLATFORM

RUN sh build.sh prepare && \
    sh build.sh build && \
    sh build.sh package && \
    sh build.sh clean

# build / test / install QBDI

# git archive -o qbdi.tar.gz --prefix=qbdi/ HEAD .
ADD qbdi.tar.gz $HOME/

WORKDIR $HOME/qbdi

RUN sudo chown -R $USER:$USER . && \
    rm -rf deps && \
    ln -s $HOME/qbdi-deps/deps deps && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=$QBDI_BUILD_TYPE -DCMAKE_CROSSCOMPILING=FALSE -DPLATFORM=$QBDI_PLATFORM -DCMAKE_INSTALL_PREFIX=$PREFIX ../ && \
    make -j2 && \
    ./test/QBDITest && \
    rm -f QBDI-*-$QBDI_PLATFORM.deb && \
    cpack -G DEB && \
    sudo dpkg -i QBDI-*-$QBDI_PLATFORM.deb

WORKDIR "$HOME/"
CMD ["/bin/bash"]
