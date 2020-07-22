// Incluindo a biblioteca do sensor de distância                      
#include <Ultrasonic.h>

// Definindo os pinos do sensor de distância
#define PIN_TRIGGER 2
#define PIN_ECHO 3

// Definindo os pinos do sensor do nivel de alcool
#define PIN_MQ_ANALOG A3

// Definindo os pinos do relay
#define PIN_RELAY_VERMELHO 11
#define PIN_RELAY_VERDE 12

// Definindo parametros                                     
#define MAX_DISTANCE 40 // Distância maxima aceita
#define WARM_UP_TIME 3 // 300 // Tempo de aquecimento do sensor de alcool em segundos
#define TIME_BETWEEN_READS 100 // Tempo de espera entre leituras consecutivas, em milisegundos
#define MIN_WASH_TIME 10 // Tempo minimo de lavado das mãos em segundos
#define NUMBER_OF_READS 3 // Número de leituras tomas para calcular a medias
#define SENSIVILITY 20 // Diferença entre medias consecutivas que caraterizam uma subida ou descida do nicel de alcool
#define TOLERANCE 2 // Valor aceito como decresimo no nivel de alcool

// Variaveis obtidas na leitura
float vectorLatestDistance[NUMBER_OF_READS];
int vectorLatestAlcoholReads[NUMBER_OF_READS];
float lastDistance, averageDistance;
int lastAlcoholRead;
float averageAlcohol, penultimateAverageAlcohol;
int washTime = 0;
int isActivate = 0;

// Criar objeto do sensor de distância
Ultrasonic ultrasonic(PIN_TRIGGER, PIN_ECHO);

// Escrevendo mensagem de inicial
void initDisp () {
  Serial.println();
  Serial.print("Iniciando dispositivos ");
  for(int i = 0; i < 3; i++){
    delay(500);
    Serial.print(".");
  }
  Serial.println();
}

// Escrevendo mensagem de espera durante o tempo de aquecimento do sensor
void warmMQSensor () {
  for (int i = 0; i < WARM_UP_TIME / 3; i++) {
    Serial.print("Aquecendo sensor ");
    for (int i = 0; i < 3; i++) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println();
  }
}
  
// Escrevendo mensagem de inicio de leituras
void initRead () {
  Serial.print("Iniciando leituras ");
  for(int i = 0; i < 3; i++){
    delay(500);
    Serial.print(".");
  }
  Serial.println();
}

// Mensage de conteio de tempo de igienização das mãos
void washedContent() {
    Serial.print("Tempo de higienização de ");
    Serial.print(washTime);
    Serial.print("s de ");
    Serial.print(MIN_WASH_TIME);
    Serial.println("s");
}

// Escrever distância
void printDistance (float distance) {
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println("cm");
}

// Escrever valor do sensor de nivel de acool
void printAlcohol (int alcohol) {
  Serial.print("Nível de alcool: ");
  Serial.println(alcohol);
}

// Ligar a lampada verde
void onGreenLed () {
  digitalWrite(PIN_RELAY_VERDE, HIGH);
}

// Ligar a lampada vermelha
void onRedLed () {
  digitalWrite(PIN_RELAY_VERMELHO, HIGH);
}

// Desligar a lampada verde
void offGreenLed () {
  digitalWrite(PIN_RELAY_VERDE, LOW);
}

// Desligar a lampada vermelha
void offRedLed () {
  digitalWrite(PIN_RELAY_VERMELHO, LOW);
}

// Função para piscar a lampada verde
void blinkGreenLed () {
  onGreenLed();
  Serial.println("------------------------------");
  Serial.println("Lampada verde");
  delay(3000);
  offGreenLed();
  delay(2000);
}

// Função para piscar a lampada vermelha
void blinkRedLed () {
  onRedLed();
  Serial.println("------------------------------");
  Serial.println("Lampada vermelha");
  delay(3000);
  offRedLed();
  delay(2000);
}

// Função para medir a distância (otimizar)
float readDistance () {
  long microsec = ultrasonic.timing();
  return ultrasonic.convert(microsec, Ultrasonic::CM);
}

