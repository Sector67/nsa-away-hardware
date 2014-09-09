int counter = 0;                  // counter

void setup() {
  // initialize control over the keyboard:
  Keyboard.begin();
  delay(1000);
}

void loop() {
    // increment the button counter
    counter++;
    Keyboard.println(counter);
    
    delay(100);
}

