FROM debian:buster-slim

#get deps
RUN apt-get update && \
  DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
  vim git ca-certificates libssl-dev wget build-essential libpqxx-dev postgresql-client

# cmake
RUN wget https://github.com/Kitware/CMake/releases/download/v3.18.4/cmake-3.18.4.tar.gz
RUN tar xzf cmake-3.18.4.tar.gz 
RUN cd cmake-3.18.4 && nice ./configure && nice make -j $(nproc) && make install


########### Start pixpq
WORKDIR /app
ENV HOME="/app"

#This user schenanigans allows for local development
ARG USER=pixpq
ARG USER_ID=1000
ARG GROUP_ID=1000

RUN groupadd -g ${GROUP_ID} ${USER} && \
    useradd -l -u ${USER_ID} -g ${USER} -d /app ${USER}

RUN chown ${USER}:${USER} /app
USER ${USER}
