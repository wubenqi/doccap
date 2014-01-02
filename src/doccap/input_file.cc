// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#include "doccap/input_file.h"

BOOL InputFile::Open(const TCHAR *filename) {
	handle_ = CreateFile(filename, GENERIC_READ, 0, NULL,
					      OPEN_EXISTING,
					      FILE_ATTRIBUTE_NORMAL, NULL);
	
    if (handle_==INVALID_HANDLE_VALUE) return FALSE;
            
	eof_        = FALSE;
	bufpos_     = 0;
	buflen_		= 0;
    file_offset_ = 0;    

    return TRUE;
}                           

void InputFile::Reset(void) {
	eof_        = FALSE;
	bufpos_     = 0;
	buflen_     = 0;
	file_offset_ = 0;

	SetFilePointer(handle_, 0, 0, FILE_BEGIN);
}

void InputFile::Close(void) {
    if (handle_!=INVALID_HANDLE_VALUE) {
		CloseHandle(handle_);
        handle_ = INVALID_HANDLE_VALUE;
    }
}


BYTE InputFile::ReadByte(void) {
	if (bufpos_ >= buflen_) {
		// input buffer empty
        if (eof_)
            return kEof;

		ReadFile(handle_, buffer_, kBuffSize, &buflen_, NULL); 
		file_offset_ += buflen_;

        bufpos_ = 0;
        if (buflen_<=0) {   
            eof_=TRUE;
            return kEof;
        }            
    }
    
    return buffer_[bufpos_++]; 
}


DWORD InputFile::ReadDWord(void) {
	DWORD *prslt;
	char   temp[4];

	if (buflen_ < bufpos_ + 4 ) {
		for (int i=0; i<4; i++)
			temp[i] = ReadByte();

		prslt = (DWORD *)temp;
	} else {
		prslt = (DWORD *) (& buffer_[bufpos_]);
		bufpos_+= 4;
	}

	return *prslt;
}


int InputFile::Read(char *buffer, int len) {
	int copied = 0;

	while (len) {
		int l = buflen_ - bufpos_;

		if (l>0) {
			// copy from current buffer
			if (l>len)
				l = len;

            if (buffer) {
			    memcpy(buffer, & buffer_[bufpos_], l);
			    buffer   += l;
            }
	
			copied   += l;
			bufpos_ += l;
			len      -= l;
		} else {
			if ( buffer==NULL ) {
				// skipping only
				file_offset_ += len;
				SetFilePointer(handle_, file_offset_, NULL, FILE_BEGIN);	// move pointer
				return copied + len;										// buffer_ is still empty
			}

			ReadFile(handle_, buffer_, kBuffSize, &buflen_, NULL);		// read a buffer-full of data
			file_offset_ += buflen_;

			bufpos_ = 0;
			
			if (buflen_<=0) {
				eof_ = TRUE;
				break;
			}
		}
	}

	return copied;
}
