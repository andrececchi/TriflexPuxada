
// Inicia Bibliotecas do sistema
#include "DWIN_Comm1.h"
#define BAUD_RATE 19200
DWIN_Comm DWIN(BAUD_RATE);
#include <EEPROM.h>


// Definição dos pinos do controlador
#define pinPulso    5
#define pinDirecao  6
#define pinMarca    2
#define pinPartida  3

int STA_Pagina_Atual;

// Define entradas e saídas
// int INP_Sensor_Partida = pinPartida;
// int INP_Sensor_Marca = pinMarca;

// Configura variáveis do sistema
int IHM_Botao_Liga = 0;
int IHM_Botao_Marca = 0;
int IHM_Botao_Jog = 0;
int IHM_Botao_Busca = 0;

int IHM_Contador_Reset = 0;
int IHM_Contador_Pause = 0;

int IHM_Contador_Boca = 0;
long IHM_Contador_Desejado = 0;
long IHM_Contador_Total = 0;

float IHM_Etiqueta = 120.0;
float IHM_Inicio_Marca = 90.0;
float IHM_Fim_Marca = 110.0;
float IHM_Etiqueta_Pos = 110.0;
float IHM_Diametro = 160.0;
float IHM_Pulso_Revolucao = 6000.0;
float IHM_Velocidade_Puxada = 1.0;
float IHM_Velocidade_Jog = 1.01;
float IHM_Velocidade_Marca = 0;
float CLP_Velocidade_Puxada = 0;
float CLP_Velocidade_Jog = 0;
float CLP_Velocidade_Marca = 0;
float CLP_Milimetro_Pulso = 0;
long CLP_Distancia =0;
long CLP_Inicio_Marca = 0;
long CLP_Fim_Marca = 0;
long CLP_Etiqueta_Pos = 0;

int E2P_Contador_Boca = 0;
long E2P_Contador_Desejado = 0;
long E2P_Contador_Total = 0;

int E2P_Pulso_Revolucao = 0;
int E2P_Velocidade_Puxada = 0;
int E2P_Velocidade_Jog = 0;
int E2P_Velocidade_Marca = 0;
long E2P_Diametro = 0;
long E2P_Etiqueta = 0;
long E2P_Inicio_Marca = 0;
long E2P_Fim_Marca = 0;
long E2P_Etiqueta_Pos = 0;

int STA_Estado = 0;
int STA_Contador_Jog = 0;
int STA_Botao_Busca = 0;
bool STA_Marca_Encontrada = false;

//Variaveis para controle de movimento
long PAR_Movimento[6];
long PAR_Inicio_Marca[6];
long PAR_Fim_Marca[6];
long PAR_Pos_Marca[6];
long PAR_Frequencia[6];
long Soma_PAR_Mov = 0;

bool STA_Jog_Out = false;


void UpdateIHM();
void UpdateCNT();
void Rampa_Movimento();
void Rampa_Pre_Marca();
void Rampa_Marca();
void interrupcaoPartida();
void interrupcaoMarca();


