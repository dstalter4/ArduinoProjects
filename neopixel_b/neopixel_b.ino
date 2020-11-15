#include <Adafruit_NeoPixel.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

// DARK RED:  {136,   0,  21}
// RED:       {237,  28,  36}
// ROSE:      {255, 174, 201}
// ORANGE:    {255, 127,  39}
// GOLD:      {255, 201,  14}
// YELLOW:    {255, 242,   0}
// LT YEL:    {239, 228, 176}
// GREEN:     { 34, 177,  76}
// LIME:      {181, 230,  29}
// TURQUOISE: {  0, 162, 232}
// LT TURQ:   {153, 217, 234}
// INDIGO:    { 63,  72, 204}
// BLUE-GRAY: {112, 146, 190}
// PURPLE:    {163,  73, 164}
// LAVENDER:  {200, 191, 231}
// BROWN:     {185, 122,  87}
// WHITE:     {255, 255, 255}


////////////////////////////////////////////////////////////////
// @class NeoPixel
///
/// Description
///
////////////////////////////////////////////////////////////////
class NeoPixel
{
  public:
    // Retrieve the singleton instance
    inline static Adafruit_NeoPixel & GetStrip() { return m_NeoPixelStrip; }

    // Call at power up to initialize everything
    static void Init();

    // Run time functionality
    static void RunTime();
    
  private:
    // Constructor / Destructor / Copy / Assignment
    NeoPixel();
    ~NeoPixel();
    NeoPixel(const NeoPixel &);
    NeoPixel & operator=(const NeoPixel &);

    // Sync the two arduinos
    static void SyncArduinos();

    // Sends a single color through the strand
    static void ColorWipe(uint32_t color, uint8_t wait);

    // Applies a rainbow throughout the strand
    static void Rainbow(uint8_t wait);
    static void RainbowCycle(uint8_t wait);

    // Marquee style flashing
    static void TheaterChase(uint32_t color, uint8_t wait);
    static void TheaterChaseRainbow(uint8_t wait);
    static void CustomTheaterChase();

    // Retrieve a color
    static uint32_t Wheel(uint8_t wheelPos);

    // Give the LEDs a color at power up
    static void SetLedsAtPowerOn(uint8_t wait);

    // Retrieve a single color value based on RGB components
    static uint32_t GetSingleColorValue(uint32_t redVal, uint32_t greenVal, uint32_t blueVal);
    
    // Turn the strip off
    static void TurnStripOff();

    // Set the strip to a solid color
    static void SetStripColor(uint32_t color);
    static void SetStripColor(uint32_t redVal, uint32_t greenVal, uint32_t blueVal);

    // Blink the strand with a solid color
    static void Blink(uint32_t color, uint16_t wait);

    // Apply alternating colors to the strand
    static void AlternateColors(uint32_t color1, uint32_t color2, uint32_t startingColor, uint8_t wait);
    static void AlternateThreeColors(uint32_t color1, uint32_t color2, uint32_t color3, uint32_t startingColor, uint8_t wait);
    
    static void StripeChase(uint32_t solidColor, uint32_t movingColor, uint8_t wait);
    static void RacingDots(uint32_t baseColor, uint32_t dotColor, uint8_t wait);

    // Vary intensity methods
    static void RampIntensityUp(uint32_t minVal, uint32_t maxVal);
    static void RampIntensityDown(uint32_t maxVal, uint32_t minVal);

    // The singleton instance
    static Adafruit_NeoPixel m_NeoPixelStrip;

    // Struct to describe color component values
    struct RgbValues
    {
      uint8_t m_Red;
      uint8_t m_Green;
      uint8_t m_Blue;
      uint8_t m_White;
    };
    
    static const RgbValues RED;
    static const RgbValues ROSE;
    static const RgbValues ORANGE;
    static const RgbValues GOLD;
    static const RgbValues YELLOW;
    static const RgbValues GREEN;
    static const RgbValues LIME;
    static const RgbValues TURQUOISE;
    static const RgbValues BLUE;
    static const RgbValues INDIGO;
    static const RgbValues PURPLE;
    static const RgbValues LAVENDER;
    static const RgbValues BROWN;
    static const RgbValues WHITE;
    
    static const RgbValues COLORS[];
    static const uint32_t NUM_COLORS;

