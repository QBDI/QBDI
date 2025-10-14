ARG DOCKER_IMG="debian:13"

FROM $DOCKER_IMG

ENV USER="docker" \
    HOME="/home/docker" \
    PREFIX="/usr" \
    CLICOLOR_FORCE=1

ARG USER_ID=1000

# Get latest package list, upgrade packages, install required packages 
# and cleanup to keep container as small as possible
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive \
    apt-get install -y --no-install-recommends \
        adduser \
        bash \
        git \
        build-essential \
        ccache \
        cmake \
        g++ \
        g++-multilib \
        libstdc++-13-dev \
        make \
        ninja-build \
        pkg-config \
        wget \
        ca-certificates \
        python3 \
        python3-dev \
        python3-yaml \
        zip \
        git \
        xxd && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# create a user
RUN adduser --uid "$USER_ID" --disabled-password --gecos '' --home "$HOME" "$USER"

WORKDIR $HOME

USER $USER