void setup() {


  // Inicializa comunicação
  Serial.begin(BAUD_RATE);
  Serial1.begin(BAUD_RATE);

  // Inicializa interface de IOs
  pinMode(pinPartida, INPUT_PULLUP);
  pinMode(pinMarca, INPUT_PULLUP);
  pinMode(pinPulso, OUTPUT);

  //IHM
  delay(3000);

  EEPROM.get(10, E2P_Pulso_Revolucao);
  EEPROM.get(20, E2P_Velocidade_Puxada);
  EEPROM.get(30, E2P_Velocidade_Jog);
  EEPROM.get(40, E2P_Diametro);
  EEPROM.get(50, E2P_Etiqueta);
  EEPROM.get(60, E2P_Inicio_Marca);
  EEPROM.get(70, E2P_Fim_Marca);
  EEPROM.get(80, E2P_Velocidade_Marca);
  EEPROM.get(90, E2P_Contador_Boca);
  EEPROM.get(100, E2P_Contador_Desejado);
  EEPROM.get(110, E2P_Contador_Total);
  EEPROM.get(120, E2P_Etiqueta_Pos);

  IHM_Contador_Boca = E2P_Contador_Boca;
  IHM_Contador_Desejado = E2P_Contador_Desejado;
  IHM_Contador_Total = E2P_Contador_Total;

  DWIN.write(0x0000, 0);

  DWIN.write(0x0135,IHM_Contador_Boca);
  DWIN.write_long(0x0125,IHM_Contador_Desejado);
  DWIN.write_long(0x0010,IHM_Contador_Total);

  IHM_Pulso_Revolucao = (float) E2P_Pulso_Revolucao;
  IHM_Velocidade_Puxada = (float) E2P_Velocidade_Puxada;
  IHM_Velocidade_Jog = (float) E2P_Velocidade_Jog;
  IHM_Velocidade_Marca = (float) E2P_Velocidade_Marca;
  IHM_Diametro = (float) E2P_Diametro / 100.0;
  IHM_Etiqueta = (float) E2P_Etiqueta / 100.0;
  IHM_Inicio_Marca = (float) E2P_Inicio_Marca / 100.0;
  IHM_Fim_Marca = (float) E2P_Fim_Marca / 100.0;
  IHM_Etiqueta_Pos = (float) E2P_Etiqueta_Pos / 100.0;

  DWIN.write(0x0104,E2P_Pulso_Revolucao);
  DWIN.write(0x0131,E2P_Velocidade_Puxada);
  DWIN.write(0x0129,E2P_Velocidade_Jog);
  DWIN.write(0x012D,E2P_Velocidade_Marca);
  DWIN.write_long(0x0100,E2P_Diametro);
  DWIN.write_long(0x010C,E2P_Etiqueta);
  DWIN.write_long(0x0139,E2P_Inicio_Marca);
  DWIN.write_long(0x013D,E2P_Fim_Marca);
  DWIN.write(0x0108,E2P_Etiqueta_Pos);

  // Memória para os botões momentários
  DWIN.write(0x0700,1);
  DWIN.write(0x0800,1);

  //Interrupção de partida
  attachInterrupt(0, interrupcaoPartida, FALLING);
  //Interrupção de marca
  // attachInterrupt(1, interrupcaoMarca, RISING);

}

void interrupcaoPartida()
{
  if(STA_Estado == 1)
  {
    Rampa_Movimento();
    UpdateCNT();

  }else if(STA_Estado == 3){
    // STA_Estado = 4;
    Rampa_Pre_Marca();
    // STA_Estado = 5;
    Rampa_Marca();
    UpdateCNT();
    STA_Marca_Encontrada = false;
    STA_Estado = 3;
  }

}
// void interrupcaoMarca()
// {
//
//   if(STA_Estado == 5)
//   {
//   STA_Marca_Encontrada = true;
//   }
//
// }

