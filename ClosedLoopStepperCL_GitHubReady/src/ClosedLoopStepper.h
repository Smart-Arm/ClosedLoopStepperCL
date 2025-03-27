#ifndef CLOSED_LOOP_STEPPER_H
#define CLOSED_LOOP_STEPPER_H

#include <Arduino.h>
#include <Wire.h>

enum ReadMode {
    READ_AS5600_I2C,
    READ_AS5600_ANALOG
};

enum PositionUnit {
    UNIT_DEGREES,
    UNIT_STEPS,
    UNIT_CUSTOM
};

class ClosedLoopStepper {
public:
    ClosedLoopStepper(uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4,
                      uint8_t encoderPinOrAddr, ReadMode mode = READ_AS5600_I2C,
                      TwoWire* wire = &Wire);

    void begin();
    void setRPM(float rpm);
    void setStepsPerRevolution(int steps);
    void setReadMode(ReadMode mode);
    void setEncoderUnit(PositionUnit unit, float customScale = 1.0);
    void setHoldTorque(bool hold);

    void stepForward();
    void stepBackward();
    void stepTo(float target);
    void release();

    float readPosition();
    int readRawEncoder();

private:
    void calculateStepDelay();

    uint8_t _in1, _in2, _in3, _in4;
    uint8_t _encoderPinOrAddr;
    ReadMode _mode;
    PositionUnit _unitMode;
    float _customScale;
    int _stepsPerRev;
    float _rpm;

    int _stepState;
    unsigned long _stepDelay;
    long _currentPosition;
    bool _holdTorque;

    TwoWire* _wire;
};

#endif