    // NeoPixel B has input on pin 10, output on pin 9
    static const uint32_t DATA_OUTPUT_PIN             = 8U;
    static const uint32_t SYNC_OUTPUT_PIN             = 9U;
    static const uint32_t SYNC_INPUT_PIN              = 10U;
    
    static const uint32_t NUM_PIXEL_LEDS              = 420U;
    
    static const uint32_t SHORT_DELAY_MS              = 20U;
    static const uint32_t LONG_DELAY_MS               = 50U;
    static const uint32_t TENTH_SECOND_DELAY_MS       = 100U;
    static const uint32_t QUARTER_SECOND_DELAY_MS     = 250U;
    static const uint32_t HALF_SECOND_DELAY_MS        = 500U;
    static const uint32_t ONE_SECOND_DELAY_MS         = 1000U;
    
    static const uint32_t MAX_R_VALUE                 = 255U;
    static const uint32_t MAX_G_VALUE                 = 255U;
    static const uint32_t MAX_B_VALUE                 = 255U;
    static const uint32_t MAX_W_VALUE                 = 255U;
    static const uint32_t RGB_VALUE_MASK              = 255U;
    static const uint32_t MAX_RGB_VALUE               = 256U;
    static const uint32_t MAX_FIRST_THIRD_RGB_VALUE   = 85U;
    static const uint32_t MAX_SECOND_THIRD_RGB_VALUE  = 170U;
    static const uint32_t MAX_THIRD_THIRD_RGB_VALUE   = 255U;
    
    static const uint32_t LED_OFF                     = 0U;
    static const uint32_t LED_MIN_INTENSITY           = 10U;
    static const uint32_t LED_MAX_INTENSITY           = 75U;
    
    static const uint32_t NUM_RAINBOW_CYCLES          = 5U;
    static const uint32_t NUM_THEATER_CHASE_CYCLES    = 5U;
    static const uint32_t NUM_BLINK_CYCLES            = 10U;
    static const uint32_t NUM_PIXELS_TO_ALT_BI_COLOR  = 13U;
    static const uint32_t NUM_PIXELS_TO_ALT_TRI_COLOR = 20U;
    static const uint32_t NUM_PIXELS_TO_GAP           = 8U;
    static const uint32_t THEATER_CHASE_LED_FREQUENCY = 3U;
};



// STATIC MEMBER DATA
Adafruit_NeoPixel NeoPixel::m_NeoPixelStrip = Adafruit_NeoPixel(NUM_PIXEL_LEDS, DATA_OUTPUT_PIN, NEO_GRB + NEO_KHZ800);

const NeoPixel::RgbValues NeoPixel::RED       = {255,   0,   0, 0};
const NeoPixel::RgbValues NeoPixel::ROSE      = {255, 174, 201, 0};
const NeoPixel::RgbValues NeoPixel::ORANGE    = {255,  80,  0 , 0};
const NeoPixel::RgbValues NeoPixel::GOLD      = {255, 201,  14, 0};
const NeoPixel::RgbValues NeoPixel::YELLOW    = {255, 242,   0, 0};
const NeoPixel::RgbValues NeoPixel::GREEN     = {  0, 255,   0, 0};
const NeoPixel::RgbValues NeoPixel::LIME      = {181, 230,  29, 0};
const NeoPixel::RgbValues NeoPixel::TURQUOISE = {  0, 162, 232, 0};
const NeoPixel::RgbValues NeoPixel::BLUE      = {  0,   0, 255, 0};
const NeoPixel::RgbValues NeoPixel::INDIGO    = { 63,  72, 204, 0};
const NeoPixel::RgbValues NeoPixel::PURPLE    = {163,  73, 164, 0};
const NeoPixel::RgbValues NeoPixel::LAVENDER  = {200, 191, 231, 0};
const NeoPixel::RgbValues NeoPixel::BROWN     = {185, 122,  87, 0};
const NeoPixel::RgbValues NeoPixel::WHITE     = {255, 255, 255, 0};
const NeoPixel::RgbValues NeoPixel::COLORS[] =
{
  RED, ORANGE, YELLOW, GREEN, TURQUOISE, BLUE, PURPLE, BROWN, WHITE
};
const uint32_t NeoPixel::NUM_COLORS = sizeof(COLORS) / sizeof(COLORS[0]);



