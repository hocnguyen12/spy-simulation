#include "cipher.h"

char* caesar_cipher(const char message[], int cle) 
{
    int length = strlen(message);
    char* cipher = malloc((length + 1) * sizeof(char));
    if (cipher == NULL) {
        return NULL; 
    }

    for (int i = 0; i < length; i++) {
        if (message[i] >= 'a' && message[i] <= 'z') {
            cipher[i] = ((message[i] - 'a' + cle) % 26) + 'a';
        } else if (message[i] >= 'A' && message[i] <= 'Z') {
            cipher[i] = ((message[i] - 'A' + cle) % 26) + 'A';
        } else {
            cipher[i] = message[i]; 
        }
    }
    cipher[length] = '\0';
    return cipher;
}

char* caesar_decipher(const char message[], int key) 
{
    return caesar_cipher(message, 26 - key);
}