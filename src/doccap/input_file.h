// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#ifndef DOCCAP_IO_STREAM_H_
#define DOCCAP_IO_STREAM_H_
#pragma once

#include <Windows.h>

#define kBuffSize	2048
#define kEof		0x1A

class InputFile {
public:
    InputFile(void) {
        handle_ = INVALID_HANDLE_VALUE;
    }

    BOOL Eof(void) const {
		return eof_;
	}

    unsigned GetFilesize(void) {
        return GetFileSize(handle_, NULL);
    }

	void Reset(void);
    BOOL Open(const char *filename);
    void Close(void);
        
    BYTE  ReadByte(void);
	DWORD ReadDWord(void);

    int   ReadTnt(void) {
        return (int) ReadDWord();
    }

	int   Read(char *buffer, int size);		

	BYTE			buffer_[kBuffSize];
	HANDLE			handle_;

private:
	unsigned long	bufpos_;                 // next read position
	unsigned long	buflen_;                 // no of bytes in buffer
	unsigned long   file_offset_;			  // offset to read from

	BOOL			eof_;        
};


#endif // DOCCAP_IO_STREAM_H_

