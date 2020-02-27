FROM qbdi/qbdi:x86_ubuntu

ENV USER="docker" \
    HOME="/home/docker"

# install some needed tools
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
        build-essential \
        cmake \
        libstdc++-8-dev \
        zlib1g-dev \
        python \
        python-dev \
        #gdb \
        #vim \
        sudo \
        bash && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# create a user
RUN adduser --disabled-password --gecos '' $USER && \
    adduser $USER sudo && \
    echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# switch to new user
USER $USER
WORKDIR $HOME

# TODO : Add yours needed files
#ADD --chown=$USER . $HOME/

CMD ["/bin/bash"]

