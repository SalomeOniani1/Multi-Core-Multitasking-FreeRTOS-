#define LED_CORE_0 4
#define LED_CORE_1 5

TaskHandle_t Task_Core0;
TaskHandle_t Task_Core1;
SemaphoreHandle_t xMutex;

int c = 0;

// Core 0-ზე გასაშვები ფუნქცია
void codeForCore0(void * pvParameters) {
  pinMode(LED_CORE_0, OUTPUT);
  
  for (;;) { 
    digitalWrite(LED_CORE_0, HIGH);
    vTaskDelay(200 / portTICK_PERIOD_MS); 
    digitalWrite(LED_CORE_0, LOW);
    vTaskDelay(200 / portTICK_PERIOD_MS);

    //Mutex-ს (ჩაკეტვა)
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
      c++;
      Serial.print("Task Core 0: ");
      Serial.println(xPortGetCoreID());
      xSemaphoreGive(xMutex); // Mutex-ის გათავისუფლება
    }
  }
}

// Core 1-ზე გასაშვები ფუნქცია
void codeForCore1(void * pvParameters) {
  pinMode(LED_CORE_1, OUTPUT);
  
  for (;;) {
    digitalWrite(LED_CORE_1, HIGH);
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
    digitalWrite(LED_CORE_1, LOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
      c++;
      Serial.print("Task Core 1: ");
      Serial.println(xPortGetCoreID());
      xSemaphoreGive(xMutex);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("--- FreeRTOS Multi-Core testing ---");
  
  xMutex = xSemaphoreCreateMutex(); // Mutex-ის ინიციალიზაცია

  // დავალების შექმნა და Core 0-ზე მიმაგრება
  //FreeRTOS-ის მთავარი ფუნქცია, რომლითაც დავალებას ვანიჭებთ კონკრეტულ ბირთვს
  xTaskCreatePinnedToCore(
    codeForCore0,   /* ფუნქციის სახელი */
    "Task_Core0",   /* დავალების სახელი */
    4096,           /* სტეკის ზომა (ბაიტებში) */
    NULL,           /* პარამეტრი */
    1,              /* პრიორიტეტი*/
    &Task_Core0,    /* დავალების მიმთითებელი */
    0               /* ბირთვის ID: 0 */
  );

  // დავალების შექმნა და Core 1-ზე მიმაგრება
  xTaskCreatePinnedToCore(
    codeForCore1,   /* ფუნქციის სახელი */
    "Task_Core1",   /* დავალების სახელი */
    4096,           /* სტეკის ზომა (ბაიტებში) */
    NULL,           /* პარამეტრი */
    1,              /* პრიორიტეტი */
    &Task_Core1,    /* დავალების მიმთითებელი */
    1               /* ბირთვის ID: 1 */
  );
}

void loop() {
  // loop() ავტომატურად გაშვეულია Core 1-ზე, მაგრამ FreeRTOS-ის დროს 
  // ძირითადი ლოგიკა დავალებებში გადაგვაქვს, ამიტომ loop-ს ცარიელი ვტოვებთ.
  //vTaskDelay(): delay()-ისგან განსხვავებით, პროცესორს არ აიძულებს გაჩერებას, 
  //არამედ ეუბნება FreeRTOS-ის დამგეგმავს, რომ ამ პერიოდში პროცესორის რესურსი 
  //სხვა დავალებებს დაუთმოს.
  vTaskDelay(1000 / portTICK_PERIOD_MS); 
  // Mutex-ით ვკითხულობთ c-ს
  if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
    Serial.print("c = ");
    Serial.println(c);
    xSemaphoreGive(xMutex);
  } 
  
}