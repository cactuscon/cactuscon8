// menu 
char buff[512];
int vref = 1100;
int btnClick = false;

// accelerometer
bool mpuDetected = false;
Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;

// used for plotting
uint16_t *data = NULL;

// jigglometer
float last_x_val;
float last_y_val;
float last_z_val;

char* bounce_title = "JIGGLES";
int bounce_offset;

int bounce = 0;
int max_threshold = 50;
int min_threshold = 25;

float last_tot_min=max_threshold;
float last_tot_max;
float last_tot_val;

// heart rate sensor
bool maxDetected = false;
MAX30105 particleSensor;

char* pulse_title = "BPM";
int pulse_offset;
float lastBPM;
float lastBeatAvg;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
long irValue;
float beatsPerMinute;
int beatAvg;