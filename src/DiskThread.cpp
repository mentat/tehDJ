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

#include "wx/wx.h"
#include "wx/file.h"
#include "wx/thread.h"

#include "mad.h"

#include "djGui.h"
#include "dj.h"
#include "DiskThread.h"

DECLARE_APP(tehDJ)

#define MadErrorString(x) mad_stream_errorstr(x)

static inline
signed int scale(mad_fixed_t sample)
{
  /* round */
  sample += (1L << (MAD_F_FRACBITS - 16));

  /* clip */
  if (sample >= MAD_F_ONE)
    sample = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
    sample = -MAD_F_ONE;

  /* quantize */
  return sample >> (MAD_F_FRACBITS + 1 - 16);
}

CoDecThread::CoDecThread(MainFrame *frame, const wxString& filename, 
                               const unsigned short buffsize, const unsigned short waveblock)
        : wxThread(), mBuffSize(buffsize), mInputBuffer(NULL), mWaveBuff(NULL), mWaveSize(waveblock)
{
    m_frame = frame;
    m_filename = filename;

    
}

void CoDecThread::OnExit()
{
#if 0
    wxCriticalSectionLocker locker(wxGetApp().m_critsect);

    wxArrayThread& threads = wxGetApp().m_threads;
    threads.Remove(this);

    if ( threads.IsEmpty() )
    {
        // signal the main thread that there are no more threads left if it is
        // waiting for us
        if ( wxGetApp().m_waitingUntilAllDone )
        {
            wxGetApp().m_waitingUntilAllDone = FALSE;

            wxMutexLocker lock(wxGetApp().m_mutexAllDone);
            wxGetApp().m_condAllDone.Signal();
        }
    }
#endif
}

