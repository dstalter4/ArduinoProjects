#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include "PS2X_lib.h"

class TankBot
{
public:
  static void Setup();
  static void Loop();

private:
  // Member functions
  static void ArcadeDrivePs2x();
  static void TankDrivePs2x();
  static void ArcadeDriveFsi6();
  static void TankDriveFsi6();
  static void AutoDrive();
  static void SetMotorValues(double leftOutput, double rightOutput);
  static void GyroSequence(bool bReadNewCenter);

  // Member variables
  static PS2X m_Ps2x;
  static Adafruit_BNO055 m_Bno;
  
  static double m_RobotAngle;
  static double m_RobotAbsoluteAngle;
  static double m_RobotRelativeAngle;
  static double m_RobotCenterPoint;

  // Constants
  static const bool           USE_PS2X_CONTROLLER                     = false;
  static const bool           USE_FS_I6_CONTROLLER                    = true;

  static const unsigned       FS_I6_SWITCH_THRESHOLD_VALUE            = 1500;
  static const unsigned long  PULSE_IN_TIMEOUT_US                     = 50000;
  
  static const int32_t        BNO055_SENSOR_ID                        = 55;
  
  static const unsigned int   H_BRIDGE_ENA                            = 3;
  static const unsigned int   H_BRIDGE_IN1                            = 5;
  static const unsigned int   H_BRIDGE_IN2                            = 6;
  static const unsigned int   H_BRIDGE_IN3                            = 7;
  static const unsigned int   H_BRIDGE_IN4                            = 8;
  static const unsigned int   H_BRIDGE_ENB                            = 9;
  
  static const unsigned int   PS2_CONTROLLER_DATA                     = 10;
  static const unsigned int   PS2_CONTROLLER_COMMAND                  = 11;
  static const unsigned int   PS2_CONTROLLER_ATTENTION_SELECT         = 12;
  static const unsigned int   PS2_CONTROLLER_CLOCK                    = 13;

  static const unsigned int   NANO_MOVE_LEFT_PIN                      = 24;
  static const unsigned int   NANO_MOVE_RIGHT_PIN                     = 25;
  
  static const unsigned int   CH1_INPUT_PIN                           = 48;
  static const unsigned int   CH2_INPUT_PIN                           = 49;
  static const unsigned int   CH3_INPUT_PIN                           = 50;
  static const unsigned int   CH4_INPUT_PIN                           = 51;
  static const unsigned int   CH5_INPUT_PIN                           = 52;
  static const unsigned int   CH6_INPUT_PIN                           = 53;
};

PS2X TankBot::m_Ps2x;
Adafruit_BNO055 TankBot::m_Bno = Adafruit_BNO055(BNO055_SENSOR_ID);
double TankBot::m_RobotAngle = 0.0;
double TankBot::m_RobotAbsoluteAngle = 0.0;
double TankBot::m_RobotRelativeAngle = 0.0;
double TankBot::m_RobotCenterPoint = 0.0;

////////////////////////////////////////////////////////////////////////////////
/// Method: setup
///
/// Details:  The Arduino initialization function called during controller
///           start up. 
////////////////////////////////////////////////////////////////////////////////
void setup()
{
  TankBot::Setup();
}


////////////////////////////////////////////////////////////////////////////////
/// Method: loop
///
/// Details:  The continuous Arduino background user loop.
////////////////////////////////////////////////////////////////////////////////
void loop()
{
  TankBot::Loop();
}


