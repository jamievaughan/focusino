#include "Moonlite.h"

#include "Arduino.h"

// Constructors
Moonlite::Moonlite()
{
    this->resetCommand();    
}

// Getters
MoonliteCommand Moonlite::getCommand()
{
    this->commandReceived = false;

    return currentCommand;
}

bool Moonlite::hasReceivedCommand()
{
    return this->commandReceived;
}

// Public Members
void Moonlite::init(int baudRate)
{
  // Set the baudrate of the serial port.
  Serial.begin(baudRate);
}

void Moonlite::loop()
{
    if (!Serial.available())
    {
        return;
    }

    char incomingByte = Serial.read();

    inputBuffer[this->inputBufferIndex++] = incomingByte;

    // A new command is incoming, reset the current command.
    if (incomingByte == ':')
    {
        this->resetCommand();
    }

    // A command terminator has been received.
    if (incomingByte == '#')
    {
        this->handleCommand();
        this->inputBufferIndex = 0;
    }
}

void Moonlite::respond(long value, int type)
{
    // Moonlite protocol only supports decimal and hexidecimal
    // output types.
    char* format = type == DEC
        ? "%02X"
        : "%04X";
    
    sprintf(this->outputBuffer, format, value);

    Serial.print(this->outputBuffer);
    Serial.print("#");
}

void Moonlite::handleCommand()
{
    char firstCharacter = this->inputBuffer[1];
    char secondCharacter = this->inputBuffer[2];

    int commandId = firstCharacter == 'C'
        ? (int)firstCharacter
        : firstCharacter | secondCharacter << 8;

    long commandParameter = 0;

    switch (commandId)
    {
        case MOONLITE_SET_STEPPER_SPEED:
            commandParameter = strtoul(&this->inputBuffer[3], NULL, DEC);
        break;

        case MOONLITE_SET_CURRENT_POSITION:
        case MOONLITE_SET_TARGET_POSITION:
            commandParameter = strtoul(&this->inputBuffer[3], NULL, HEX);
        break;

        case MOONLITE_SET_TEMPERATURE_COEFFICIENT:
            commandParameter = strtol(&this->inputBuffer[3], NULL, DEC);
        break;

        case MOONLITE_SET_TEMPERATURE_CALIBRATION_OFFSET:
            commandParameter = strtol(&this->inputBuffer[3], NULL, DEC);
        break;
    }

    this->currentCommand.id = commandId;
    this->currentCommand.parameter = commandParameter;

    this->commandReceived = true;
}

void Moonlite::resetCommand()
{
    this->commandReceived = false;

    // Reset the current command.
    this->currentCommand.id = 0;
    this->currentCommand.parameter = 0;
}