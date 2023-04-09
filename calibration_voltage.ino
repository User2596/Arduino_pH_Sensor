// Defining macros
#define sensor A0            // pH Analog output to Arduino Analog input A0
#define LED 13               // LED indicator
#define sampling_interval 10 // Interval between two samples
#define print_interval 1000  // Interval between two outputs
#define array_length 100     // Total no. of samples

// Initialising variables
double average_voltage;                                 // Output pH value
int analog_array[array_length], i = 0;                  // Array of samples

// Finding the average of samples
double average_array(int* array, int number, int downrange, int uprange)
{
  for(int i = 0; i < (number - 1); i++)
  {
    for(int j = i + 1; j < number; j++)
    {
      if(array[i] > array[j])
      {
        int temp = array[i];                  // Sorting the analog samples
        array[i] = array[j];
        array[j] = temp;
      }
    }
  }
  double total_analog = 0;
  for(int i = downrange; i < uprange;  i++)
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
    average_voltage = average_analog * 5.0 / 1024.0;
    sampling_time = millis();
  }

  // Printing Output  
  if(millis() - print_time > print_interval)       // Output every 1000 ms
  {
    Serial.print("Voltage:");  
    Serial.print(average_voltage, 2);              // Serial monitor output
    Serial.println(" Volt");
    print_time = millis();      
  }
}