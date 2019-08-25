/*
 * Project aes
 * Description: Penerapan AES-128 pada Modul IoT
 * Author: Ricoen
 * Date: 30 July 2019
 * References of implementation: Nayuki Project, Niyaz PK, Particle Reference
*/

//Tambah library di sini
#include <Adafruit_BME280.h>

#include "Adafruit_Sensor.h"
#include "Adafruit_BME280.h"

#define BME_SCK D1
#define BME_MOSI D0

#include<stdio.h>
#include <string.h>

Adafruit_BME280 bme; // I2C BME280
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);

//definisi tipe data sensor
double temp;
double humidity;

// Inisialisasi TCP/IP
TCPClient client;

IPAddress tcpIP(192,168,43,113);
int tcpPort = 5050;

// fungsi setup().
void setup() {
    Serial.begin(9600);
    Serial.println(F("BME280 test"));

    client.connect(tcpIP, tcpPort);
  
    // if (!bme.begin(0x76)) {
    if (!bme.begin()) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
    }
}

// Program enkripsi dimulai di sini
// Menyatakan jumlah state pada AES. Merupakan konstanta AES. Nilai=4
#define Nb 4

// Jumlah putaran pada cipher AES. Dimulai dari nol. Nilai sebenarnya didapatkan dari program.
int Nr=0;

// Jumlah 32 bit kata pada kunci. Dimulai dari nol. Nilai sebenarnya didapatkan dari program.
int Nk=0;

// in - array yang digunakan untuk menyimpan plainteks yang akan dienkripsi.
// out - array yang digunakan untuk menyimpan cipherteks.
// state - array yang menyimpan hasil enkripsi tiap state.
unsigned char in[16], out[16], state[4][4];

// Array untuk menyimpan roundkey.
unsigned char RoundKey[240];

// Array untuk menyimpan kunci AES.
unsigned char Key[32];

// Sbox - Terdiri dari 8 bit input, c, dengan 8 bit input, s = S(c).
// Keduanya merupakan interpretasi dari polinomial Galois Field GF(2).
// Input pertama merupakan hasil perkalian inverse dalam GF(28) = GF(2)[x]/(x8 + x4 + x3 + x + 1).
int getSBoxValue(int num)
{
	int sbox[256] =   {
	//0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, //0
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, //1
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, //2
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, //3
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, //4
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, //5
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, //6
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, //7
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, //8
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, //9
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, //A
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, //B
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, //C
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, //D
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, //E
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 }; //F
	return sbox[num];
}

// Putaran konstan untuk array kata, Rcon[i], berisi nilai yang diperoleh dari
// x ke (i-1) menjadi pangkat dari x (x dapat dinotasikan sebagai {02}) dalam field GF(28)
// Perlu dicatat bahwa i dimulai dari 1, bukan 0).
int Rcon[255] = {
	0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
	0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
	0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
	0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
	0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
	0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
	0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
	0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
	0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
	0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
	0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
	0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
	0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
	0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
	0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
	0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb  };

