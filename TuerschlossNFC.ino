/**
 * @file TuerschlossNFC.ino
 * @author Luca Mazzoleni
 *
 * @brief  Doorlock with Keypad and NFC
 *
 *  Board: Arduino Leonardo \n
 *  Library: \link{https://github.com/adafruit/Adafruit-PN532\endlink \n
 *  Item List will follow..
 *
 */

/* Includes */
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

/**Keypad Nr. 0 Pin 2*/
#define button0  (2)
/**Keypad Nr. 1 Pin 10*/
#define button1 (10)
/**Keypad Nr. 2 Pin 0*/
#define button2  (0)
/**Keypad Nr. 3 Pin 11*/
#define button3 (11)
/**Keypad Nr. 4 Pin 7*/
#define button4  (7)
/**Keypad Nr. 5 Pin 8*/
#define button5  (8)
/**Keypad Nr. 6 Pin 9*/
#define button6  (9)
/**Keypad Nr. 7 Pin 4*/
#define button7  (4)
/**Keypad Nr. 8 Pin 5*/
#define button8  (5)
/**Keypad Nr. 9 Pin 6*/
#define button9  (6)
/**Keypad Nr. * Pin 1*/
#define buttonStar (1)
/**Keypad Nr. # Pin 3*/
#define buttonHash (3)
/**Green LED Pin 12*/
#define ledOpen   (12)
/**Red LED Pin 13*/
#define ledClose  (13)
/**Operates a 5V Relais Pin A0*/
#define doorPin   (A0)
//NFC
/**NFC Serial Clock Pin A1*/
#define PN532_SCK  (A1)
/**NFC Master Output, Slave Input Pin A2*/
#define PN532_MOSI (A2)
/**NFC Slave Select Pin A3*/
#define PN532_SS   (A3)
 /**NFC Master Input, Slave Output Pin A4*/
#define PN532_MISO (A4)

/**NFC Interrupt Request Pin A1*/
#define PN532_IRQ   (A1)
/**Not connected by default on the NFC Shield*/
#define PN532_RESET (A2)

//SPI connection
/**Check Adafruit library for more Information \link{https://github.com/adafruit/Adafruit-PN532\endlink
 * @param PN532_SCK Serial Clock
 * @param PN532_MISO Master Input, Slave Outpu
 * @param PN532_MOSI Master Output, Slave Input
 * @param PN532_SS Slave Select
 * */
Adafruit_PN532 nfc(PN532_SCK,
                   PN532_MISO,
                   PN532_MOSI,
                   PN532_SS);

/** Max length of Input*/
const int maxIN = (10 + 1);

/** customize your Code here */
char secretCode[] = {button1, button1, button1, button1};

/** Length of your SecretCode */
const int k = sizeof(secretCode) / sizeof(secretCode[0]);

/**Initialize a Array for the Inputs*/
char inputCode[maxIN];

/**Set a Timeout for the Keypad */
const long keypadTimeout = 8000;

#if defined(ARDUINO_ARCH_SAMD)
/** for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
 *also change #define in Adafruit_PN532.cpp library file
 */
#define Serial SerialUSB
#endif

//=============================================================================================================
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

//==========================================================================================
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata)
  {
    Serial.print("Didn't find PN53x board");
//    while (1)
//      ; // stop if Board not found
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);


  nfc.SAMConfig();   // configure board to read RFID tags

  Serial.println("Waiting for an ISO14443A Card ...");
}

//=========================================================================================
void loop()
{
  unsigned long callTime = 0;
  unsigned long runTime = 0;
  int breakFlag = 0;
  int p = 0;          //VarCountlenghtIN

  uint8_t success;
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};  /* Buffer to store the returned UID */
  uint8_t uidLength; /* Length of the UID (4 or 7 bytes depending on ISO14443A card type)*/

  void accesgranted();
  void accesdenied();
  void reset();
  _Bool checkid(double idcard);
  _Bool buttonPressed(int button);
  void checkCode(int p);

