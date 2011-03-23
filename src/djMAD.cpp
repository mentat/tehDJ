/*
 * tehDJ - A realtime mp3 dj-ing system
 * (C) 2002 - Jesse Lovelace
 *
 * Based on some work from: MPEG Audio codec (using MAD)
 * Copyright (C) 2001 Robert Leslie
 *
 */
#include "wx/wx.h"
#include "djCodec.h"
#include "djStream.h"
#include "mad.h"

#define SAMPLE_DEPTH	16
#define CIC_INVALIDGENRE			((unsigned char)0xFF)
#define CIC_INVALIDTRACKNUM			((unsigned char)0xFF)

enum {
  XING_FRAMES = 0x00000001L,
  XING_BYTES  = 0x00000002L,
  XING_TOC    = 0x00000004L,
  XING_SCALE  = 0x00000008L
};

struct ID3Tag
{
	char m_cTAG[3];		// Must equal "TAG"
	char m_cSong[30];
	char m_cArtist[30];
	char m_cAlbum[30];
	char m_cYear[4];
	char m_cComment[30];
	unsigned char m_cGenre;
};


# define XING_MAGIC	(('X' << 24) | ('i' << 16) | ('n' << 8) | 'g')

void equalizer_filter(struct mad_frame *frame, mad_fixed_t eqfactor[32])
{
  unsigned int nch, ch, ns, s, sb;

  nch = MAD_NCHANNELS(&frame->header);
  ns  = MAD_NSBSAMPLES(&frame->header);

  for (ch = 0; ch < nch; ++ch) {
    for (s = 0; s < ns; ++s) {
      for (sb = 0; sb < 32; ++sb) {
	frame->sbsample[ch][s][sb] =
	  mad_f_mul(frame->sbsample[ch][s][sb], eqfactor[sb]);
      }
    }
  }
}

int MADCoDec::ParseXing(xing *_xing, struct mad_bitptr ptr, unsigned int bitlen)
{
  if (bitlen < 64 || mad_bit_read(&ptr, 32) != XING_MAGIC)
    goto fail;

  _xing->flags = mad_bit_read(&ptr, 32);
  bitlen -= 64;

  if (_xing->flags & XING_FRAMES) {
    if (bitlen < 32)
      goto fail;

    _xing->frames = mad_bit_read(&ptr, 32);
    bitlen -= 32;
  }

  if (_xing->flags & XING_BYTES) {
    if (bitlen < 32)
      goto fail;

    _xing->bytes = mad_bit_read(&ptr, 32);
    bitlen -= 32;
  }

  if (_xing->flags & XING_TOC) {
    int i;

    if (bitlen < 800)
      goto fail;

    for (i = 0; i < 100; ++i)
      _xing->toc[i] = (unsigned char) mad_bit_read(&ptr, 8);

    bitlen -= 800;
  }

  if (_xing->flags & XING_SCALE) {
    if (bitlen < 32)
      goto fail;

    _xing->scale = mad_bit_read(&ptr, 32);
    bitlen -= 32;
  }

  return 0;

 fail:
  _xing->flags = 0;
  return -1;
}

/*
 * NAME:		scan_header()
 * DESCRIPTION:	read the initial frame(s) to get stream statistics
 */

int MADCoDec::ScanHeader(InStream* pInStream, mad_header *header, xing *_xing)
{
	mad_stream stream;
	mad_frame frame;
	unsigned char buffer[8192];
	unsigned int buflen = 0;
	int count = 0, result = 0;

	mad_stream_init(&stream);
	mad_frame_init(&frame);

	if (_xing)
		_xing->flags = 0;

	while (1) {
		if (buflen < sizeof(buffer)) {
			unsigned int bytes;

			if(pInStream->Read(buffer + buflen, sizeof(buffer) - buflen, bytes) == FALSE 
				|| bytes == 0) 
			{
				result = -1;
				break;
			}

			buflen += bytes;
		}

		mad_stream_buffer(&stream, buffer, buflen);

		while (1) {
			if (mad_frame_decode(&frame, &stream) == -1) {
				if (!MAD_RECOVERABLE(stream.error))
					break;

				continue;
			}

			if (count++ ||
				(_xing && ParseXing(_xing, stream.anc_ptr,
									stream.anc_bitlen) == 0))
				break;
		}

		if (count || stream.error != MAD_ERROR_BUFLEN)
			break;

		memmove(buffer, stream.next_frame,
				buflen = &buffer[buflen] - stream.next_frame);
	}

	if (count) {
		if (header)
			*header = frame.header;
	}
	else
		result = -1;

	mad_frame_finish(&frame);
	mad_stream_finish(&stream);

	return result;
}

