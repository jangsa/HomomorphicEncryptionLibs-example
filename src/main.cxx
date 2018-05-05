#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>



void makeCloudData(
                const LweSample* ciphertextA,
                const LweSample* ciphertextB,
                const TFheGateBootstrappingParameterSet* parameterSet) {

    FILE* cloudDataFile = fopen("cloud.data","wb");

    for (int i=0; i < 16; i++)
            export_gate_bootstrapping_ciphertext_toFile(
                            cloudDataFile,
                            &ciphertextA[i],
                            parameterSet);

    for (int i=0; i < 16; i++)
            export_gate_bootstrapping_ciphertext_toFile(
                            cloudDataFile,
                            &ciphertextB[i],
                            parameterSet);

    fclose(cloudDataFile);

}

void saveCloudKey(
                const char* filename,
                const TFheGateBootstrappingSecretKeySet* keySet) {

    FILE* cloudKeyFile = fopen(filename,"wb");

    export_tfheGateBootstrappingCloudKeySet_toFile(
                cloudKeyFile,
                &keySet -> cloud);

    fclose(cloudKeyFile);

}

void saveSecretKey(
        const char* filename,
        const TFheGateBootstrappingSecretKeySet* keySet) {

    FILE* secretKeyFile = fopen(filename,"wb");
    
    export_tfheGateBootstrappingSecretKeySet_toFile(
                    secretKeyFile,
                    keySet);

    fclose(secretKeyFile);

}

// Testing homomorphic encryption
void encrypt() {

    uint32_t seeds[] = { 987, 654, 321, 234, 567, 890, 777 };
    tfhe_random_generator_setSeed(seeds, 7);

    // Setting up parameters
    int minLambda = 123;
    TFheGateBootstrappingParameterSet* parameterSet =
            new_default_gate_bootstrapping_parameters(minLambda);

    // Generating a pair of keys, i.e. cloud & private key
    TFheGateBootstrappingSecretKeySet* keySet =
            new_random_gate_bootstrapping_secret_keyset(parameterSet);

    // Generating a pair of keys, i.e. cloud & private key
    saveCloudKey("cloud.key", keySet);
    saveSecretKey("secret.key", keySet);

    // Plaintexts to encrypt (here just 16bits)
    // *caution* Bigger bits make bigger ciphertext
    int16_t plaintextA = 255;
    int16_t plaintextB = 127;

    printf("-------\n");
    printf("INPUT:\nplaintextA -> %d\nplaintextB -> %d\n",
           plaintextA,
           plaintextB);
    printf("-------\n");
    printf("SHOULD BE:\nmin -> %d\naddition -> %d\nmultiplication -> %d\n",
           plaintextB,
           plaintextA + plaintextB,
           plaintextA * plaintextB);
    printf("-------\n");

    // Encrypting both plaintexts
    LweSample* ciphertextA =
            new_gate_bootstrapping_ciphertext_array(16, parameterSet);
    LweSample* ciphertextB =
            new_gate_bootstrapping_ciphertext_array(16, parameterSet);

    for (int i=0; i<16; i++)
        bootsSymEncrypt(&ciphertextA[i], (plaintextA>>i)&1, keySet);
    for (int i=0; i<16; i++)
        bootsSymEncrypt(&ciphertextB[i], (plaintextB>>i)&1, keySet);

    makeCloudData(ciphertextA, ciphertextB, parameterSet);

    // Disposing
    delete_gate_bootstrapping_parameters(parameterSet);
    delete_gate_bootstrapping_secret_keyset(keySet);
    delete_gate_bootstrapping_ciphertext_array(16, ciphertextA);
    delete_gate_bootstrapping_ciphertext_array(16, ciphertextB);
}



void compareBit(
                LweSample* result,
                const LweSample* a,
                const LweSample* b,
                const LweSample* lsb_carry,
                LweSample* tmp,
                const TFheGateBootstrappingCloudKeySet* cloudKeySet) {

    // XNOR = 1 when (0,0) or (1,1)
    bootsXNOR(tmp, a, b, cloudKeySet);

    bootsMUX(result, tmp, lsb_carry, a, cloudKeySet);

}

