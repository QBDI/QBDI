ARG UBUNTU_TAG=latest

FROM i386/ubuntu:$UBUNTU_TAG

ENV USER="docker" \
    HOME="/home/docker" \
    PREFIX="/usr" \
    QBDI_PLATFORM="linux-X86" \
    CLICOLOR_FORCE=1

# Get latest package list, upgrade packages, install required packages 
# and cleanup to keep container as small as possible
RUN apt-get update \
    && apt-get install -y \
    bash \
    sudo \
    git \
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
    python-pathlib \
    python3 \
    python3-dev

# create a user
RUN adduser --disabled-password --gecos '' $USER && \
    adduser $USER sudo && \
    echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

WORKDIR $HOME

ADD . $HOME/qbdi

RUN chown -R $USER:$USER $HOME

# switch to new user
USER $USER

