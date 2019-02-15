FROM i386/ubuntu:18.10

ARG QBDI_BUILD_TYPE=Release

ENV USER docker
ENV HOME /home/$USER
ENV PREFIX /usr
ENV QBDI_PLATFORM linux-X86

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
    libstdc++-6-dev \
    make \
    pkg-config \
    python \
    python-dev \
    wget \
    zlib1g-dev \
    vim \
    libncurses5 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

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

# uncomment this line to test with validation_runner
# run test with "docker run -it --cap-add SYS_PTRACE --rm qbdi_ubuntu_i386:latest /bin/bash"
#RUN sudo apt-get update && \
#    sudo apt-get install -y python-yaml tmux git imagemagick zip


WORKDIR "$HOME/"
CMD ["/bin/bash"]
