//=====================================
// ESP32 CNC Controller
//=====================================
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <Arduino_JSON.h>
#include <Stepper.h>
#include <Preferences.h>

#include "ssid_pass.h"

//---------------------Configuration wifi-------------

const char *ssid = SSID;
const char *password = PASS;
//---------------------------------------------------

AsyncWebServer server(80);
// Créer une source event sur /events
AsyncEventSource events("/events");

//---------Stock des valeurs permanentes dans la mémoire flash---------

Preferences preferences;

//---------------------------------------------------

// Cherche les parametres dans la requête HTTP POST

// DC_direction
const char *PARAM_INPUT_1 = "DC_direction";

// STEPPER_directionX
const char *PARAM_INPUT_2 = "STEPPER_directionX";
// STEPPER_distanceX
const char *PARAM_INPUT_3 = "STEPPER_distanceX";
// STEPPER_vitesseX
const char *PARAM_INPUT_4 = "STEPPER_vitesseX";

// STEPPER_directionY
const char *PARAM_INPUT_5 = "STEPPER_directionY";
// STEPPER_distanceY
const char *PARAM_INPUT_6 = "STEPPER_distanceY";
// STEPPER_vitesseY
const char *PARAM_INPUT_7 = "STEPPER_vitesseY";

// STEPPER_directionZ
const char *PARAM_INPUT_8 = "STEPPER_directionZ";
// STEPPER_distanceZ
const char *PARAM_INPUT_9 = "STEPPER_distanceZ";
// STEPPER_vitesseZ
const char *PARAM_INPUT_10 = "STEPPER_vitesseZ";

//========================CONFIG==========================

// STEPPER_STEPS_PER_REV_X
const char *PARAM_INPUT_11 = "STEPPER_STEPS_PER_REV_X";
// STEPPER_micropasX
const char *PARAM_INPUT_12 = "STEPPER_micropasX";
// STEPPER_pasfiltageX
const char *PARAM_INPUT_13 = "STEPPER_pasfiltageX";
//========================================================
// STEPPER_STEPS_PER_REV_Y
const char *PARAM_INPUT_14 = "STEPPER_STEPS_PER_REV_Y";
// STEPPER_micropasY
const char *PARAM_INPUT_15 = "STEPPER_micropasY";
// STEPPER_pasfiltageY
const char *PARAM_INPUT_16 = "STEPPER_pasfiltageY";
//========================================================
// STEPPER_STEPS_PER_REV_Z
const char *PARAM_INPUT_17 = "STEPPER_STEPS_PER_REV_Z";
// STEPPER_micropasZ
const char *PARAM_INPUT_18 = "STEPPER_micropasZ";
// STEPPER_pasfiltageZ
const char *PARAM_INPUT_19 = "STEPPER_pasfiltageZ";
//========================================================
// MAX_POS_X
const char *PARAM_INPUT_20 = "MAX_POS_X";
// MAX_POS_Y
const char *PARAM_INPUT_21 = "MAX_POS_Y";
// MAX_POS_Z
const char *PARAM_INPUT_22 = "MAX_POS_Z";
//========================================================
// DC_MOTOR_SPEED_MAX
const char *PARAM_INPUT_23 = "DC_MOTOR_SPEED_MAX";

//---------------------------------------------------

String DC_direction, STEPPER_directionX, STEPPER_distanceX, STEPPER_vitesseX, STEPPER_directionY, STEPPER_distanceY, STEPPER_vitesseY, STEPPER_directionZ, STEPPER_distanceZ, STEPPER_vitesseZ, STEPPER_SpeedString_X, STEPPER_SpeedString_Y, STEPPER_SpeedString_Z;

String STEPPER_micropas_toFlash_X, STEPPER_pasfiltage_toFlash_X, STEPPER_micropas_toFlash_Y, STEPPER_pasfiltage_toFlash_Y, STEPPER_micropas_toFlash_Z, STEPPER_pasfiltage_toFlash_Z;

//========================PARAMETRE MOTEUR CC==========================

String DC_MOTOR_SPEED_MAX_toFlash;

unsigned long DC_PWM = 0; // Modulation à largeur d'impulsions

#define DC_pot 35

int DC_GAUGESpeed, DC_PIn_read;

unsigned long DC_motorSpeed = 0; // Vitesse moteur CC

