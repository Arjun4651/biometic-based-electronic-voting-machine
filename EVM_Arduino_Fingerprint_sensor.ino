#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Define LCD address, column, row
SoftwareSerial mySerial(2, 3); // RX, TX for fingerprint sensor
const int buttonPin1 = 4;  
const int buttonPin2 = 5; 
const int buttonPin3 = 6; 
const int buzzer = 7;

int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

int id = 0, previous_voter_id = 0, vote_taken = 0;
int party_1_count = 0, party_2_count = 0, party_3_count = 0;
String winner_name = "";

void setup()  
{
  pinMode(buzzer, OUTPUT);
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  while (!Serial); // Wait for serial to be ready

  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // Set data rate for fingerprint sensor
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); 
  Serial.println(" templates");
  
  lcd.clear(); 
  lcd.setCursor(0, 0);  
  lcd.print("Smart Electronic"); 
  lcd.setCursor(0, 1);
  lcd.print("Voting Machine");
  delay(3000);
}

void loop()  
{
  vote_taken = 0;
  lcd.clear(); 
  lcd.setCursor(0, 0);  
  lcd.print("Please place your"); 
  lcd.setCursor(0, 1);
  lcd.print("finger");

  delay(100);
  id = getFingerprintIDez(); // Get fingerprint ID

  if(id > 0)
  {
    lcd.clear(); 
    lcd.setCursor(0, 0);  
    lcd.print("Your Voter ID"); 
    lcd.setCursor(0, 1);
    lcd.print(id);
    delay(2000);

    if(id == 4)
    {
      winner_name = getWinner();
      displayWinner(winner_name);
      while(1); // End the loop after showing the winner
    }

    if(previous_voter_id != id)
    {
      handleVoting();
    }
    else
    {
      handleDuplicateVote();
    }
  }
}

void handleVoting()
{
  vote_taken = 0;
  do
  {
    lcd.clear();
    lcd.setCursor(0, 0);  
    lcd.print("Give Your vote"); 
    lcd.setCursor(0, 1);
    lcd.print("Press Button");
    
    delay(500);
    previous_voter_id = id;
    buttonState1 = digitalRead(buttonPin1);
    buttonState2 = digitalRead(buttonPin2);
    buttonState3 = digitalRead(buttonPin3);

    if (buttonState1 == HIGH)
    {
      party_1_count++;
      vote_taken = 1;
    } 
    else if(buttonState2 == HIGH)
    {
      party_2_count++;
      vote_taken = 1;
    }
    else if(buttonState3 == HIGH)
    {
      party_3_count++;
      vote_taken = 1;
    }

    if(vote_taken == 1)
    {
      lcd.clear(); 
      lcd.setCursor(0, 0);  
      lcd.print("Thanks for your"); 
      lcd.setCursor(0, 1);
      lcd.print("vote");
      delay(200);  
      digitalWrite(buzzer, HIGH);   
      delay(1000);                       
      digitalWrite(buzzer, LOW);    
      delay(1000);
    }
  } while(vote_taken == 0);
}

void handleDuplicateVote()
{
  lcd.clear(); 
  lcd.setCursor(0, 0);  
  lcd.print("Duplicate Vote"); 
  lcd.setCursor(0, 1);
  lcd.print("Buzzer On");
  delay(2000);

  for(int i = 0; i < 3; i++) {
    digitalWrite(buzzer, HIGH);
    delay(1000);
    digitalWrite(buzzer, LOW);
    delay(1000);
  }
}

String getWinner()
{
  if(party_1_count > party_2_count && party_1_count > party_3_count)
    return "BJP";
  else if(party_2_count > party_1_count && party_2_count > party_3_count)
    return "NCP";
  else
    return "Congress";
}

void displayWinner(String winner)
{
  lcd.clear(); 
  lcd.setCursor(0, 0);  
  lcd.print("Winner Party:"); 
  lcd.setCursor(0, 1);
  lcd.print(winner);
}

uint8_t getFingerprintIDez() 
{
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}
