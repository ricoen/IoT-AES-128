## IoT_AES-128
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Plain AES-128 implementation (Rijndael Algorithm) for embedded system based IoT module (Tested in Particle Photon).
References of this implementation (Many thanks to them):

Project Nayuki: https://www.nayuki.io/page/cryptographic-primitives-in-plain-python

Niyaz PK: http://www.hoozi.com/posts/advanced-encryption-standard-aes-implementation-in-cc-with-comments-part-2-decryption/

This implementation use sensor data from BME280 environmental sensor for plaintext. Added lines for send ciphertext via UDP and TCP/IP (Particle device only). Although it's resulted fair performance and good value of Avalanche Effect, but it can't be used for securing real IoT system because its vulnerabilities of using default mode (ECB mode) that can be analyzed with Side Channel Attack. This code is just for simplicity and readability to explain how AES algorithm is.

# Important
USE FOR EDUCATIONAL PURPOSE ONLY.

# For real IoT system use this cryptographic API or library instead

General: https://github.com/kokke/tiny-AES-c

Embedded system: https://tls.mbed.org/

ESP32: https://github.com/espressif/arduino-esp32/tree/master/tools/sdk/include/mbedtls


# TCP/IP & UDP Lines
This lines is for TCP/IP and UDP function.

To define TCP/IP connection, you can change this line with your destination IP and port.
```
TCPClient client;

IPAddress tcpIP(192,168,43,113);
int tcpPort = 5050;
```

To define UDP connection, you can change this line.
```
UDP Udp;

IPAddress remoteIP(192,168,43,113);
int udpPort = 1337;
```

To send packet, you can use this line:
1. For TCP/IP
```
if (client.connected())
  {
    client.write(enc);
  }
```

2. For UDP
```
if (Udp.sendPacket(enc, sizeof(enc), remoteIP, udpPort) < 0)
  {
    Serial.printlnf("Error");
  }
```

Fore more line functions, you can explore in this reference: 
>Particle Docs.
>https://docs.particle.io/reference/device-os/firmware/photon/

# Don't need to connect to UDP and TCP/IP?
View aes_upnocon.c file.

# Test Results
__1. AES Performance__</br>
This test purpose is to find throughput and Avalanche Effect of the algorithm implementation in the system. The test results was taken from 10 times encryption.
<p align="center">
<img width="320" height="240" src="https://i.postimg.cc/C15gMP9v/Screenshot-from-2021-02-22-09-48-02.png">
</p>
Notes:</br> 
X-axis is encryption time</br>
Y-Axis is troughput</br>

__2. Energy Consumption__</br>
This test purpose is to find how much energy used for one time encryption. The method used for this test is calculate Vshunt of the IoT modules such in the picture below.
<p align="center">
<img width="320" height="240" src="https://i.postimg.cc/QCWZ1B81/Screenshot-from-2021-02-22-09-42-57.png">
</p>

The value of energy consumption is measured in 12 miliseconds time interval using oscilloscope. The results is the average of energy in time interval.
<p align="center">
<img width="320" height="240" src="https://i.postimg.cc/vZ9NLmqy/Screenshot-from-2021-02-22-10-02-43.png">
</P>
<p align="center">
<img width="320" height="240" src="https://i.postimg.cc/tTcsrsL2/Screenshot-from-2021-02-22-09-55-00.png">
</P>
  
# References
>Abdelgawad, A., 2014. Distributed data fusion algorithm for Wireless Sensor Network. Miami, IEEE.</br>
>
>Elminaam, D. S. A., Kader, H. M. A. & Hadhoud, M. M., 2010. Evaluating The Performance of Symmetric Encryption Algorithms. International Journal of Network Security, 10(3), pp. 213-219.</br>
>
>Donnay, V. J., 2013. Solar Panels, Energy and Area Under the Curve Teacher Guide, Bryn Mawr: Bryn Mawr College.
>
>Moreno, C. & Fischmeister, S., 2017. Accurate Measurement of Small Execution Times – Getting Around Measurement Errors, Waterloo: University of Waterloo.
