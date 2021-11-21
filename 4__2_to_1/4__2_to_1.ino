#define trigPin 4 
#define echoPin 2   //בהדק זה מתאפשרת תוכנית פסיקה
#define led 3
unsigned long duration;   //משתנה עבור משך רוחב הפולס
int distance=0;      //משתנה עבור המרחק הנמדד
unsigned long timer=0;   //טיימר עבור מדידה כל זמן קבוע

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


void setup() {
  Serial.begin(9600); //קבע את קצב השידור ל115200 סיביות בשניה
  pinMode(trigPin, OUTPUT);   //הגדרות ואתחול חיישן מרחק
  pinMode(echoPin, INPUT);
  trigUs();   //דרבון ראשוני של 10us ואפשור פסיקה
}

void loop() {
  if(millis()-timer > 50) { // 50mS מדידת מרחק כל
    Serial.print(distance);  // הדפס את המרחק בס"מ
    Serial.println(" cm");   //הדפס ס"מ ועבור שורה
    trigUs();   // 10uS יצירת 'דרבון' של
    timer=millis();
  }
  if (distance<15 ) // אם המרחק קטן מ 15 ס"מ
    digitalWrite(led,HIGH); // הדלק את הלד
  else
    digitalWrite(led,LOW); // כבה את הלד
}