// Fungsi untuk menghasilkan Nb(Nr+1) kunci putaran. Round Keys digunakan untuk menghasilkan cipherteks pada tiap states.
void KeyExpansion()
{
	int i,j;
	unsigned char temp[4],k;

	// Putaran pertama dengan kunci yang dihasilkan sendiri.
	for(i=0;i<Nk;i++)
	{
		RoundKey[i*4]=Key[i*4];
		RoundKey[i*4+1]=Key[i*4+1];
		RoundKey[i*4+2]=Key[i*4+2];
		RoundKey[i*4+3]=Key[i*4+3];
	}

	// Semua kunci Round Key lain dihasilkan dari setiap kunci pada Round Key sebelumnya.
	while (i < (Nb * (Nr+1)))
	{
					for(j=0;j<4;j++)
					{
						temp[j]=RoundKey[(i-1) * 4 + j];
					}
					if (i % Nk == 0)
					{
						// Fungsi untuk menggeser 4 bytes pada setiap baris ke kiri.
						// [a0,a1,a2,a3] menjadi [a1,a2,a3,a0]

						// Fungsi RotWord()
						{
							k = temp[0];
							temp[0] = temp[1];
							temp[1] = temp[2];
							temp[2] = temp[3];
							temp[3] = k;
						}

						// SubWord() adalah fungsi yang mengambil 4 bytes input kata dan
						// diterapkan S-box pada tiap 4 bytes untuk mengasilan output kata.

						// Fungsi Subword()
						{
							temp[0]=getSBoxValue(temp[0]);
							temp[1]=getSBoxValue(temp[1]);
							temp[2]=getSBoxValue(temp[2]);
							temp[3]=getSBoxValue(temp[3]);
						}

						temp[0] =  temp[0] ^ Rcon[i/Nk];
					}
					else if (Nk > 6 && i % Nk == 4)
					{
						// Fungsi Subword()
						{
							temp[0]=getSBoxValue(temp[0]);
							temp[1]=getSBoxValue(temp[1]);
							temp[2]=getSBoxValue(temp[2]);
							temp[3]=getSBoxValue(temp[3]);
						}
					}
					RoundKey[i*4+0] = RoundKey[(i-Nk)*4+0] ^ temp[0];
					RoundKey[i*4+1] = RoundKey[(i-Nk)*4+1] ^ temp[1];
					RoundKey[i*4+2] = RoundKey[(i-Nk)*4+2] ^ temp[2];
					RoundKey[i*4+3] = RoundKey[(i-Nk)*4+3] ^ temp[3];
					i++;
	}
}

// Fungsi untuk menambahkan Round Key pada State.
// Round Key ditambahkan dengan meng-XOR kan tiap bytes.
void AddRoundKey(int round)
{
	int i,j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			state[j][i] ^= RoundKey[round * Nb * 4 + i * Nb + j];
		}
	}
}

// Fungsi substitusi digunakan untuk mensubstitusikan
// matriks state dengan suatu nilai pada S-box.
void SubBytes()
{
	int i,j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			state[i][j] = getSBoxValue(state[i][j]);

		}
	}
}

// Fungsi ShiftRows() merupakan fungsi untuk menggeser tiap baris pada state ke kiri.
// Setiap baris digeser dengan batas (offset) yang berbeda.
// Offset = Row number (dimulai dari nol). Jadi baris pertama tidak digeser.
void ShiftRows()
{
	unsigned char temp;

	// Putar baris pertama 1 kolom ke kiri
	temp=state[1][0];
	state[1][0]=state[1][1];
	state[1][1]=state[1][2];
	state[1][2]=state[1][3];
	state[1][3]=temp;

	// Putar baris ke dua 2 kolom ke kiri
	temp=state[2][0];
	state[2][0]=state[2][2];
	state[2][2]=temp;

	temp=state[2][1];
	state[2][1]=state[2][3];
	state[2][3]=temp;

	// Putar baris ke tiga 3 kolom ke kiri
	temp=state[3][0];
	state[3][0]=state[3][3];
	state[3][3]=state[3][2];
	state[3][2]=state[3][1];
	state[3][1]=temp;
}

// xtime is a macro that finds the product of {02} and the argument to xtime modulo {1b}
// Transformasi ini dilakukan satu demi satu dalam CPU menggunakan 48 byte RAM internal
// yang dapat dialamatkan secara langsung, dan mengambil sekitar 12.000 siklus clock
// untuk mengenkripsi data 128 bit blokir dengan kunci 128-bit.
#define xtime(x)   ((x<<1) ^ (((x>>7) & 1) * 0x1b))

// Fungsi MixColumns untuk mencampur kolom dengan matriks state
void MixColumns()
{
	int i;
	unsigned char Tmp,Tm,t;
	for(i=0;i<4;i++)
	{
		t=state[0][i];
		Tmp = state[0][i] ^ state[1][i] ^ state[2][i] ^ state[3][i] ;
		Tm = state[0][i] ^ state[1][i] ; Tm = xtime(Tm); state[0][i] ^= Tm ^ Tmp ;
		Tm = state[1][i] ^ state[2][i] ; Tm = xtime(Tm); state[1][i] ^= Tm ^ Tmp ;
		Tm = state[2][i] ^ state[3][i] ; Tm = xtime(Tm); state[2][i] ^= Tm ^ Tmp ;
		Tm = state[3][i] ^ t ; Tm = xtime(Tm); state[3][i] ^= Tm ^ Tmp ;
	}
}

