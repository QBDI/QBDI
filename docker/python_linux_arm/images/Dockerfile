ARG DOCKER_IMG="arm32v7/python:3"

FROM $DOCKER_IMG

ENV USER="docker" \
    HOME="/home/docker" \
    PREFIX="/usr" \
    QBDI_PLATFORM="linux"

# Get latest package list, upgrade packages, install required packages
# and cleanup to keep container as small as possible
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive \
    apt-get install -y --no-install-recommends \
        adduser \
        bash \
        build-essential \
        ca-certificates \
        ccache \
        cmake \
        g++ \
        git \
        ninja-build \
        pkg-config \
        python3 \
        python3-dev \
        python3-pip \
        wget \
        xxd \
        zip \
        && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# create a user
RUN adduser --disabled-password --gecos '' --home "$HOME" "$USER"

# switch to new user
USER $USER

ENV PATH="${PATH}:${HOME}/.local/bin"

RUN mkdir "${HOME}/QBDI" && \
    pip install --upgrade --user pip build setuptools

WORKDIR $HOME/QBDI

ARG USER_ID=1000

