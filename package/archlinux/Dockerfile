FROM pritunl/archlinux:2017-10-21
MAINTAINER Charles Hubain <chubain@quarkslab.com>

ARG QBDI_BUILD_TYPE=Release

ENV USER docker
ENV HOME /home/$USER
ENV PREFIX /usr
ENV QBDI_PLATFORM linux-X86_64

# Get latest package list, upgrade packages, install required packages 
# and cleanup to keep container as small as possible
RUN pacman -S --noconfirm \
    sudo \
    gcc \
    make \
    cmake \
    pkg-config \
    fakeroot \
    python \
    wget \
    vim \
    && pacman -Sc --noconfirm

# create a user
RUN useradd -Gwheel $USER && \
    echo '%wheel ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

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
    cp ../package/archlinux/PKGBUILD . && \
    makepkg -fc && \
    sudo pacman -U --noconfirm QBDI-*-x86_64.pkg.tar.xz

WORKDIR "$HOME/"
CMD ["/bin/bash"]
