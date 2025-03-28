# ClosedLoopStepperCL

A powerful library for closed-loop stepper control using L298N driver and AS5600 encoder.

## Features
- 4-wire stepper motor control with full-step sequence
- Encoder feedback using AS5600 (I2C or analog)
- Position tracking in degrees, steps, or custom unit
- RPM-based speed control
- Option to **hold torque** or release after movement
- Simple API for embedded applications

## Usage Example

```cpp
motor.setStepsPerRevolution(200);
motor.setRPM(30);
motor.setEncoderUnit(UNIT_DEGREES);
motor.setHoldTorque(true); // Keep motor locked after movement
motor.stepTo(90.0);
```

## License
MIT
