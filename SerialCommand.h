#ifndef SerialCommand_h
#define SerialCommand_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <string.h>
#include <SoftwareSerial.h>

#define SERIAL_CMD_DBG_EN     0     /* Set this value to 1 to enable debugging */
#define SERIAL_CMD_BUFF_LEN  64     /* Max length for each serial command */

/*
 * Token delimeters (setup '=', query '?', separator ',') 
 */
const char delimiters[] = "=,?\r\n";

/* 
 * End Of Line: <CR><LF>
 * <CR> = <Carriage Return, 0x0D, 13, '\r'>
 * <LF> = <Line Feed, 0x0A, 10, '\n'>
 */
const char EOL[] = "\r\n";

class SerialCommand: public Stream
{
    public:
        /**
         * Class constructor
         */
        SerialCommand();//Constructor

        /**
         * Start connection to serial port
         *
         * @param serialPort - Serial port to listen for commands
         * @param baud - Baud rate
         */
        void begin(SoftwareSerial &serialPort, uint32_t baud);
        void begin(HardwareSerial &serialPort, uint32_t baud);

        /**
         * Execute this function inside Arduino's loop function.
         */
        void loop(void);

        /**
         * Add a new command
         *
         * @param cmd - Command to listen
         * @param test - Test command callback
         * @param read - Read command callback
         * @param write - Write command callback
         * @param execute - Execute command callback
         */
        void addCommand(const char* cmd, void(*test)(), void(*read)(), void(*write)(), void(*execute)());

        /**
         * Add a read-only command
         *
         * @param cmd - Command to listen
         * @param callback - Read command callback
         */
        void addReadCommand(const char* cmd, void(*callback)())
        {
            addCommand(cmd, NULL, callback, NULL, NULL);
        }
        ;

        /**
         * Add a write-only command
         *
         * @param cmd - Command to listen
         * @param callback - Write command callback
         */
        void addWriteCommand(const char* cmd, void(*callback)())
        {
            addCommand(cmd, NULL, NULL, callback, NULL);
        }
        ;

        /**
         * Add a execute-only command
         *
         * @param cmd - Command to listen
         * @param callback - Execute command callback
         */
        void addExecuteCommand(const char* cmd, void(*callback)())
        {
            addCommand(cmd, NULL, NULL, NULL, callback);
        }
        ;

        /**
         * Default function to execute when no match is found
         *
         * @param callback - Function to execute when command is received
         */
        void addError(void(*callback)());

        /* 
         * Return next argument found in command buffer 
         */
        char *next(void);

        /* 
         * Send "OK" message trough the serial port 
         */
        void sendOK(void);

        /* 
         * Send "ERROR" message trough the serial port 
         */
        void sendERROR(void);

        /* 
         * Virtual methods to match Stream class 
         */
        virtual size_t write(uint8_t);
        virtual int available();
        virtual int read();
        virtual int peek();
        virtual void flush();

    private:
        /* Setup serial port */
        void setup(uint32_t baud);

        /* Sets the command buffer to all '\0' (nulls) */
        void clear(void);

        /* Send error message and clear buffer */
        void error();

        /* Process buffer */
        void bufferHandler(char c);

        /* Check for command instances and handle callbacks and queries */
        bool commandHandler(void);

        /* User defined error handler */
        void (*userErrorHandler)();

        /* Data structure for serial port handler */
        typedef struct serialPorthandler
        {
                SoftwareSerial* _soft;
                HardwareSerial* _hard;
                bool isSoftSerial;
        };

        /* Data structure to hold Command/Handler function key-value pairs */
        typedef struct serialCommandCallback
        {
                char command[SERIAL_CMD_BUFF_LEN];
                void (*test)();
                void (*read)();
                void (*write)();
                void (*execute)();
        };

        /* ESP8266 Serial Port handler */
        serialPorthandler _serialPortHandler;

        /* Actual definition for command/handler array */
        serialCommandCallback *commandList;

        /* Buffer of stored characters while waiting for terminator character */
        char buffer[SERIAL_CMD_BUFF_LEN];

        /* Pointer to buffer, used to store data in the buffer */
        char* pBuff;

        /* State variable used by strtok_r during processing */
        char* last;

        /* Number of available commands registered by new() */
        uint8_t commandCount;
};

#endif //SerialCommand_h
