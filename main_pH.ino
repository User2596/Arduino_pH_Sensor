// Including libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Defining macros
#define sensor A0            // pH Analog output to Arduino Analog input A0
#define pH_offset 0.00       // pH deviation
#define LED 13               // LED indicator
#define sampling_interval 10 // Interval between two samples
#define print_interval 1000  // Interval between two outputs
#define array_length 100     // Total no. of samples
#define screen_width 128     // OLED display width, in pixels
#define screen_height 64     // OLED display height, in pixels
#define OLED_RESET -1        // Reset pin # (or -1 if sharing reset pin)

// Initialising variables
static double volt_at_pH_7 = 2.87;       // Obtained value from calibration
static double volt_per_pH = 0.1637778;   // Obtained value from manual
double pH_value;                         // Output pH value
long double total_analog = 0;
int analog_array[array_length], i = 0;   // Array of samples

// Adafruit logo
Adafruit_SSD1306 display(screen_width, screen_height, &Wire, OLED_RESET);

// Finding the average of samples
double average_array (int* array, int number, int downrange, int uprange){
  for (int i = 0; i < (number - 1); i++)
  {
    for (int j = i + 1; j < number; j++)
    {
      if (array[i] > array[j])
      {
        int temp = array[i];                  // Sorting the analog samples
        array[i] = array[j];
        array[j] = temp;
      }
    }
  }
  double total_analog = 0;
  for(int i = downrange; i < uprange; i++)
  {
    total_analog += array[i];           // Total value of 50 center samples
  }
  // Averaging samples
  double avg_analog = total_analog / (uprange - downrange);
  return avg_analog;
}

// Setup
void setup(void)
{
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  Serial.println("Arduino based pH Sensor:"); // Testing the serial monitor

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  { 
      Serial.println(F("SSD1306 allocation failed"));
      for(;;);                               // Don't proceed, loop forever
  }
  display.display();
  delay(2000);                               // Delay for 2 seconds
 
  display.clearDisplay();
  display.setTextColor(WHITE);               // Setting text colour
  display.setTextSize(2);                    // Setting text size
  display.setCursor(12, 25);
  display.print("pH Sensor");                // Initialising text
  display.display();
  delay(3000);                               // Initial text for 3 seconds
}

// Main loop
void loop(void)
{
  static unsigned long sampling_time = millis(); // Milliseconds passed
  static unsigned long print_time = millis();    // Milliseconds passed

  // Collecting samples
  if(millis() - sampling_time > sampling_interval) 
  {
    analog_array[i++] = analogRead(sensor);
    if(i == array_length)
    {
      i = 0;                                 // Resetting array
    }
    double average_analog = average_array(analog_array, array_length,
     (array_length / 4), (array_length * 0.75));
    // Convering the analog into Volt
    double average_voltage = average_analog * 5.0 / 1024.0;
    pH_value = 7.00 - ((average_voltage - volt_at_pH_7) / volt_per_pH)
     - pH_offset;                            // Converting the Volt into pH
    sampling_time = millis();
  }

  // Printing Output
  if(millis() - print_time > print_interval)       // Output every 1000 ms
  {
    Serial.print("pH:");  
    Serial.print(pH_value, 2);                     // Serial monitor output
    Serial.println("");

    if (pH_value <= 14 && pH_value >= 0)
    {
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(17, 0);
      display.println("pH Value");     // Displaying header
 
      display.setTextSize(3);
      display.setCursor(20, 30);
      if (pH_value < 10 && pH_value >= 0)
      {
      display.print("0");              // To put a 0 before single digit
      }
      display.print(pH_value);         // Displaying pH value
              
    }
    else
    {
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(12, 25);
      display.print("pH Sensor");      // Displaying initial text for error
    }
    display.display();

    digitalWrite(LED, HIGH);
    digitalWrite(LED, LOW);

    print_time = millis();             // Milliseconds passed since     
  }
}