////////////////////////////////////////////////////////////////////////////////
/// Method: Setup
///
/// Details:  Tank bot setup function.
////////////////////////////////////////////////////////////////////////////////
void TankBot::Setup()
{
  const unsigned int ONE_SECOND_DELAY_MS = 1000;
  
  Serial.begin(115200);
  Serial.println("Tank bot.");

  if (USE_PS2X_CONTROLLER)
  {
    // ps2 controller setup
    // setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
    delay(300);
    int error = 0;
    int tries = 0;
    do
    {
      error = m_Ps2x.config_gamepad(PS2_CONTROLLER_CLOCK, PS2_CONTROLLER_COMMAND, PS2_CONTROLLER_ATTENTION_SELECT, PS2_CONTROLLER_DATA, false, false);
      tries++;
    } while (error != 0);
    Serial.print("Attempts to connect to PS2 controller: ");
    Serial.println(tries);
  }

  else if (USE_FS_I6_CONTROLLER)
  {
    // Set pin mode for all controller inputs
    pinMode(CH1_INPUT_PIN, INPUT);
    pinMode(CH2_INPUT_PIN, INPUT);
    pinMode(CH3_INPUT_PIN, INPUT);
    pinMode(CH4_INPUT_PIN, INPUT);
    pinMode(CH5_INPUT_PIN, INPUT);
    pinMode(CH6_INPUT_PIN, INPUT); 
    Serial.println("FlySky i6 Controller.");
  }

  else
  {
    Serial.println("No controller configured.");
  }

  // h-bridge outputs
  pinMode(H_BRIDGE_ENA, OUTPUT);
  pinMode(H_BRIDGE_IN1, OUTPUT);
  pinMode(H_BRIDGE_IN2, OUTPUT);
  pinMode(H_BRIDGE_IN3, OUTPUT);
  pinMode(H_BRIDGE_IN4, OUTPUT);
  pinMode(H_BRIDGE_ENB, OUTPUT);

  pinMode(NANO_MOVE_LEFT_PIN, INPUT);
  pinMode(NANO_MOVE_RIGHT_PIN, INPUT);
  
  // Initialize the 9-axis sensor
  while (!m_Bno.begin())
  {
    // There was a problem detecting the BNO055 ... check your connections
    Serial.println("No BNO055 detected... check wiring or I2C ADDR!");

    // Delay before trying again
    delay(ONE_SECOND_DELAY_MS);
  }
  
  m_Bno.setExtCrystalUse(true);

  delay(ONE_SECOND_DELAY_MS);
}


////////////////////////////////////////////////////////////////////////////////
/// Method: Loop
///
/// Details:  Tank bot main loop.
////////////////////////////////////////////////////////////////////////////////
void TankBot::Loop()
{
  static bool bWasAuto = false;
  static bool bWasManual = true;
  static bool bReadCenter = true;
  static const unsigned MODE_SWITCH_DELAY_MS = 100;
  
  GyroSequence(bReadCenter);

  // Check if the controller switch for auto is flipped
  if (pulseIn(CH6_INPUT_PIN, HIGH, PULSE_IN_TIMEOUT_US) > FS_I6_SWITCH_THRESHOLD_VALUE)
  {
    // First time here from manual, switch state and read a new center
    if (bWasManual)
    {
      bWasAuto = true;
      bWasManual = false;
      bReadCenter = true;
      delay(MODE_SWITCH_DELAY_MS);
    }
    // Not the first time through, don't get a new center
    else
    {
      bReadCenter = false;
    }
    
    AutoDrive();
  }
  else
  {
    
    // First time here from auto, switch state
    if (bWasAuto)
    {
      bWasAuto = false;
      bWasManual = true;
      delay(MODE_SWITCH_DELAY_MS);
    }
    // Manual control never reads a new center
    bReadCenter = false;
    
    if (USE_PS2X_CONTROLLER)
    {
      ArcadeDrivePs2x();
      //TankDrivePs2x();
    }
    else if (USE_FS_I6_CONTROLLER)
    {
      ArcadeDriveFsi6();
      //TankDriveFsi6();
    }
    else
    {
    }
  }
}