#define DC_Moteur_sens1 32 // IN1
#define DC_Moteur_sens2 33 // IN2
#define DC_moteur1_PWM 25  // EN1

// Paramètre des propriété PWM
const int DC_freq = 30000;
const int DC_pwmChannel = 0;
const int DC_resolution = 8;
int DC_dutyCycle = 50;

int DC_MOTOR_SPEED_MAX = 4000;

//========================STEPPER X MOTOR SETUP==========================
// Valeurs par défaut.
int STEPPER_STEPS_PER_REV_X = 400;

int STEPPER_STEPS_PER_MM_X = 3200;

int STEPPER_micropasX = 16;

int STEPPER_pasfiltageX = 2;

int MAX_POS_X = 700;

String STEPPER_STEPS_PER_REV_toFlash_X, STEPPER_STEPS_PER_MM_toFlash_X;

//========================STEPPER Y MOTOR SETUP==========================
// Valeurs par défaut.
int STEPPER_STEPS_PER_REV_Y = 400;

int STEPPER_STEPS_PER_MM_Y = 3200;

int STEPPER_micropasY = 16;

int STEPPER_pasfiltageY = 2;

int MAX_POS_Y = 400;

String STEPPER_STEPS_PER_REV_toFlash_Y, STEPPER_STEPS_PER_MM_toFlash_Y;

//========================STEPPER Z MOTOR SETUP==========================
// Valeurs par défaut.
int STEPPER_STEPS_PER_REV_Z = 400;

int STEPPER_STEPS_PER_MM_Z = 3200;

int STEPPER_micropasZ = 16;

int STEPPER_pasfiltageZ = 2;

int MAX_POS_Z = 300;

String STEPPER_STEPS_PER_REV_toFlash_Z, STEPPER_STEPS_PER_MM_toFlash_Z;

//========================DEFINE PINS==========================

#define STEPPER_IN1_X 26 // IN1
#define STEPPER_IN2_X 27 // IN2
#define STEPPER_IN3_X 14 // IN3
#define STEPPER_IN4_X 12 // IN4

#define STEPPER_IN1_Y 21 // IN1
#define STEPPER_IN2_Y 19 // IN2
#define STEPPER_IN3_Y 18  // IN3
#define STEPPER_IN4_Y 5  // IN4

#define STEPPER_IN1_Z 4 // IN1
#define STEPPER_IN2_Z 0 // IN2
#define STEPPER_IN3_Z 5  // IN3
#define STEPPER_IN4_Z 15 // IN4

// Librairie moteur pas à pas
Stepper stepperMotor_X(STEPPER_STEPS_PER_REV_X, STEPPER_IN4_X, STEPPER_IN3_X, STEPPER_IN2_X, STEPPER_IN1_X);
Stepper stepperMotor_Y(STEPPER_STEPS_PER_REV_Y, STEPPER_IN4_Y, STEPPER_IN3_Y, STEPPER_IN2_Y, STEPPER_IN1_Y);
Stepper stepperMotor_Z(STEPPER_STEPS_PER_REV_Z, STEPPER_IN4_Z, STEPPER_IN3_Z, STEPPER_IN2_Z, STEPPER_IN1_Z);

#define JOYSTOCK_PIN 13

//==============================END SWITCH============================

#define END_SWITCH_PIN_X 36
#define END_SWITCH_PIN_Y 39
#define END_SWITCH_PIN_Z 34

//==============================POSITION==============================

int Position_absolue_X, Position_absolue_Y, Position_absolue_Z, Position_relative_X, Position_relative_Y, Position_relative_Z;

String MAX_POS_toFlash_X, MAX_POS_toFlash_Y, MAX_POS_toFlash_Z;

//=============================CONFIGURATION===========================

// Variable de temps
unsigned long lastTime = 0;
unsigned long timerDelay = 250;

bool newRequest = false, newRequest_config = false, Homing_end = false, Homing_request = false;

bool Red_X = true, Red_Y = true, Red_Z = true;

// Stores Homing state
String HominState;

// Variable JSON pour stocker les valeurs qui permets de les afficher sur la page html
JSONVar readings;

//=================================STATIC IP ADDRESS==================

// Set your Static IP address
IPAddress local_IP(192, 168, 137, 241);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);

