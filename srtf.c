//
// srtf.c
//
// This file implements the functionality declared in srtf.h.
//
// The MIT License (MIT)
// 
// Copyright (c) 2021 Luke Andrews.  All Rights Reserved.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this
// software and associated documentation files (the "Software"), to deal in the Software
// without restriction, including without limitation the rights to use, copy, modify, merge,
// publish, distribute, sub-license, and/or sell copies of the Software, and to permit persons
// to whom the Software is furnished to do so, subject to the following conditions:
// 
// * The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
// FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "srtf.h"
#include "clock.h"
#include "constants.h"
#include "structs.h"

//
// srtf_enqueue
//
// Enqueues a process by ascending process ID.
//
void srtf_enqueue(queue_t* queue, process_t* process)
{
	if (queue->head == NULL)
	{
		queue->head = process;
	}
	else
	{
		process_t* current  = queue->head;
		process_t* previous = NULL;

		while (current != NULL)
		{
			if (current->pid >= process->pid)
			{
				break;
			}

			previous = current;
			current  = current->next;
		}

		if (previous == NULL)
		{
			queue->head   = process;
			process->next = current;
		}
		else
		{
			previous->next = process;
			process->next  = current;
		}
	}

	++(queue->count);
}

//
// srtf_free_queue
//
// Frees all allocated memory in a queue_t.
//
void srtf_free_queue(queue_t* queue)
{
	if (queue != NULL)
	{
		process_t* next = NULL;

		for (process_t* head = queue->head; head != NULL; head = next)
		{
			next = head->next;

			free(head->command);
			free(head);
		}

		free(queue);
	}
}

//
// srtf_strdup
//
// Dynamically duplicates and returns a source string (malloc).
// Returns NULL on error or when source is NULL.
//
char* srtf_strdup(const char* src)
{
	char* dest = NULL;

	if (src != NULL)
	{
		size_t count = strlen(src) + 1;

		dest = malloc(count);

		if (dest != NULL)
		{
			strcpy(dest, src);
		}
	}

	return dest;
}

header_t* srtf_init()
{
	int result = SUCCESS;

	//
	// Allocate and initialize header:
	//
	header_t* header = NULL;

	if (result == SUCCESS)
	{
		header = calloc(1, sizeof(header_t));

		if (header == NULL)
		{
			result = FAILURE;
		}
	}

	//
	// Allocate and initialize queues:
	//
	if (result == SUCCESS)
	{
		header->ready_queue   = calloc(1, sizeof(queue_t));
		header->stopped_queue = calloc(1, sizeof(queue_t));
		header->defunct_queue = calloc(1, sizeof(queue_t));

		if (header->ready_queue   == NULL || 
			header->stopped_queue == NULL || 
			header->defunct_queue == NULL)
		{
			result = FAILURE;
		}
	}

	//
	// Return header, or NULL on error:
	//
	if (result != SUCCESS)
	{
		srtf_free(header);
		header = NULL;
	}

	return header;
}

void srtf_free(header_t* header)
{
	if (header != NULL)
	{
		srtf_free_queue(header->defunct_queue);
		srtf_free_queue(header->stopped_queue);
		srtf_free_queue(header->ready_queue);

		free(header);
	}
}

process_t* srtf_generate(char* command, int pid, int time_remaining, int is_sudo)
{
	int result = SUCCESS;

	//
	// Validate parameters:
	//
	if (result == SUCCESS)
	{
		if (command == NULL || time_remaining <= 0)
		{
			result = FAILURE;
		}
	}

	//
	// Allocate the new process: 
	//
	process_t* process = NULL;

	if (result == SUCCESS)
	{
		process = calloc(1, sizeof(process_t));

		if (process == NULL)
		{
			result = FAILURE;
		}
	}

	//
	// Allocate and initialize the process command:
	//
	if (result == SUCCESS)
	{
		process->command = srtf_strdup(command);

		if (process->command == NULL)
		{
			result = FAILURE;
		}
	}

	//
	// Initialize remaining process fields:
	//
	if (result == SUCCESS)
	{
		process->pid            = pid;
		process->flags          = 0 | STATE_CREATED;
		process->time_remaining = time_remaining;
		process->time_last_run  = clock_get_time();

		if (is_sudo != 0)
		{
			process->flags |= PF_SUPERPRIV;
		}
	}

	//
	// Return new process, or NULL on error:
	//
	if (result != SUCCESS)
	{
		if (process != NULL)
		{
			free(process->command);
			free(process);
			process = NULL;
		}
	}

	return process;
}

int srtf_add(header_t* header, process_t* process)
{
	int result = SUCCESS;

	//
	// Validate parameters:
	//
	if (result == SUCCESS)
	{
		if (header == NULL || process == NULL)
		{
			result = FAILURE;
		}
		else if (header->ready_queue   == NULL ||
				 header->stopped_queue == NULL ||
				 header->defunct_queue == NULL)
		{
			result = FAILURE;
		}
		else if (process->time_remaining < 0)
		{
			result = FAILURE;
		}
	}

	//
	// Extract and handle process state:
	//
	if (result == SUCCESS)
	{
		int state = process->flags & STATE_MASK;

		switch (state)
		{
			case STATE_CREATED:
			{
				process->flags ^= (STATE_CREATED + STATE_READY);
				srtf_enqueue(header->ready_queue, process);
				break;
			}

			case STATE_READY:
			{
				if (process->time_remaining > 0)
				{
					srtf_enqueue(header->ready_queue, process);
				}
				else
				{
					process->flags ^= (STATE_READY + STATE_DEFUNCT);
					srtf_enqueue(header->defunct_queue, process);
				}

				break;
			}

			case STATE_DEFUNCT:
			{
				srtf_enqueue(header->defunct_queue, process);
				break;
			}

			default:
			{
				result = FAILURE;
				break;
			}
		}
	}

	return result;
}

