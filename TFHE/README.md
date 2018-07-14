# TFHE's introductory example
TFHE is fully homomorphic encryption library by SPQlios team.

# How to setup TFHE on Ubuntu
sudo apt install build-essential cmake libgmp-dev

git clone https://github.com/tfhe/tfhe.git

git checkout master

mkdir build && cd build

cmake ../src

make

sudo make install

g++ ./src/main.cxx -o ./main.out -ltfhe-nayuki-portable

./main.out

