// --*-c++-*--
/*
    tehDJ - Mp3 DJ System
    Copyright (C) 2002  Jesse Lovelace
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef DJ_CODEC_H
#define DJ_CODEC_H

#include "mad.h"
#include "mpg123.h"
#include "mpglib.h"
#include "djStream.h"
#include "djFilter.h"

struct xing {
  long flags;
  unsigned long frames;
  unsigned long bytes;
  unsigned char toc[100];
  long scale;
};

struct dither {
	mad_fixed_t error[3];
	mad_fixed_t random;
};

class wxVisual;
class CoDec
{
public:

    CoDec():mFilter(NULL) {}

	virtual bool OpenFile(const char* pcFilename) = 0;
	virtual void CloseFile() = 0;
	virtual void Seek(const int iNumerator, const int iDenominator) = 0;
	virtual void GetFileInfo() = 0;
    virtual bool GetPCMBlock(void* pBlock, unsigned long* pdwBlockSize) = 0;
	virtual int GetCurrentTime() = 0;
    virtual int GetTotalTime() = 0;
    virtual bool Ok() = 0;

    void AddFilter(Filter * flt){ if (mFilter) mFilter->Add(flt); else mFilter = flt; }

  //  void InitialiseFileAssociations();
   // void EmptyFileAssociations();
   // void AddFileAssociation(const char* pcExtension, unsigned long dwCookie);
   // bool IsAssociated(const char* pcExtension, unsigned long* pdwCookie);
  //  void AssociateWithEXE();

    string GetTitle() { return m_Song; }
    string GetArtist() { return m_Artist; }
    string GetAlbum() { return m_Album; }
    string GetYear() { return m_Year; }
    int GetBitrate() { return m_iBitRate_Kbs; }
    int GetFreq() { return m_iFreq_Hz; }
    bool IsStereo() { return m_bStereo; }
    string GetTrackNum() { return m_TrackNum; }
    string GetGenre();

    void ReadTagID3();
    static string DecodeID3String(const char* pcSource, const int iLength);

protected:

    Filter * mFilter;
    int m_iPretendOption;
    InStream * m_pInStream;

    string m_Song;
	string m_Artist;
	string m_Album;
	string m_Year;
	string m_Comment;
    string m_TrackNum;
	unsigned char m_Genre;

public:

    int m_iFileLength_Secs;
	int m_iBitRate_Kbs;
	int m_iFreq_Hz;
	bool m_bStereo;
	bool m_b16bit;

};

class MADCoDec: public CoDec
{
public:
    MADCoDec();
    ~MADCoDec();

    void Cleanup();
    bool Ok() 
    { if (!m_pInStream) return false; return true; }

	virtual bool OpenFile(const char* pcFilename);
	virtual void CloseFile();
	virtual void Seek(const int iNumerator, const int iDenominator);
	virtual void GetFileInfo();
    virtual bool GetPCMBlock(void* pBlock, unsigned long* pdwBlockSize);
	virtual int GetCurrentTime();
    virtual int GetTotalTime();

private:

	unsigned long mSize;		    /* stream size, in bytes */

	xing mXing;				        /* Xing VBR tag data */

	mad_stream mStream;		        /* MAD stream structure */
	mad_frame mFrame;			    /* MAD frame structure */
	mad_synth mSynth;			    /* MAD synth structure */

	unsigned int mSamplecount;		/* samples output from current frame */

	mad_timer_t mTimer;				/* current playing time position */
	mad_timer_t mLength;				/* total playing time of current stream */

	unsigned long mRate;				/* bitrate sum for computing average */
	unsigned long mFrames;			/* number of frames decoded */

	unsigned char mBuffer[40000];	/* input stream buffer */
	unsigned int mBuflen;			/* input stream buffer length */

public:

    static void PackPcm(unsigned char **pcm, unsigned int nsamples,
			  mad_fixed_t const *ch1, mad_fixed_t const *ch2);
    static inline signed int Dither(mad_fixed_t sample, dither *dither);
    static inline unsigned long Prng(unsigned long state);
    static inline signed int Round(mad_fixed_t sample, dither *_dither);
    static int ScanHeader(InStream* pInStream, mad_header *header, xing *_xing);
    static int ParseXing(struct xing *xing, mad_bitptr ptr, unsigned int bitlen);

private:



};

class MPGLibCoDec: public CoDec
{
public:
    MPGLibCoDec();

    virtual bool OpenFile(const char* pcFilename);
	virtual void CloseFile();
	virtual void Seek(const int iNumerator, const int iDenominator);
	virtual void GetFileInfo();
    virtual bool GetPCMBlock(void* pBlock, unsigned long* pdwBlockSize);
	virtual int GetCurrentTime();
    virtual bool Ok();


private:

    unsigned char mBuffer[40000];	/* input stream buffer */
	unsigned int mBuflen;			/* input stream buffer length */
    mpstr m_mp;

    unsigned long mSize;


};

#if 0
class WavModule: public CoDecModule
{
public:
    WavModule();
};

class OggModule: public CoDecModule
{
public:
    OggModule();
};
#endif
#endif