// Função para iniciar os vetores com todos os valores iguais a zero
void initVectorInt (int *v) {
  for (int i = 0; i < NUMBER_OF_READS; i++) {
    v[i] = 0;
  }
}


// Função para iniciar os vetores com todos os valores iguais a zero
void initVectorFloat (float *v) {
  for (int i = 0; i < NUMBER_OF_READS; i++) {
    v[i] = 0;
  }
}

// Função para calcular a média dos valores contidos nos vetores
float calculateAverageInt (int *v) {
  float average = 0;
  for (int i = 0; i < NUMBER_OF_READS; i++) {
    average += v[i];
  }
  return average/NUMBER_OF_READS;
}

// Função para calcular a média dos valores contidos nos vetores
float calculateAverageFloat (float *v) {
  float average = 0;
  for (int i = 0; i < NUMBER_OF_READS; i++) {
    average += v[i];
  }
  return average/NUMBER_OF_READS;
}

// Função para realizar um conjunto de leituras de distância e calcular a media delas
void enterDataDistance() {
  for (int i = 0; i < NUMBER_OF_READS; i++) {
    lastDistance = readDistance();
    
    vectorLatestDistance[i] = lastDistance;
    printDistance(lastDistance);

    delay(150);
  }

  averageDistance = calculateAverageFloat(vectorLatestDistance);
}

// Função para realizar um conjunto de leituras de nivel de alcool e calcular a media delas
void enterDataAlcohol() {
  for (int i = 0; i < NUMBER_OF_READS; i++) {
    lastAlcoholRead = analogRead(PIN_MQ_ANALOG);
  
    vectorLatestAlcoholReads[i] = lastAlcoholRead;
    printAlcohol(lastAlcoholRead);
    
    delay(150);
  }

  averageAlcohol = calculateAverageInt(vectorLatestAlcoholReads);
}

// Função para ler todos os dados
void enterData () {
  Serial.println("------------------------------");
  enterDataDistance();
  enterDataAlcohol();
}

// Função que estabelece se o usuário está ou não higienizando as mãos
/*int isWashing () {
  if ( averageAlcohol > penultimateAverageAlcohol*(1.0 + SENSIVILITY) ) {
    return 1;    
  }
  if ( averageAlcohol > penultimateAverageAlcohol*(1.0 - TOLERANCE) ) {
    if ( washTime > 0 ) {  
      return 1;
    }
  }
  return 0;
}*/

// Função que estabelece se o usuário está ou não higienizando as mãos
int isWashing () {
  if ( averageAlcohol - penultimateAverageAlcohol > SENSIVILITY ) {
    return 1;    
  }
  if ( penultimateAverageAlcohol - averageAlcohol <= TOLERANCE ) {
    if ( washTime > 0 ) {  
      return 1;
    }
  }
  return 0;
}

void setup() {
  // Definindo o modo dos pinos
  pinMode(PIN_RELAY_VERMELHO, OUTPUT);
  pinMode(PIN_RELAY_VERDE, OUTPUT);

  // Definindo a porta do monitor serial
  Serial.begin(9600);

  initVectorFloat(vectorLatestDistance);
  initVectorInt(vectorLatestAlcoholReads);

  initDisp();
  warmMQSensor(); 
  initRead();

  enterDataAlcohol();
  penultimateAverageAlcohol = averageAlcohol;
}

void loop() {
  Serial.println("------------------------------");
  enterDataDistance();
  while ( averageDistance < MAX_DISTANCE && washTime < MIN_WASH_TIME  ) {
    enterData();
    if ( isWashing() ) {
      washTime++;
    }
    delay(TIME_BETWEEN_READS);
    penultimateAverageAlcohol = averageAlcohol;
    isActivate = 1;
    washedContent();
  }

  if ( washTime != MIN_WASH_TIME  ) {
    if ( isActivate == 1 ) {
      blinkRedLed();
    }
  }

  isActivate = 0;
  
  if ( washTime == MIN_WASH_TIME ) {
    blinkGreenLed();
  }
  
  washTime = 0;
  
  delay(TIME_BETWEEN_READS);
}
