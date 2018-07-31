# The Configurable LED Block Cipher

## LED Specification
- https://sites.google.com/site/ledblockcipher/design
- http://www.springerlink.com/content/630873l12h23484p

## Purpose
This implementation is meant for security research purposes only, and should
not be used in production. It supports both encryption and decryption.

## Usage

- 64-bit key:
  - ``` ./led-64 -e -x "01234567 89abcdef" -k "01234567 89abcdef" ```  
    Will produce ciphertext: ```fdd6fb98 45f81456```
  - ``` ./led-64 -d -x "fdd6fb98 45f81456" -k "01234567 89abcdef"```  
    Will produce plaintext: ```01234567 89abcdef```
- 128-bit key:
  - ``` ./led-128 -e -x "01234567 89abcdef" -k "01234567 89abcdef 01234567 89abcdef" ```  
    Will produce ciphertext: ```3131c231 205c3664```
  - ``` ./led-128 -d -x "3131c231 205c3664" -k "01234567 89abcdef 01234567 89abcdef" ```  
    Will produce plaintext: ```01234567 89abcdef```
