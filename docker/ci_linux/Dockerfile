ARG DOCKER_IMG="ubuntu:20.04"

FROM $DOCKER_IMG

ENV USER="docker" \
    HOME="/home/docker" \
    PREFIX="/usr" \
    CLICOLOR_FORCE=1

# Get latest package list, upgrade packages, install required packages 
# and cleanup to keep container as small as possible
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive \
    apt-get install -y --no-install-recommends \
        bash \
        sudo \
        git \
        build-essential \
        cmake \
        ccache \
        g++ \
        g++-multilib \
        libstdc++-8-dev \
        make \
        ninja-build \
        pkg-config \
        wget \
        ca-certificates \
        zlib1g-dev \
        python3 \
        python3-dev \
        python3-yaml \
        zip \
        git \
        xxd && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

ARG USER_ID=1000

# create a user
RUN adduser --uid $USER_ID --disabled-password --gecos '' $USER && \
    adduser $USER sudo && \
    echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

WORKDIR $HOME

USER $USER

