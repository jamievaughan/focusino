#ifndef Moonlite_h
#define Moonlite_h

#include <Arduino.h>

#define MOONLITE_UNKNOWN_COMMAND 0
#define MOONLITE_TEMPERATURE_CONVERSION 67 // C

#define MOONLITE_GOTO_TARGET_POSITION 18246 // FG
#define MOONLITE_STOP_MOVEMENT 20806 // FQ
#define MOONLITE_GOTO_HOME 18512 // PH

#define MOONLITE_GET_TEMPERATURE_COEFFICIENT 17223 // GC
#define MOONLITE_GET_STEPPER_SPEED 17479 // GD
#define MOONLITE_GET_STEP_MODE 18503 // GH (0xFF if stepping mode is set to half-step otherwise 0x00)
#define MOONLITE_GET_RUNNING_STATE 18759 // GI
#define MOONLITE_GET_TARGET_POSITION 20039 // GN
#define MOONLITE_GET_CURRENT_POSITION 20551 // GP
#define MOONLITE_GET_TEMPERATURE 21575 // GT
#define MOONLITE_GET_FIRMWARE_VERSION 22087 // GV
#define MOONLITE_GET_BACKLIGHT 16967 // GB
#define MOONLITE_GET_TEMPERATURE_CALIBRATION_OFFSET 20295 // GO

#define MOONLITE_SET_TEMPERATURE_COEFFICIENT 17235 // SC
#define MOONLITE_SET_STEPPER_SPEED 17491 // SD (Stepper speed values 02, 04, 08, 10, 20.)
#define MOONLITE_SET_FULL_STEP_MODE 18003 // SF
#define MOONLITE_SET_HALF_STEP_MODE 18515 // SH
#define MOONLITE_SET_TARGET_POSITION 20051 // SN
#define MOONLITE_SET_CURRENT_POSITION 20563 // SP
#define MOONLITE_SET_TEMPERATURE_CALIBRATION_OFFSET 20304 // PO

#define MOONLITE_STEPPER_HALF_STEP 0 // Half-step mode
#define MOONLITE_STEPPER_FULL_STEP 255 // Full-step mode

#define MOONLITE_RUNNING 1
#define MOONLITE_NOT_RUNNING 0

#define MOONLITE_INPUT_BUFFER_SIZE 8
#define MOONLITE_OUTPUT_BUFFER_SIZE 4

typedef struct MoonliteCommand
{
    int id;
    long parameter;
} MoonliteCommand;

class Moonlite
{
    public:
        // Constructors:
        Moonlite();

        // Getters:
        MoonliteCommand getCommand();
        bool hasReceivedCommand();

        // Public Members:
        void init(int baudRate);
        void loop();
        void respond(long value, int length);

    private:
        // Variables:
        int inputBufferIndex;
        char inputBuffer[MOONLITE_INPUT_BUFFER_SIZE];
        char outputBuffer[MOONLITE_OUTPUT_BUFFER_SIZE];
        bool commandReceived;
        MoonliteCommand currentCommand;

        // Private Members:
        void handleCommand();
        void resetCommand();
};

#endif