////////////////////////////////////////////////////////////////////////////////
/// Method: ArcadeDrivePs2x
///
/// Details:  Tank bot arcade drive with a PS2 controller.
////////////////////////////////////////////////////////////////////////////////
void TankBot::ArcadeDrivePs2x()
{
  // PS2X axis inputs:
  //      0
  //      |
  // 0 ------- 255
  //      |
  //     255
  //
  
  m_Ps2x.read_gamepad();
  
  // Base values
  double xAxis = m_Ps2x.Analog(PSS_LX);
  double yAxis = m_Ps2x.Analog(PSS_LY);

  // Normalize to -1 -> +1
  xAxis = (xAxis - 128.0) / 128.0;
  yAxis = (yAxis - 128.0) / 128.0;

  // Calculate motor outputs
  // Tank bot uses these equations
  //double leftOutput = xAxis - yAxis;
  //double rightOutput = -xAxis - yAxis;
  // George's bot uses these equations
  double leftOutput = -xAxis + yAxis;
  double rightOutput = xAxis + yAxis;

  SetMotorValues(leftOutput, rightOutput);
}


////////////////////////////////////////////////////////////////////////////////
/// Method: TankDrivePs2x
///
/// Details:  Tank bot tank drive with a PS2 controller.
////////////////////////////////////////////////////////////////////////////////
void TankBot::TankDrivePs2x()
{
  // PS2X axis inputs:
  //      0
  //      |
  // 0 ------- 255
  //      |
  //     255
  //
  
  m_Ps2x.read_gamepad();
  
  // Base values
  double leftValue = m_Ps2x.Analog(PSS_LY);
  double rightValue = m_Ps2x.Analog(PSS_RY);

  // Normalize to -1 -> +1
  leftValue = (leftValue - 128.0) / 128.0;
  rightValue = (rightValue - 128.0) / 128.0;

  // Tank drive is just passing the values through
  SetMotorValues(leftValue, rightValue);
}


////////////////////////////////////////////////////////////////////////////////
/// Method: ArcadeDriveFsi6
///
/// Details:  Tank bot arcade drive with a FlySky i6 controller.
////////////////////////////////////////////////////////////////////////////////
void TankBot::ArcadeDriveFsi6()
{
  // FSi6 axis inputs:
  //       2000
  //         |
  // 1000 ------- 2000 ch1
  //         |
  //       1000
  //        ch3
  //
  
  // Base values
  double xAxis = pulseIn(CH1_INPUT_PIN, HIGH, PULSE_IN_TIMEOUT_US);
  double yAxis = pulseIn(CH2_INPUT_PIN, HIGH, PULSE_IN_TIMEOUT_US);

/*
  int c1 = pulseIn(CH1_INPUT_PIN, HIGH, PULSE_IN_TIMEOUT_US);
  int c2 = pulseIn(CH2_INPUT_PIN, HIGH, PULSE_IN_TIMEOUT_US);
  int c3 = pulseIn(CH3_INPUT_PIN, HIGH, PULSE_IN_TIMEOUT_US);
  int c4 = pulseIn(CH4_INPUT_PIN, HIGH, PULSE_IN_TIMEOUT_US);
  int c5 = pulseIn(CH5_INPUT_PIN, HIGH, PULSE_IN_TIMEOUT_US);
  int c6 = pulseIn(CH6_INPUT_PIN, HIGH, PULSE_IN_TIMEOUT_US);
  Serial.println(c1);
  Serial.println(c2);
  Serial.println(c3);
  Serial.println(c4);
  Serial.println(c5);
  Serial.println(c6);
  Serial.println();
  delay(500);
  return;
*/

  if ((xAxis == 0.0) || (yAxis == 0.0))
  {
    // Controller off
    return;
  }

  // Normalize to -1 -> +1
  xAxis = (xAxis - 1500.0) / 500.0;
  yAxis = (yAxis - 1500.0) / 500.0;

  // Calculate motor outputs
  double leftOutput = xAxis + yAxis;
  double rightOutput = -xAxis + yAxis;

  SetMotorValues(leftOutput, rightOutput);

/*
  Serial.println("L, R");
  Serial.print(leftOutput);
  Serial.print(", ");
  Serial.println(rightOutput);
  
  delay(500);
*/
}


////////////////////////////////////////////////////////////////////////////////
/// Method: TankDriveFsi6
///
/// Details:  Tank bot tank drive with a FlySky i6 controller.
////////////////////////////////////////////////////////////////////////////////
void TankBot::TankDriveFsi6()
{
  // Not implemented
}