void loop() {

  // Serial.print("Estado: "); Serial.println(STA_Estado);
  // Serial.print("Jog: "); Serial.println(IHM_Botao_Jog);
  // Máquina de estados
  switch (STA_Estado){

    // Estado WAIT
    //Aguarda comando do operador
    case 0:
      UpdateIHM();
      CLP_Velocidade_Puxada = IHM_Velocidade_Puxada/100;
      CLP_Velocidade_Jog = 100/IHM_Velocidade_Jog;
      CLP_Velocidade_Marca = 100/IHM_Velocidade_Marca;
      CLP_Milimetro_Pulso = (IHM_Diametro * 3.14159265359) / (IHM_Pulso_Revolucao);
      CLP_Distancia = long ((IHM_Etiqueta / CLP_Milimetro_Pulso) + 0.5)*2;
      STA_Marca_Encontrada = false;
      if (IHM_Botao_Liga){
        if (IHM_Botao_Marca){
          STA_Estado = 3;
        }else{
          STA_Estado = 1;
        }
      }else if (IHM_Botao_Jog){
        STA_Estado = 6;
      }else if (IHM_Botao_Marca && IHM_Botao_Busca){
      //  STA_Estado = 8;
      }

    break;

     // Estado SENSOR_LIVRE
     // Aguarda sinal do sensor para partir
     case 1:
      UpdateIHM();
      CLP_Velocidade_Puxada = IHM_Velocidade_Puxada/100; //porcentagem
      PAR_Frequencia[0] = (long) (20/CLP_Velocidade_Puxada);
      PAR_Frequencia[1] = (long) (16/CLP_Velocidade_Puxada);
      PAR_Frequencia[2] = (long) (12/CLP_Velocidade_Puxada);
      PAR_Frequencia[3] = (long) (8/CLP_Velocidade_Puxada);
      PAR_Frequencia[4] = (long) (4/CLP_Velocidade_Puxada);
      PAR_Frequencia[5] = (long) (1/CLP_Velocidade_Puxada);
      CLP_Velocidade_Jog = 100/IHM_Velocidade_Jog;
      CLP_Velocidade_Marca = 100/IHM_Velocidade_Marca;
      CLP_Milimetro_Pulso = (IHM_Diametro * 3.14159265359) / IHM_Pulso_Revolucao;
      CLP_Distancia = long ((IHM_Etiqueta / CLP_Milimetro_Pulso) + 0.5)*2;
      PAR_Movimento[0] = (CLP_Distancia*4)/180;
      PAR_Movimento[1] = (CLP_Distancia*8)/180;
      PAR_Movimento[2] = (CLP_Distancia*12)/180;
      PAR_Movimento[3] = (CLP_Distancia*16)/180;
      PAR_Movimento[4] = (CLP_Distancia*20)/180;
      Soma_PAR_Mov = 0;
      for(int i = 0; i<5;i++)
      {
        Soma_PAR_Mov += PAR_Movimento[i];
      }
      PAR_Movimento[5] = (CLP_Distancia - 2*Soma_PAR_Mov);

      // Serial.print("CLP_Velocidade_Puxada"); Serial.println(CLP_Velocidade_Puxada);
      // for(int i = 0; i<6; i++)
      // {
      //   Serial.print(i); Serial.print("PAR_Frequencia "); Serial.println(PAR_Frequencia[i]);
      // }
      //
      // Serial.print("CLP_Distancia"); Serial.println(CLP_Distancia);
      //
      // for(int i = 0; i<6; i++)
      // {
      //   Serial.print(i); Serial.print("PAR_Movimento "); Serial.println(PAR_Movimento[i]);
      // }




      if (IHM_Botao_Marca){
          STA_Estado = 3;
      }
      if (!IHM_Botao_Liga){
        STA_Estado = 0;
      }
    break;

    // Estado RUN_LIVRE
    // Executa movimento sem sensor de marca
    case 2:
      // Rampa_Movimento();
      // UpdateCNT();
      // STA_Estado = 1;

    break;

    //Estado SENSOR_MARCA
     //Aguarda sinal do sensor para partir
    case 3:
      UpdateIHM();
      CLP_Velocidade_Puxada = IHM_Velocidade_Puxada/100; //porcentagem
      PAR_Frequencia[0] = (long) (20/CLP_Velocidade_Puxada);
      PAR_Frequencia[1] = (long) (16/CLP_Velocidade_Puxada);
      PAR_Frequencia[2] = (long) (12/CLP_Velocidade_Puxada);
      PAR_Frequencia[3] = (long) (8/CLP_Velocidade_Puxada);
      PAR_Frequencia[4] = (long) (4/CLP_Velocidade_Puxada);
      PAR_Frequencia[5] = (long) (1/CLP_Velocidade_Puxada);
      CLP_Velocidade_Jog = 100/IHM_Velocidade_Jog;
      CLP_Velocidade_Marca = 100/IHM_Velocidade_Marca;
      CLP_Milimetro_Pulso = (IHM_Diametro * 3.14159265359) / IHM_Pulso_Revolucao;
      CLP_Distancia = long ((IHM_Etiqueta / CLP_Milimetro_Pulso) + 0.5)*2;

      CLP_Inicio_Marca = long ((IHM_Inicio_Marca / CLP_Milimetro_Pulso) + 0.5)*2;

      PAR_Inicio_Marca[0] = (CLP_Inicio_Marca*4)/180;
      PAR_Inicio_Marca[1] = (CLP_Inicio_Marca*8)/180;
      PAR_Inicio_Marca[2] = (CLP_Inicio_Marca*12)/180;
      PAR_Inicio_Marca[3] = (CLP_Inicio_Marca*16)/180;
      PAR_Inicio_Marca[4] = (CLP_Inicio_Marca*20)/180;
      Soma_PAR_Mov = 0;
      for(int i = 0; i<5;i++)
      {
        Soma_PAR_Mov += PAR_Inicio_Marca[i];
      }
      PAR_Inicio_Marca[5] = (CLP_Inicio_Marca - Soma_PAR_Mov);
      CLP_Fim_Marca = long (((IHM_Fim_Marca - IHM_Inicio_Marca) / CLP_Milimetro_Pulso) + 0.5)*2;
      CLP_Etiqueta_Pos = long ((IHM_Etiqueta_Pos / CLP_Milimetro_Pulso));
      STA_Marca_Encontrada = false;
      if (!IHM_Botao_Marca){
          STA_Estado = 1;
      }
      if (!IHM_Botao_Liga){
        STA_Estado = 0;
      }
    break;

    // Estado RUN_MARCA
    // Executa movimento e busca sensor de marca sensor de marca
    case 4:

    // Rampa_Pre_Marca();
    // STA_Estado = 5;
    break;

    // Estado STOP_MARCA
    // Executa procura da marca
    case 5:
      // Rampa_Marca();
      // UpdateCNT();
      // STA_Marca_Encontrada = false;
      // STA_Estado = 3;

    break;

    // Estado JOG
    // Executa movimento de jog
    case 6:
      if (STA_Contador_Jog == 10){
       UpdateIHM();
       STA_Contador_Jog = 0;
      }
      STA_Contador_Jog = STA_Contador_Jog +1;
      if (IHM_Botao_Jog == 0){
        STA_Estado = 0;
        STA_Contador_Jog = 0;
      }else{
        STA_Jog_Out = !STA_Jog_Out;
        digitalWrite(5, STA_Jog_Out);
        delayMicroseconds(CLP_Velocidade_Jog);
      }
      // UpdateIHM();
    break;
/*
    // Estado Busca_MARCA
    // Executa parada após leitura da marca de impressão
    case 8:
      if (STA_Contador_Jog == 1000){
        UpdateIHM();
        STA_Contador_Jog = 0;
      }
      STA_Contador_Jog = STA_Contador_Jog +1;

      if (digitalRead(INP_Sensor_Marca)){
        stepper.stop();
        STA_Botao_Busca=1;
      }else{
        stepper.runSpeed();
        STA_Botao_Busca=0;
      }

      if (IHM_Botao_Busca == 0){
         STA_Estado = 0;

      }

    break;

    // Flush da biblioteca
    case 19:
      if (STA_Contador_Jog == 100){
        stepper.setSpeed(CLP_Velocidade_Puxada);
        STA_Contador_Jog = 0;
        STA_Estado = 9;
      }
      STA_Contador_Jog = STA_Contador_Jog +1;

      break;

    // Estado Movimento_Pós_Marca
    // Executa parada após leitura da marca de impressão
    case 9:
      stepper.runSpeed();

     if (stepper.currentPosition() >= CLP_Etiqueta_Pos){
        stepper.stop();
        UpdateCNT();
        STA_Estado = 3;
        //Serial.println("to no 9");
        //Serial.println(stepper.currentPosition());
      }

    break;
*/
  }
}

