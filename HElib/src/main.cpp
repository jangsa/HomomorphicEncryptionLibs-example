#include <cstdio>
#include <NTL/ZZ.h>
#include <NTL/BasicThreadPool.h>
#include <NTL/lzz_pXFactoring.h>
#include "FHE.h"
#include "EncryptedArray.h"

int main() {

  long k = 123; // security parameter  [ default=80 ]
  long L = 8; // number of levels in the modulus chain  [ default=heuristic ]
  long c = 2; // number of columns in the key-switching matrices  [ default=2 ]
  long p = 5077; // plaintext base  [ default=2 ] ***this number should be bigger than calculation result***
  long d = 1; // degree of the field extension  [ default=1 ] d == 0 => factors[0] defines extension
  long s = 0;// minimum number of slots  [ default=0 ]

  long r = 1; // lifting  [ default=1 ]
  long w = 64; // hamming weight of secret key

  long chosen_m = 0;
  long m = FindM(k, L, c, p, d, s, chosen_m, 0); // use specified value as modulus

  FHEcontext context(m, p, r);
  buildModChain(context, L, c);

  ZZX G = makeIrredPoly(p, d); // polynomial

  std::cout << "***configurations***\n"
            << ", p=" << p
            << ", r=" << r
            << ", d=" << d
            << ", c=" << c
            << ", k=" << k
            << ", w=" << w
            << ", L=" << L
            << ", m=" << m
            << ", G=" << G
            << endl;

  // generate keys
  FHESecKey secretKey(context);
  const FHEPubKey& publicKey = secretKey;
  secretKey.GenSecKey(w);
  addSome1DMatrices(secretKey); // compute key-switching matrices that we need

  // values to encrypt
  int plaintextA = 7;
  int plaintextB = 70;
  int plaintextC = 700;

  // encrypt plaintexts
  Ctxt c0(publicKey), c1(publicKey), c2(publicKey);
  publicKey.Encrypt(c0, to_ZZX(plaintextA));
  publicKey.Encrypt(c1, to_ZZX(plaintextB));
  publicKey.Encrypt(c2, to_ZZX(plaintextC));
  
  // do some computations
  // 1. addition
  Ctxt cSum = c0; // holder of encrypted sum result
  ZZX pSum;       // holder of decrypted sum result
  cSum += c1;
  cSum += c2;
  secretKey.Decrypt(pSum, cSum);
  std::cout << "addition result:" << pSum[0] << endl; // should print "777"

}


