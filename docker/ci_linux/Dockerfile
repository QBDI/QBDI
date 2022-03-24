ARG DOCKER_IMG="debian:10"

FROM $DOCKER_IMG

ENV USER="docker" \
    HOME="/home/docker" \
    PREFIX="/usr" \
    CLICOLOR_FORCE=1

# create a user
RUN adduser --uid $USER_ID --disabled-password --gecos '' --home $HOME $USER

# Get latest package list, upgrade packages, install required packages 
# and cleanup to keep container as small as possible
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive \
    apt-get install -y --no-install-recommends \
        bash \
        git \
        build-essential \
        cmake \
        ccache \
        g++ \
        g++-multilib \
        libstdc++-10-dev \
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

ARG USER_ID=1000

WORKDIR $HOME

USER $USER

