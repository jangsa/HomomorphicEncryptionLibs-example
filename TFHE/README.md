# TFHE's introductory example
[TFHE](https://tfhe.github.io/tfhe/) is fully homomorphic encryption library by SPQlios team.

# How to setup TFHE on Ubuntu
sudo apt install build-essential cmake libgmp-dev

git clone https://github.com/tfhe/tfhe.git

git checkout master

mkdir build && cd build

cmake ../src

make

sudo make install

sudo ldconfig

g++ ./src/main.cxx -o ./main.out -ltfhe-nayuki-portable

./main.out

