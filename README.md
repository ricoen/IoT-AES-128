# IoT_AES-128
Plain AES-128 implementation (Rijndael Algorithm) for IoT Module (Tested in Particle Photon).
References of this implementation (Many thanks to them):

Niyaz PK: http://www.hoozi.com/posts/advanced-encryption-standard-aes-implementation-in-cc-with-comments-part-2-decryption/

Project Nayuki: https://www.nayuki.io/page/cryptographic-primitives-in-plain-python

Added lines for send ciphertext via UDP and TCP/IP (Particle device only), although it resulted fair performance and good value of Avalanche Effect, but not recommended for securing real IoT system because its vulnerabilities of using ECB mode that can be analyzed with Side Channel Attack. This code is for simplicity, USE FOR EDUCATIONAL PURPOSE ONLY.