void *CoDecThread::Entry()
{
    if (mWaveSize == 0)
        return NULL;

    mWaveBuff = new short[mWaveSize];
    mInputBuffer = new unsigned char[mBuffSize];
    
    wxString text;
    int		        Status = 0;
    unsigned long   i = 0, FrameCount=0;
    short           *OutputPtr=mWaveBuff;
	const short     *OutputBufferEnd = mWaveBuff + mWaveSize * sizeof(mWaveBuff[0]);

    if ((mInputBuffer == NULL) || (mWaveBuff == NULL))
    {
        WriteText(wxT("ERROR: Buffer in thread did not allocate."));
        text.Printf(wxT("Thread 0x%x finished.\n"), GetId());
        WriteText(text);
        return NULL;
    }
    
    text.Printf(wxT("Thread 0x%x started (priority = %u).\n"),
                GetId(), GetPriority());

    WriteText(text);

    mFile.Open((char *) m_filename.c_str());

    if (!mFile.IsOpened()) 
    {
        WriteText( _("Could not open file: ") + m_filename);
        text.Printf(wxT("Thread 0x%x finished.\n"), GetId());
        WriteText(text);
        return NULL;
    }  

    m_filesize = mFile.Length();

    struct mad_stream	Stream;
	struct mad_frame	Frame;
	struct mad_synth	Synth;
	mad_timer_t			Timer;

    /* First the structures used by libmad must be initialized. */
	mad_stream_init(&Stream);
	mad_frame_init(&Frame);
	mad_synth_init(&Synth);
	mad_timer_reset(&Timer);

    for (;;)
    {
        // check if we were asked to exit
        if ( TestDestroy() )
            break;

        if(Stream.buffer==NULL || Stream.error==MAD_ERROR_BUFLEN)
		{
			size_t			ReadSize,
							Remaining;
			unsigned char	*ReadStart;

			/* {1} libmad may not consume all bytes of the input
			 * buffer. If the last frame in the buffer is not wholly
			 * contained by it, then that frame's start is pointed by
			 * the next_frame member of the Stream structure. This
			 * common situation occurs when mad_frame_decode() fails,
			 * sets the stream error code to MAD_ERROR_BUFLEN, and
			 * sets the next_frame pointer to a non NULL value. (See
			 * also the comment marked {2} bellow.)
			 *
			 * When this occurs, the remaining unused bytes must be
			 * put back at the beginning of the buffer and taken in
			 * account before refilling the buffer. This means that
			 * the input buffer must be large enough to hold a whole
			 * frame at the highest observable bit-rate (currently 448
			 * kb/s). XXX=XXX Is 2016 bytes the size of the largest
			 * frame? (448000*(1152/32000))/8
			 */
			if(Stream.next_frame!=NULL)
			{
				Remaining=Stream.bufend-Stream.next_frame;
				memmove(mInputBuffer,Stream.next_frame,Remaining);
				ReadStart=mInputBuffer+Remaining;
				ReadSize=mBuffSize-Remaining;
			}
			else
				ReadSize=mBuffSize,
					ReadStart=mInputBuffer,
					Remaining=0;
			
			/* Fill-in the buffer. If an error occurs print a message
			 * and leave the decoding loop. If the end of stream is
			 * reached we also leave the loop but the return status is
			 * left untouched.
			 */
            ReadSize=mFile.Read(ReadStart, ReadSize);
			
            if(ReadSize<=0)
			{
				if(ReadSize == -1)
				{
					Status=1;
                    WriteText(wxT("Read error on bitstream\n"));
				}
                
                if(mFile.Eof())
					WriteText(wxT("End of input stream.\n"));

				break;
			}

			/* Pipe the new buffer content to libmad's stream decoder
             * facility.
			 */
			mad_stream_buffer(&Stream,mInputBuffer,ReadSize+Remaining);
			Stream.error=MAD_ERROR_NONE;
		}

		/* Decode the next mpeg frame. The streams is read from the
		 * buffer, its constituents are break down and stored the the
		 * Frame structure, ready for examination/alteration or PCM
		 * synthesis. Decoding options are carried in the Frame
		 * structure from the Stream structure.
		 *
		 * Error handling: mad_frame_decode() returns a non zero value
		 * when an error occurs. The error condition can be checked in
		 * the error member of the Stream structure. A mad error is
		 * recoverable or fatal, the error status is checked with the
		 * MAD_RECOVERABLE macro.
		 *
		 * {2} When a fatal error is encountered all decoding
		 * activities shall be stopped, except when a MAD_ERROR_BUFLEN
		 * is signaled. This condition means that the
		 * mad_frame_decode() function needs more input to achieve
		 * it's work. One shall refill the buffer and repeat the
		 * mad_frame_decode() call. Some bytes may be left unused at
		 * the end of the buffer if those bytes forms an incomplete
		 * frame. Before refilling, the remainign bytes must be moved
		 * to the begining of the buffer and used for input for the
		 * next mad_frame_decode() invocation. (See the comments marked
		 * {1} earlier for more details.)
		 *
		 * Recoverable errors are caused by malformed bit-streams, in
		 * this case one can call again mad_frame_decode() in order to
		 * skip the faulty part and re-sync to the next frame.
		 */
		if(mad_frame_decode(&Frame,&Stream))
			if(MAD_RECOVERABLE(Stream.error))
			{
                wxString errmsg;
                errmsg.Printf("Recoverable frame level error (%s)\n", MadErrorString(&Stream));
                mFile.Flush();
				continue;
			}
			else
				if(Stream.error==MAD_ERROR_BUFLEN)
					continue;
				else
				{
					wxString errmsg;
                    errmsg.Printf("Unrecoverable frame level error (%s).\n", MadErrorString(&Stream));
					Status=1;
					break;
				}

		/* The characteristics of the stream's first frame is printed
		 * on stderr. The first frame is representative of the entire
		 * stream.
		 */
		if(FrameCount==0)
			PrintFrameInfo(&Frame.header);
			//{
				//Status=1;
			//	break;
			//}

		/* Accounting. The computed frame duration is in the frame
		 * header structure. It is expressed as a fixed point number
		 * whole data type is mad_timer_t. It is different from the
		 * samples fixed point format and unlike it, it can't directly
		 * be added or substracted. The timer module provides several
		 * functions to operate on such numbers. Be careful there, as
		 * some functions of mad's timer module receive some of their
		 * mad_timer_t arguments by value!
		 */
		FrameCount++;
		mad_timer_add(&Timer,Frame.header.duration);
				
		/* Once decoded the frame is synthesized to PCM samples. No errors
		 * are reported by mad_synth_frame();
		 */
		mad_synth_frame(&Synth,&Frame);

		/* Synthesized samples must be converted from mad's fixed
		 * point number to the consumer format. Here we use unsigned
		 * 16 bit big endian integers on two channels. Integer samples
		 * are temporarily stored in a buffer that is flushed when
		 * full.
		 */
		for(i=0;i<Synth.pcm.length;i++)
		{
            
            short	Sample;

			/* Left channel */
			Sample=scale(Synth.pcm.samples[0][i]);
			//*(OutputPtr++)= (Sample & 0xff);
			//*(OutputPtr++)= ((Sample >> 8) & 0xff);
            *(OutputPtr++)= Sample;

			/* Right channel. If the decoded stream is monophonic then
			 * the right output channel is the same as the left one.
			 */
			if(MAD_NCHANNELS(&Frame.header)==2)
				Sample=scale(Synth.pcm.samples[1][i]);
            *(OutputPtr++)= Sample;
			//*(OutputPtr++)= (Sample & 0xff);
			//*(OutputPtr++)= ((Sample >> 8) & 0xff);

			/* Flush the buffer if it is full. */
			if(OutputPtr==OutputBufferEnd)
			{
               
                m_frame->mQueue->push(mWaveBuff);
                
                if ( TestDestroy() )
                       break;
           
                text.Printf(wxT("Wrote %u bytes to wave queue.\n"), mWaveSize);
                WriteText(text);

				OutputPtr=mWaveBuff;

			}     
		}

        if (OutputPtr != mWaveBuff)
        {
            text.Printf(wxT("Disk wave buffer at %u.\n"), (100 * int(OutputPtr - mWaveBuff))/mWaveSize);
            WriteText(text);
        }

        if ( TestDestroy() )
            break;
        
        wxThread::Sleep(200);
    }
    /* Mad is no longer used, the structures that were initialized must
     * now be cleared.
	 */

    mad_synth_finish(&Synth);
	mad_frame_finish(&Frame);
	mad_stream_finish(&Stream);

    text.Printf(wxT("Thread 0x%x finished.\n"), GetId());
    WriteText(text);

    delete [] mWaveBuff;
    delete [] mInputBuffer;
    mWaveBuff = NULL;
    mInputBuffer = NULL;

    //wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, WORKER_EVENT );
    //event.SetInt(-1); // that's all
    //wxPostEvent( m_frame, event );

    return NULL;
}