// Atualiza Contador
void UpdateCNT()
{
  if (!IHM_Contador_Pause){
    IHM_Contador_Total = IHM_Contador_Total + IHM_Contador_Boca;
  }
}
// Realiza varredura na IHM
void UpdateIHM()
{

  if (STA_Estado == 6){
    IHM_Botao_Jog = DWIN.read(0x0001);
  }else if (STA_Estado == 8){
    IHM_Botao_Busca = DWIN.read(0x0005);
    if ((DWIN.page() != 2) || (STA_Botao_Busca == 1)){
      DWIN.write(0x0005,0);
    }
  }else{

  IHM_Botao_Liga = DWIN.read(0x0000);
  IHM_Botao_Jog = DWIN.read(0x0001);
  IHM_Botao_Marca = DWIN.read(0x0121);
  IHM_Botao_Busca = DWIN.read(0x0005);

  IHM_Contador_Reset = DWIN.read(0x0004);
  if(IHM_Contador_Reset){
    IHM_Contador_Total = 0;
    DWIN.write_long(0x0010,IHM_Contador_Total);
    DWIN.write(0x0004,0);
  }
  IHM_Contador_Pause = DWIN.read(0x0003);

  IHM_Pulso_Revolucao = (float) DWIN.read(0x0104);
  if (IHM_Pulso_Revolucao != (float) E2P_Pulso_Revolucao){
    E2P_Pulso_Revolucao = (int) IHM_Pulso_Revolucao;
    EEPROM.put(10, E2P_Pulso_Revolucao);
  }

  IHM_Velocidade_Puxada = (float) DWIN.read(0x0131);
  if (IHM_Velocidade_Puxada != (float) E2P_Velocidade_Puxada){
    E2P_Velocidade_Puxada = (int) (IHM_Velocidade_Puxada);
    EEPROM.put(20, E2P_Velocidade_Puxada);
  }

  IHM_Velocidade_Jog = (float) DWIN.read(0x0129);
  if (IHM_Velocidade_Jog != (float) E2P_Velocidade_Jog){
    E2P_Velocidade_Jog = (int) (IHM_Velocidade_Jog);
    EEPROM.put(30, E2P_Velocidade_Jog);
  }

  IHM_Diametro = (float) DWIN.read_long(0x0100) / 100.0;
  if (IHM_Diametro != (float) E2P_Diametro / 100.0){
    E2P_Diametro = (long) (IHM_Diametro * 100.0);
    EEPROM.put(40, E2P_Diametro);
  }

  IHM_Etiqueta = (float) DWIN.read_long(0x010C) / 100.0;
  if (IHM_Etiqueta != (float) E2P_Etiqueta / 100.0){
    E2P_Etiqueta = (long) (IHM_Etiqueta * 100.0);
    EEPROM.put(50, E2P_Etiqueta);
  }

  IHM_Inicio_Marca = (float) DWIN.read_long(0x0139) / 100.0;
  if (IHM_Inicio_Marca != (float) E2P_Inicio_Marca / 100.0){
    E2P_Inicio_Marca = (long) (IHM_Inicio_Marca * 100.0);
    EEPROM.put(60, E2P_Inicio_Marca);
  }

  IHM_Fim_Marca = (float) DWIN.read_long(0x013D) / 100.0;
  if (IHM_Fim_Marca != (float) E2P_Fim_Marca / 100.0){
    E2P_Fim_Marca = (long) (IHM_Fim_Marca * 100.0);
    EEPROM.put(70, E2P_Fim_Marca);
  }

   IHM_Velocidade_Marca = (float) DWIN.read(0x012D);
  if (IHM_Velocidade_Marca != (float) E2P_Velocidade_Marca){
    E2P_Velocidade_Marca = (int) IHM_Velocidade_Marca;
    EEPROM.put(80, E2P_Velocidade_Marca);
  }

  IHM_Contador_Boca = DWIN.read(0x0135);
  if (IHM_Contador_Boca != E2P_Contador_Boca){
    E2P_Contador_Boca = IHM_Contador_Boca;
    EEPROM.put(90, E2P_Contador_Boca);
  }

  IHM_Contador_Desejado = DWIN.read_long(0x0125);
  if (IHM_Contador_Desejado != E2P_Contador_Desejado){
    E2P_Contador_Desejado = IHM_Contador_Desejado;
    EEPROM.put(100, E2P_Contador_Desejado);
  }

  IHM_Etiqueta_Pos = (float) DWIN.read(0x0108) / 100.0;
  if (IHM_Etiqueta_Pos != (float) E2P_Etiqueta_Pos / 100.0){
    E2P_Etiqueta_Pos = (long) (IHM_Etiqueta_Pos * 100.0);
    EEPROM.put(120, E2P_Etiqueta_Pos);
  }

  if (DWIN.read(0x0000)){
    if (IHM_Contador_Total != E2P_Contador_Total){
      DWIN.write_long(0x0010,IHM_Contador_Total);
    }
  }else{
    if (IHM_Contador_Total != E2P_Contador_Total){
      DWIN.write_long(0x0010,IHM_Contador_Total);
      E2P_Contador_Total = IHM_Contador_Total;
      EEPROM.put(110, E2P_Contador_Total);
    }
    }
  }
}



