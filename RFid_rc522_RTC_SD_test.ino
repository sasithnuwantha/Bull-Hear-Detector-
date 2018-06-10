#include <RFID.h>

/*
* Read a card using a mfrc522 reader on your SPI interface
* Pin layout should be as follows (on Arduino Uno):
* MOSI: Pin 11 / ICSP-4
* MISO: Pin 12 / ICSP-1
* SCK: Pin 13 / ISCP-3
* SS/SDA: Pin 10
* RST: Pin 9
*/

#include <SPI.h>
#include <RFID.h>
#include <DS3231.h>
#include <SD.h>

File myFile;

#define SS_PIN 10
#define RST_PIN 9

RFID rfid(SS_PIN,RST_PIN);

int pinCS = 4; // Pin 10 on Arduino Uno
int led = 7;
int power = 8; 
int serNum[5];
int cards[][5] = {
  {136,4,2,157,19}
  
};

bool access = false;

DS3231  rtc(SDA, SCL);

void setup()
{

    Serial.begin(9600);
    SPI.begin();
    rfid.init();
    rtc.begin();

    pinMode(led, OUTPUT);
    pinMode(pinCS, OUTPUT);
    digitalWrite(led, LOW);

    // SD Card Initialization
  if (SD.begin())
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return;
  }
  
  // Create/Open file 
  myFile = SD.open("test.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.println("Writing to file...");
    // Write to file
    myFile.println("Testing RFID with SD at once");
    myFile.close(); // close the file
    Serial.println("Done.");
  }
  // if the file didn't open, print an error:
  else {
    Serial.println("error opening test.txt");
  }

  // Reading the file
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("Read:");
    // Reading the whole file
    while (myFile.available()) {
      Serial.write(myFile.read());
   }
    myFile.close();
  }
  else {
    Serial.println("error opening test.txt");
  }
}

void loop(){
    
    if(rfid.isCard()){
    
        if(rfid.readCardSerial()){
            Serial.print(rfid.serNum[0]);
            Serial.print(" ");
            Serial.print(rfid.serNum[1]);
            Serial.print(" ");
            Serial.print(rfid.serNum[2]);
            Serial.print(" ");
            Serial.print(rfid.serNum[3]);
            Serial.print(" ");
            Serial.print(rfid.serNum[4]);
            Serial.println("");
            
            for(int x = 0; x < sizeof(cards); x++){
              for(int i = 0; i < sizeof(rfid.serNum); i++ ){
                  if(rfid.serNum[i] != cards[x][i]) {
                      access = false;
                      break;
                  } else {
                      access = true;
                  }
              }
              if(access) break;
            }
           
        }
        
       if(access){
          Serial.println("Welcome!");
           digitalWrite(led, HIGH); 
           delay(1000);
           digitalWrite(led, LOW);
           digitalWrite(power, HIGH);
           delay(1000);
           digitalWrite(power, LOW);
           
      } else {
           Serial.println("Not allowed!"); 
           digitalWrite(led, HIGH);
           delay(500);
           digitalWrite(led, LOW); 
           delay(500);
           digitalWrite(led, HIGH);
           delay(500);
           digitalWrite(led, LOW); 

           // Send Day-of-Week
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  
  // Send date
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");

  // Send time
  Serial.print(rtc.getTimeStr());
  
  //Get temp
  Serial.print("  Temperature: ");
  Serial.print(rtc.getTemp());
  Serial.println(" C");
       }        
    }
    
    
    
    rfid.halt();

}


