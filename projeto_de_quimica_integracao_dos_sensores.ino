#include <MD_TCS230.h>//Biblioteca para o sensor rgb tcs3200
#include <FreqCount.h>//Biblioteca para o sensor rgb  tcs3200

#include <Wire.h>

//Bibliotecas para o sensor rgb  TSL2561:
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

//Biblioteca sensor apds
#include <SparkFun_APDS9960.h>


// Definição de pinos sensor para o sensor tcs3200
#define  S2_OUT  12
#define  S3_OUT  13
#define  OE_OUT   8    // LOW = ENABLED
//OUT => Pino 5~



//definindo variaveis
String dados, uni, value,rgbData;
char espaco;
int red,green,blue,rgbDataInt,ldr;

// Variáveis Globais sensor APDS9960
SparkFun_APDS9960 apds = SparkFun_APDS9960(); // Cria uma instância do sensor APDS-9960
uint16_t ambient_light = 0; // Variável para armazenar a luz ambiente
uint16_t red_light = 0;     // Variável para armazenar a luz vermelha
uint16_t green_light = 0;   // Variável para armazenar a luz verde
uint16_t blue_light = 0;    // Variável para armazenar a luz azul

MD_TCS230  CS(S2_OUT, S3_OUT, OE_OUT);//sensor rgb TCS3200

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);//sensor TSL2561

void setup(){
  Serial.begin(9600);
  
  Wire.begin();//inicializando  a comunicação I2C 
  
  CS.begin();//inicializando sensor rgb tcs3200

  configureSensorTSL();//sensor TSL2561

  tsl.begin();//Inicialização sensor TSL25

  // Inicializa o APDS-9960 (configuração I2C e valores iniciais)
  if ( apds.init()) {
    Serial.println(F("Inicialização do APDS-9960 completa"));
  } else {
    Serial.println(F("Algo deu errado durante a inicialização do APDS-9960!"));
  }
  /* "F()" é uma macro que é usada para armazenar uma string na memória flash em vez de na memória RAM*/

  if ( apds.enableLightSensor(false) ) {
    Serial.println(F("O sensor de luz está funcionando agora"));
  } else {
    Serial.println(F("Algo deu errado durante a inicialização do sensor de luz!"));
  }

  Serial.println("Unidades de medições dos sensores disponiveis: 'APDS', 'TSL', 'RGB' e 'LDR'");
  // Aguarda a inicialização e calibração serem concluídas
  delay(2000);
}

void loop(){
  if(Serial.available() > 0){
    dados = Serial.readStringUntil('\n');//Lendo inputs da porta serial

    espaco = dados.indexOf(' ');//quebrando a string até o espaço ' '

    value = dados.substring(0, espaco);//valor numerico da strind 
    uni = dados.substring(espaco+1);//valor da unidade da string(+1 carrega na variavel a parti do caractere " "(espaço)+1 da string, até o final
      
    Serial.print(uni + ":" + "\t" + value + "\t");

    //Indentificando função
    if(uni == "ldr" || uni == "LDR"){
      readSensorLDR();
    }else if(uni == "rgb" || uni == "RGB"){
      readSensorRGB();
    }else if(uni == "tsl" || uni == "TSL"){
      readSensorTSL();
    }else if(uni == "apds" || uni == "APDS"){
      readSensorAPDS();
    }else{
      Serial.println("Input invalido");
    }
    delay(1000);
  }  
 // readSensorRGB();
  //readSensorLDR();
  //readSensorTSL();
  //readSensorAPDS();
  delay(100); // Pequeno atraso entre leituras
}

void readSensorLDR(){
  ldr = analogRead(A0);

  Serial.println("LDR:"+ldr);
}

void readSensorRGB()
{
  static  bool  waiting;
 
  if (!waiting)
  {
    CS.read();
    waiting = true;
  }
  else
  {
    if (CS.available())
    {
      colorData  rgb;
      
      CS.getRGB(&rgb);

      int red = rgb.value[TCS230_RGB_R];
      int green = rgb.value[TCS230_RGB_G];
      int blue = rgb.value[TCS230_RGB_B];
  
      String rgbString = "RGB [" + String(red) + "," + String(green) + "," + String(blue) + "]";
      Serial.println(rgbString);
      
      waiting = false;
    }
  }
}

void readSensorTSL()
{
  sensors_event_t event;
  tsl.getEvent(&event);
 
  /* Exibir os resultados (a luz é medida em lux) */
  if (event.light)
  {
    Serial.print(event.light); Serial.println(" lux");
  }
  else
  {
    /* Se event.light = 0 lux, o sensor está provavelmente saturado
       e nenhum dado confiável pôde ser gerado! */
    Serial.println("Sobrecarga do sensor");
  }
  delay(250);
}

void readSensorAPDS(){
  // Lê os níveis de luz (ambiente, vermelha, verde, azul)
  if (  !apds.readAmbientLight(ambient_light) ||
        !apds.readRedLight(red_light) ||
        !apds.readGreenLight(green_light) ||
        !apds.readBlueLight(blue_light) ) {
    Serial.println("Erro ao ler os valores de luz");
  } else {
    Serial.print("Ambiente: ");
    Serial.print(ambient_light);
    Serial.print(" Vermelha: ");
    Serial.print(red_light);
    Serial.print(" Verde: ");
    Serial.print(green_light);
    Serial.print(" Azul: ");
    Serial.println(blue_light);
  }
  
  // Aguarda 1 segundo antes da próxima leitura
  delay(1000);
}

void configureSensorTSL(void){
  
  tsl.enableAutoRange(true); /* Ganho automático ... alterna automaticamente entre 1x e 16x */
  
  /* Alterar o tempo de integração proporciona uma melhor resolução do sensor (402ms = dados de 16 bits) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);     
}
