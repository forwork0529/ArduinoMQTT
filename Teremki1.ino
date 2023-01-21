#include <ESP8266WiFi.h>
#include <PubSubClient.h>

int buildInLED = 2;

const char* ssid = ""; // 
const char* password = ""; // 

const char* mqtt_server = "node02.myqtthub.com";  // сервер mqtt
const char* mqttUser = "Teremki1";              // 
const char* mqttPassword = "12345678";   // 
const char* mqttClientId = "Teremki1";  // 


// Callback function header Сигнатура функции выполняемой при получениии сообщения
void callback(char* topic, byte* payload, unsigned int length);

WiFiClient espClient;     //  Создали объект типа wi-fi клиент из импортированной библиотеки
PubSubClient client(mqtt_server, 1883, callback, espClient);     // Создали объект общающийся с mqtt, передали в него сервер, порт, нашу функцию и нашего сетевого клиента


void setup_wifi() {                 // Функция подключения по wi-fi

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void Blink(int repeat){

  for (int i = 0; i < repeat; i++){
    digitalWrite(buildInLED, LOW);
    delay(500);
    digitalWrite(buildInLED, HIGH);
    delay(500);
  }
}

// Реализация нашей функции, она будет выполнена при получении сообщения
void callback(char* topic, byte* payload, unsigned int length) {  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String messageTemp;  // Наша функция по умолчанию принимает массив байт - ,
  // мы создали переменную строка и далее в цикле переделываем её в строку..
    for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }

    if (String(topic) == "command") {
    // Serial.print("Swithcing to");
    if(messageTemp == "reset"){
      Serial.println("command reset");
      Blink(3);
    } 
    else if(messageTemp == "start"){
    Serial.println("command start");
    Blink(6);
    }
  }
    
  //turn the LED OFF if the payload is '0' and publish to the MQTT server a confirmation message
  //if (payload[0] == '0'){
    //digitalWrite(lightPin, LOW);
    //client.publish("outTopic", "Light Off");
    //}
} // void callback 


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(mqttClientId, mqttUser, mqttPassword)) {
      Serial.println("connected");
      client.subscribe("command");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


bool Need(){
  long time = millis();
  if (time % 180000 == 1){ // 300000 = 5 минут
    return true;
  }
  return false;
}

void setup() { 
  pinMode(buildInLED, OUTPUT);
  digitalWrite(buildInLED, HIGH); // подавая high выключаем светодиод так как выход и светодиод инвертированы
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
   if (Need()){
      Serial.println("I am publishing");
  client.publish("temperatures", "76 55 55 44 33 25");
  }   
 
  client.loop();
}