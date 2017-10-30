FROM debian:stretch
MAINTAINER Charles Hubain <chubain@quarkslab.com>

ARG QBDI_BUILD_TYPE=Release

ENV USER docker
ENV HOME /home/$USER
ENV PREFIX /usr
ENV QBDI_PLATFORM android-ARM
ENV LLVM_VERSION 5.0
ENV NDK_VERSION r13b

# Install base tools
RUN apt-get update \
    && apt-get install -y \
    bash \
    gpg \
    wget

# Add LLVM debian repository
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key| apt-key add - && \
    echo "deb http://apt.llvm.org/stretch/ llvm-toolchain-stretch main" >> /etc/apt/sources.list && \
    echo "deb-src http://apt.llvm.org/stretch/ llvm-toolchain-stretch main" >> /etc/apt/sources.list && \
    echo "deb http://apt.llvm.org/stretch/ llvm-toolchain-stretch-4.0 main" >> /etc/apt/sources.list && \
    echo "deb-src http://apt.llvm.org/stretch/ llvm-toolchain-stretch-4.0 main" >> /etc/apt/sources.list && \
    echo "deb http://apt.llvm.org/stretch/ llvm-toolchain-stretch-5.0 main" >> /etc/apt/sources.list && \
    echo "deb-src http://apt.llvm.org/stretch/ llvm-toolchain-stretch-5.0 main" >> /etc/apt/sources.list

# Get latest package list, upgrade packages, install required packages 
# and cleanup to keep container as small as possible
RUN apt-get update \
    && apt-get install -y \
    bash \
    sudo \
    unzip \
    build-essential \
    cmake \
    ninja-build \
    g++ \
    gcc-arm-linux-gnueabi \
    g++-arm-linux-gnueabi \
    libncurses5-dev \
    libstdc++-6-dev \
    make \
    pkg-config \
    python \
    wget \
    zlib1g-dev \
    llvm-$LLVM_VERSION \
    qemu-user \
    vim \
    && ln -s /usr/bin/llvm-tblgen-$LLVM_VERSION /usr/bin/llvm-tblgen \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# create a user
RUN adduser --disabled-password --gecos '' $USER && \
    adduser $USER sudo && \
    echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# switch to new user
USER $USER

WORKDIR $HOME

# Install NDK
RUN sudo chown -R $USER:$USER . && \
    wget https://dl.google.com/android/repository/android-ndk-$NDK_VERSION-linux-x86_64.zip && \
    unzip android-ndk-$NDK_VERSION-linux-x86_64.zip && \
    rm android-ndk-$NDK_VERSION-linux-x86_64.zip

# install QBDI compilation dependencies

# git archive -o qbdi-deps.tar.gz --prefix=qbdi-deps/ HEAD deps
ADD qbdi-deps.tar.gz $HOME/

RUN sudo chown -R $USER:$USER qbdi-deps

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
    sh ../cmake/config-$QBDI_PLATFORM.sh && \
    make -j2 && \
    rm -f QBDI-*-$QBDI_PLATFORM.tar.gz && \
    cpack

WORKDIR "$HOME/"
CMD ["/bin/bash"]
