#include <Servo.h> //ספריית מנוע סרוו
#include <Wire.h> //ספריית I2C

Servo myservo; //אובייקט עבור המנוע

const int MPU_addr = 0x68; //כתובת החישן MPU6050
double AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ; //משתנים עבור הנתונים במד התאוצה
uint32_t timer; //טיימר בשביל לחסוך את השימוש בפקודה דילאי
double compAngleX, compAngleY; //משתנים עבור הזוויות שנחשב
#define degconvert 57.2957786 //המרה מרדיאנים למעלות


void setup() {
  //הגדרות ואתחול
  myservo.attach(9); //התאמת המנוע להדק 9
  Wire.begin(); //אתחול יחידת הI2C של הארדואינו
  Wire.setClock(400000UL); // Set I2C frequency to 400kHz
  Wire.beginTransmission(MPU_addr); //התחלת תקשורת עם החיישן
  Wire.write(0x6B);  //פניה לרגיסטר 6B .
  //עמוד 40 ב MPU-6000-Register-Map1.pdf בדרייב
  Wire.write(0);     //איפוס הרגיסטר
  //האיפוס קבע ערך קבוע בשעון הפנימי, השבתת RESET, מצב שינה, ומד טמפרטורה, והאפשרות להשתמש במצב מחזור
  Wire.endTransmission(true); //סיום התקשורת
  Serial.begin(115200); //קבע את קצב התקשורת ל115200 סיביות בשניה
  delay(100);

  //הגדרת זווית התחלתית
  //שלב 1: איסוף נתונים
  Wire.beginTransmission(MPU_addr); //התחלת תקשורת עם החיישן
  Wire.write(0x3B);  //פנייה לרגיסטר 3B
  //עמוד 29 ב MPU-6000-Register-Map1.pdf בדרייב
  //הרגיסטרים 3B-48 מכילים את הנתונים. כל נתון (16 סיביות) בשני רגיסטרים
  Wire.endTransmission(false); //הפסקת השידור, בלי להפסיק את התקשורת
  Wire.requestFrom(MPU_addr, 14, true); //גישה לבתים בכתובת החיישן, 14 קריאות, והפסקת התקשורת בסיום
  //כתובת החיישן ממנו נבקש את הבתים
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  //שלב 2: calculate pitch and roll
  double roll = atan2(AcY, AcZ) * degconvert;
  double pitch = atan2(-AcX, AcZ) * degconvert;
  //צריך לבדוק מאיפה הנוסחה הספציפית הזאת ^

  //3) set the starting angle to this pitch and roll
  double gyroXangle = roll;
  double gyroYangle = pitch;
  double compAngleX = roll;
  double compAngleY = pitch;

  //start a timer
  timer = micros();
}

unsigned long mTime = 0;

void loop() {
  //איסוף הנתונים כל פעם מחדש
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B); 
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true);
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  double dt = (double)(micros() - timer) / 1000000; //שמירת הזמן שחושב ע"י הטיימר, המרה ממיקרו שניות לשניות
  timer = micros(); //הפעלת הטיימר מחדש לחישוב הdt

  //the next two lines calculate the orientation of the accelerometer relative to the earth and convert the output of atan2 from radians to degrees
  //We will use this data to correct any cumulative errors in the orientation that the gyroscope develops.
  //חישוב כיוון מד התאוצה ביחס לכדו"א והמרה למעלות. החישוב שימש לתיקון שגיאות מצטברות בחישוב של הגיירוסקופ
  double roll = atan2(AcY, AcZ) * degconvert;
  double pitch = atan2(-AcX, AcZ) * degconvert;
  //הנוסחאות בעמוד 10 ב Accelerometer.pdf בדרייב
  //למה מותר להציב בנוסחה את הערכים ממד התאוצה במקום מהגיירו?

  //The gyroscope outputs angular velocities.  To convert these velocities from the raw data to deg/second, divide by 131.
  //Notice, we're dividing by a double "131.0" instead of the int 131.
  double gyroXrate = GyX / 131.0; //המרת הנתון ממספר דיגיטלי ליחידה של שנייה
  //לא מצאתי את ההסבר למה 131...
  double gyroYrate = GyY / 131.0;

  //THE COMPLEMENTARY FILTER
  //This filter calculates the angle based MOSTLY on integrating the angular velocity to an angular displacement.
  //dt, recall, is the time between gathering data from the MPU6050.  We'll pretend that the
  //angular velocity has remained constant over the time dt, and multiply angular velocity by
  //time to get displacement.
  //The filter then adds a small correcting factor from the accelerometer ("roll" or "pitch"), so the gyroscope knows which way is down.
  //משתמשים בשני החישובים כדי לצמצם את הסטיות שעלולות להצטבר עם הזמן
  //אפשר לבחור באחוזים אחרים- זה ענין של ניסוי וטעייה
  //החישוב בתאוצה מדויק יותר אבל לוקח יותר זמן- הוא משמש בתור הפילטר לתיקון הסטייה
  //החישוב בגיירו מהיר יותר אבל עלול לצבור סטיות עם הזמן
  //הזמן מאיסוף הנתונים- dt, מוכפל במהירות הזוויתית (נניח שהמהירות נותרה קבועה בזמן הזה
  compAngleX = 0.99 * (compAngleX + gyroXrate * dt) + 0.01 * roll; //חישוב הזווית
  compAngleY = 0.99 * (compAngleY + gyroYrate * dt) + 0.01 * pitch;

  Serial.print(compAngleX); Serial.print("\t"); //הדפסת הזוויות (לא חובה)
  Serial.print(compAngleY); Serial.print("\n");
  myservo.write(compAngleX + 90); //מצב האפס של המנוע הוא 90 מעלות
}
