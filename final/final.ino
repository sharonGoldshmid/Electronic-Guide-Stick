#include <Servo.h> //ספריית מנוע סרוו
#include <Wire.h> //ספריית I2C

Servo myservo;   //אובייקט של המחלקה סרוו עבור המנוע
int servoPin = 5;

const int MPU_addr = 0x68;   //כתובת הרכיב MPU6050
double AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ; //משתנים עבור הנתונים שנקרא מהרכיב
unsigned long timer;  //טיימר עבור מדידת הזמן בין חישובי הזווית  
double compAngleX, compAngleY;  //משתנים עבור הזוויות שנחשב 
#define degconvert 57.2957786   //המרה מרדיאנים למעלות 

#define trigPin 4 
#define echoPin 2   //בהדק זה מתאפשרת תוכנית פסיקה
#define led 3   //הדק מנוע הרטט
unsigned long timer_retet=0;  //טיימר עבור יצירת פעימות רטט
int rate_retet=0;  //משתנה לשמירת תדירות הרטט כתלות במרחק
unsigned long duration;   //משתנה עבור משך רוחב הפולס
int distance=0;      //משתנה עבור המרחק הנמדד
//==============================================================
unsigned long timer_distance=millis();  //טיימר עבור מדידת המרחק


//הפונקציה נותנת דרבון 10us במבוא trigger של החיישן
void trigUs()   {
  digitalWrite(trigPin, HIGH);   //יצירת דרבון
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  attachInterrupt(0, startCount ,RISING);  // startCount () כאשר יש עליה בהדק 2 בצע
}

//הפונקציה מאפשרת את מדידת הזמן
void startCount()     {
  duration=micros();   //התחלת רוחב הפולס
  detachInterrupt(0);  // חסום פסיקה מספר 0
  attachInterrupt(0,  measurement, FALLING); // measurement() כאשר יש ירידה בהדק 2 בצע
}   
         
//הפונקציה מודדת את הזמן ומחשבת את המרחק
void measurement()
{
  duration=micros()-duration;   //חישוב רוחב הפולס
  distance=duration/58;      // חישוב המרחק בס"מ
  detachInterrupt(0);      //   חסום פסיקה מספר 0
}


void vibrate()  {   //פונקציה המפעילה את מנוע הרטט בקצב ביחס למרחק
  if ( millis() - timer_retet > 50)   //עדכן את הקצב כל 50ms
  {
    rate_retet =  map(distance, 20, 200, 100, 800);
    timer_retet = millis();
  }
  
  static unsigned long vibrationTimer = 0;
  if (millis() - vibrationTimer < rate_retet)  //בדיקה האם אנחנו באותו מחזור
  {
    if (millis() - vibrationTimer < 100)  //בדיקה האם להדליק או לכבות
      analogWrite(led, 100);
    else
      analogWrite(led, 0);
  }
  else
    vibrationTimer = millis();
}

void servoLoop()  {   //איסוף הנתונים כל פעם מחדש- תוכנית גיירו

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B); 
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true);
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B & 0x3C
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D & 0x3E
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F & 0x40
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 & 0x42
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 & 0x44
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 & 0x46
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 & 0x48

  double dt = (double)(micros() - timer) / 1000000;  //חישוב הזמן. המרה ממיקרו שניות לשניות
  timer = micros(); //הפעלת הטיימר מחדש לחישוב הdt   

  double roll = atan2(AcY, AcZ) * degconvert;
  double pitch = atan2(-AcX, AcZ) * degconvert;

  double gyroXrate = GyX / 131.0;  //המרת הנתון ממספר דיגיטלי ליחידה של מעלה
  double gyroYrate = GyY / 131.0;

  compAngleX = 0.99 * (compAngleX + gyroXrate * dt) + 0.01 * roll; //חישוב הזווית
  compAngleY = 0.99 * (compAngleY + gyroYrate * dt) + 0.01 * pitch;

  int servoAngle = compAngleX + 90;
  if(servoAngle > 160)  //תנאי להגבלת הזוית כך שיתאים להגבלות הקופסא
    myservo.write(160);
  else if(servoAngle < 20)
    myservo.write(20);
  myservo.write(servoAngle);
}

void setup() {
  myservo.attach(servoPin);   //הגדרות ואתחול מנוע
  myservo.write(90);
  

  //הגדרות ואתחול- גיירו
  Wire.begin(); //אתחול יחידת הI2C של הארדואינו  
  Wire.setClock(400000UL);   //קובע את תדירות התקשורת ל 400kHz
  Wire.beginTransmission(MPU_addr); //התחלת תקשורת עם החיישן - מקבל את הכתובת לשידור
  Wire.write(0x6B);  //פניה לרגיסטר 6B .
  Wire.write(0);     //איפוס הרגיסטר
  Wire.endTransmission(true); //סיום התקשורת - שחרור הקו ע"י שליחת stop
  Serial.begin(115200); //קבע את קצב השידור ל115200 סיביות בשניה
  delay(100);

  Wire.beginTransmission(MPU_addr); //התחלת תקשורת עם החיישן
  Wire.write(0x3B);  //פנייה לרגיסטר 3B לצורך התחלת קריאת נתונים
  Wire.endTransmission(false); //הפסקת השידור, בלי להפסיק את התקשורת- אין שליחת stop
  Wire.requestFrom(MPU_addr, 14, true); 
  //גישה לבתים בכתובת החיישן, 14 קריאות, והפסקת התקשורת בסיום
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B & 0x3C
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D & 0x3E
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F & 0x40
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 & 0x42
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 & 0x44
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 & 0x46
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 & 0x48

  double roll = atan2(AcY, AcZ) * degconvert;
  double pitch = atan2(-AcX, AcZ) * degconvert;

  compAngleX = roll;
  compAngleY = pitch;

  timer = micros(); //הפעלת הטיימר לחישוב הdt   


  pinMode(trigPin, OUTPUT);   //הגדרות ואתחול חיישן מרחק
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  trigUs();   //דרבון ראשוני של 10us ואפשור פסיקה
  //=====================================================
  timer_distance=millis();
}

void loop() {
    if ( millis() - timer_distance > 50)   //מדידת מרחק כל 50ms
    {
      trigUs();   //דרבון ואפשור פסיקה מחדש
      timer_retet = millis();
    }
    servoLoop();   //תוכנית גיירו  
    vibrate();  //הפעלת רטטים
}