//====================================================================
// Permets de remplacer la variable "STATE" du code html par le mot "CALIBER" ou "NON CALIBRÉ"
String processor(const String &var)
{
  Serial.print(var);
  if (var == "STATE")
  {
    if (Homing_end)
    {
      HominState = " CALIBRER";
    }
    else if (!Homing_end)
    {
      HominState = " NON CALIBR&Eacute;";
    }
    Serial.println(HominState);
    return HominState;
  }
  return String();
  delay(10);
}
//====================================================================

// Initialise le SPIFFS (stock la page html)
void initSPIFFS()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  else
  {
    Serial.println("SPIFFS mounted successfully");
  }
}
//====================================================================

// Initialise le WiFi
void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println("STA Failed to configure");
  }
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println("");
  Serial.println(WiFi.localIP());
  Serial.println(ssid);
}
//====================================================================
String getValueReadings()
{
  // Récupére les variables 'string' liée a la page principal et les stocks dans une variable JSON
  readings["pwm"] = String(DC_PWM);
  readings["gauge"] = String(DC_GAUGESpeed);
  readings["STEPPER_speedX"] = STEPPER_SpeedString_X;
  readings["STEPPER_speedY"] = STEPPER_SpeedString_Y;
  readings["STEPPER_speedZ"] = STEPPER_SpeedString_Z;
  readings["STEPPER_off"] = String(Red_X);
  readings["STEPPER_absolueX"] = String(Position_absolue_X);
  readings["STEPPER_relativeX"] = String(Position_relative_X);
  readings["STEPPER_absolueY"] = String(Position_absolue_Y);
  readings["STEPPER_relativeY"] = String(Position_relative_Y);
  readings["STEPPER_absolueZ"] = String(Position_absolue_Z);
  readings["STEPPER_relativeZ"] = String(Position_relative_Z);

  String jsonString = JSON.stringify(readings);
  return jsonString;
}

String getFashReadings()
{
  // Récupére les variables 'string' liée à la configuration et les stocks dans une variable JSON
  readings["DC_MOTOR_SPEED_MAX"] = String(DC_MOTOR_SPEED_MAX);

  readings["STEPPER_STEPS_PER_REV_X"] = String(STEPPER_STEPS_PER_REV_X);
  readings["STEPPER_STEPS_PER_MM_X"] = String(STEPPER_STEPS_PER_MM_X);
  readings["STEPPER_micropasX"] = String(STEPPER_micropasX);
  readings["STEPPER_pasfiltageX"] = String(STEPPER_pasfiltageX);

  readings["STEPPER_STEPS_PER_REV_Y"] = String(STEPPER_STEPS_PER_REV_Y);
  readings["STEPPER_STEPS_PER_MM_Y"] = String(STEPPER_STEPS_PER_MM_Y);
  readings["STEPPER_micropasY"] = String(STEPPER_micropasY);
  readings["STEPPER_pasfiltageY"] = String(STEPPER_pasfiltageY);

  readings["STEPPER_STEPS_PER_REV_Z"] = String(STEPPER_STEPS_PER_REV_Z);
  readings["STEPPER_STEPS_PER_MM_Z"] = String(STEPPER_STEPS_PER_MM_Z);
  readings["STEPPER_micropasZ"] = String(STEPPER_micropasZ);
  readings["STEPPER_pasfiltageZ"] = String(STEPPER_pasfiltageZ);

  readings["MAX_POS_X"] = String(MAX_POS_X);
  readings["MAX_POS_Y"] = String(MAX_POS_Y);
  readings["MAX_POS_Z"] = String(MAX_POS_Z);

  String jsonString = JSON.stringify(readings);
  return jsonString;
}

//====================================================================

