int counter = 0;                  // counter

void setup() {
  // initialize control over the keyboard:
  Keyboard.begin();
}

void loop() {
    // increment the button counter
    counter++;
    Keyboard.println(counter);
}