void Rampa_Movimento()
{
  bool outState = false;
  for(long i = 0; i<PAR_Movimento[0]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[0]);
  }
  for(long i = 0; i<PAR_Movimento[1]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[1]);
  }
  for(long i = 0; i<PAR_Movimento[2]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[2]);
  }

  for(long i = 0; i<PAR_Movimento[3]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[3]);
  }

  for(long i = 0; i<PAR_Movimento[4]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[4]);
  }
  for(long i = 0; i<PAR_Movimento[5]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[5]);
  }
  for(long i = 0; i<PAR_Movimento[4]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[4]);
  }

  for(long i = 0; i<PAR_Movimento[3]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[3]);
  }
  for(long i = 0; i<PAR_Movimento[2]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[2]);
  }

  for(long i = 0; i<PAR_Movimento[1]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[1]);
  }

  for(long i = 0; i<PAR_Movimento[0]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[0]);
  }

}

void Rampa_Pre_Marca()
{
  bool outState = false;
  for(long i = 0; i<PAR_Inicio_Marca[0]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[0]);
  }

  for(long i = 0; i<PAR_Inicio_Marca[1]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[1]);
  }
  for(long i = 0; i<PAR_Inicio_Marca[2]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[2]);
  }

  for(long i = 0; i<PAR_Inicio_Marca[3]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[3]);
  }

  for(long i = 0; i<PAR_Inicio_Marca[4]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[4]);
  }
  for(long i = 0; i<PAR_Inicio_Marca[5]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[5]);
  }
  for(long i = 0; i<PAR_Inicio_Marca[4]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[4]);
  }

  for(long i = 0; i<PAR_Inicio_Marca[3]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[3]);
  }
  for(long i = 0; i<PAR_Inicio_Marca[2]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[2]);
  }

  for(long i = 0; i<PAR_Inicio_Marca[1]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[1]);
  }

  for(long i = 0; i<PAR_Inicio_Marca[0]; i++ )
  {
      outState = !outState;
      digitalWrite(5, outState);

      delayMicroseconds(PAR_Frequencia[0]);
  }

}

void Rampa_Marca()
{
  bool outState = false;
  for(long i = 0; i<CLP_Fim_Marca; i++ )
  {
      // if (STA_Marca_Encontrada == true){
      //   break;
      // }
      if (digitalRead(3) == true){
        break;
      }
      outState = !outState;
      digitalWrite(5, outState);
      delayMicroseconds(CLP_Velocidade_Marca);
  }
}