int srtf_reap(header_t* header, int pid)
{
	int result   = SUCCESS;
	int exitCode = 0;

	//
	// Validate parameters:
	//
	if (result == SUCCESS)
	{
		if (header                      == NULL ||
			header->defunct_queue       == NULL ||
			header->defunct_queue->head == NULL)
		{
			result   = FAILURE;
			exitCode = -1;
		}
	}

	//
	// Remove process and extract exit code:
	//
	if (result == SUCCESS)
	{
		process_t* current  = header->defunct_queue->head;
		process_t* previous = NULL;

		if (current->pid == pid)
		{
			header->defunct_queue->head = current->next;
			free(current->command);
			free(current);
			current = NULL;
			--(header->defunct_queue->count);
		}
		else
		{
			while (current != NULL)
			{
				if (current->pid == pid)
				{
					break;
				}

				previous = current;
				current = current->next;
			}

			if (current == NULL)
			{
				result   = FAILURE;
				exitCode = -1;
			}
			else if (previous == NULL)
			{
				header->defunct_queue->head = current->next;
			}
			else
			{
				previous->next = current->next;
			}

			if (result == SUCCESS)
			{
				exitCode = current->flags >> STATE_COUNT;
				free(current->command);
				free(current);
				current = NULL;
				--(header->defunct_queue->count);
			}
		}
	}

	return exitCode;
}

process_t* srtf_schedule(header_t* header)
{
	int result = SUCCESS;

	//
	// Validate parameters:
	//
	if (result == SUCCESS)
	{
		if (header == NULL)
		{
			result = FAILURE;
		}
		else if (header->ready_queue == NULL)
		{
			result = FAILURE;
		}
		else if (header->ready_queue->head == NULL)
		{
			result = FAILURE;
		}
	}

	//
	// Schedule and remove process:
	//
	process_t* process = NULL;

	if (result == SUCCESS)
	{
		process_t* current  = header->ready_queue->head;
		process_t* previous = NULL;

		process = current;

		if (clock_get_time() - current->time_last_run < TIME_STARVATION)
		{
			while (current->next != NULL)
			{
				if (clock_get_time() - current->next->time_last_run >= TIME_STARVATION)
				{
					previous = current;
					process  = current->next;
					break;
				}

				if (current->next->time_remaining < process->time_remaining)
				{
					previous = current;
					process  = current->next;
				}

				current  = current->next;
			}
		}

		if (previous == NULL)
		{
			header->ready_queue->head = process->next;
		}
		else
		{
			previous->next = process->next;
		}

		process->next = NULL;

		--(header->ready_queue->count);
	}

	return process;
}

int srtf_stop(header_t* header, int pid)
{
	int result = SUCCESS;

	//
	// Validate parameters:
	//
	if (result == SUCCESS)
	{
		if (header                      == NULL ||
			header->ready_queue         == NULL ||
			header->ready_queue->head   == NULL)
		{
			result = FAILURE;
		}
	}

	//
	// Transfer 'ready' process to 'stopped' queue:
	//
	if (result == SUCCESS)
	{
		process_t* current  = header->ready_queue->head;
		process_t* previous = NULL;

		while (current != NULL)
		{
			if (current->pid == pid)
			{
				break;
			}

			previous = current;
			current = current->next;
		}

		if (current == NULL)
		{
			result = FAILURE;
		}
		else if (previous == NULL)
		{
			header->ready_queue->head = current->next;
			--(header->ready_queue->count);
		}
		else
		{
			previous->next = current->next;
			--(header->ready_queue->count);
		}

		if (result == SUCCESS)
		{
			current->flags ^= (STATE_READY + STATE_STOPPED);
			srtf_enqueue(header->stopped_queue, current);
		}
	}

	return result;
}

int srtf_continue(header_t* header, int pid)
{
	int result = SUCCESS;

	if (result == SUCCESS)
	{
		if (header                      == NULL ||
			header->stopped_queue       == NULL ||
			header->stopped_queue->head == NULL)
		{
			result = FAILURE;
		}
	}

	//
	// Transfer 'stopped' process to 'ready' queue:
	//
	if (result == SUCCESS)
	{
		process_t* current  = header->stopped_queue->head;
		process_t* previous = NULL;

		while (current != NULL)
		{
			if (current->pid == pid)
			{
				break;
			}

			previous = current;
			current = current->next;
		}

		if (current == NULL)
		{
			result = FAILURE;
		}
		else if (previous == NULL)
		{
			header->stopped_queue->head = current->next;
			--(header->stopped_queue->count);
		}
		else
		{
			previous->next = current->next;
			--(header->stopped_queue->count);
		}

		if (result == SUCCESS)
		{
			current->flags ^= (STATE_STOPPED + STATE_READY);
			srtf_enqueue(header->ready_queue, current);
		}
	}

	return result;
}

int srtf_count(queue_t* queue)
{
	return queue != NULL? queue->count: FAILURE;
}