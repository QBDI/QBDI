ARG DOCKER_IMG="debian:latest"
FROM $DOCKER_IMG

ARG QBDI_ARCH="AARCH64"

ENV USER="docker" \
    HOME="/home/docker" \
    PREFIX="/usr" \
    QBDI_PLATFORM="linux" \
    CLICOLOR_FORCE=1

# create a user
RUN adduser --disabled-password --gecos '' --home "$HOME" "$USER"

# Get latest package list, upgrade packages, install required packages
# and cleanup to keep container as small as possible
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive \
    apt-get install -y --no-install-recommends \
        bash \
        build-essential \
        ca-certificates \
        ccache \
        cmake \
        file \
        g++ \
        git \
        gzip \
        imagemagick \
        libstdc++-10-dev \
        ninja-build \
        openssl \
        pkg-config \
        python3 \
        python3-dev \
        python3-pip \
        python3-yaml \
        wget \
        xxd \
        zip \
        && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# switch to new user
USER $USER

RUN mkdir "$HOME/QBDI"

WORKDIR $HOME/QBDI
