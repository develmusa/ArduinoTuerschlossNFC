//tuerschloss Luca Mazzoleni 09.07.2015
//Arduino Leonardo

//Erweiterung auf NFC-TAGs 22.06.2016

//library https://github.com/adafruit/Adafruit-PN532

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#define button0  (2)
#define button1 (10)
#define button2  (0)
#define button3 (11)
#define button4  (7)
#define button5  (8)
#define button6  (9)
#define button7  (4)
#define button8  (5)
#define button9  (6)
#define buttonStar (1)
#define buttonHash (3)
#define ledOpen   (12)
#define ledClose  (13)
#define doorPin   (A0)
//NFC
#define PN532_SCK  (A1)
#define PN532_MOSI (A2)
#define PN532_SS   (A3)
#define PN532_MISO (A4)

#define PN532_IRQ   (A1)
#define PN532_RESET (A2)  // Not connected by default on the NFC Shield

//SPI connection
Adafruit_PN532 nfc(PN532_SCK,
                   PN532_MISO,
                   PN532_MOSI,
                   PN532_SS);

//Bei PW wechsel k und secretCode anpassen!!oder mit Sizeof Array l�sen
const int k = 4; //Passwordlength
const int maxIN = (10 + 1); //Max Input
char secretCode[k] = {'1', '1', '1', '1'}; //Code anpassen
char inputCode[maxIN];

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
// also change #define in Adafruit_PN532.cpp library file
#define Serial SerialUSB
#endif

void setup()
{
  Serial.begin(115200);

  pinMode(ledOpen, OUTPUT);
  pinMode(ledClose, OUTPUT);
  pinMode(doorPin, INPUT);
  pinMode(button0, INPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
  pinMode(button5, INPUT);
  pinMode(button6, INPUT);
  pinMode(button7, INPUT);
  pinMode(button8, INPUT);
  pinMode(button9, INPUT);
  pinMode(buttonStar, INPUT);
  pinMode(buttonHash, INPUT);
  digitalWrite(ledOpen, LOW);
  digitalWrite(ledClose, LOW);
  analogWrite(doorPin, 0);
  digitalWrite(button0, HIGH);
  digitalWrite(button1, HIGH);
  digitalWrite(button2, HIGH);
  digitalWrite(button3, HIGH);
  digitalWrite(button4, HIGH);
  digitalWrite(button5, HIGH);
  digitalWrite(button6, HIGH);
  digitalWrite(button7, HIGH);
  digitalWrite(button8, HIGH);
  digitalWrite(button9, HIGH);
  digitalWrite(buttonStar, HIGH);
  digitalWrite(buttonHash, HIGH);

  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata)
  {
    Serial.print("Didn't find PN53x board");
//    while (1)
//      ; // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  // configure board to read RFID tags
  nfc.SAMConfig();

  Serial.println("Waiting for an ISO14443A Card ...");
}

void loop()
{
  int n;  //VarCountNumber
  int i;  //VarInputCode
  int correct = 0;  //VarCountPW
  int p = 0; //VarCountlenghtIN

  uint8_t success;
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};  // Buffer to store the returned UID
  uint8_t uidLength; // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  void accesgranted();
  void accesdenied();
  void reset(); //reset Inputcode-array
  _Bool checkid(double idcard);

//=====================================================================================================
//ABFRAGE NFC
  Serial.println("Abfrage NFC");
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength,100);

  if (success)
  {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");
    Serial.print(uidLength, DEC);
    Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);

    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ... 
      uint32_t cardid = uid[0];
      cardid <<= 8;
      cardid |= uid[1];
      cardid <<= 8;
      cardid |= uid[2];
      cardid <<= 8;
      cardid |= uid[3];
      Serial.print("Seems to be a Mifare Classic card #");
      Serial.println(cardid);

      if (checkid(cardid))
      {
        accesgranted();
      }
      else
      {
        accesdenied();
      }
    }
    Serial.println("");
  }

