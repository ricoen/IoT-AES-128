# IoT_AES-128
Plain AES-128 implementation (Rijndael Algorithm) for IoT Module (Tested in Particle Photon).
References of this implementation (Many thanks to them):

Project Nayuki: https://www.nayuki.io/page/cryptographic-primitives-in-plain-python

Niyaz PK: http://www.hoozi.com/posts/advanced-encryption-standard-aes-implementation-in-cc-with-comments-part-2-decryption/

This implementation use sensor data from BME280 environmental sensor for plaintext. Added lines for send ciphertext via UDP and TCP/IP (Particle device only). Although it's resulted fair performance and good value of Avalanche Effect, but not recommended for securing real IoT system because its vulnerabilities of using ECB mode that can be analyzed with Side Channel Attack. This code is just for simplicity and readability. 

# TCP/IP & UDP Lines
This lines is for TCP/IP and UDP function

To define TCP/IP connection, you can change this line with your destination IP and port.

'''
TCPClient client;

IPAddress tcpIP(192,168,43,113);
int tcpPort = 5050;
'''

To define UDP connection, you can change this line.
'''
UDP Udp;

IPAddress remoteIP(192,168,43,113);
int udpPort = 1337;
'''

To send packet, you can use this line:
1. For TCP/IP
'''
if (client.connected())
  {
    client.write(enc);
  }
'''

2. For UDP
'''
if (Udp.sendPacket(enc, sizeof(enc), remoteIP, udpPort) < 0)
  {
    Serial.printlnf("Error");
  }
'''

Fore more line function, you can explore in this reference:
https://docs.particle.io/reference/device-os/firmware/photon/

# Important
USE FOR EDUCATIONAL PURPOSE ONLY.