void CoDecThread::WriteText(const wxString& text)
{
    wxString msg;

    // before doing any GUI calls we must ensure that this thread is the only
    // one doing it!

    wxMutexGuiEnter();

    msg << text;
    m_frame->WriteText(msg);

    wxMutexGuiLeave();
}

void CoDecThread::PrintFrameInfo(mad_header *Header)
{
	const char	*Layer,
				*Mode,
				*Emphasis;

	/* Convert the layer number to it's printed representation. */
	switch(Header->layer)
	{
		case MAD_LAYER_I:
			Layer="I";
			break;
		case MAD_LAYER_II:
			Layer="II";
			break;
		case MAD_LAYER_III:
			Layer="III";
			break;
		default:
			Layer="(unexpected layer value)";
			break;
	}

	/* Convert the audio mode to it's printed representation. */
	switch(Header->mode)
	{
		case MAD_MODE_SINGLE_CHANNEL:
			Mode="single channel";
			break;
		case MAD_MODE_DUAL_CHANNEL:
			Mode="dual channel";
			break;
		case MAD_MODE_JOINT_STEREO:
			Mode="joint (MS/intensity) stereo";
			break;
		case MAD_MODE_STEREO:
			Mode="normal LR stereo";
			break;
		default:
			Mode="(unexpected mode value)";
			break;
	}

	/* Convert the emphasis to it's printed representation. */
	switch(Header->emphasis)
	{
		case MAD_EMPHASIS_NONE:
			Emphasis="no";
			break;
		case MAD_EMPHASIS_50_15_US:
			Emphasis="50/15 us";
			break;
		case MAD_EMPHASIS_CCITT_J_17:
			Emphasis="CCITT J.17";
			break;
		default:
			Emphasis="(unexpected emphasis value)";
			break;
	}

    wxString info;
    info.Printf("%u kb/s audio mpeg layer %s stream %s crc, "
			"%s with %s emphasis at %d Hz sample rate\n",
			Header->bitrate,Layer,
			Header->flags&MAD_FLAG_PROTECTION?"with":"without",
			Mode,Emphasis,Header->samplerate);

    WriteText(info);
}