////////////////////////////////////////////////////////////////
// @method NeoPixel::SyncArduinos
///
/// This method ...
///
////////////////////////////////////////////////////////////////
void NeoPixel::SyncArduinos()
{
  digitalWrite(SYNC_OUTPUT_PIN, HIGH);
  while (digitalRead(SYNC_INPUT_PIN) == LOW)
  {
  }
  digitalWrite(SYNC_OUTPUT_PIN, LOW);
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::Init
///
/// This method contains the initialization code for the
/// NeoPixel start up.
///
////////////////////////////////////////////////////////////////
void NeoPixel::Init()
{
  // Initialize all pixels to 'off'
  m_NeoPixelStrip.begin();
  m_NeoPixelStrip.setBrightness(LED_MAX_INTENSITY);
  m_NeoPixelStrip.show();

  // Configure input/output pins
  pinMode(SYNC_INPUT_PIN, INPUT);
  pinMode(SYNC_OUTPUT_PIN, OUTPUT);

  // Make sure output pin is low
  digitalWrite(SYNC_OUTPUT_PIN, LOW);
  
  SetLedsAtPowerOn(SHORT_DELAY_MS);
  //SyncArduinos();
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::SetLedsAtPowerOn
///
/// This method ...
///
////////////////////////////////////////////////////////////////
void NeoPixel::SetLedsAtPowerOn(uint8_t wait)
{
  for (uint32_t i = 0; i < m_NeoPixelStrip.numPixels(); i++)
  {
    m_NeoPixelStrip.setPixelColor(i, Wheel(i % RGB_VALUE_MASK));
    m_NeoPixelStrip.show();
    delay(wait);
  }
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::ColorWipe
///
/// This method will fill the dots one after the other with a
/// color.
///
////////////////////////////////////////////////////////////////
void NeoPixel::ColorWipe(uint32_t color, uint8_t wait)
{
  for (uint32_t i = 0; i < m_NeoPixelStrip.numPixels(); i++)
  {
    m_NeoPixelStrip.setPixelColor(i, color);
    m_NeoPixelStrip.show();
    delay(wait);
  }
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::Rainbow
///
/// This method ...
///
////////////////////////////////////////////////////////////////
void NeoPixel::Rainbow(uint8_t wait)
{
  for (uint32_t j = 0; j < MAX_RGB_VALUE; j++)
  {
    for (uint32_t i = 0; i < m_NeoPixelStrip.numPixels(); i++)
    {
      m_NeoPixelStrip.setPixelColor(i, Wheel((i + j) & RGB_VALUE_MASK));
    }
    
    m_NeoPixelStrip.show();
    delay(wait);
  }
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::RainbowCycle
///
/// This method is slightly different from Rainbow.  This makes
/// the rainbow equally distributed throughout the strand.
///
////////////////////////////////////////////////////////////////
void NeoPixel::RainbowCycle(uint8_t wait)
{
  // Cycles of all colors on wheel
  for (uint32_t j = 0; j < (NUM_RAINBOW_CYCLES * MAX_RGB_VALUE); j++)
  {
    for (uint32_t i = 0; i < m_NeoPixelStrip.numPixels(); i++)
    {
      m_NeoPixelStrip.setPixelColor(i, Wheel(((i * MAX_RGB_VALUE / m_NeoPixelStrip.numPixels()) + j) & RGB_VALUE_MASK));
    }
    
    m_NeoPixelStrip.show();
    delay(wait);
  }
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::TheaterChase
///
/// This method displays theatre-style crawling lights.
///
////////////////////////////////////////////////////////////////
void NeoPixel::TheaterChase(uint32_t color, uint8_t wait)
{
  // Cycles of chasing
  for (uint32_t j = 0; j < NUM_THEATER_CHASE_CYCLES; j++)
  {
    for (uint32_t q = 0; q < THEATER_CHASE_LED_FREQUENCY; q++)
    {
      for (uint32_t i = 0; i < m_NeoPixelStrip.numPixels(); i += THEATER_CHASE_LED_FREQUENCY)
      {
        m_NeoPixelStrip.setPixelColor(i + q, color);
      }
      
      m_NeoPixelStrip.show();
      delay(wait);

      for (uint32_t i = 0; i < m_NeoPixelStrip.numPixels(); i += THEATER_CHASE_LED_FREQUENCY)
      {
        // Pixel back off
        m_NeoPixelStrip.setPixelColor(i + q, LED_OFF);
      }
    }
  }
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::TheaterChaseRainbow
///
/// This method displays theatre-style crawling lights with a
/// rainbow effect.
///
////////////////////////////////////////////////////////////////
void NeoPixel::TheaterChaseRainbow(uint8_t wait)
{
  // Cycle all colors in the wheel
  for (uint32_t j = 0; j < MAX_RGB_VALUE; j++)
  {
    for (uint32_t q = 0; q < THEATER_CHASE_LED_FREQUENCY; q++)
    {
      for (uint32_t i = 0; i < m_NeoPixelStrip.numPixels(); i += THEATER_CHASE_LED_FREQUENCY)
      {
        m_NeoPixelStrip.setPixelColor(i + q, Wheel((i+j) % RGB_VALUE_MASK));
      }
      
      m_NeoPixelStrip.show();
      delay(wait);

      for (uint16_t i = 0; i < m_NeoPixelStrip.numPixels(); i += THEATER_CHASE_LED_FREQUENCY)
      {
        // Pixel back off
        m_NeoPixelStrip.setPixelColor(i + q, LED_OFF);
      }
    }
  }
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::CustomTheaterChase
///
/// This method will loop through the custom array of colors,
/// theater chasing all of them.
///
////////////////////////////////////////////////////////////////
void NeoPixel::CustomTheaterChase()
{
  for (uint32_t i = 0; i < NUM_COLORS; i++)
  {
    TheaterChase(NeoPixel::GetStrip().Color(COLORS[i].m_Red,
                                            COLORS[i].m_Green,
                                            COLORS[i].m_Blue),
                                            LONG_DELAY_MS);
    SyncArduinos();
  }
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::Wheel
///
/// This method takes a value between 0 and 255 to get a color
/// value.  The colors are a transition r - g - b - back to r.
///
////////////////////////////////////////////////////////////////
uint32_t NeoPixel::Wheel(uint8_t wheelPos)
{
  wheelPos = MAX_THIRD_THIRD_RGB_VALUE - wheelPos;

  uint32_t color = 0U;
  
  // First third of colors
  if (wheelPos < MAX_FIRST_THIRD_RGB_VALUE)
  {
    color = m_NeoPixelStrip.Color(MAX_THIRD_THIRD_RGB_VALUE - (wheelPos * 3), 0, (wheelPos * 3));
  }
  // Middle third of colors
  else if (wheelPos < MAX_SECOND_THIRD_RGB_VALUE)
  {
    wheelPos -= MAX_FIRST_THIRD_RGB_VALUE;
    color = m_NeoPixelStrip.Color(0, (wheelPos * 3), MAX_THIRD_THIRD_RGB_VALUE - (wheelPos * 3));
  }
  // Final third of colors
  else
  {
    wheelPos -= MAX_SECOND_THIRD_RGB_VALUE;
    color = m_NeoPixelStrip.Color((wheelPos * 3), MAX_THIRD_THIRD_RGB_VALUE - (wheelPos * 3), 0);
  }

  return color;
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::TurnStripOff
///
/// This method ...
///
////////////////////////////////////////////////////////////////
void NeoPixel::TurnStripOff()
{
  for (uint32_t i = 0; i < m_NeoPixelStrip.numPixels(); i++)
  {
    m_NeoPixelStrip.setPixelColor(i, LED_OFF);
  }

  m_NeoPixelStrip.show();
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::RampIntensityUp
///
/// This method ...
///
////////////////////////////////////////////////////////////////
void NeoPixel::RampIntensityUp(uint32_t minVal, uint32_t maxVal)
{
  for (uint32_t i = minVal; i <= maxVal; i++)
  {
    m_NeoPixelStrip.setBrightness(i);
    m_NeoPixelStrip.show();
    delay(TENTH_SECOND_DELAY_MS);
  }
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::RampIntensityDown
///
/// This method ...
///
////////////////////////////////////////////////////////////////
void NeoPixel::RampIntensityDown(uint32_t maxVal, uint32_t minVal)
{
  for (uint32_t i = maxVal; i >= minVal; i--)
  {
    m_NeoPixelStrip.setBrightness(i);
    m_NeoPixelStrip.show();
    delay(TENTH_SECOND_DELAY_MS);
  }
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::SetStripColor
///
/// This method ...
///
////////////////////////////////////////////////////////////////
void NeoPixel::SetStripColor(uint32_t color)
{
  for (uint32_t i = 0; i < m_NeoPixelStrip.numPixels(); i++)
  {
    m_NeoPixelStrip.setPixelColor(i, color);
  }
  
  m_NeoPixelStrip.show();
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::SetStripColor
///
/// This method ...
///
////////////////////////////////////////////////////////////////
void NeoPixel::SetStripColor(uint32_t redVal, uint32_t greenVal, uint32_t blueVal)
{
  SetStripColor(GetSingleColorValue(redVal, greenVal, blueVal));
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::GetSingleColorValue
///
/// This method ...
///
////////////////////////////////////////////////////////////////
uint32_t NeoPixel::GetSingleColorValue(uint32_t redVal, uint32_t greenVal, uint32_t blueVal)
{
  return m_NeoPixelStrip.Color(redVal, greenVal, blueVal);
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::Blink
///
/// This method ...
///
////////////////////////////////////////////////////////////////
void NeoPixel::Blink(uint32_t color, uint16_t wait)
{
  for (uint32_t blinkCycle = 0; blinkCycle < NUM_BLINK_CYCLES; blinkCycle++)
  {
    for (uint32_t i = 0; i < m_NeoPixelStrip.numPixels(); i++)
    {
      m_NeoPixelStrip.setPixelColor(i, color);
    }
  
    m_NeoPixelStrip.show();
    
    delay(wait);
  
    TurnStripOff();

    delay(wait);
  }
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::AlternateColors
///
/// This method ...
///
////////////////////////////////////////////////////////////////
void NeoPixel::AlternateColors(uint32_t color1, uint32_t color2, uint32_t startingColor, uint8_t wait)
{
  static uint32_t startingPixel = 0U;
  
  uint32_t currentColor = startingColor;
  uint32_t currentColorCounter = 0U;
  
  for (uint32_t i = 0U; i < m_NeoPixelStrip.numPixels(); i++)
  {
    uint32_t pixelToSet = startingPixel + i;
    if (pixelToSet >= m_NeoPixelStrip.numPixels())
    {
      pixelToSet -= m_NeoPixelStrip.numPixels();
    }
    
    m_NeoPixelStrip.setPixelColor(pixelToSet, currentColor);
    currentColorCounter++;
    
    if (currentColorCounter == NUM_PIXELS_TO_ALT_BI_COLOR)
    {
      // Leave a gap between the colors to try and prevent color bleeding
      for (uint32_t j = 0U; j < NUM_PIXELS_TO_GAP; j++)
      {
        pixelToSet++;
        if (pixelToSet >= m_NeoPixelStrip.numPixels())
        {
          pixelToSet -= m_NeoPixelStrip.numPixels();
        }
        m_NeoPixelStrip.setPixelColor(pixelToSet, 0U);
      }
      i += NUM_PIXELS_TO_GAP;
      
      if (currentColor == color1)
      {
        currentColor = color2;
      }
      else
      {
        currentColor = color1;
      }
      
      currentColorCounter = 0U;
    }
  }

  startingPixel++;

  if (startingPixel >= m_NeoPixelStrip.numPixels())
  {
    startingPixel = 0U;
  }

  m_NeoPixelStrip.show();
  delay(wait);
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::AlternateThreeColors
///
/// This method ...
///
////////////////////////////////////////////////////////////////
void NeoPixel::AlternateThreeColors(uint32_t color1, uint32_t color2, uint32_t color3, uint32_t startingColor, uint8_t wait)
{
  static uint32_t startingPixel = 0U;
  
  uint32_t currentColor = startingColor;
  uint32_t currentColorCounter = 0U;
  
  for (uint32_t i = 0U; i < m_NeoPixelStrip.numPixels(); i++)
  {
    uint32_t pixelToSet = startingPixel + i;
    if (pixelToSet >= m_NeoPixelStrip.numPixels())
    {
      pixelToSet -= m_NeoPixelStrip.numPixels();
    }
    
    m_NeoPixelStrip.setPixelColor(pixelToSet, currentColor);
    currentColorCounter++;
    
    if (currentColorCounter == NUM_PIXELS_TO_ALT_TRI_COLOR)
    {
      // Leave a gap between the colors to try and prevent color bleeding
      for (uint32_t j = 0U; j < NUM_PIXELS_TO_GAP; j++)
      {
        pixelToSet++;
        if (pixelToSet >= m_NeoPixelStrip.numPixels())
        {
          pixelToSet -= m_NeoPixelStrip.numPixels();
        }
        m_NeoPixelStrip.setPixelColor(pixelToSet, 0U);
      }
      i += NUM_PIXELS_TO_GAP;
      
      if (currentColor == color1)
      {
        currentColor = color2;
      }
      else if (currentColor == color2)
      {
        currentColor = color3;
      }
      else
      {
        currentColor = color1;
      }
      
      currentColorCounter = 0U;
    }
  }

  startingPixel++;

  if (startingPixel >= m_NeoPixelStrip.numPixels())
  {
    startingPixel = 0U;
  }

  m_NeoPixelStrip.show();
  delay(wait);
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::StripeChase
///
/// This method ...
///
////////////////////////////////////////////////////////////////
void NeoPixel::StripeChase(uint32_t solidColor, uint32_t movingColor, uint8_t wait)
{
  uint32_t startInnerPixel = 0;
  uint32_t endInnerPixel = 31;
  static bool bIncreasing = true;

  do
  {
    SetStripColor(solidColor);
    for (uint32_t i = startInnerPixel; i <= endInnerPixel; i++)
    {
      m_NeoPixelStrip.setPixelColor(i, movingColor);
    }
    
    m_NeoPixelStrip.show();
    delay(wait);
  
    if (bIncreasing)
    {
      startInnerPixel++;
      endInnerPixel++;
  
      if (endInnerPixel == m_NeoPixelStrip.numPixels())
      {
        bIncreasing = false;
        SyncArduinos();
      }
    }
    else
    {
      startInnerPixel--;
      endInnerPixel--;
  
      if (startInnerPixel == 0)
      {
        bIncreasing = true;
        SyncArduinos();
      }
    }
  }
  while (startInnerPixel != 0);
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::RacingDots
///
/// This method ...
///
////////////////////////////////////////////////////////////////
void NeoPixel::RacingDots(uint32_t baseColor, uint32_t dotColor, uint8_t wait)
{
  for (uint32_t iterations = 0; iterations < 20; iterations++)
  {
    uint32_t sum = 0;
    for (uint32_t segment = 0; segment < 6; segment++)
    {
      sum += segment;
      uint32_t pixel = (sum*20) + (iterations * (segment+1));

      // Silence warning
      static_cast<void>(pixel);
    }
    static_cast<void>(baseColor);
    static_cast<void>(dotColor);
    delay(wait);
  }
}


////////////////////////////////////////////////////////////////
// @method NeoPixel::RunTime
///
/// This method ...
///
////////////////////////////////////////////////////////////////
void NeoPixel::RunTime()
{
  //RainbowCycle(SHORT_DELAY_MS);
  
  uint32_t redValue   = GetSingleColorValue(MAX_R_VALUE, 0, 0);
  //uint32_t greenValue = GetSingleColorValue(0, MAX_G_VALUE, 0);
  uint32_t blueValue  = GetSingleColorValue(0, 0, MAX_B_VALUE);
  uint32_t whiteValue = GetSingleColorValue(MAX_R_VALUE, MAX_G_VALUE, MAX_B_VALUE);
  uint32_t startingColor = redValue;

  // AlternateColors is for Christmas
  //AlternateColors(redValue, greenValue, startingColor, LONG_DELAY_MS);
  AlternateThreeColors(redValue, blueValue, whiteValue, startingColor, SHORT_DELAY_MS);
  
  /*
  // Blink all predefined colors
  for (uint32_t i = 0; i < NUM_COLORS; i++)
  {
    Blink(GetSingleColorValue(COLORS[i].m_Red,
                              COLORS[i].m_Green,
                              COLORS[i].m_Blue),
                              LONG_DELAY_MS);
    SyncArduinos();
  }
  */
  
  /*
  Rainbow(SHORT_DELAY_MS);
  SyncArduinos();
  RainbowCycle(SHORT_DELAY_MS);
  SyncArduinos();
  TheaterChaseRainbow(LONG_DELAY_MS);
  SyncArduinos();
  */
}




////////////////////////////////////////////////////////////////
// 
/// ARDUINO SKETCH METHODS
////////////////////////////////////////////////////////////////

void setup()
{
  NeoPixel::Init();
}

void loop()
{
  NeoPixel::RunTime();
}

