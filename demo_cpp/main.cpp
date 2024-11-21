#include <openssl/aes.h>
#include <cstring>
#include <iostream>

// test update the line

// Function to encrypt a 128-bit block using AES-128 ECB mode
void AES128_ECB_encrypt(const unsigned char *input, unsigned char *output, const unsigned char *key) {
    AES_KEY aesKey;
    // Set encryption key (128-bit)
    AES_set_encrypt_key(key, 128, &aesKey);
    
    // Perform AES encryption on the input block (128-bit)
    AES_ecb_encrypt(input, output, &aesKey, AES_ENCRYPT);
}

// Function to decrypt a 128-bit block using AES-128 ECB mode
void AES128_ECB_decrypt(const unsigned char *input, unsigned char *output, const unsigned char *key) {
    AES_KEY aesKey;
    // Set decryption key (128-bit)
    AES_set_decrypt_key(key, 128, &aesKey);

    // Perform AES decryption on the input block (128-bit)
    AES_ecb_encrypt(input, output, &aesKey, AES_DECRYPT);
}

int main() {
    // 128-bit key (16 bytes)
    unsigned char key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                             0xab, 0xf7, 0x5f, 0x19, 0x0b, 0x6a, 0x7b, 0x4d};

    // 128-bit input block (16 bytes)
    unsigned char input[16] = {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d,
                               0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34};

    unsigned char encrypted[16];
    unsigned char decrypted[16];

    std::cout << "Original Input:\n";
    for (int i = 0; i < 16; i++)
        std::cout << std::hex << static_cast<int>(input[i]) << " ";
    std::cout << "\n";

    // Encrypt
    AES128_ECB_encrypt(input, encrypted, key);

    std::cout << "Encrypted Output:\n";
    for (int i = 0; i < 16; i++)
        std::cout << std::hex << static_cast<int>(encrypted[i]) << " ";
    std::cout << "\n";

    // Decrypt
    AES128_ECB_decrypt(encrypted, decrypted, key);

    std::cout << "Decrypted Output:\n";
    for (int i = 0; i < 16; i++)
        std::cout << std::hex << static_cast<int>(decrypted[i]) << " ";
    std::cout << "\n";

    return 0;
}
