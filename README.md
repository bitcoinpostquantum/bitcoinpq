
Bitcoin Post-Quantum
=====================================

What is Bitcoin Post-Quantum?
----------------

See https://bitcoinpq.org, or read the [original whitepaper](https://bitcoinpq.org/download/bitcoinpq-whitepaper-english.pdf).

License
-------

The Bitcoin Post-Quantum is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

Development Process
-------------------

The `master` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/bitcoinpostquantum/bitcoinpq/tags) are created
regularly to indicate new official, stable release versions of the Bitcoin Post-Quantum.

Install
-------

### Linux
```bash
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install --assume-yes build-essential libtool autotools-dev autoconf automake pkg-config libssl-dev libboost-all-dev libqt5gui5 libqt5dbus5 qttools5-dev qttools5-dev-tools libprotobuf-dev protobuf-compiler libqrencode-dev libminiupnpc-dev libevent-dev libdb++-dev git cmake
```
##### Install libsodium 
If you have **Ubuntu 17.10** and higher

```bash
sudo apt-get install --assume-yes libsodium-dev
```
If you have **Ubuntu 16.04** and early
```bash
wget https://download.libsodium.org/libsodium/releases/libsodium-1.0.13.tar.gz
tar -xzvf libsodium-1.0.13.tar.gz
cd libsodium-1.0.13/
./configure
make && make check
sudo make install && cd ..
```

##### Install Botan-2.7.0 
```bash
wget https://botan.randombit.net/releases/Botan-2.7.0.tgz
tar -xzvf Botan-2.7.0.tgz
cd Botan-2.7.0/
./configure.py --disable-modules=locking_allocator
make
sudo make install && cd ..
sudo ldconfig

# check if installed
ldconfig -p | grep botan
```

### Mac OS X

#### Homebrew packages

```bash
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

```bash
brew install autoconf automake berkeley-db4 boost miniupnpc pkg-config protobuf qt libtool libevent zeromq
```

#### MacPorts packages

```bash
sudo port install wget autoconf automake libtool pkgconfig libsodium boost libevent protobuf
```

### Install Bitcoin Post-Quantum
```bash
git clone https://github.com/bitcoinpostquantum/bitcoinpq
cd bitcoinpq
git submodule update --init
./autogen.sh
./configure  
make
```

### Start Mainnet
```bash
cd src
./bpqd -printtoconsole
```

### Start Testnet
```bash
cd src
./bpqd -printtoconsole -testnet
```
