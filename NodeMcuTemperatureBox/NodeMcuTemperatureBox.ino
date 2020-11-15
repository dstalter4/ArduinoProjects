// PROPAGATOR THERMOSTAT
// IoT project using NodeMCU in combination with Blynk to control the soil temperature in a heated seed propagator
// C 2018 FoxhillSoft
// https://www.instructables.com/Propagator-Thermostat-Using-ESP8266NodeMCU-and-Bly/


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>


// NodeMCU pin layout
#define LCD_SCL_PIN D1          // GPI04
#define LCD_SDA_PIN D2          // GPI05
#define ONE_WIRE_BUS D3         // GPI00
#define BUTTON_DOWN_PIN D5      // GPIO14
#define BUTTON_UP_PIN D6        // GPI012
#define HEAT_PIN D7             // GPIO13
#define BUTTON_BACKLIGHT_PIN D9 // GPIO3

// Global defines
#define T_TARGET_MIN 15           // Minimum target soil temperature (in degrees Celsius)
#define T_TARGET_MAX 35           // Maximum target soil temperature (in degrees Celsius)
#define COLOR_RED "#E3233F"       // Color of the red LED in Blynk app
#define COLOR_GREEN "#68FC79"     // Color of the green LED in Blynk app
#define TIMER_CHECK_BLYNK  11000L // Frequency in ms to check Blynk connection 
#define TIMER_TURN_OFF_LCD 60000L // Frequency in ms to turn off LCD backlight

// Setup a oneWire instance to communicate with the DS1820 temperature Sensors
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature Sensors(&oneWire);

// Setup Blynk timer, used to periodically check the Blynk connection
BlynkTimer Timer1;

// Setup Blynk timer, used to turn off LCD backlight after some time
BlynkTimer Timer2;

// Construct an LCD object and pass it the
// I2C address, width (in characters) and
// height (in characters). Depending on the
// Actual device, the IC2 address may change.
LiquidCrystal_I2C LCD(0x3F, 16, 2);

// Important: enter your own data below!
char MySSID[]           = "<<<your SSID goes here>>>";                 // SSID of your WiFi network
char MyPassword[]       = "<<<your password goes here>>>";                  // Password of your WiFi network
char BlynkAuthToken[] = "<<<your Blynk authorization token goes here>>>"; // Your Auth Token in the Blynk App, see the Project Settings (nut icon).

float TempAir;                                              // Air temperature
float TempSoil;                                             // Soil temperature
int   TempTarget;                                           // Target soil temperature
bool  Connected2Blynk;                                      // Blynk connection status
bool  ActuatorHeat;                                         // Actuator heating status
bool  ErrorStatus;                                          // Error status


// ------------------------FUNCTIONS------------------------------------------------------
BLYNK_WRITE(V2)
// This function will be calLED every time Slider Widget
// in Blynk app writes values to the Virtual Pin V2
{
  TempTarget = param.asInt();                            // Assigning incoming value from pin V2 to variable
  Blynk.virtualWrite(V5, TempTarget);
  Serial.println(TempTarget);
  LCD.backlight();                                       // Turn on the backlight
}

WidgetLED LED(V3);
// Register LED in Blynk app to Virtual Pin V3

static void UpdateRelays();

// Fill in real names and pins
static const int RELAY_1_NAME = 0;
static const int RELAY_2_NAME = 0;
static const int RELAY_3_NAME = 0;
static const int RELAY_4_NAME = 0;


void setup() {
  // --------- Step 1: initialize the device
  InitDevice();

  // --------- Step 2: start WiFi
  InitWiFi();

  // --------- Step 3: connect to Blynk
  InitBlynk();

  // --------- Step 4: initialize LCD display
  //InitLCD();
}

void loop() {
  // --------- Step 1: get sensor data
  GetSensorData();

  // --------- Step 2: get manual input
  GetManualInput();

  // --------- Step 3: take action if required
  Check4Action();

  // --------- Step 4: update Blynk
  ProcessBlynk();

  // --------- Step 5: update LCD display
  //UpdateLCD();

  UpdateRelays();
}

