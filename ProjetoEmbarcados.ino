#include <MFRC522.h> //biblioteca responsável pela comunicação com o módulo RFID-RC522
#include <SPI.h> //biblioteca para comunicação do barramento SPI
#include <WiFi.h> // biblioteca para realizar a conexão no wifi
#include "SpotifyClient.h"
#include "settings.h" // Inclui as configurações

#define SS_PIN 5
#define RST_PIN 22

//esse objeto 'chave' é utilizado para autenticação
MFRC522::MIFARE_Key key;
//código de status de retorno da autenticação
MFRC522::StatusCode status;

// Definicoes pino modulo RC522
MFRC522 mfrc522(SS_PIN, RST_PIN); 

// Inicializa o client do spotify
SpotifyClient spotify = SpotifyClient(clientId, clientSecret, deviceName, refreshToken);

void setup() {
  // Inicia a serial
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  
  connectWifi();

  // Inicia MFRC522
  mfrc522.PCD_Init(); 

  // Atualiza token do spotify e id do aparelho
  //spotify.FetchToken();
  spotify.GetDevices();

  // Mensagens iniciais no serial monitor
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
}

void loop() {
   // Aguarda a aproximacao do cartao
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Seleciona um dos cartoes
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }

  String teste = retornaIdTag();
  Serial.print("Id: "); Serial.println(teste);
  String url = RetornaUrlSpotify(teste);
  Serial.print("URL: "); Serial.println(url);
  if(url == "pause"){
    spotify.Pause(url);
  }
  else if(url == "next"){
    spotify.Next();
  }
  else{
    spotify.Play(url);
  }
  

  // instrui o PICC quando no estado ACTIVE a ir para um estado de "parada"
  mfrc522.PICC_HaltA(); 
  // "stop" a encriptação do PCD, deve ser chamado após a comunicação com autenticação, caso contrário novas comunicações não poderão ser iniciadas
  mfrc522.PCD_StopCrypto1(); 

}

String RetornaUrlSpotify(String idTag){
  if(idTag.equals(" d3 d3 39 39 56 56 0f 0f")) {
    return "pause";
  }
  else if(idTag.equals(" 0a 0a f3 f3 bc bc 5c 5c")){
    return "next";
  } 
  else if(idTag.equals(" 53 53 b6 b6 93 93 6b 6b 20 20 00 00 01 01")){
    return "spotify:album:6Ls690GTjzbU4YNHnyCg1M";
  }
  else {
    return "galo";
  }   
}

String retornaIdTag(){
  //Aguarda cartao
  while ( ! mfrc522.PICC_IsNewCardPresent())
  {
    delay(100);
  }
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return "";
  }
  //Mostra UID na serial
  Serial.print("UID da tag : ");
  String conteudo = "";
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    conteudo.concat(String(mfrc522.uid.uidByte[i]<0x10 ? " 0" : " "));
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    conteudo.concat(String(mfrc522.uid.uidByte[i]<0x10 ? " 0" : " "));
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  Serial.println();
  return conteudo;
}

void connectWifi()
{
  WiFi.begin(ssid, pass);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
