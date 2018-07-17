# HElib's introductory example
[HElib](https://github.com/shaih/HElib) is fully homomorphic encryption library by Shai Halevi & Victor Shoup at IBM Research.

# How to setup HElib on Ubuntu
1. Install [GMP](https://gmplib.org/)
```
curl -O https://gmplib.org/download/gmp/gmp-6.1.2.tar.lz
sudo apt install lzip # if lzip is not installed yet
sudo apt-get install m4
tar --lzip -xvf ./gmp-6.1.2.tar.lz gmp-6.1.2/
cd gmp-6.1.2
./configure
make
sudo make install
```

2. Install [NTL](http://www.shoup.net/ntl/)
```
curl -O http://www.shoup.net/ntl/ntl-11.2.1.tar.gz
tar zxf ./ntl-11.2.1.tar.gz 
cd ntl-11.2.1/src
./configure NTL_THREADS=on NTL_THREAD_BOOST=on NTL_EXCEPTIONS=on
make
sudo make install
```

3. Get HElib source and make the essence
```
git clone https://github.com/shaih/HElib.git
cd ./HElib/src
make # This generates "fhe.a" in the current directory and that's what you application works with.
```

4. Run test programs (optional)
```
make check
```

5. This is how to prepare your own program (Seems the build system is very immature at this moment@July 2018)
```
vi your_program_name.cpp
make your_program_name_x
```
* In this way, you can find a number of samples in the official repository. -> [example](https://github.com/shaih/HElib/blob/master/src/Test_General.cpp)

