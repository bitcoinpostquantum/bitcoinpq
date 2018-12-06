
  sudo apt-get update 
  sudo apt-get upgrade 
  sudo apt-get install build-essential libtool autotools-dev autoconf pkg-config libssl-dev 
  sudo apt-get install libboost-all-dev 
  sudo apt-get install libqt5gui5 libqt5core5 libqt5dbus5 qttools5-dev qttools5-dev-tools libprotobuf-dev protobuf-compiler 
  sudo apt-get install libqrencode-dev 
  sudo apt-get install libminiupnpc-dev 

  wget https://botan.randombit.net/releases/Botan-2.4.0.tgz
  tar -xzvf Botan-2.4.0.tgz
  cd Botan-2.4.0/
  ./configure.py
  make
  sudo make install
  make clean

  wget 'http://download.oracle.com/berkeley-db/db-4.8.30.NC.tar.gz'
  tar -xzvf db-4.8.30.NC.tar.gz
  cd db-4.8.30.NC/build_unix/
  ../dist/configure --enable-cxx --disable-shared --with-pic --prefix=/home/vagrant/bpq161/.build/db4
  make

  git clone https://bitbucket.org/kfginternational/bpq161.git
  git fetch
  git checkout dev
  cd bpq161
  ./autogen.sh
  ./configure LDFLAGS=\"-L/home/vagrant/bpq161/.build/db4/lib/\" CPPFLAGS=\"-I/home/vagrant/bpq161/.build/db4/include/\"
  make

  # Application bpqd and bpq-cli
  # bpq161/src

  #-------------------------------------------------
  # Only vagrant Create swap file
  sudo fallocate -l 1G /swapfile
  ls -lh /swapfile
  -rw-r--r-- 1 root root 4.0G Apr 28 17:19 /swapfile
  sudo chmod 600 /swapfile
  ls -lh /swapfile
  -rw------- 1 root root 4.0G Apr 28 17:19 /swapfile
  sudo mkswap /swapfile
  sudo swapon /swapfile
  sudo swapon -s
  Filename                Type        Size    Used    Priority
  /swapfile               file        4194300 0       -1


  #

Test network

* Изменить параметры equihash в src/chainparams.cpp maintest/testnet/regtest
* Удалить каталог ~./bpq/[testnet | regtest]
* Запустить две ноды.
* Сгенерировать legacy блоки ./bpq-cli generate 2000
* Сделать premine ./bpq-cli generate 100 (премайн делается только на определенные адреса)
* Проверить что прошла синхронизация блоков между нодами UpdateTip: ..... height = содержит номер последнего блока
# Проверить совпадение ./bpq-cli getblocks
* Создать legacy адрес ./bpq-cli getnewaddress "" legacy
* Вставить адрес в pool z-nomp-bpq/pool_configs
* Запустить pool 
  npm start
* При корректом запуске 
  Network Connected:  Mainnet
  Detected Reward Type: POW
  Current Block Height: 2101
  Current Block Diff: 0.000244141
  Current Connect Peers:  1
  Network Hash Rate:  0.01 KH
  Stratum Port(s):  3858
  Block polling every:  500 ms  
* ./nhminer -l localhost:3858

Containers

lxc list
lxc init ubuntu:16.04 container-bpq -p default
lxc network list
lxc network attach lxdbr0 container-bpq eth0
lxc config device set container-bpq eth0 ipv4.address 10.16.99.2
lxc start container-bpq
lxc exec container-bpq /bin/bash



