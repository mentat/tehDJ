/*
 * tehDJ - A realtime mp3 dj-ing system
 * (C) 2002 - Jesse Lovelace
*/
#include "wx/wx.h"

#include "djCodec.h"
#include "djStream.h"

#include "mpglib.h"

MPGLibCoDec::MPGLibCoDec()
: CoDec()
{
    InitMP3(&m_mp);

    m_pInStream = NULL;
	mSize = 0;
    mBufflen = 0;


}

void MPGLibCoDec::Cleanup()
{
}

bool MPGLibCoDec::Ok() 
{ 
    if (!m_pInStream) 
        return false; 
    return true; 
}

bool MPGLibCoDec::OpenFile(const char* pcFilename)
{
    return false;
}

void MPGLibCoDec::CloseFile()
{
}

void MPGLibCoDec::Seek(const int iNumerator, const int iDenominator)
{
}

void MPGLibCoDec::GetFileInfo()
{
}

bool MPGLibCoDec::GetPCMBlock(void* pBlock, unsigned long* pdwBlockSize)
{
    return false;
}

int MPGLibCoDec::GetCurrentPos_secs()
{
    return 0;
}
int MPGLibCoDec::GetTotalTime()
{
    return 0;
}