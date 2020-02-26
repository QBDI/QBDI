ARG DOCKER_IMG="quay.io/pypa/manylinux2010_x86_64"

FROM $DOCKER_IMG

ARG QBDI_PLATFORM="linux-X86_64"

ENV USER="docker" \
    HOME="/home/docker" \
    PREFIX="/usr" \
    PYTHON_OPT="/opt/python"

# Get latest package list, upgrade packages, install required packages
# and cleanup to keep container as small as possible
RUN yum update -y && \
    yum install -y \
        bash \
        sudo \
        cmake3 \
        make \
        wget \
        zlib-devel \
        ca-certificates \
        zlib1g-dev && \
    yum clean all && \
    ln -s /usr/bin/cmake3 /usr/local/bin/cmake && \
    ln -s /usr/bin/ctest3 /usr/local/bin/ctest && \
    ln -s /usr/bin/cpack3 /usr/local/bin/cpack && \
    ln -s /usr/bin/ccmake3 /usr/local/bin/ccmake 


# create a user
RUN adduser --password '' -m $USER && \
    usermod -aG wheel $USER && \
    echo '%wheel ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# switch to new user
USER $USER

# install QBDI compilation dependencies

# git archive -o qbdi-deps.tar.gz --prefix=qbdi-deps/ HEAD deps
ADD qbdi-deps.tar.gz $HOME/

WORKDIR $HOME

RUN sudo chown -R $USER:$USER .

WORKDIR $HOME/qbdi-deps/deps/llvm/

RUN export PATH=$PYTHON_OPT/cp37-cp37m/bin:$PATH && \
    python3 build.py prepare $QBDI_PLATFORM && \
    python3 build.py build $QBDI_PLATFORM && \
    python3 build.py package $QBDI_PLATFORM && \
    python3 build.py clean $QBDI_PLATFORM

# build / test / install QBDI
ARG CMAKE_ARGUMENT=""

# git archive -o qbdi.tar.gz --prefix=qbdi/ HEAD .
ADD qbdi.tar.gz $HOME/

WORKDIR $HOME/qbdi

RUN sudo chown -R $USER:$USER . && \
    # link llvm and gtest
    rm -rf deps && \
    ln -s $HOME/qbdi-deps/deps deps && \
    # build wheel
    /opt/python/cp35-cp35m/bin/python setup.py bdist_wheel && \
    /opt/python/cp36-cp36m/bin/python setup.py bdist_wheel && \
    /opt/python/cp37-cp37m/bin/python setup.py bdist_wheel && \
    /opt/python/cp38-cp38/bin/python setup.py bdist_wheel

RUN for i in dist/*; do auditwheel repair $i -w outwheel; done