void setup()
{
  Serial.begin(115200);
  initWiFi();
  initSPIFFS();

  //==========================URL HTTP GET===========================

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });
  delay(1);
  server.serveStatic("/", SPIFFS, "/");

  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/config.html", "text/html"); });
  delay(1);
  server.serveStatic("/config", SPIFFS, "/config");

  // Requête pour la derniere lecture des variables.
  server.on("/readingsMain", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    delay(1);
    String json = getValueReadings();
    request->send(200, "application/json", json);
    json = String(); });
  // Requête pour la derniere lecture des variables de configuration.
  server.on("/readingsConfig", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    delay(1);
    String json = getFashReadings();
    request->send(200, "application/json", json);
    json = String(); });

  events.onConnect([](AsyncEventSourceClient *client)
                   {
    if (client->lastId()) {
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    } });

  //==============================BOUTON===============================

  server.on("/homing", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    delay(1);
    Homing_request = true;
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.on("/homing_rest", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    delay(1);
    Serial.println("CALIBRATION RESET");
    Homing_end = false;
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.on("/relativeX", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    delay(1);
        Serial.println("Relative X=0");
        Position_relative_X = 0;
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.on("/relativeY", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    delay(1);
        Serial.println("Relative Y=0");
    Position_relative_Y = 0;
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.on("/relativeZ", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    delay(1);
    Serial.println("Relative Z=0");
    Position_relative_Z = 0;
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  //================================CONFIG BUTTON====================

  server.on("/index", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    delay(1);
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.on("/load", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    delay(1);
    LOAD_CONFIGURATION();
    request->send(SPIFFS, "/config.html", String(), false, processor); });

  //================================URL HTTP POST====================

  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    int params = request->params();
    for (int i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      if (p->isPost()) {

        // HTTP POST la valeur de l'entrée
        if (p->name() == PARAM_INPUT_1) {
          DC_direction = p->value().c_str();
        }
        if (p->name() == PARAM_INPUT_2) {
          STEPPER_directionX = p->value().c_str();
        }
        if (p->name() == PARAM_INPUT_3) {
          STEPPER_distanceX = p->value().c_str();
        }
        if (p->name() == PARAM_INPUT_4) {
          STEPPER_vitesseX = p->value().c_str();
        }
        if (p->name() == PARAM_INPUT_5) {
          STEPPER_directionY = p->value().c_str();
        }
        if (p->name() == PARAM_INPUT_6) {
          STEPPER_distanceY = p->value().c_str();
        }
        if (p->name() == PARAM_INPUT_7) {
          STEPPER_vitesseY = p->value().c_str();
        }
        if (p->name() == PARAM_INPUT_8) {
          STEPPER_directionZ = p->value().c_str();
        }
        if (p->name() == PARAM_INPUT_9) {
          STEPPER_distanceZ = p->value().c_str();
        }
        if (p->name() == PARAM_INPUT_10) {
          STEPPER_vitesseZ = p->value().c_str();
        }

        newRequest = true;
      }
    }
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  //========================URL HTTP POST CONFIG=====================

  server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    int params = request->params();
    for (int i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      if (p->isPost()) {

        // HTTP POST la valeur de l'entrée
        if (p->name() == PARAM_INPUT_11) {
          STEPPER_STEPS_PER_REV_toFlash_X = p->value().c_str();
        }
        if (p->name() == PARAM_INPUT_12) {
          STEPPER_micropas_toFlash_X = p->value().c_str();
        }
        if (p->name() == PARAM_INPUT_13) {
           STEPPER_pasfiltage_toFlash_X= p->value().c_str();
        }
        //========================================================
        if (p->name() == PARAM_INPUT_14) {
          STEPPER_STEPS_PER_REV_toFlash_Y = p->value().c_str();
        }
        if (p->name() == PARAM_INPUT_15) {
          STEPPER_micropas_toFlash_Y = p->value().c_str();
        }
        if (p->name() == PARAM_INPUT_16) {
          STEPPER_pasfiltage_toFlash_Y = p->value().c_str();
        }
        //========================================================
        if (p->name() == PARAM_INPUT_17) {
          STEPPER_STEPS_PER_REV_toFlash_Z = p->value().c_str();
        }
        if (p->name() == PARAM_INPUT_18) {
          STEPPER_micropas_toFlash_Z = p->value().c_str();
        }
        if (p->name() == PARAM_INPUT_19) {
          STEPPER_pasfiltage_toFlash_Z = p->value().c_str();
        }
        //========================================================
        if (p->name() == PARAM_INPUT_20) {
          MAX_POS_toFlash_X = p->value().c_str();
        }
        if (p->name() == PARAM_INPUT_21) {
          MAX_POS_toFlash_Y = p->value().c_str();
        }
        if (p->name() == PARAM_INPUT_22) {
          MAX_POS_toFlash_Z = p->value().c_str();
        }
        //========================================================
        if (p->name() == PARAM_INPUT_23) {
          DC_MOTOR_SPEED_MAX_toFlash = p->value().c_str();
        }

        newRequest_config = true;
      }
    }
    request->send(SPIFFS, "/config.html", String(), false, processor); });

  // Démarre le serveur et la mémoire flash.
  server.addHandler(&events);
  server.begin();
  preferences.begin("cnc", false);

  //----------------------------------------DC MOTOR---------------------------------------
  pinMode(DC_moteur1_PWM, OUTPUT);  // Pin 10 Arduino en sortie PWM
  pinMode(DC_Moteur_sens1, OUTPUT); // Pin 4 Arduino en sortie digitale
  pinMode(DC_Moteur_sens2, OUTPUT); // Pin 5 Arduino en sortie digitale

  // configure LED PWM functionalitites
  ledcSetup(DC_pwmChannel, DC_freq, DC_resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(DC_moteur1_PWM, DC_pwmChannel);

  //----------------------------------------END SWITCH---------------------------------------

  pinMode(END_SWITCH_PIN_X, INPUT);
  pinMode(END_SWITCH_PIN_Y, INPUT);
  pinMode(END_SWITCH_PIN_Z, INPUT);
}

//====================================================================

void loop()
{
  //-------------Lecture analogique--------------------
  DC_PIn_read = analogRead(DC_pot);
  DC_GAUGESpeed = map(DC_PIn_read, 0, 4095, 0, DC_MOTOR_SPEED_MAX);
  DC_PWM = map(DC_PIn_read, 0, 4095, 0, 255);
  DC_motorSpeed = (DC_MOTOR_SPEED_MAX * DC_PIn_read) / 4095;

  //-------------analogWrite est remplacé par ledcWrite pour l'ESP32----
  ledcWrite(DC_pwmChannel, DC_PWM);
  //--------------------------------------------------

  CALIBRATION_FUNCTION();

  if (newRequest)
  {
    Serial.println("----------------------------------------------------");

    DC_CONTROL();

    if (Homing_end)
    {
      STEPPER_CONTROL_X();
      STEPPER_STOP_X();
      STEPPER_CONTROL_Y();
      STEPPER_STOP_Y();
      STEPPER_CONTROL_Z();
      STEPPER_STOP_Z();
    }

    Serial.println("----------------------------------------------------");
    newRequest = false;
  }
  //-------------Configuration--------------------
  if (newRequest_config)
  {
    CONFIGURATION();
    newRequest_config = false;
  }

  // Envoi les event au client avec lecture des données toutes les 0.5 secondes
  SEND_EVENT();
}

void CALIBRATION_FUNCTION()
{

  if (Homing_request)
  {
    if (!Homing_end)
    {
      //----------CALIBRATION AXE X-------------//
      Serial.println("Calibration axe X");
      while (digitalRead(END_SWITCH_PIN_X))
      {
        stepperMotor_X.setSpeed(5);
        stepperMotor_X.step(1);
      }
      Serial.println("SWITCH X touché");
      Position_absolue_X = MAX_POS_X;
      Position_relative_X = MAX_POS_X;
      Serial.println("Axe X calibré");
      STEPPER_STOP_X();
      delay(30);

      //----------CALIBRATION AXE Y-------------//
      Serial.println("Calibration axe Y");
      while (digitalRead(END_SWITCH_PIN_Y))
      {
        stepperMotor_Y.setSpeed(5);
        stepperMotor_Y.step(1);
      }
      Serial.println("SWITCH Y touché");
      Position_absolue_Y = MAX_POS_Y;
      Position_relative_Y = MAX_POS_Y;
      Serial.println("Axe Y calibré");
      STEPPER_STOP_Y();
      delay(30);

      //----------CALIBRATION AXE Z-------------//
      Serial.println("Calibration axe Z");
      while (digitalRead(END_SWITCH_PIN_Z))
      {
        stepperMotor_Z.setSpeed(5);
        stepperMotor_Z.step(1);
      }
      Serial.println("SWITCH Z touché");
      Position_absolue_Z = MAX_POS_Z;
      Position_relative_Z = MAX_POS_Z;
      Serial.println("Axe Z calibré");
      STEPPER_STOP_Z();
      delay(30);

      Homing_end = true;

      Serial.println("Calibration fini !");
    }
    Homing_request = false;
  }
}

void DC_CONTROL()
{
  Serial.println("MOTEUR CC OK !!");

  if (DC_direction == "CW")
  {
    digitalWrite(DC_Moteur_sens1, HIGH);
    digitalWrite(DC_Moteur_sens2, LOW);
    Serial.print("[DC] SENS HORAIRE - PWM : ");
    Serial.println(DC_PWM);
  }
  if (DC_direction == "CCW")
  {
    digitalWrite(DC_Moteur_sens1, LOW);
    digitalWrite(DC_Moteur_sens2, HIGH);
    Serial.print("[DC] SENS ANTI HORAIRE - PWM : ");
    Serial.println(DC_PWM);
  }
  if (DC_direction == "STOP")
  {
    digitalWrite(DC_Moteur_sens1, LOW);
    digitalWrite(DC_Moteur_sens2, LOW);
    Serial.println("[DC] MOTEUR ARRET");
  }
}

void STEPPER_CONTROL_X()
{

  float STEPPER_steps_X = STEPPER_distanceX.toFloat() * STEPPER_STEPS_PER_MM_X;
  Serial.print("[STEPPER] AXE X Nombres de pas : ");
  Serial.println(STEPPER_steps_X);

  Serial.print("[STEPPER] AXE X Vitesse : ");
  Serial.println(STEPPER_vitesseX + " tr/min");
  // Envoi la vitesse à la page html
  STEPPER_SpeedString_X = STEPPER_vitesseX;
  stepperMotor_X.setSpeed(STEPPER_vitesseX.toInt());

  if (STEPPER_directionX == "CW")
  {
    Red_X = false;
    stepperMotor_X.step(STEPPER_steps_X);
    Position_absolue_X += STEPPER_distanceX.toFloat();
    Position_relative_X += STEPPER_distanceX.toFloat();
    Red_X = true;
  }
  if (STEPPER_directionX == "CCW")
  {
    stepperMotor_X.step(-STEPPER_steps_X);
    Position_absolue_X -= STEPPER_distanceX.toFloat();
    Position_relative_X -= STEPPER_distanceX.toFloat();
  }

  pinMode(STEPPER_IN1_X, LOW);
  pinMode(STEPPER_IN2_X, LOW);
  pinMode(STEPPER_IN3_X, LOW);
  pinMode(STEPPER_IN4_X, LOW);
}

void STEPPER_CONTROL_Y()
{
  float STEPPER_steps_Y = STEPPER_distanceY.toFloat() * STEPPER_STEPS_PER_MM_Y;
  Serial.print("[STEPPER] AXE Y Nombres de pas : ");
  Serial.println(STEPPER_steps_Y);

  Serial.print("[STEPPER] AXE Y Vitesse : ");
  Serial.println(STEPPER_vitesseY + " tr/min");
  // Envoi la vitesse à la page html
  STEPPER_SpeedString_Y = STEPPER_vitesseY;
  stepperMotor_Y.setSpeed(STEPPER_vitesseY.toInt());

  if (STEPPER_directionY == "CW")
  {
    Red_Y = false;
    stepperMotor_Y.step(STEPPER_steps_Y);
    Position_absolue_Y += STEPPER_distanceY.toFloat();
    Position_relative_Y += STEPPER_distanceY.toFloat();
    Red_Y = true;
  }
  if (STEPPER_directionY == "CCW")
  {
    Red_Y = false;
    stepperMotor_Y.step(-STEPPER_steps_Y);
    Position_absolue_Y -= STEPPER_distanceY.toFloat();
    Position_relative_Y -= STEPPER_distanceY.toFloat();
    Red_Y = true;
  }
}

void STEPPER_CONTROL_Z()
{

  float STEPPER_steps_Z = STEPPER_distanceZ.toFloat() * STEPPER_STEPS_PER_MM_Z;
  Serial.print("[STEPPER] AXE Z Nombres de pas : ");
  Serial.println(STEPPER_steps_Z);

  Serial.print("[STEPPER] AXE Z Vitesse : ");
  Serial.println(STEPPER_vitesseZ + " tr/min");
  // Envoi la vitesse à la page html
  STEPPER_SpeedString_Z = STEPPER_vitesseZ;
  stepperMotor_Z.setSpeed(STEPPER_vitesseZ.toInt());

  if (STEPPER_directionZ == "CW")
  {
    Red_Z = false;
    stepperMotor_Z.step(STEPPER_steps_Z);
    Position_absolue_Z += STEPPER_distanceZ.toFloat();
    Position_relative_Z += STEPPER_distanceZ.toFloat();
    Red_Z = true;
  }
  if (STEPPER_directionZ == "CCW")
  {
    Red_Z = false;
    stepperMotor_Z.step(-STEPPER_steps_Z);
    Position_absolue_Z -= STEPPER_distanceZ.toFloat();
    Position_relative_Z -= STEPPER_distanceZ.toFloat();
    Red_Z = true;
  }
}

void STEPPER_STOP_X()
{
  digitalWrite(STEPPER_IN1_X, LOW);
  digitalWrite(STEPPER_IN2_X, LOW);
  digitalWrite(STEPPER_IN3_X, LOW);
  digitalWrite(STEPPER_IN4_X, LOW);
  delay(1);
}
void STEPPER_STOP_Y()
{
  digitalWrite(STEPPER_IN1_Y, LOW);
  digitalWrite(STEPPER_IN2_Y, LOW);
  digitalWrite(STEPPER_IN3_Y, LOW);
  digitalWrite(STEPPER_IN4_Y, LOW);
  delay(1);
}
void STEPPER_STOP_Z()
{
  digitalWrite(STEPPER_IN1_Z, LOW);
  digitalWrite(STEPPER_IN2_Z, LOW);
  digitalWrite(STEPPER_IN3_Z, LOW);
  digitalWrite(STEPPER_IN4_Z, LOW);
  delay(1);
}

void CONFIGURATION()
{
  Serial.println("----------------------------------------------------");
  Serial.println("Configuration");

  //   /!\ NE PAS METTRE PLUS DE 16 CARACTÈRES POUR LA KEY /!\ 
  // STOCK LES VALEURS

  preferences.putInt("DC_SPEED_MAX", DC_MOTOR_SPEED_MAX_toFlash.toInt());

  preferences.putInt("STEPS_PER_REV_X", STEPPER_STEPS_PER_REV_toFlash_X.toInt());
  preferences.putInt("micropasX", STEPPER_micropas_toFlash_X.toInt());
  preferences.putInt("pasfiltageX", STEPPER_pasfiltage_toFlash_X.toInt());

  preferences.putInt("STEPS_PER_REV_Y", STEPPER_STEPS_PER_REV_toFlash_Y.toInt());
  preferences.putInt("micropasY", STEPPER_micropas_toFlash_Y.toInt());
  preferences.putInt("pasfiltageY", STEPPER_pasfiltage_toFlash_Y.toInt());

  preferences.putInt("STEPS_PER_REV_Z", STEPPER_STEPS_PER_REV_toFlash_Z.toInt());
  preferences.putInt("micropasZ", STEPPER_micropas_toFlash_Z.toInt());
  preferences.putInt("pasfiltageZ", STEPPER_pasfiltage_toFlash_Z.toInt());

  preferences.putInt("MAX_POS_X", MAX_POS_toFlash_X.toInt());
  preferences.putInt("MAX_POS_Y", MAX_POS_toFlash_Y.toInt());
  preferences.putInt("MAX_POS_Z", MAX_POS_toFlash_Z.toInt());

  STEPPER_STEPS_PER_MM_X = (STEPPER_STEPS_PER_REV_toFlash_X.toInt() * STEPPER_micropas_toFlash_X.toInt()) / STEPPER_pasfiltage_toFlash_X.toInt();
  STEPPER_STEPS_PER_MM_Y = (STEPPER_STEPS_PER_REV_toFlash_Y.toInt() * STEPPER_micropas_toFlash_Y.toInt()) / STEPPER_pasfiltage_toFlash_Y.toInt();
  STEPPER_STEPS_PER_MM_Z = (STEPPER_STEPS_PER_REV_toFlash_Z.toInt() * STEPPER_micropas_toFlash_Z.toInt()) / STEPPER_pasfiltage_toFlash_Z.toInt();

  preferences.putInt("STEPS_PER_MM_X", STEPPER_STEPS_PER_MM_X);
  preferences.putInt("STEPS_PER_MM_Y", STEPPER_STEPS_PER_MM_Y);
  preferences.putInt("STEPS_PER_MM_Z", STEPPER_STEPS_PER_MM_Z);

  // RECUPÉRE LES VALEURS

  DC_MOTOR_SPEED_MAX = preferences.getInt("DC_SPEED_MAX");

  STEPPER_STEPS_PER_REV_X = preferences.getInt("STEPS_PER_REV_X");
  STEPPER_STEPS_PER_MM_X = preferences.getInt("STEPS_PER_MM_X");
  STEPPER_micropasX = preferences.getInt("micropasX");
  STEPPER_pasfiltageX = preferences.getInt("pasfiltageX");

  STEPPER_STEPS_PER_REV_Y = preferences.getInt("STEPS_PER_REV_Y");
  STEPPER_STEPS_PER_MM_Y = preferences.getInt("STEPS_PER_MM_Y");
  STEPPER_micropasY = preferences.getInt("micropasY");
  STEPPER_pasfiltageY = preferences.getInt("pasfiltageY");

  STEPPER_STEPS_PER_REV_Z = preferences.getInt("STEPS_PER_REV_Z");
  STEPPER_STEPS_PER_MM_Z = preferences.getInt("STEPS_PER_MM_Z");
  STEPPER_micropasZ = preferences.getInt("micropasZ");
  STEPPER_pasfiltageZ = preferences.getInt("pasfiltageZ");

  MAX_POS_X = preferences.getInt("MAX_POS_X");
  MAX_POS_Y = preferences.getInt("MAX_POS_Y");
  MAX_POS_Z = preferences.getInt("MAX_POS_Z");

  Serial.println("----------------------------------------------------");
}

void LOAD_CONFIGURATION()
{
  Serial.println("----------------------------------------------------");
  Serial.println("Chargement configuration");

  // RECUPÉRE LES VALEURS

  DC_MOTOR_SPEED_MAX = preferences.getInt("DC_SPEED_MAX");

  STEPPER_STEPS_PER_REV_X = preferences.getInt("STEPS_PER_REV_X");
  STEPPER_STEPS_PER_MM_X = preferences.getInt("STEPS_PER_MM_X");
  STEPPER_micropasX = preferences.getInt("micropasX");
  STEPPER_pasfiltageX = preferences.getInt("pasfiltageX");

  STEPPER_STEPS_PER_REV_Y = preferences.getInt("STEPS_PER_REV_Y");
  STEPPER_STEPS_PER_MM_Y = preferences.getInt("STEPS_PER_MM_Y");
  STEPPER_micropasY = preferences.getInt("micropasY");
  STEPPER_pasfiltageY = preferences.getInt("pasfiltageY");

  STEPPER_STEPS_PER_REV_Z = preferences.getInt("STEPS_PER_REV_Z");
  STEPPER_STEPS_PER_MM_Z = preferences.getInt("STEPS_PER_MM_Z");
  STEPPER_micropasZ = preferences.getInt("micropasZ");
  STEPPER_pasfiltageZ = preferences.getInt("pasfiltageZ");

  MAX_POS_X = preferences.getInt("MAX_POS_X");
  MAX_POS_Y = preferences.getInt("MAX_POS_Y");
  MAX_POS_Z = preferences.getInt("MAX_POS_Z");

  Serial.println("----------------------------------------------------");
}

void STEPPER_JOYSTICK_X()
{

  int val = analogRead(JOYSTOCK_PIN);

  // if the joystic is in the middle ===> stop the motor
  if ((val > 500) && (val < 523))
  {
    digitalWrite(STEPPER_IN1_X, LOW);
    digitalWrite(STEPPER_IN2_X, LOW);
    digitalWrite(STEPPER_IN3_X, LOW);
    digitalWrite(STEPPER_IN4_X, LOW);
  }

  else
  {
    // move the motor in the first direction
    while (val >= 523)
    {
      // map the speed between 5 and 500 rpm
      int speed_ = map(val, 523, 1023, 5, 500);
      // set motor speed
      stepperMotor_X.setSpeed(speed_);

      // move the motor (1 step)
      stepperMotor_X.step(1);

      val = analogRead(JOYSTOCK_PIN);
    }

    // move the motor in the other direction
    while (val <= 500)
    {
      // map the speed between 5 and 500 rpm
      int speed_ = map(val, 500, 0, 5, 500);
      // set motor speed
      stepperMotor_X.setSpeed(speed_);

      // move the motor (1 step)
      stepperMotor_X.step(-1);

      val = analogRead(JOYSTOCK_PIN);
    }
  }
}

void SEND_EVENT()
{
  // Envoie les valeurs de la fonction getValueReadings toutes les 250ms.
  if ((millis() - lastTime) > timerDelay)
  {

    events.send("ping", NULL, millis());
    events.send(getValueReadings().c_str(), "new_readings", millis());
    lastTime = millis();
  }
}