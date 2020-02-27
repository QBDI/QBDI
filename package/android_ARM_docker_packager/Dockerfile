FROM debian:10
MAINTAINER Charles Hubain <chubain@quarkslab.com>

ENV USER="docker" \
    HOME="/home/docker" \
    PREFIX="/usr" \
    QBDI_PLATFORM="android-ARM" \
    NDK_VERSION="r20" \
    ANDROID_PLATFORM="23"

# Get latest package list, upgrade packages, install required packages 
# and cleanup to keep container as small as possible
RUN apt-get update && \
    apt-get install -y \
        bash \
        sudo \
        unzip \
        build-essential \
        cmake \
        ninja-build \
        g++ \
        libstdc++-8-dev \
        make \
        pkg-config \
        python3 \
        wget \
        zlib1g-dev \
        qemu-user && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

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

# Test if off, no need of gtest
#
#WORKDIR $HOME/qbdi-deps/deps/gtest/$QBDI_PLATFORM
#
#RUN sh build.sh prepare && \
#    sh build.sh build && \
#    sh build.sh package && \
#    sh build.sh clean

WORKDIR $HOME/qbdi-deps/deps/llvm/

RUN python3 build.py prepare $QBDI_PLATFORM && \
    python3 build.py build $QBDI_PLATFORM && \
    python3 build.py package $QBDI_PLATFORM && \
    python3 build.py clean $QBDI_PLATFORM

# build / test / install QBDI

# git archive -o qbdi.tar.gz --prefix=qbdi/ HEAD .
ADD qbdi.tar.gz $HOME/

WORKDIR $HOME/qbdi

RUN sudo chown -R $USER:$USER . && \
    rm -rf deps && \
    ln -s $HOME/qbdi-deps/deps deps && \
    mkdir build && \
    cd build && \
    cmake -DPLATFORM=$QBDI_PLATFORM \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_TOOLCHAIN_FILE=$HOME/android-ndk-$NDK_VERSION/build/cmake/android.toolchain.cmake \
          -DANDROID_ABI=armeabi-v7a \
          -DANDROID_PLATFORM=$ANDROID_PLATFORM \
          -DTEST_QBDI=off \
          .. && \
    make -j2 && \
    rm -f QBDI-*-$QBDI_PLATFORM.tar.gz && \
    cpack

WORKDIR "$HOME/"
CMD ["/bin/bash"]
