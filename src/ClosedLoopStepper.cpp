#include "ClosedLoopStepper.h"

ClosedLoopStepper::ClosedLoopStepper(uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4,
                                     uint8_t encoderPinOrAddr, ReadMode mode, TwoWire* wire)
    : _in1(in1), _in2(in2), _in3(in3), _in4(in4),
      _encoderPinOrAddr(encoderPinOrAddr), _mode(mode), _wire(wire),
      _stepState(0), _stepDelay(1000), _currentPosition(0), _rpm(30),
      _unitMode(UNIT_DEGREES), _customScale(1.0), _stepsPerRev(200), _holdTorque(true) {}

void ClosedLoopStepper::begin() {
    pinMode(_in1, OUTPUT);
    pinMode(_in2, OUTPUT);
    pinMode(_in3, OUTPUT);
    pinMode(_in4, OUTPUT);
    if (_mode == READ_AS5600_I2C) {
        _wire->begin();
    }
    calculateStepDelay();
}

void ClosedLoopStepper::setRPM(float rpm) {
    _rpm = rpm;
    calculateStepDelay();
}

void ClosedLoopStepper::setStepsPerRevolution(int steps) {
    _stepsPerRev = steps;
    calculateStepDelay();
}

void ClosedLoopStepper::setReadMode(ReadMode mode) {
    _mode = mode;
}

void ClosedLoopStepper::setEncoderUnit(PositionUnit unit, float customScale) {
    _unitMode = unit;
    _customScale = customScale;
}

void ClosedLoopStepper::setHoldTorque(bool hold) {
    _holdTorque = hold;
}

void ClosedLoopStepper::calculateStepDelay() {
    _stepDelay = (60000000UL / (_rpm * _stepsPerRev));
}

float ClosedLoopStepper::readPosition() {
    int raw = (_mode == READ_AS5600_I2C) ? readRawEncoder() : analogRead(_encoderPinOrAddr);
    switch (_unitMode) {
        case UNIT_DEGREES:
            return raw * 360.0 / 4096.0;
        case UNIT_CUSTOM:
            return raw * _customScale;
        case UNIT_STEPS:
        default:
            return raw * _stepsPerRev / 4096.0;
    }
}

int ClosedLoopStepper::readRawEncoder() {
    if (_mode == READ_AS5600_I2C) {
        _wire->beginTransmission(_encoderPinOrAddr);
        _wire->write(0x0E);
        _wire->endTransmission(false);
        _wire->requestFrom(_encoderPinOrAddr, (uint8_t)2);
        if (_wire->available() == 2) {
            int angle = (_wire->read() << 8) | _wire->read();
            return angle & 0x0FFF;
        }
        return -1;
    } else {
        return analogRead(_encoderPinOrAddr);
    }
}

void ClosedLoopStepper::stepTo(float target) {
    float current = readPosition();
    while (abs(current - target) > 1) {
        current = readPosition();
        if (current < target) stepForward();
        else stepBackward();
    }
    if (!_holdTorque) release();
}

void ClosedLoopStepper::stepForward() {
    switch (_stepState) {
        case 0:
            digitalWrite(_in1, HIGH);
            digitalWrite(_in2, LOW);
            digitalWrite(_in3, LOW);
            digitalWrite(_in4, HIGH);
            break;
        case 1:
            digitalWrite(_in1, LOW);
            digitalWrite(_in2, HIGH);
            digitalWrite(_in3, LOW);
            digitalWrite(_in4, HIGH);
            break;
        case 2:
            digitalWrite(_in1, LOW);
            digitalWrite(_in2, HIGH);
            digitalWrite(_in3, HIGH);
            digitalWrite(_in4, LOW);
            break;
        case 3:
            digitalWrite(_in1, HIGH);
            digitalWrite(_in2, LOW);
            digitalWrite(_in3, HIGH);
            digitalWrite(_in4, LOW);
            break;
    }
    delayMicroseconds(_stepDelay);
    _stepState = (_stepState + 1) % 4;
    _currentPosition++;
}

void ClosedLoopStepper::stepBackward() {
    switch (_stepState) {
        case 0:
            digitalWrite(_in1, HIGH);
            digitalWrite(_in2, LOW);
            digitalWrite(_in3, HIGH);
            digitalWrite(_in4, LOW);
            break;
        case 1:
            digitalWrite(_in1, LOW);
            digitalWrite(_in2, HIGH);
            digitalWrite(_in3, HIGH);
            digitalWrite(_in4, LOW);
            break;
        case 2:
            digitalWrite(_in1, LOW);
            digitalWrite(_in2, HIGH);
            digitalWrite(_in3, LOW);
            digitalWrite(_in4, HIGH);
            break;
        case 3:
            digitalWrite(_in1, HIGH);
            digitalWrite(_in2, LOW);
            digitalWrite(_in3, LOW);
            digitalWrite(_in4, HIGH);
            break;
    }
    delayMicroseconds(_stepDelay);
    _stepState = (_stepState - 1 + 4) % 4;
    _currentPosition--;
}

void ClosedLoopStepper::release() {
    digitalWrite(_in1, LOW);
    digitalWrite(_in2, LOW);
    digitalWrite(_in3, LOW);
    digitalWrite(_in4, LOW);
}
