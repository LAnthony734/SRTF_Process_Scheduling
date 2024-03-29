//
// Constants.h
//
// This file contains constant and macro definitions.
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
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define STATE_COUNT         5
#define STATE_CREATED    0x01
#define STATE_READY      0x02
#define STATE_STOPPED    0x04
#define STATE_DEFUNCT    0x08
#define STATE_TERMINATED 0x10

#define STATE_MASK ((1 << STATE_COUNT) - 1)

#define PF_COUNT            1
#define PF_SUPERPRIV     0x20

#define TIME_STARVATION 5    
#define MAX_COMMAND     255   // Max process command length
#define MAX_LINE_LEN    512   // Max trace file line length

#define SUCCESS 0
#define FAILURE -1

#endif // CONSTANTS_H