void min(
                LweSample* result,
                const LweSample* a,
                const LweSample* b,
                const int nb_bits,
                const TFheGateBootstrappingCloudKeySet* cloudKeySet) {

    LweSample* tmps = new_gate_bootstrapping_ciphertext_array(2, cloudKeySet->params);

    bootsCONSTANT(&tmps[0], 0, cloudKeySet);

    for (int i=0; i<nb_bits; i++)
        compareBit(&tmps[0], &a[i], &b[i], &tmps[0], &tmps[1], cloudKeySet);

    for (int i=0; i<nb_bits; i++)
        bootsMUX(&result[i], &tmps[0], &b[i], &a[i], cloudKeySet);

    delete_gate_bootstrapping_ciphertext_array(2, tmps);
}

TFheGateBootstrappingCloudKeySet* readCloudKey(const char* filename) {

    FILE* cloudKeyFile = fopen(filename,"rb");

    TFheGateBootstrappingCloudKeySet* cloudKeySet = new_tfheGateBootstrappingCloudKeySet_fromFile(cloudKeyFile);

    fclose(cloudKeyFile);

    return cloudKeySet;

}

// Testing computation through homomorphic encryption
void compute() {
 
    TFheGateBootstrappingCloudKeySet* cloudKeySet = readCloudKey("cloud.key");

    const TFheGateBootstrappingParameterSet* params = cloudKeySet->params;

    LweSample* ciphertextA = new_gate_bootstrapping_ciphertext_array(16, params);
    LweSample* ciphertextB = new_gate_bootstrapping_ciphertext_array(16, params);

    FILE* cloudDataFile = fopen("cloud.data","rb");
    for (int i=0; i<16; i++) import_gate_bootstrapping_ciphertext_fromFile(cloudDataFile, &ciphertextA[i], params);
    for (int i=0; i<16; i++) import_gate_bootstrapping_ciphertext_fromFile(cloudDataFile, &ciphertextB[i], params);
    fclose(cloudDataFile);

    LweSample* result = new_gate_bootstrapping_ciphertext_array(16, params);
    min(result, ciphertextA, ciphertextB, 16, cloudKeySet);

    FILE* answerDataFile = fopen("answer.min.data","wb");
    for (int i=0; i<16; i++) export_gate_bootstrapping_ciphertext_toFile(answerDataFile, &result[i], params);
    fclose(answerDataFile);

    delete_gate_bootstrapping_ciphertext_array(16, result);
    delete_gate_bootstrapping_ciphertext_array(16, ciphertextB);
    delete_gate_bootstrapping_ciphertext_array(16, ciphertextA);
    delete_gate_bootstrapping_cloud_keyset(cloudKeySet);
}

TFheGateBootstrappingSecretKeySet* readSecretKey(const char* filename) {

    FILE* secretKeyFile = fopen(filename,"rb");

    TFheGateBootstrappingSecretKeySet* secretKey = new_tfheGateBootstrappingSecretKeySet_fromFile(secretKeyFile);

    fclose(secretKeyFile);

    return secretKey;

}

// Testing decryption from homomorphic encryption
void decrypt() {

    TFheGateBootstrappingSecretKeySet* key = readSecretKey("secret.key");
 
    const TFheGateBootstrappingParameterSet* params = key->params;

    LweSample* answer = new_gate_bootstrapping_ciphertext_array(16, params);

    FILE* answerDataFile = fopen("answer.min.data","rb");
    for (int i=0; i<16; i++) 
        import_gate_bootstrapping_ciphertext_fromFile(answerDataFile, &answer[i], params);
    fclose(answerDataFile);

    // Decrypting and copying the answer bit by bit
    int16_t int_answer = 0;
    for (int i=0; i<16; i++) {
        int ai = bootsSymDecrypt(&answer[i], key);
        int_answer |= (ai<<i);
    }

    printf("RESULT:\nmin -> %d\naddition -> \nmultiplication -> \n",int_answer);
    printf("-------\n");

    delete_gate_bootstrapping_ciphertext_array(16, answer);
    delete_gate_bootstrapping_secret_keyset(key);

}



int main() {

    encrypt();

    compute();

    decrypt();

}