//=====================================================================================================
//ABFRAGE ZAHLENFELD
//TODO TIMEOUT
  Serial.println("Abfrage Zahlenfeld");

  if (digitalRead(buttonStar) == LOW)
  {
    reset();
    while (1)
    {
      if (digitalRead(buttonStar) == HIGH)
      {
        Serial.println("--*--");
        delay(1);
        do
        {

          if (digitalRead(button0) == LOW)
          {
            while (1)
            {
              if (digitalRead(button0) == HIGH)
              {
                inputCode[p] = ('0');
                Serial.println("--0--");
                delay(1);
                ++p;
                break;
              }
            }
          }

          if (digitalRead(button1) == LOW)
          {
            while (1)
              if (digitalRead(button1) == HIGH)
              {
                inputCode[p] = ('1');
                Serial.println("--1--");
                delay(1);
                ++p;
                break;
              }
          }

          if (digitalRead(button2) == LOW)
          {
            while (1)
              if (digitalRead(button2) == HIGH)
              {
                inputCode[p] = ('2');
                Serial.println("--2--");
                delay(1);
                ++p;
                break;
              }
          }

          if (digitalRead(button3) == LOW)
          {
            while (1)
              if (digitalRead(button3) == HIGH)
              {
                inputCode[p] = ('3');
                Serial.println("--3--");
                delay(1);
                ++p;
                break;
              }
          }

          if (digitalRead(button4) == LOW)
          {
            while (1)
              if (digitalRead(button4) == HIGH)
              {
                inputCode[p] = ('4');
                Serial.println("--4--");
                delay(1);
                ++p;
                break;
              }
          }

          if (digitalRead(button5) == LOW)
          {
            while (1)
              if (digitalRead(button5) == HIGH)
              {
                inputCode[p] = ('5');
                Serial.println("--5--");
                delay(1);
                ++p;
                break;
              }
          }

          if (digitalRead(button6) == LOW)
          {
            while (1)
              if (digitalRead(button6) == HIGH)
              {
                inputCode[p] = ('6');
                Serial.println("--6--");
                delay(1);
                ++p;
                break;
              }
          }

          if (digitalRead(button7) == LOW)
          {
            while (1)
              if (digitalRead(button7) == HIGH)
              {
                inputCode[p] = ('7');
                Serial.println("--7--");
                delay(1);
                ++p;
                break;
              }
          }

          if (digitalRead(button8) == LOW)
          {
            while (1)
              if (digitalRead(button8) == HIGH)
              {
                inputCode[p] = ('8');
                Serial.println("--8--");
                delay(1);
                ++p;
                break;
              }
          }

          if (digitalRead(button9) == LOW)
          {
            while (1)
              if (digitalRead(button9) == HIGH)
              {
                inputCode[p] = ('9');
                Serial.println("--9--");
                delay(1);
                ++p;
                break;
              }
          }
        } while (digitalRead(buttonHash) == HIGH);
        Serial.println("Break*");
        break;
      }
    }

  }
  if (digitalRead(buttonHash) == LOW)
  {
    Serial.println("--#--");
    Serial.println("--P: ");
    Serial.println(p);
    Serial.println("--maxIn");
    Serial.println(maxIN);
    while (1)
    {
      if (digitalRead(buttonHash) == HIGH)
      {
        for (i = 0; i < (k); i++)
        {
          if (inputCode[i] == secretCode[i])
          {
            correct++;
          }
          //Check Code
          Serial.println("InputCode:");
          Serial.println(inputCode[i]);
          Serial.println("SecretCode:");
          Serial.println(secretCode[i]);
        }

        Serial.println("correct");
        Serial.println(correct);
        Serial.println("p");
        Serial.println(p);
        Serial.println("k");
        Serial.println(k);

        reset();

        if ((correct == k) && (p == k))
        {
          accesgranted();
          break;
        }
        else
        {
          accesdenied();
          break;
        }
      }
    }
  }
}

//=================================================================================================

_Bool checkid(double idcard) //NFC ID's with Access
{
  Serial.println(idcard);
  if (idcard == 1111111111)
  {
    Serial.println("Card1");
    return true;
  }
 
  else if (idcard == 1111111111)
  {
    Serial.println("Card2");
    return true;
  }
   else if (idcard ==  1111111111)
  {
    Serial.println("Card3");
    return true;
  }
  else
  {
    Serial.println("NoAcsess with this Card");
    return false;
  }
}

void accesgranted()
{
  Serial.println("Access granted");
  delay(1);
  digitalWrite(ledOpen, HIGH);
    Serial.println(doorPin);
  analogWrite(doorPin, 255);
  delay(2500); //1sec
  digitalWrite(ledOpen, LOW);
  analogWrite(doorPin, 0);
}

void accesdenied()
{
  Serial.println("Access denied1");
  delay(1);
  digitalWrite(ledClose, HIGH);
  delay(1000);  //1sec
  digitalWrite(ledClose, LOW);
}

void reset()  //InputCode-Array mit 0 f�llen
{
  Serial.println("Reset");
  int r;  //VarReset
  for (r = 0; r < (maxIN); ++r)
  {
    inputCode[r] = '0';
  }
}
