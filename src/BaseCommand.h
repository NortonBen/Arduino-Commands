/*--------------------------------------------------------------------
Author		: Pedro Pereira
License		: BSD
Repository	: https://github.com/ppedro74/Arduino-SerialCommands
--------------------------------------------------------------------*/

//#define SERIAL_COMMANDS_DEBUG

#ifndef BASE_COMMANDS_H
#define BASE_COMMANDS_H

#include <Arduino.h>

typedef enum ternary_base
{
	BASE_COMMAND_SUCCESS = 0,
	BASE_COMMAND_ERROR_NO_SERIAL,
	BASE_COMMAND_ERROR_BUFFER_FULL
} BASE_COMMAND_ERRORS;

class BaseCommand;

typedef class Command Command;
class Command
{
public:
	Command(const char* cmd, void(*func)(BaseCommand*))
		: command(cmd),
		function(func),
		next(NULL)
	{
	}
    Command(const char* cmd)
		: command(cmd),
		function(NULL),
		next(NULL)
	{
	}

	const char* command;
	void(*function)(BaseCommand*);
	Command* next;
};


class BaseCommand
{
public:
	BaseCommand(uint8_t(*readByte_fnc)(), uint8_t(*available_fnc)(), void(*witeByte_fnc)(uint8_t), char* buffer, int16_t buffer_len, char* term = (char *)&"\r\n", char* delim = (char *)&" ") :
		readByte(readByte_fnc),
        writeByte(witeByte_fnc),
        available(available_fnc),
		buffer_(buffer),
		buffer_len_(buffer!=NULL && buffer_len > 0 ? buffer_len - 1 : 0), //string termination char '\0'
		term_(term),
		delim_(delim),
		default_handler_(NULL),
		buffer_pos_(0),
		last_token_(NULL), 
		term_pos_(0),
		commands_head_(NULL),
		commands_tail_(NULL),
		commands_count_(0)
	{
	}

    BaseCommand(uint8_t(*readByte_fnc)(), uint8_t(*available_fnc)(), char* buffer, int16_t buffer_len, char* term = (char *)&"\r\n", char* delim = (char *)&" ") :
		readByte(readByte_fnc),
        available(available_fnc),
		buffer_(buffer),
		buffer_len_(buffer!=NULL && buffer_len > 0 ? buffer_len - 1 : 0), //string termination char '\0'
		term_(term),
		delim_(delim),
		default_handler_(NULL),
		buffer_pos_(0),
		last_token_(NULL), 
		term_pos_(0),
		commands_head_(NULL),
		commands_tail_(NULL),
		commands_count_(0)
	{
	}

	BaseCommand(char* buffer, int16_t buffer_len, char* term = (char *)&"\r\n", char* delim = (char *)&" ") :
		buffer_(buffer),
		buffer_len_(buffer!=NULL && buffer_len > 0 ? buffer_len - 1 : 0), //string termination char '\0'
		term_(term),
		delim_(delim),
		default_handler_(NULL),
		buffer_pos_(0),
		last_token_(NULL), 
		term_pos_(0),
		commands_head_(NULL),
		commands_tail_(NULL),
		commands_count_(0)
	{
	}


	/**
	 * \brief Adds a command handler (Uses a linked list)
	 * \param command 
	 */
	void AddCommand(Command* command);

	/**
	 * \brief Checks the Serial port, reads the input buffer and calls a matching command handler.
	 * \return SERIAL_COMMANDS_SUCCESS when successful or SERIAL_COMMANDS_ERROR_XXXX on error.
	 */
BASE_COMMAND_ERRORS ReadData();

	void write(uint8_t c);

    void write(uint8_t* data, int size);

    void command(Command* cmd, uint8_t* data, int size);
	
	/**
	 * \brief Sets a default handler can be used for a catch all or unrecognized commands
	 * \param function 
	 */
	void SetDefaultHandler(void(*function)(BaseCommand*, const char*));
	
	/**
	 * \brief Clears the buffer, and resets the indexes.
	 */
	void ClearBuffer();
	
	/**
	 * \brief Gets the next argument
	 * \return returns NULL if no argument is available
	 */
	char* Next();

protected:
	Stream* serial_;
	char* buffer_;
	int16_t buffer_len_;
	char* term_;
	char* delim_;
	void(*default_handler_)(BaseCommand*, const char*);
	uint8_t(*readByte)();
	uint8_t(*available)();
	void(*writeByte)(uint8_t);
	int16_t buffer_pos_;
	char* last_token_;
	int8_t term_pos_;
	Command* commands_head_;
	Command* commands_tail_;
	uint8_t commands_count_;
};

#endif
