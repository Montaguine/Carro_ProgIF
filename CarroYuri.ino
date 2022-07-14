#include <Ultrasonic.h> //Biblioteca para o ultrasom. Deve ser adicionada à IDE

//Variável de leitura Bluetooth

char comandoBT(){
  char leitura;
  if (Serial.available()>0){
    leitura = Serial.read();
    delay(2);
        
    return leitura;
  }
}

char leitura; //Variável que armazena o comando do Bluetooth
boolean modo; //Alterna entre controle remoto (FALSE) e automático (TRUE)

//Itens de segurança do carro
int luzRe = 9;


//Sensores infra
int lE = A1;
int lD = A2;

//Valores de leitura dos sensores infra
int vE = 0;
int vD = 0;

//Conexões dos motores vindos da ponte H
int IN1 = 4;
int IN2 = 5;
int IN3 = 6;
int IN4 = 7;

//Portas do controle de velocidade para os motores
int vME = 11;
int vMD = 10;

//Conexões ultrasom contador
int UltraTrig = 8;
int UltraEcho = 12;

//Conexões ultrasom anti-colisão
int UltraTrigF = 3;
int UltraEchoF = 2;


#define ESTADO_OFF1 0
#define ESTADO_ON1 1
#define ESTADO_OFF2 2
#define ESTADO_FRENTE 3


Ultrasonic ultrasom(UltraTrig, UltraEcho);    //Definição do módulo ultrasom como "ultrasom", através do tipo Ultrasonic importado da biblioteca Ultrasonic.h
Ultrasonic ultrasomF(UltraTrigF, UltraEchoF); // Definição do ultrasom frontal

//Variáveis
long microsec;  //Variavel de tempo do ultrassom lateral
long microsecF; //Variavel de tempo do ultrassom frontal
float distancia; //Valor de leitura do ultrasom lateral
float distanciaF; //Valor de leitura do ultrasom frontal
int contador = ESTADO_OFF1;  //Contador para identificar quando pular de quadra

void setup()
{
  //Definição de pinos como entrada/saída
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(vME, OUTPUT);
  pinMode(vMD, OUTPUT);
  pinMode(luzRe, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

void emFrente() //Função para andar em frente
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
};

void darRe() //Função para andar em frente
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  digitalWrite(luzRe, HIGH);
};

void viraEsquerda()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void viraDireita()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
}

void parar()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
  digitalWrite(luzRe, HIGH);
}

void loop()
{
  digitalWrite(LED_BUILTIN, LOW);
  microsec = ultrasom.timing();
  distancia = ultrasom.convert(microsec, Ultrasonic::CM);

  microsecF = ultrasomF.timing();
  distanciaF = ultrasomF.convert(microsecF, Ultrasonic::CM);

  analogWrite(vME, 76);
  analogWrite(vMD, 72);

  if (Serial.available()>0){
    leitura = comandoBT();

    if (leitura == 'm'){
      modo = !modo;
    }

    if (leitura == 'f') {
      emFrente();
      digitalWrite(luzRe, LOW);
    }

    if (leitura == 'r') {
      darRe();
    }

    if (leitura == 'd'){
      viraDireita();
      digitalWrite(luzRe, LOW);
    }

    if (leitura == 'e'){
      viraEsquerda();
      digitalWrite(luzRe, LOW);
    }

    if (leitura == 'p'){
      parar();
    }
  }

    while (distanciaF < 15)
    {
      microsecF = ultrasomF.timing();
      distanciaF = ultrasomF.convert(microsecF, Ultrasonic::CM);

      if(Serial.available()>0){
        leitura = comandoBT();
      }
        while(leitura == 'r'){       
          darRe();
          if(Serial.available()>0){
            leitura = comandoBT();
          }
        }
      parar();
    }

  while(modo == true){

    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(luzRe, LOW);

    microsec = ultrasom.timing();
    distancia = ultrasom.convert(microsec, Ultrasonic::CM);

    microsecF = ultrasomF.timing();
    distanciaF = ultrasomF.convert(microsecF, Ultrasonic::CM);

    if ((distancia < 12) && (contador == ESTADO_OFF1) )
    {
      contador = ESTADO_ON1;
    }

    if (distancia > 12 && contador == ESTADO_ON1)
    {
      contador = ESTADO_OFF2;
    }

    if ((distancia < 12) && (contador == ESTADO_OFF2) )
    {
      contador = ESTADO_FRENTE;
    }

    if ((distancia > 12) && (contador == ESTADO_FRENTE))
    {
      emFrente();
      delay(2000);
      contador = ESTADO_OFF1;
    }

    analogWrite(vME, 76);
    analogWrite(vMD, 72);
    vE = analogRead(lE);
    vD = analogRead(lD);

    emFrente();

    while (analogRead(lE) >= 48)
    {
      viraEsquerda();
    }

    while (analogRead(lD) >= 48)
    {
      analogWrite(vME, 80);
      viraDireita();
    }

    while (distanciaF < 10)
    {
      parar();
      return;
    }

    if (Serial.available()>0){
      leitura = comandoBT();
      if(leitura == 'm'){
        modo = !modo;
      }
    }
  }
}
