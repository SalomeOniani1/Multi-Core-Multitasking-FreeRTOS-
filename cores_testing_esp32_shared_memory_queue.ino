TaskHandle_t Task_Core0;
TaskHandle_t Task_Core1;
QueueHandle_t nQueue;


void codeForCore0(void * pvParameters) {
  for (;;) {
    int n = random(0, 100); // შემთხვევითი რიცხვის დაგენერირება

    // რიგში მონაცემის დამატება
    // xQueueSend( Queue, მონაცემის მისამართი, ლოდინის მაქს. დრო )
    BaseType_t result = xQueueSend(nQueue, &n, portMAX_DELAY);

    if (result == pdPASS) {
      Serial.print("[Core 0] data has been sent ");
      Serial.println(n);
    } else {
      Serial.println("[Core 0] errir: queue is full");
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void codeForCore1(void * pvParameters) {
  int receivedN;

  for (;;) {
    // რიგიდან მონაცემის წაკითხვა
    // xQueueReceive( Queue, სადაც ინახება წაკითხული, ლოდინის დრო )
    // portMAX_DELAY ნიშნავს, რომ დავალება "იძინებს", სანამ რიგი ცარიელია
    if (xQueueReceive(nQueue, &receivedN, portMAX_DELAY) == pdTRUE) {
      Serial.print(" [Core 1] data is received ");
      Serial.print(receivedN);
      Serial.println(" C");
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("--- FreeRTOS Queue testing ---");

  // რიგის შექმნა: 5 int ტიპის ელემენტი
  nQueue = xQueueCreate(5, sizeof(int));

  if (nQueue == NULL) {
    Serial.println("Error: Queue have not been created");
    return;
  }

  xTaskCreatePinnedToCore(
    codeForCore0,
    "SenderTask",
    4096,
    NULL,
    1,
    &Task_Core0,
    0
  );

 
  xTaskCreatePinnedToCore(
    codeForCore1,
    "ReceiverTask",
    4096,
    NULL,
    1,
    &Task_Core1,
    1
  );
}

void loop() {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}