////////////////////////////////////////////////////////////////////////////////
/// Method: AutoDrive
///
/// Details:  Tank bot autonomous drive with return to center capability.
////////////////////////////////////////////////////////////////////////////////
void TankBot::AutoDrive()
{
  static const double ROBOT_DRIVE_SPEED = 1.0;
  //static const double ROBOT_DRIVE_TARGET_COMPENSATE_SPEED = 0.85;//0.50;
  static const double ROBOT_MAX_DRIFT_ANGLE_DEGREES = 2.5;

  // Input from the Nano (LIDAR) overrules normal movement
  // LIDAR is on the right side of the robot
  // @todo: use arc correction for a set time, then go straight
  if (digitalRead(NANO_MOVE_LEFT_PIN) == HIGH)
  {
    // Robot is too close to the target, return left
    //SetMotorValues(-ROBOT_DRIVE_SPEED, ROBOT_DRIVE_SPEED);
    SetMotorValues(0.0, .80);//ROBOT_DRIVE_TARGET_COMPENSATE_SPEED, ROBOT_DRIVE_SPEED);
  }
  else if (digitalRead(NANO_MOVE_RIGHT_PIN) == HIGH)
  {
    // Robot is too far from the target, return right
    //SetMotorValues(ROBOT_DRIVE_SPEED, -ROBOT_DRIVE_SPEED);
    SetMotorValues(.80, 0.0);//ROBOT_DRIVE_SPEED, ROBOT_DRIVE_TARGET_COMPENSATE_SPEED);
  }
  else
  {
    // No guidance from the LIDAR, drive by the gyro
    if (m_RobotAngle > ROBOT_MAX_DRIFT_ANGLE_DEGREES)
    {
      // Robot is drifting right, return left
      SetMotorValues(0.0, .80);//-ROBOT_DRIVE_SPEED, ROBOT_DRIVE_SPEED);
      //Serial.println("Returning left.");
    }
    else if (m_RobotAngle < -ROBOT_MAX_DRIFT_ANGLE_DEGREES)
    {
      // Robot is drifting left, return right
      SetMotorValues(.80, 0.0);//ROBOT_DRIVE_SPEED, -ROBOT_DRIVE_SPEED);
      //Serial.println("Returning right.");
    }
    else
    {
      SetMotorValues(ROBOT_DRIVE_SPEED, ROBOT_DRIVE_SPEED);
      //Serial.println("Staying straight.");
    }
  }
}


////////////////////////////////////////////////////////////////////////////////
/// Method: SetMotorValues
///
/// Details:  Function to set the tank bot motor values.
////////////////////////////////////////////////////////////////////////////////
void TankBot::SetMotorValues(double leftOutput, double rightOutput)
{
  // Trim output to -1/+1
  leftOutput = (leftOutput > 1.0) ? 1.0 : leftOutput;
  leftOutput = (leftOutput < -1.0) ? -1.0 : leftOutput;
  rightOutput = (rightOutput > 1.0) ? 1.0 : rightOutput;
  rightOutput = (rightOutput < -1.0) ? -1.0 : rightOutput;

  // Limit output to be at least 10%
  leftOutput = (abs(leftOutput) < .10) ? 0.0 : leftOutput;
  rightOutput = (abs(rightOutput) < .10) ? 0.0 : rightOutput;

  // Scale back up for analog output range
  leftOutput *= 255.0;
  rightOutput *= 255.0;

  // Set the h-bridge IN pins to control motor direction
  if (leftOutput >= 0.0)
  {
    digitalWrite(H_BRIDGE_IN1, HIGH);
    digitalWrite(H_BRIDGE_IN2, LOW);
    //Serial.println("1H, 2L");
  }
  else
  {
    digitalWrite(H_BRIDGE_IN1, LOW);
    digitalWrite(H_BRIDGE_IN2, HIGH);
    //Serial.println("1L, 2H");
  }

  if (rightOutput >= 0.0)
  {
    digitalWrite(H_BRIDGE_IN3, HIGH);
    digitalWrite(H_BRIDGE_IN4, LOW);
    //Serial.println("3H, 4L");
  }
  else
  {
    digitalWrite(H_BRIDGE_IN3, LOW);
    digitalWrite(H_BRIDGE_IN4, HIGH);
    //Serial.println("3L, 4H");
  }

  // The analog pulse must be a positive number
  leftOutput = abs(leftOutput);
  rightOutput = abs(rightOutput);

  // Set the h-bridge for forward/reverse control
  analogWrite(H_BRIDGE_ENA, leftOutput);
  analogWrite(H_BRIDGE_ENB, rightOutput);

/*
  Serial.print("x-axis: ");
  Serial.print(xAxis);
  Serial.print(", y-axis: ");
  Serial.println(yAxis);
  Serial.print("left output: ");
  Serial.print(leftOutput);
  Serial.print(", right output: ");
  Serial.println(rightOutput);  
  delay(250);
*/
}


