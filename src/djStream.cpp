/*
 * tehDJ - A realtime mp3 dj-ing system
 * (C) 2002 - Jesse Lovelace
*/
#include "wx/wx.h"
#include "wx/file.h"

#include "djStream.h"

FileStream::FileStream(const char* pcFlexiURL)
: InStream(pcFlexiURL), m_file(pcFlexiURL)
{
}

FileStream::~FileStream()
{ 
    m_file.Close(); 
}

bool FileStream::Read(void* pDestBuffer, const unsigned int iBytesToRead, unsigned int& piBytesRead)
{
	off_t byts = m_file.Read(pDestBuffer, iBytesToRead);

    piBytesRead = byts;

    if (byts == -1)
    {
        wxLogDebug("FileStream::Read - No data read. Offset: %d!", m_file.Tell());
        return false;
    }
    return true;
}

bool FileStream::Seek(const unsigned int iNewOffset)
{
    off_t off;
    off = m_file.Seek(iNewOffset);
    if (off == wxInvalidOffset)
    {
        wxLogDebug("FileStream::Seek - Invalid Offset!");
        return false;
    }
    return true;
}

bool FileStream::SeekEnd(const int iNewOffset)
{
    off_t off;
    off = m_file.SeekEnd(iNewOffset);
    if (off == wxInvalidOffset)
    {
        wxLogDebug("FileStream::SeekEnd - Invalid Offset!");
        return false;
    }
    return true;


}

unsigned int FileStream::GetLength()
{
	return m_file.Length();
}

bool FileStream::IsSeekable()
{
	return true;
}

bool FileStream::Error()
{
    return m_file.Error();
}

off_t FileStream::GetPosition()
{
    return m_file.Tell();

}

bool FileStream::Eof()
{

    if (m_file.Eof())
        return true;
    else
        return false;
}