/*
 * NAME:		round()
 * DESCRIPTION:	round and scale sample
 */

signed int MADCoDec::Round(mad_fixed_t sample, dither *_dither)
{
	enum {
		MIN = -MAD_F_ONE,
		MAX =  MAD_F_ONE - 1
	};

	/* round */
	sample += (1L << (MAD_F_FRACBITS - SAMPLE_DEPTH));

	/* clip */
	if (sample > MAX)
		sample = MAX;
	else if (sample < MIN)
		sample = MIN;

	/* quantize */
	return sample >> (MAD_F_FRACBITS + 1 - SAMPLE_DEPTH);
}

/*
 * NAME:		prng()
 * DESCRIPTION:	32-bit pseudo-random number generator
 */

unsigned long MADCoDec::Prng(unsigned long state)
{
  return (state * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;
}

/*
 * NAME:		dither()
 * DESCRIPTION:	dither and scale sample
 */
signed int MADCoDec::Dither(mad_fixed_t sample, dither *_dither)
{
	unsigned int scalebits;
	mad_fixed_t output, mask, random;

	enum {
		MIN = -MAD_F_ONE,
		MAX =  MAD_F_ONE - 1
	};

	/* noise shape */
	sample += _dither->error[0] - _dither->error[1] + _dither->error[2];

	_dither->error[2] = _dither->error[1];
	_dither->error[1] = _dither->error[0] / 2;

	/* bias */
	output = sample + (1L << (MAD_F_FRACBITS + 1 - SAMPLE_DEPTH - 1));

	scalebits = MAD_F_FRACBITS + 1 - SAMPLE_DEPTH;
	mask = (1L << scalebits) - 1;

	/* dither */
	random  = Prng(_dither->random);
	output += (random & mask) - (_dither->random & mask);

	_dither->random = random;

	/* clip */
	if (output > MAX) {
		output = MAX;

		if (sample > MAX)
			sample = MAX;
	}
	else if (output < MIN) {
		output = MIN;

		if (sample < MIN)
			sample = MIN;
	}

	/* quantize */
	output &= ~mask;

	/* error feedback */
	_dither->error[0] = sample - output;

	/* scale */
	return output >> scalebits;
}

void MADCoDec::PackPcm(unsigned char **pcm, unsigned int nsamples,
			  mad_fixed_t const *ch1, mad_fixed_t const *ch2)
{
	register signed int s0, s1;
	static dither d0, d1;

	if (ch2) {  /* stereo */
		while (nsamples--) {
			s0 = Dither(*ch1++, &d0);
			s1 = Dither(*ch2++, &d1);
# if SAMPLE_DEPTH == 16
			(*pcm)[0 + 0] = s0 >> 0;
			(*pcm)[0 + 1] = s0 >> 8;
			(*pcm)[2 + 0] = s1 >> 0;
			(*pcm)[2 + 1] = s1 >> 8;

			*pcm += 2 * 2;
# elif SAMPLE_DEPTH == 8
			(*pcm)[0] = s0 ^ 0x80;
			(*pcm)[1] = s1 ^ 0x80;

			*pcm += 2;
# else
#  error "bad SAMPLE_DEPTH"
# endif
		}
	}
	else {  /* mono */
		while (nsamples--) {
			s0 = Dither(*ch1++, &d0);

# if SAMPLE_DEPTH == 16
			(*pcm)[0] = s0 >> 0;
			(*pcm)[1] = s0 >> 8;

			*pcm += 2;
# elif SAMPLE_DEPTH == 8
			*(*pcm)++ = s0 ^ 0x80;
# endif
		}
	}
}


void MADCoDec::Cleanup()
{
	if(m_pInStream)
	{
		delete m_pInStream;
		m_pInStream = NULL;
        mad_synth_finish(&mSynth);
	    mad_frame_finish(&mFrame);
	    mad_stream_finish(&mStream);
		
	}

}

MADCoDec::MADCoDec()
: CoDec()
{
	
	// Setup private data
	m_pInStream = NULL;
	mSize = 0;

	m_iPretendOption = 42;

	mXing.flags = 0;

	mSamplecount = 0;

	mTimer  = mad_timer_zero;
	mLength = mad_timer_zero;
	mRate   = 0;
	mFrames = 0;

	m_iFileLength_Secs = 0;
	m_iBitRate_Kbs     = 0;
	m_iFreq_Hz         = 0;
	m_bStereo          = TRUE;
	m_b16bit           = (SAMPLE_DEPTH == 16);

	mBuflen = 0;

	//InitialiseFileAssociations(this);
	//AddFileAssociation(this, "MP3", 0L);
	//AddFileAssociation(this, "MP2", 0L);
	//AddFileAssociation(this, "MP1", 0L);
}

MADCoDec::~MADCoDec()
{
    mad_synth_finish(&mSynth);
	mad_frame_finish(&mFrame);
	mad_stream_finish(&mStream);
    delete m_pInStream;
	m_pInStream = NULL;

}

bool MADCoDec::OpenFile(char const *path)
{
	Cleanup();

    wxLogDebug("CPICoDec::OpenFile: \"%s\"", path);

	m_pInStream = new FileStream(path);

	if(!m_pInStream) 
	{
        wxLogDebug("CPICoDec::OpenFile: 'new' error");
		return false;
	}

    if(m_pInStream->Error()) 
	{
        wxLogDebug("CPICoDec::OpenFile: error");
        delete m_pInStream;
        m_pInStream = NULL;
		return false;
	}

    wxLogDebug("Geting id3.");
    ReadTagID3();


	mad_stream_init(&mStream);
	mad_frame_init(&mFrame);
	mad_synth_init(&mSynth);

    m_pInStream->Seek(0);
	if (ScanHeader(m_pInStream,
					&mFrame.header, &mXing) == -1) 
    {
        wxLogDebug("CPICoDec::OpenFile: failed to read file header");
		Cleanup();
		return false;
	}

	m_pInStream->Seek(0);

	mSize             = m_pInStream->GetLength();
	mSynth.pcm.length = 0;
	mSamplecount      = 0;
	mTimer            = mad_timer_zero;

	if (mXing.flags & XING_FRAMES) 
    {
		mLength = mFrame.header.duration;
		mad_timer_multiply(&mLength, mXing.frames);
	}
	else 
    {
		/* estimate playing time from file size */

		mad_timer_set(&mLength, 0, 1, mFrame.header.bitrate / 8);
		mad_timer_multiply(&mLength, mSize);
	}

	mRate   = 0;
	mFrames = 0;

	m_iFileLength_Secs = mad_timer_count(mLength, MAD_UNITS_SECONDS);
	m_iBitRate_Kbs     = mFrame.header.bitrate / 1000;
	m_iFreq_Hz         = mFrame.header.samplerate;
    
    if (mFrame.header.mode == MAD_MODE_SINGLE_CHANNEL)
        m_bStereo = false;
    else
        m_bStereo = true;
	m_b16bit = (SAMPLE_DEPTH == 16);

	mBuflen = 0;

	return true;
}

void MADCoDec::CloseFile()
{
	wxLogDebug("Close MPEG file");

	Cleanup();
}

void MADCoDec::Seek(int const numer, int const denom)
{
   	double fraction;
	unsigned long position;

	wxASSERT(m_pInStream);
	wxASSERT(numer >= 0 && denom > 0);

	// If the IStream doesn't support seeking - ignore the seek
	if(m_pInStream->IsSeekable() == false)
    {
        wxLogDebug("Stream is not seekable.");
		return;
    }

	fraction = double(numer)/ double(denom);
	position = (unsigned long)
		(mad_timer_count(mLength, MAD_UNITS_MILLISECONDS) * fraction);
	
	if (mXing.flags & XING_TOC) {
		int percent, p1, p2;

		percent = (int) (fraction * 100);
		p1 = (percent < 100) ? mXing.toc[percent    ] : 0x100;
		p2 = (percent <  99) ? mXing.toc[percent + 1] : 0x100;

		fraction = (p1 + (p2 - p1) * (fraction * 100 - percent)) / 0x100;
	}

    wxLogDebug("Seeking to: %d, out of size: %d", long(mSize * fraction), m_pInStream->GetLength());
    
    if (long(mSize * fraction) > m_pInStream->GetLength())
    {
        wxLogDebug("Cannot seek past end of file!");
        return;
    }

	if (!m_pInStream->Seek((long) (mSize * fraction)))
    {
        wxLogDebug("File could not seek to that location: %u!", long(mSize * fraction));
        return;
    }

	if(m_pInStream->Read(mBuffer, sizeof(mBuffer),mBuflen) == false)
    {
        wxLogDebug("File could read at that location, closing!");
		mBuflen = 0;
        Cleanup();
        return;
    }

    if (mBuflen == 0)
    {
        wxLogDebug("Buffer length is 0!");
    }

    mad_timer_set(&mTimer, position / 1000, position % 1000, 1000);

	mad_stream_buffer(&mStream, mBuffer, mBuflen);

	//mad_frame_mute(&mFrame);
	//mad_synth_mute(&mSynth);

	if (numer) {
		int skip;

		skip = 2;
		do {
			if (mad_frame_decode(&mFrame, &mStream) == 0) {
				mad_timer_add(&mTimer, mFrame.header.duration);
				if (--skip == 0)
					mad_synth_frame(&mSynth, &mFrame);
			}
			else if (!MAD_RECOVERABLE(mStream.error))
				break;
		}
		while (skip);
	}

	mSynth.pcm.length = 0;
	mSamplecount      = 0;
}

bool MADCoDec::GetPCMBlock( void *block, unsigned long *size)
{
	unsigned char *samples = (unsigned char *)block;
    unsigned int nsamples;

	if (!m_pInStream)
    {
        wxLogError("MADCoDec::GetPCMBlock - instream is null");
        return false;
    }

	nsamples = (*size / (SAMPLE_DEPTH / 8)) >>
		(m_bStereo ? 1 : 0);
	*size = 0;

	while (nsamples) 
    {
		unsigned int count, bitrate;

		count = mSynth.pcm.length - mSamplecount;
		if (count > nsamples)
			count = nsamples;

		if (count) {
			const mad_fixed_t *ch1, *ch2;

			ch1 = mSynth.pcm.samples[0] + mSamplecount;
			ch2 = mSynth.pcm.samples[1] + mSamplecount;

			if (m_bStereo == false)
				ch2 = 0;
			else if (mSynth.pcm.channels == 1)
				ch2 = ch1;

			PackPcm(&samples, count, ch1, ch2);

			mSamplecount += count;
			nsamples             -= count;

			if (nsamples == 0)
				break;
		}

		while (mad_frame_decode(&mFrame, &mStream) == -1) 
        {
            if (m_pInStream->Eof())
            {
                wxLogDebug("End of file.");
                return false;
            }
			unsigned int bytes;

			if (MAD_RECOVERABLE(mStream.error))
				continue;
 
			if (mStream.next_frame) {
				memmove(mBuffer, mStream.next_frame,
						mBuflen = mBuffer +
						mBuflen - mStream.next_frame);
			}

			if(m_pInStream->Read(mBuffer + mBuflen,
                sizeof(mBuffer) - mBuflen, bytes) == false || bytes == 0)
			{
				return false;
			}

			mad_stream_buffer(&mStream, mBuffer, mBuflen += bytes);
		}

		bitrate = mFrame.header.bitrate / 1000;

		mRate += bitrate;
		mFrames++;

		m_iBitRate_Kbs = bitrate;

		mad_synth_frame(&mSynth, &mFrame);

		mSamplecount = 0;

		mad_timer_add(&mTimer, mFrame.header.duration);
	}

	*size = samples - (unsigned char *) block;

	return true;
}

void MADCoDec::GetFileInfo()
{


	if (!(mXing.flags & XING_FRAMES) && mFrames) {
		/* update length estimate */

		mad_timer_set(&mLength, 0,
					  1, (mRate / mFrames) * (1000 / 8));
		mad_timer_multiply(&mLength, mSize);

		if (mad_timer_compare(mTimer, mLength) > 0) 
		{
			mLength = mTimer;
			mSize   = m_pInStream->GetLength();
		}

		m_iFileLength_Secs = mad_timer_count(mLength, MAD_UNITS_SECONDS);
	}

}

int MADCoDec::GetCurrentTime()
{

	return mad_timer_count(mTimer, MAD_UNITS_SECONDS);
}

int MADCoDec::GetTotalTime()
{

    return mad_timer_count(mLength, MAD_UNITS_SECONDS);
}

string CoDec::DecodeID3String(const char* pcSource, const int iLength)
{
    int spaceIndex = iLength - 1;
    for (int i = (iLength - 1); i >= 0; i--)
    {
        if (pcSource[i] == ' ')
            spaceIndex = i;
        else
            break;
    }
	
	return string(pcSource, spaceIndex);
}

void CoDec::ReadTagID3()
{
	ID3Tag ID3;

	if(m_Artist != "")
    {
        wxLogDebug("ID3 Tag already read!");
		return;
    }

    if (!m_pInStream)
    {
        wxLogError("CoDec::ReadTagID3 - Input stream not open!");
        return;
    }

    if (!m_pInStream->IsSeekable())
    {
        wxLogDebug("CoDec::ReadTagID3 - Input not seekable");
        return;
    }

    off_t oldpos = m_pInStream->GetPosition();


    if (!m_pInStream->SeekEnd(-int(sizeof(ID3))))
    {
        wxLogError("CoDec::ReadTagID3 - Could not seek to end of file.");
        return;
    }

    unsigned int actual = 0;
    wxLogDebug("size of id3 = %d", sizeof(ID3));
    if (!m_pInStream->Read((void *)&ID3, sizeof(ID3), actual))
    {
        wxLogError("CoDec::ReadTagID3 - could not read from file.");
        return;
    }

	// Not enough file data returned - or the data returned does not look like an ID3
	if(actual != sizeof(ID3))
    {
        wxLogDebug("CoDec::ReadTagID3 - Not enough file data returned: %d", actual);
        wxLogDebug("Offset = %d", m_pInStream->GetPosition());
		//wxLogDebug(string((char *)&ID3, sizeof(ID3)).c_str());
        return;
    }
    if (memcmp(ID3.m_cTAG, "TAG", 3) != 0)
    {
        wxLogDebug("CoDec::ReadTagID3 - The data returned does not look like an ID3");
		//wxLogDebug(string((char *)&ID3, sizeof(ID3)).c_str());
        return;
    }

	m_Song = DecodeID3String(ID3.m_cSong, 30);  
	m_Artist = DecodeID3String(ID3.m_cArtist, 30);    
	m_Album = DecodeID3String(ID3.m_cAlbum, 30);
	m_Year = DecodeID3String(ID3.m_cYear, 4);

    wxLogDebug("Song = %s", m_Song.c_str());
    wxLogDebug("Artist = %s", m_Artist.c_str());
    wxLogDebug("Album = %s", m_Album.c_str());
    wxLogDebug("Year = %s", m_Year.c_str());

	// ID3v1.1 - If the 29th byte of the comment is 0 then the 30th byte is the track num
	// ** Some dodgy implementations of ID3v1.1 just slap a <32 char byte at position 30 and hope
	//    for the best - handle these too <hmph!>
	if(ID3.m_cComment[28] == '\0' || ID3.m_cComment[29] < 32)
	{
		m_Comment = DecodeID3String(ID3.m_cComment, 28);
		m_TrackNum = ID3.m_cComment[29];
	}
	else
	{
		m_Comment = DecodeID3String(ID3.m_cComment, 30);
		m_TrackNum = CIC_INVALIDTRACKNUM;
	}

}
