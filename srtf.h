// 
// srtf.h
//
// This file declares functionality for manipulating header_t structs (see structs.h) and
// enqueing and dequeuing processes by Shortest Run Time First (SRTF)
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
#ifndef SRTF_H
#define SRTF_H

#include "structs.h"

//
// srtf_init
//
// Initializes the header_t Struct. 
// Returns a pointer to the new header_t or NULL on any error.
//
header_t* srtf_init();

//
// srtf_free
//
// Frees all allocated memory in the header_t.
//
void srtf_free(header_t* header);

//
// srtf_generate
//
// Creates a new process_t with the given information.
// Returns the process_t on success or a NULL on any error.
//
process_t* srtf_generate(char* command, int pid, int time_remaining, int is_sudo);

//
// srtf_add
//
// Adds the process into the appropriate linked list.
// Returns a 0 on success or a -1 on any error.
//
int srtf_add(header_t* header, process_t* process);

//
// srtf_reap
//
// Removes the process with matching pid from Defunct.
// Returns its exit code (from flags) on success or a -1 on any error.
//
int srtf_reap(header_t *header, int pid);

// 
// srtf_schedule
//
// Schedules the next process to run from Ready Queue.
// Returns the process selected or NULL if none available or on any errors.
//
process_t* srtf_schedule(header_t *header);

//
// srtf_stop
//
// Moves the process with matching pid from Ready to Stopped.
// Returns a 0 on success or a -1 on any error.
//
int srtf_stop(header_t* header, int pid);

//
// srtf_continue
//
// Moves the process with matching pid from Stopped to Ready.
// Returns a 0 on success or a -1 on any error.
//
int srtf_continue(header_t* header, int pid);

// 
// srtf_count
//
// Returns the number of items in a given queue_t.
// Returns the number of processes in the list or -1 on any errors.
//
int srtf_count(queue_t* queue);

#endif 
