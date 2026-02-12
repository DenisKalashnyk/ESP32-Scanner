#ifndef LCD1602_HPP
#define LCD1602_HPP

#include <Arduino.h>

class LCD1602 {
public:
    LCD1602(uint8_t address, uint8_t cols, uint8_t rows, uint8_t rs, int8_t rw, uint8_t enable, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7){
        _address = address;
        _cols = cols;
        _rows = rows;
        _rs = rs;
        _rw = rw;
        _enable = enable;
        _d4 = d4;
        _d5 = d5;
        _d6 = d6;
        _d7 = d7;
        initDisplay();
    }
    void begin(){
        sendCommand(0x33); // Initialize LCD in 4-bit mode
        sendCommand(0x32); // Set to 4-bit mode
        sendCommand(0x28); // 2 lines, 5x8 dots
        sendCommand(0x0C); // Display ON, Cursor OFF
        sendCommand(0x06); // Entry mode set: increment cursor
        sendCommand(0x01); // Clear display
    }
    void print(const String &message){
        for (size_t i = 0; i < message.length(); ++i) {
            sendData(message[i]);
        }
    }
    void print(const char *message){
        while (*message) {
            sendData(*message++);
        }
    }
    void print(float value, unsigned int decimalPlaces = 2) {
        String strValue = String(value, decimalPlaces);
        print(strValue);
    }
    void clear(){
        sendCommand(0x01); // Clear display
        delay(2); // Wait for the command to complete
    }
    void setCursor(uint8_t col, uint8_t row){
        if (row >= _rows || col >= _cols) return; // Out of bounds
        uint8_t address = col + (0x40 * row);
        sendCommand(0x80 | address); // Set DDRAM address
    }

private:
    uint8_t _address;
    uint8_t _cols;
    uint8_t _rows;
    uint8_t _rs;
    int8_t _rw;
    uint8_t _enable;
    uint8_t _d4;
    uint8_t _d5;
    uint8_t _d6;
    uint8_t _d7;
    void sendNibble(uint8_t nibble){
        digitalWrite(_d4, (nibble >> 0) & 0x01);
        digitalWrite(_d5, (nibble >> 1) & 0x01);
        digitalWrite(_d6, (nibble >> 2) & 0x01);
        digitalWrite(_d7, (nibble >> 3) & 0x01);
        digitalWrite(_enable, HIGH); // Enable pulse
        delayMicroseconds(1);
        digitalWrite(_enable, LOW);
        delayMicroseconds(100); // Wait for command to be processed
    }
    void sendCommand(uint8_t command){
        digitalWrite(_rs, LOW); // Command mode
        if(_rw != -1) {
            digitalWrite(_rw, LOW); // Write mode
        }
        sendNibble(command >> 4); // Send high nibble
        sendNibble(command & 0x0F); // Send low nibble
    }
    void sendData(uint8_t data){
        digitalWrite(_rs, HIGH); // Data mode
        if(_rw != -1) {
            digitalWrite(_rw, LOW); // Write mode
        }
        sendNibble(data >> 4); // Send high nibble
        sendNibble(data & 0x0F); // Send low nibble
    }
    void initDisplay(){
        pinMode(_rs, OUTPUT);
        if(_rw != -1) {
            pinMode(_rw, OUTPUT);
        }
        pinMode(_enable, OUTPUT);
        pinMode(_d4, OUTPUT);
        pinMode(_d5, OUTPUT);
        pinMode(_d6, OUTPUT);
        pinMode(_d7, OUTPUT);

        digitalWrite(_rs, LOW);
        if(_rw != -1) {
            digitalWrite(_rw, LOW); // Write mode
        }
        digitalWrite(_enable, LOW);

        delay(50); // Wait for LCD to power up
    }
    
};

#endif // LCD1602_HPP