////////////////////////////////////////////////////////////////////////////////
/// Method: GyroSequence
///
/// Details:  Reads information from the BNO055 9-axis sensor.
////////////////////////////////////////////////////////////////////////////////
void TankBot::GyroSequence(bool bReadNewCenter)
{
  static const unsigned int BNO055_SAMPLE_RATE_DELAY_MS = 100U;
  static const double ONE_HUNDRED_EIGHTY_DEGREES = 180.0;
  static const double THREE_HUNDRED_SIXTY_DEGREES = 360.0;
  static unsigned int lastGyroTimeMs = 0U;
  unsigned int currentTimeMs = millis();
  
  if ((currentTimeMs - lastGyroTimeMs) > BNO055_SAMPLE_RATE_DELAY_MS)
  {
    // Get a new sensor event
    sensors_event_t bnoSensorEvent;
    m_Bno.getEvent(&bnoSensorEvent);

    // For normalization to arbitrary angle as center:
    //
    // Defaults to zero
    // double m_CenterSetPointDegrees = 0.0;
    // m_RobotAbsoluteAngle = bnoSensorEvent.orientation.x;
    // m_RobotRelativeAngle = m_RobotAbsoluteAngle - m_CenterSetPointDegrees;
    // if (m_RobotRelativeAngle < 0.0)
    // {
    //   // Negative angles must be normalized from 360 (the addition here is actually subtraction)
    //   m_RobotRelativeAngle += THREE_HUNDRED_SIXTY_DEGREES;
    // }

    // Save off a new center if one was requested
    if (bReadNewCenter)
    {
      Serial.println("Reading new center...");
      m_RobotCenterPoint = bnoSensorEvent.orientation.x;
    }

    // The absolute angle is what the sensor reports.
    // The relative angle needs to be computed from the absolute angle.
    m_RobotAbsoluteAngle = bnoSensorEvent.orientation.x;
    m_RobotRelativeAngle = m_RobotAbsoluteAngle - m_RobotCenterPoint;

    // The relative angle can be anywhere from -360 to + 360 at this point.
    // We need an angle between 0 -> 360 only.
    // Negative angles must be normalized from 360 (the addition here is actually subtraction).
    if (m_RobotRelativeAngle < 0.0)
    {
      m_RobotRelativeAngle += THREE_HUNDRED_SIXTY_DEGREES;
    }

    // Now that we have an angle from 0 -> 360, convert it to -180 -> 180
    if (m_RobotRelativeAngle > ONE_HUNDRED_EIGHTY_DEGREES)
    {
      // convert left half of unit circle to negative
      // old: 360 (top) -> 270 (left) -> 180 (bottom)
      // new: 0 (top) -> -90 (left) -> -180 (bottom)
      m_RobotRelativeAngle -= THREE_HUNDRED_SIXTY_DEGREES;
    }

    // Robot angle is the relative angle
    m_RobotAngle = m_RobotRelativeAngle;

/*
    Serial.print("Abs angle: ");
    Serial.println(m_RobotAbsoluteAngle);
    Serial.print("Rel angle: ");
    Serial.println(m_RobotRelativeAngle);
    delay(1000);
*/

    lastGyroTimeMs = currentTimeMs;
  }
}

