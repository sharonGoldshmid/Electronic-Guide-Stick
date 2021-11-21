//תוספת- הפחתת עוצמת הרטט
#define trigPin1 8     //     10uS בהדק 8 נותנים את ה'דרבון' של 
#define echoPin1 2   //     )interrupt 0להדק 2  (  Echo חיבור מוצא החיישן 
#define led1 10    
#define trigPin2 9     //     10uS בהדק 8 נותנים את ה'דרבון' של 
#define echoPin2 3   //     )interrupt 0להדק 2  (  Echo חיבור מוצא החיישן 
#define led2 11 
unsigned long time_d=0;
int time1=0;
int time2=0;
int dis=30;
int dis1=20;
int dis2=10;
int d1=0;
int d2=0;
bool x1=true;
bool x2=true;
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
  Serial.begin(115200);
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
 if(millis()-time_d>100) {  //  50mSמדידת מרחק כל 
  Serial.print(distance1);     // הדפס את המרחק בס"מ
  Serial.print(" cm- distance1       ");     // ועבור שורה הדפס ס"מ
  Serial.print(distance2);     // הדפס את המרחק בס"מ
  Serial.println(" cm- distance2");     // ועבור שורה הדפס ס"מ
  trigUs1(); 
  trigUs2();
   time_d=millis();
 }
if(distance1>dis) 
{
    //analogWrite(led1,0);
    digitalWrite(led1,LOW);
    x1=true;
    time1=millis();
}
else
 {
  if(distance1>dis1) d1=1000;
  else if(distance1>dis2) d1=500;
  else d1=200;
    if (millis()-time1>d1)                        
    {
       if(x1)
       {
             //analogWrite(led1,80);
             digitalWrite(led1,HIGH);
             x1=false;
       }
       else
       {
        //analogWrite(led1,0);
        digitalWrite(led1,LOW);
             x1=true;
       }
       time1=millis();
    }
 }
 
if(distance2>dis) 
{
    //analogWrite(led2,0);
    digitalWrite(led2,LOW);
    x2=true;
    time2=millis();
}
else
 {
  if(distance2>dis1) d2=1000;
  else if(distance2>dis2) d2=500;
  else d2=200;
    if (millis()-time2>d2)                        
    {
       if(x2)
       {
             //analogWrite(led2,80);
             digitalWrite(led2,HIGH);
             x2=false;
       }
       else
       {
        //analogWrite(led2,0);
        digitalWrite(led2,LOW);
             x2=true;
       }
       time2=millis();
    }
 }
}
