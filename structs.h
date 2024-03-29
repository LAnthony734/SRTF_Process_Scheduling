//
// structs.h
//
// This file contains struct definitions.
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
#ifndef STRUCTS_H
#define STRUCTS_H

//
// process_t
//
// Defines a single process in a queue of processes. Contains a pointer to the 
// next process in the queue.
//
typedef struct process_struct {
  char*  command;      			// Process command
  int    pid;            		// Process ID (unique)
  int    flags;          		// Process flags
  int    time_remaining; 		// Time left to execute
  int    time_last_run;  		// Last time executed (or time created)
  struct process_struct* next; 	// The next process in the queue
} process_t;

//
// queue_t
//
// Defines a queues of processes. Contains a pointer to the head of the queue 
// and a count of total processes in the queue.
//
typedef struct list_struct {
  process_t* head;
  int        count;
} queue_t;

//
// header_t
//
// Defines a header of queues. Contains a single queue for each process state.
//
typedef struct header_struct {
  queue_t* ready_queue;
  queue_t* stopped_queue;
  queue_t* defunct_queue;
} header_t;

#endif // STRUCTS_H