// Cipher merupakan fungsi utama untuk mengenkripsi plainteks.
void Cipher()
{
	int i,j,round=0;

	//Copy plainteks ke array state.
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			state[j][i] = in[i*4 + j];
		}
	}

	// Menambahkan kunci pertama pada Round Key sebelum dilakukan putaran.
	AddRoundKey(0);

	// Akan dilakukan putaran Nr.
	// Nr-1 (putaran pertama).
	// Nr-1 merupakan putaran yang telah dieksekusi pada fungsi di atas.
	for(round=1;round<Nr;round++)
	{
		SubBytes();
		ShiftRows();
		MixColumns();
		AddRoundKey(round);
	}

	// Putaran terakhir.
	// Fungsi MixColumns tidak diterapkan pada putaran terakhir.
	SubBytes();
	ShiftRows();
	AddRoundKey(Nr);

	// Proses enkripsi selesai.
	// Copy array state ke array output.
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			out[i*4+j]=state[j][i];
		}
	}
}

// Fungsi loop().
void loop() {

    int i;

	// Panjang kunci AES.
	Nr = 128;

	// Hitung Nk dan Nr.
	Nk = Nr / 32;
	Nr = Nk + 6;

    // Ambil nilai dari sensor
    float t = bme.readTemperature(); // *C
    float h = bme.readHumidity(); // %
    //float p = bme.readPressure() / 100.0F; // hPa
    //float a = bme.readAltitude(SEALEVELPRESSURE_HPA);

    //Defini ulang variabel nilai sensor
    temp = t;
    humidity = h;

    //Format data sensor menjadi format string
    String data = "t "+ String::format("%.1f",temp)+ ", "+ "h "+ String::format("%.2f",humidity);

    Serial.printlnf(data);

    // Konversi kunci dan string sensor menjadi hexadesimal
    char temp[16] = "inikuncinyagan";
    int len = 16;
    
    // Untuk Kunci
    Serial.printf("Kunci(HEX): ");
    for (i = 0; i < len; i++)
    {
        if (i > 0) Serial.printf(" ");
        Serial.printf("%02X", temp[i]);
    }
    Serial.printlnf("\n");
    
    // Untuk string sensor
    Serial.printf("Data sensor(HEX): ");
    for (i = 0; i < len; i++)
    {
        if (i > 0) Serial.printf(" ");
        Serial.printf("%02X", data[i]);
    }
    Serial.printlnf("\n");

	// Copy kunci dan plainteks
	for(i=0;i<Nk*4;i++)
	{
	    for (i = 0; i < len; ++i)
	    {
		Key[i]=("%02x", temp[i]);
	    }
	}

	for(i=0;i<Nb*4;i++)
	{
	    for (i = 0; i < len; ++i)
		{
        in[i]=("%02x", data[i]);
		}
	}

    // Fungsi ekspansi kunci harus dipanggil sebelum proses enkripsi.
	KeyExpansion();

	// Memanggil fungsi untuk melakukan enkripsi plainteks dengan algoritma AES.
	Cipher();

	// Cipherteks.
	char enc[64];
	Serial.printf("Cipherteks(HEX): ");
	for(i=0;i<Nk*4;i++)
	{
		Serial.printf("%02x ",out[i]);
		snprintf(enc, sizeof(enc), "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7], out[8], out[9], out[10], out[11], out[12], out[13], out[14], out[15]);
	}
	Serial.printlnf("\n");
	Serial.printf("Mengirim cipherteks via TCP/IP: %s", enc);
	Serial.printlnf("\n\n");
	
	// Mengirim via TCP/IP
    if (client.connected())
    {
        client.write(enc);
    }
    
delay (3000);
}