void InitDevice()
// Initialize device hardware and software
{
  Serial.begin(115200);
  delay(10);

  Serial.println("Initializing device");

  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);                // Activate 'up button' pin as input including internal pullup
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);              // Activate 'down button' pin as input including internal pullup
  pinMode(BUTTON_BACKLIGHT_PIN, INPUT_PULLUP);         // Activate 'backlight button' pin as input including internal pullup
  pinMode(HEAT_PIN, OUTPUT);                           // Activate 'heat relay' pin as output

  // Configure relays as outputs
  pinMode(RELAY_1_NAME, OUTPUT);
  pinMode(RELAY_2_NAME, OUTPUT);
  pinMode(RELAY_3_NAME, OUTPUT);
  pinMode(RELAY_4_NAME, OUTPUT);

  // Start by turning heat relay OFF
  ActuatorHeat = false;
  digitalWrite(HEAT_PIN, LOW);                         // Turn heat relay off

  //Initialize global variables
  TempTarget = T_TARGET_MIN;
  Connected2Blynk = false;
  ErrorStatus = false;

  // Setup the LCD display. The begin call takes the width and height.
  // This should match the number provided to the constructor.
  Wire.begin(LCD_SDA_PIN, LCD_SCL_PIN);
  LCD.begin(16, 2);                                     // Initialize LCD display
  LCD.backlight();                                      // Turn on the backlight
  LCD.setCursor(0, 0);
  LCD.print("Blynk Thermostat");
  LCD.setCursor(0, 1);
  LCD.print("FoxhillSoft 2018");
  delay(2000);
  LCD.setCursor(0, 0);
  LCD.print("Initializing    ");
  LCD.setCursor(0, 1);
  LCD.print("Sensors...      ");
  // Start up the temperatur sensor library
  Sensors.begin();
}

void InitWiFi()
//Start WiFi connection
{
  // Check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // Don't continue:
    while (true);
  }

  // Connect to WiFi
  Serial.print("Connecting to ");
  Serial.println(MySSID);
  LCD.setCursor(0, 1);
  LCD.print("WiFi...         ");

  WiFi.begin(MySSID, MyPassword);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    if (!DebounceKey(BUTTON_BACKLIGHT_PIN)) ESP.reset();  // Sometimes Wifi connection fails, use backlight button as reset button
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void InitBlynk()
// Connect to Blynk
{
  Serial.println("Connecting to Blynk");
  LCD.setCursor(0, 1);
  LCD.print("Blynk...        ");

  Blynk.config(BlynkAuthToken);
  // line below needs to be BEFORE Blynk.connect()
  Timer1.setInterval(TIMER_CHECK_BLYNK, CheckConnection); // Periodically check if Blynk still connected
  Blynk.connect();
  if (Blynk.connected())
  {
    Connected2Blynk = true;
    Serial.println("Connected to Blynk server");

    // Inform Blynk about initial status
    Blynk.setProperty(V3, "color", COLOR_GREEN);
    Blynk.setProperty(V3, "label", "Off");
    Blynk.virtualWrite(V4, ActuatorHeat);
    Blynk.virtualWrite(V2, TempTarget);
    Blynk.virtualWrite(V5, TempTarget);
    LED.on();
    LED.setValue(255);
  }
}

void InitLCD()
// Show labels on LCD display
{
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("T Air  Soil Trgt");
  LCD.setCursor(0, 1);
  LCD.print("C               ");
  Timer2.setInterval(TIMER_TURN_OFF_LCD, TurnOffLCD); // Periodically turn LCD backlight off
}

void GetSensorData()
// Get data from all temperature Sensors
{
  // Issue a global temperature request to all devices on the bus
  Serial.print("Requesting temperatures...");
  Sensors.requestTemperatures();   // Send the command to get temperatures
  TempSoil = Sensors.getTempCByIndex(0);
  TempAir = Sensors.getTempCByIndex(1);
  Serial.println("done");
  Serial.print("Soil temperature via device 1 (index 0) is: ");
  Serial.println(TempSoil);
  Serial.print("Air temperature via device 2 (index 1) is: ");
  Serial.println(TempAir);

  // Check if Sensors still work properly
  if ((TempSoil < -75) or (TempAir < -75) or (TempSoil > 75) or (TempAir > 75))
  {
    Serial.println("Invalid temperature(s) detected, sensor error");
    ErrorStatus = true;
  }
  else ErrorStatus = false;
}