//=====================================================================================================
//ABFRAGE NFC
  Serial.println("Abfrage NFC");

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength,
                                    150); // Timeout is 150ms

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
//Check Keypad-Input
  Serial.println("Check for Input on Keypad");

  int Zahlenfeld[4][3] = {  {button1, button2, button3},
                            {button4, button5, button6},
                            {button7, button8, button9},
                            {buttonStar, button0, buttonHash}};

  char* ZahlenfeldPrint[4][3] = { {"--1--", "--2--", "--3--"},
                                  {"--4--","--5--", "--6--"},
                                  {"--7--", "--8--", "--9--"},
                                  {"--*--", "--0--", "--#--"}};

  if (buttonPressed(buttonStar))
  {
    Serial.println("--*--");
    digitalWrite(ledClose, HIGH); /**TODO:Fix Hardware Issue->switch to ledOpen when fixed*/
    delay(20);
    digitalWrite(ledClose, LOW);
    reset();
    callTime = millis(); //Set time of function-call
    while (1)
    {
      runTime=millis();
      if (runTime - callTime >= keypadTimeout) //Check for Timeout
      {
        Serial.println("Keypad Timeout");
        breakFlag = 1;
      }
      if (breakFlag)
        break;
      //Reset rows and cols var for loop
      int n = 0;
      int m = 0;

      for (n = 0; n < 4; n++)  //Check cols
      {
        for (m = 0; m < 3; m++) //Check rows
        {
          if (buttonPressed(buttonHash) || (p >= maxIN))
          {
            checkCode(p);
            breakFlag = 1;
          }
          else if (buttonPressed(Zahlenfeld[n][m]))
          {
            inputCode[p] = (Zahlenfeld[n][m]);
            Serial.println(ZahlenfeldPrint[n][m]);
            delay(1);
            ++p;
          }
        }
      }
    }
  }
}

//=================================================================================================

_Bool checkid(double idcard)
/** @brief Check if ID is authorized
 *
 * If the NFC ID from the scanned Card is saved in the authorized List \n
 * You need to Edit here, if you like to add your Card!
 * @param idcard NFC ID from the scanned Card
 * @return True if Authorized \n
 *         False if not*/
{
  Serial.println(idcard); //you need to add the id of your authorized card here instead of 1's
  if (idcard == 0000000000)
  {
    Serial.println("Card1");
    return true;
  }
  else if (idcard == 0000000000)
  {
    Serial.println("Card2");
    return true;
  }
  else if (idcard == 0000000000)
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

_Bool buttonPressed(int button)
/** @brief Check if Button is pressed
 *
 * Check if Button switch from High to Low and Back to High
 * @param button scanned Button
 * @return True if Pressed
 *         False if not*/
{
  if (digitalRead(button) == LOW)
  {
    while (1)
    {
      if (digitalRead(button) == HIGH)
        return true;
    }
  }
  return false;
}

void checkCode(int p)
/** @brief Check if Code is correct
 *
 * Check if input code is the same as the secret code
 * @param p Count of pressed Buttons
 * @return Void
 */
{
  Serial.println("Function checkCode");
  int i;              //VarInputCode
  int correct = 0;    //VarCountPW
  for (i = 0; i < (k); i++)
  {
    if (inputCode[i] == secretCode[i])
    {
      correct++;
    }
  }
  reset(); //reset code-vector
  if ((correct == k) && (p == k))
  {
    accesgranted();
  }
  else
  {
    accesdenied();
  }
}

void accesgranted()
/** @brief Open the Door
 *
 *  Set the Green Led to HIGH and open the Door for 2 Sec.
 *  @return Void
 */
{
  Serial.println("Access granted");
  delay(1);
  digitalWrite(ledOpen, HIGH);
  analogWrite(doorPin, 255);
  delay(2000); //2sec
  digitalWrite(ledOpen, LOW);
  analogWrite(doorPin, 0);
}

void accesdenied()
/** @brief Leave the Door closed
 *
 * Set the Red LED to HIGH for 1 Sec.
 * @return Void.
 */
{
  Serial.println("Access denied");
  delay(1);
  digitalWrite(ledClose, HIGH);
  delay(1000);  //1sec
  digitalWrite(ledClose, LOW);
}

void reset()
/** @brief Reset Input-Array
 *
 * Reset InputCode-array by filling with Zeros
 * @return Void
 */
{
  Serial.println("Reset");
  int r;  //VarReset
  for (r = 0; r < (maxIN); ++r)
  {
    inputCode[r] = '0';
  }
}
