import processing.serial.*;

Serial myPort;
int xPos = 0;
int yPos = 0;

void setup() {
  size(800, 400);
  background(0);

  String portName = Serial.list()[0]; // Modify this line to select the appropriate serial port
  myPort = new Serial(this, portName, 9600); // Configure the serial port
}

void draw() {
  while (myPort.available() > 0) {
    String data = myPort.readStringUntil('\n'); // Read data from the serial port
    if (data != null) {
      data = data.trim(); // Remove leading/trailing white space
      int flag = Integer.parseInt(data); // Parse the data as an integer

      if (flag == 1) {
        yPos = 100; // Set the height of the pulse signal in the ON state
      } else {
        yPos = 0; // Set the height of the pulse signal in the OFF state
      }
    }
  }

  stroke(255);
  line(xPos, height / 2, xPos, height / 2 - yPos); // Draw the pulse signal

  xPos++; // Move the position of the pulse signal
  if (xPos >= width) {
    xPos = 0;
    background(0); // Clear the screen
  }
}
