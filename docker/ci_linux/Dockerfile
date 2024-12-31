ARG DOCKER_IMG="debian:12"

FROM $DOCKER_IMG

ENV USER="docker" \
    HOME="/home/docker" \
    PREFIX="/usr" \
    CLICOLOR_FORCE=1

ARG USER_ID=1000

# create a user
RUN adduser --uid "$USER_ID" --disabled-password --gecos '' --home "$HOME" "$USER"

# setup backport to use cmake >= 3.28
RUN echo 'deb http://deb.debian.org/debian bookworm-backports main contrib non-free' > /etc/apt/sources.list.d/backports.list

# Get latest package list, upgrade packages, install required packages 
# and cleanup to keep container as small as possible
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive \
    apt-get install -y --no-install-recommends \
        bash \
        git \
        build-essential \
        cmake/bookworm-backports \
        ccache \
        g++ \
        g++-multilib \
        libstdc++-11-dev \
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

WORKDIR $HOME

USER $USER

