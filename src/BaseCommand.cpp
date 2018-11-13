/*--------------------------------------------------------------------
Author		: Pedro Pereira
License		: BSD
Repository	: https://github.com/ppedro74/Arduino-BaseCommand
--------------------------------------------------------------------*/

#include "BaseCommand.h"

void BaseCommand::AddCommand(Command* command)
{
#ifdef BASE_COMMANDS_DEBUG
	Serial.print("Adding #");
	Serial.print(commands_count_);
	Serial.print(" cmd=[");
	Serial.print(command->command);
	Serial.println("]");
#endif
	command->next = NULL;
	if (commands_head_ == NULL)
	{
		commands_head_ = commands_tail_ = command;
	}
	else
	{
		commands_tail_->next = command;
		commands_tail_ = command;
	}
	commands_count_++;
}

BASE_COMMAND_ERRORS BaseCommand::ReadData()
{
	while ((*available)() > 0)
	{
		int ch = (*readByte)();
#ifdef BASE_COMMANDS_DEBUG
		Serial.print("Read: bufLen=");
		Serial.print(buffer_len_);
		Serial.print(" bufPos=");
		Serial.print(buffer_pos_);
		Serial.print(" termPos=");
		Serial.print(term_pos_);
		if (ch<32)
		{
			Serial.print(" ch=#");
			Serial.print(ch);
		}
		else
		{
			Serial.print(" ch=[");
			Serial.print((char)ch);
			Serial.print("]");
		}
		Serial.println();
#endif
		if (ch < 0)
		{
			continue;
		}

		if (buffer_pos_ < buffer_len_)
		{
			buffer_[buffer_pos_++] = ch;
		}
		else
		{
#ifdef BASE_COMMANDS_DEBUG			
			Serial.println("Buffer full");
#endif
			return BASE_COMMAND_ERROR_BUFFER_FULL;
		}

		if (term_[term_pos_] != ch)
		{
			term_pos_ = 0;
			continue;
		}

		if (term_[++term_pos_] == 0)
		{
			buffer_[buffer_pos_ - strlen(term_)] = '\0';

#ifdef BASE_COMMANDS_DEBUG
			Serial.print("Received: [");
			Serial.print(buffer_);
			Serial.println("]");
#endif
			char* command = strtok_r(buffer_, delim_, &last_token_);
			if (command != NULL)
			{
				boolean matched = false;
				int cx;
				Command* cmd;
				for (cmd = commands_head_, cx = 0; cmd != NULL; cmd = cmd->next, cx++)
				{
#ifdef BASE_COMMANDS_DEBUG
					Serial.print("Comparing [");
					Serial.print(command);
					Serial.print("] to [");
					Serial.print(cmd->command);
					Serial.println("]");
#endif

					if (strncmp(command, cmd->command, strlen(cmd->command) + 1) == 0)
					{
#ifdef BASE_COMMANDS_DEBUG
						Serial.print("Matched #");
						Serial.println(cx);
#endif
						if(cmd->function != NULL) {
							cmd->function(this);
							matched = true;
						}
						
						break;
					}
				}
				if (!matched && default_handler_ != NULL)
				{
					(*default_handler_)(this, command);
				}
			}

			ClearBuffer();
		}
	}

	return BASE_COMMAND_SUCCESS;
}

void BaseCommand::write(uint8_t* data, int size)
{
	for(int i =0; i < size; i++) {
		write(data[i]);
	}
}

void BaseCommand::write(uint8_t c)
{
	
	(*writeByte)(c);
	
}

void BaseCommand::command(Command* cmd,uint8_t* data, int size)
{
	write((uint8_t *)cmd->command, sizeof(cmd->command));
	write((uint8_t *)delim_, sizeof(delim_));
	write(data, size);
	write((uint8_t *)term_, sizeof(term_));
}


void BaseCommand::SetDefaultHandler(void(*function)(BaseCommand*, const char*))
{
	default_handler_ = function;
}

void BaseCommand::ClearBuffer()
{
	buffer_[0] = '\0';
	buffer_pos_ = 0;
	term_pos_ = 0;
}

char* BaseCommand::Next()
{
	return strtok_r(NULL, delim_, &last_token_);
}

