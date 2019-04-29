FROM base/archlinux:2018.10.01
MAINTAINER Charles Hubain <chubain@quarkslab.com>

ARG QBDI_BUILD_TYPE=Release

ENV USER docker
ENV HOME /home/$USER
ENV PREFIX /usr
ENV QBDI_PLATFORM linux-X86_64

# Get latest package list, upgrade packages, install required packages 
# and cleanup to keep container as small as possible
RUN pacman -Sy && pacman -S --noconfirm \
    sudo \
    gcc \
    make \
    cmake \
    pkg-config \
    fakeroot \
    python2 \
    wget \
    vim \
    && pacman -Sc --noconfirm

# create a user
RUN useradd -Groot $USER && \
    echo '%root ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

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

RUN python build.py prepare $QBDI_PLATFORM && \
    python build.py build $QBDI_PLATFORM && \
    python build.py package $QBDI_PLATFORM && \
    python build.py clean $QBDI_PLATFORM

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
