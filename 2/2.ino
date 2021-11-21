//מחוברים שני חיישנים ללדים המהבהבים במהירות גבוה יותר ככל שהמרחק קטן יותר
//בעיה:האתר הגדול הפתרון:שנינו את התנאי כך שהוספנו פונקציה דילאי בה השתמשנו באובייקט שנוצר מהכפלה של קבוע במרחק 
//בעיה: הלדים לא יכולים לדלוק בו זמנית בגלל הדילאי.
 (קובץ 3)//פיתרון- פסיקה פנימית
#define trigPin1 8     //     10uS בהדק 8 נותנים את ה'דרבון' של 
#define echoPin1 2   //     )interrupt 0להדק 2  (  Echo חיבור מוצא החיישן 
#define led1 11    
#define trigPin2 9     //     10uS בהדק 8 נותנים את ה'דרבון' של 
#define echoPin2 3   //     )interrupt 0להדק 2  (  Echo חיבור מוצא החיישן 
#define led2 12 
unsigned long time=0;
unsigned long duration1;  // משתנה עבור משך הזמן של רוחב הפולס 
unsigned long duration2;  // משתנה עבור משך הזמן של רוחב הפולס 
int distance1=0;      // משתנה עבור המרחק בס"מ
int distance2=0;      // משתנה עבור המרחק בס"מ

//--------------   של החיישן trigger   במבוא 10uS 'דרבון' של נותנת trigUs()   פונקציה     ---------------
void trigUs1()   {
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  attachInterrupt(0, startCount1 ,RISING);  // startCount () : יש לבצע כאשר יש 'עליה' בהדק 2,
}
void trigUs2()   {
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  attachInterrupt(1, startCount2 ,RISING);  // startCount () : יש לבצע כאשר יש 'עליה' בהדק 2,
}
//----------------מאפשרת את מדידת הזמן      startCount()   פונקציה     ----------------------
void startCount1()     {
  duration1=micros();  //  duration קרא זמן נוכחי למשתנה  
  detachInterrupt(0);  // חסום פסיקה מספר 0 . 
  attachInterrupt(0,  measurement1, FALLING); // measurement()  : בצע כאשר יש 'ירידה' ,
}   
void startCount2()     {
  duration2=micros();  //  duration קרא זמן נוכחי למשתנה  
  detachInterrupt(1);  // חסום פסיקה מספר 0 . 
  attachInterrupt(1,  measurement2, FALLING); // measurement()  : בצע כאשר יש 'ירידה' ,
}           
//----------- מודדת את הזמן ומחשבת את המרחק      measurement()פונקציה       --------------
void measurement1()
{
  duration1=micros()-duration1;  // ההפרש בין הזמן העכשוי (ב 'ירידה') לזמן הקודם (ב 'עליה')
  distance1=duration1/58;      // distance והצבה למשתנה  חישוב המרחק בס"מ
  detachInterrupt(0);      //   חסום פסיקה מספר 0 . 

}
void measurement2()
{
  duration2=micros()-duration2;  // ההפרש בין הזמן העכשוי (ב 'ירידה') לזמן הקודם (ב 'עליה')
  distance2=duration2/58;      // distance והצבה למשתנה  חישוב המרחק בס"מ
  detachInterrupt(1);      //   חסום פסיקה מספר 0 . 

}
void setup()
{
  Serial.begin(9600);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(led1, OUTPUT);
    pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(led2, OUTPUT);
   trigUs1();
     trigUs2();
}
void loop()
{
 if(millis()-time>100) {  //  50mSמדידת מרחק כל 
  Serial.print(distance1);     // הדפס את המרחק בס"מ
  Serial.print(" cm- distance1       ");     // ועבור שורה הדפס ס"מ
  Serial.print(distance2);     // הדפס את המרחק בס"מ
  Serial.println(" cm- distance2");     // ועבור שורה הדפס ס"מ
  trigUs1(); 
  trigUs2();
   time=millis();
 }
if (distance1<10 )                        // אם המרחק קטן מ 15 ס"מ
     digitalWrite(led1,HIGH);
     delay(distance1*10);     
     digitalWrite(led1,LOW);
     delay(distance1*10); 
if (distance2<10)                        
     digitalWrite(led2,HIGH);
     delay(distance2*10);     
     digitalWrite(led2,LOW);
     delay(distance2*10); 
        }
