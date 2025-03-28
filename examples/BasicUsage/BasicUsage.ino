#include <ClosedLoopStepper.h>

ClosedLoopStepper motor(2, 3, 4, 7, 0x36, READ_AS5600_I2C);

void setup() {
    Serial.begin(115200);
    motor.begin();
    motor.setStepsPerRevolution(200);
    motor.setRPM(60);
    motor.setEncoderUnit(UNIT_DEGREES);
    motor.setHoldTorque(false); // قفل بماند پس از رسیدن به موقعیت

    motor.stepTo(90.0); // حرکت به ۹۰ درجه
}

void loop() {
}