void  GetManualInput()
// Check for manual input
{
  if (!DebounceKey(BUTTON_DOWN_PIN))
  {
    // Button to decrease target temperature is pressed
    Serial.println("Button down pressed");
    LCD.backlight();                  // Turn on the backlight
    TempTarget--;
    if (TempTarget < T_TARGET_MIN) TempTarget = T_TARGET_MIN;
    UpdateBlynk();
  }
  if (!DebounceKey(BUTTON_UP_PIN))
  {
    // button to increase target temperature is pressed
    Serial.println("Button up pressed");
    LCD.backlight();                  // Turn on the backlight
    TempTarget++;
    if (TempTarget > T_TARGET_MAX) TempTarget = T_TARGET_MAX;
    UpdateBlynk();
  }
  if (!DebounceKey(BUTTON_BACKLIGHT_PIN))
  {
    // button to turn on LCD backlight is pressed
    Serial.println("Button LCD backlight pressed");
    LCD.backlight();                  // Turn on the backlight
  }
}

void Check4Action()
// See if it is time to turn on or off the heat
{
  if (ErrorStatus) TurnHeatOn(false);
  else
  {
    if (!ActuatorHeat && (TempTarget > TempSoil))TurnHeatOn(true);
    if ( ActuatorHeat && (TempTarget < TempSoil))TurnHeatOn(false);
  }
}

void TurnHeatOn(bool TurnOn)
// Turn heat on or off
{
  if (TurnOn)
  {
    Serial.println("Turning heat on");
    ActuatorHeat = true;
    // Turn heat relay on
    digitalWrite(HEAT_PIN, HIGH);
    // Inform Blynk
    Blynk.setProperty(V3, "color", COLOR_RED);
    Blynk.setProperty(V3, "label", "On");
    UpdateBlynk();
  }
  else
  {
    Serial.println("Turning heat off");
    ActuatorHeat = false;
    // Turn heat relay off
    digitalWrite(HEAT_PIN, LOW);
    // Inform Blynk
    Blynk.setProperty(V3, "color", COLOR_GREEN);
    Blynk.setProperty(V3, "label", "Off");
    UpdateBlynk();
  }
}

void ProcessBlynk()
// Process Blynk
{
  if (Connected2Blynk) {
    Blynk.run();                    // Process incoming commands and perform housekeeping of Blynk connection
  }
  Timer1.run();                      // Poll timer1 to see if it is time for action
  Timer2.run();                      // Poll timer2 to see if it is time for action
}

void UpdateLCD()
// Show data on LCD display
{
  LCD.setCursor(0, 1);
  if (ErrorStatus)
  {
    LCD.backlight();                // Turn on the backlight
    LCD.print("! SENSOR ERROR !");
  }
  else
  {
    LCD.print("C               ");
    LCD.setCursor(2, 1);
    LCD.print(TempAir, 1);
    LCD.setCursor(7, 1);
    LCD.print(TempSoil, 1);
    LCD.setCursor(13, 1);
    LCD.print(TempTarget);
  }
}

void CheckConnection()
// Timer function to periodically check if we are still connected to Blynk server
{
  if (!Blynk.connected())
  {
    Serial.println("Not connected to Blynk server");
    Connected2Blynk = false;
    Blynk.connect();  // Try to connect to server with default timeout
  }
  else
  {
    Serial.println("Connected to Blynk server");
    Connected2Blynk = true;
    UpdateBlynk();   // Time to update data in the Blynk app
  }
}

void UpdateBlynk()
// Write device data to Blynk app via virtual pins
{
  Blynk.virtualWrite(V0, TempAir);
  Blynk.virtualWrite(V1, TempSoil);
  Blynk.virtualWrite(V2, TempTarget);
  Blynk.virtualWrite(V4, ActuatorHeat);
  Blynk.virtualWrite(V5, TempTarget);
}

void TurnOffLCD()
{
  LCD.noBacklight(); // Turn off LCD backlight, can be switched on manually again
}

boolean DebounceKey(int pin)
// Deboucing a key
{
  boolean State;
  boolean PreviousState;
  const int DebounceDelay = 30;

  PreviousState = digitalRead(pin);
  for (int Counter = 0; Counter < DebounceDelay; Counter++)
  {
    delay(1);
    State = digitalRead(pin);
    if (State != PreviousState)
    {
      Counter = 0;
      PreviousState = State;
    }
  }
  return State;
}

static void UpdateRelays()
{
  // Turn a relay on
  if (TempAir >= 100.0)
  {
    digitalWrite(RELAY_1_NAME, HIGH);
  }
}

