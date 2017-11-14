#include "DirectSoundWrapper.h"


DirectSoundWrapper::DirectSoundWrapper(void)
	: m_dsound_dll(NULL), m_dsound_context(NULL), m_dsound_buffer(NULL),
	m_buffer_count(0), m_buffer_length(0), m_last_buffer(0)
{
}

DirectSoundWrapper::~DirectSoundWrapper(void)
{
	CleanUp();
}

bool DirectSoundWrapper::Initialize(uint8_t channels, uint32_t samples, uint32_t sample_rate, uint8_t bits_per_sample)
{
	const uint16_t buffer_count = 10;
	if (!DirectSoundCreate())
	{
		return false;
	}
	if (!DirectSoundCreatePCMBuffer(channels, samples, sample_rate, bits_per_sample, buffer_count))
	{
		DirectSoundDestroy();
		return false;
	}
	return true;
}

void DirectSoundWrapper::CleanUp()
{
	DirectSoundDestroyPCMBuffer();
	DirectSoundDestroy();
}

uint32_t DirectSoundWrapper::GetSampleSize()
{
	return m_buffer_length;
}

bool DirectSoundWrapper::FillSample(uint8_t *pcm_data, uint32_t data_size)
{
	return DirectSoundFillBuffer(pcm_data, data_size);
}

void DirectSoundWrapper::WaitDone()
{
	DWORD status = 0;
	DWORD cursor = 0;
	DWORD junk = 0;
	HRESULT result = DS_OK;

	result = IDirectSoundBuffer_GetCurrentPosition(m_dsound_buffer, &junk, &cursor);
	if (result != DS_OK) 
	{
		if (result == DSERR_BUFFERLOST)
		{
			IDirectSoundBuffer_Restore(m_dsound_buffer);
		}
		return;
	}
	while ((cursor / m_buffer_length) == m_last_buffer) 
	{
		Sleep(1);
		IDirectSoundBuffer_GetStatus(m_dsound_buffer, &status);
        if ((status & DSBSTATUS_BUFFERLOST))
		{
			IDirectSoundBuffer_Restore(m_dsound_buffer);
			IDirectSoundBuffer_GetStatus(m_dsound_buffer, &status);
            if ((status & DSBSTATUS_BUFFERLOST))
			{
				break;
            }
        }
		if (!(status & DSBSTATUS_PLAYING)) 
		{
			result = IDirectSoundBuffer_Play(m_dsound_buffer, 0, 0, DSBPLAY_LOOPING);
			if (result == DS_OK)
			{
                continue;
            }
            return;
        }
        result = IDirectSoundBuffer_GetCurrentPosition(m_dsound_buffer, &junk, &cursor);
        if (result != DS_OK) 
		{
            return;
        }
    }
}

bool DirectSoundWrapper::DirectSoundCreate()
{
	m_dsound_dll = ::LoadLibraryA("dsound.dll");
	if ( m_dsound_dll == NULL )
	{
		return false;
	}
	
	HRESULT (WINAPI *DirectSoundCreate8)( LPCGUID , LPDIRECTSOUND8 *, LPUNKNOWN ) = 
		(HRESULT (WINAPI *)( LPCGUID , LPDIRECTSOUND8 *, LPUNKNOWN ))::GetProcAddress(m_dsound_dll, "DirectSoundCreate8");
	if (DirectSoundCreate8 == NULL)
	{
		::FreeLibrary(m_dsound_dll);
		m_dsound_dll = NULL;
		return false;
	}
	if ( DirectSoundCreate8( NULL, &m_dsound_context, NULL) != DS_OK )
	{
		//Close Device
		DirectSoundDestroy();
		return false;
	}
	return true;
}

void DirectSoundWrapper::DirectSoundDestroy()
{
	if (m_dsound_context)
	{
		IDirectSound8_Release(m_dsound_context);
		m_dsound_context = NULL;
	}
	if (m_dsound_dll)
	{
		::FreeLibrary(m_dsound_dll);
		m_dsound_dll = NULL;
	}
}

bool DirectSoundWrapper::DirectSoundCreatePCMBuffer(uint8_t channels, uint32_t samples,
													uint32_t sample_rate, uint8_t bits_per_sample, uint16_t buffer_count)
{
	LPVOID pvAudioPtr1	= NULL;
	LPVOID pvAudioPtr2	= NULL;
	DWORD dwAudioBytes1	= 0;
	DWORD dwAudioBytes2	= 0;
	WAVEFORMATEX wfmt	= {0};
	DSBUFFERDESC format	= {0};

	m_buffer_count = buffer_count;
	m_buffer_length = samples * bits_per_sample / 8 * channels;

	wfmt.wFormatTag = WAVE_FORMAT_PCM;
	wfmt.wBitsPerSample = bits_per_sample;
	wfmt.nChannels = channels;
	wfmt.nSamplesPerSec = sample_rate;
	wfmt.nBlockAlign = wfmt.nChannels * (wfmt.wBitsPerSample / 8);
	wfmt.nAvgBytesPerSec = wfmt.nSamplesPerSec * wfmt.nBlockAlign;

	if ( IDirectSound_SetCooperativeLevel(m_dsound_context, GetDesktopWindow(), DSSCL_NORMAL) != DS_OK ) 
	{
		//"DirectSound SetCooperativeLevel failed"
		return false;
	}

	format.dwSize = sizeof(format);
	format.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME;
	format.dwBufferBytes = m_buffer_count * m_buffer_length;
	if ( (format.dwBufferBytes < DSBSIZE_MIN) || (format.dwBufferBytes > DSBSIZE_MAX) )
	{
		return false;
	}
	format.dwReserved = 0;
	format.lpwfxFormat = &wfmt;
	if (IDirectSound_CreateSoundBuffer(m_dsound_context, &format, &m_dsound_buffer, NULL) != DS_OK) 
	{
		//"DirectSound CreateSoundBuffer"
		return false;
	}
	IDirectSoundBuffer_SetFormat(m_dsound_buffer, &wfmt);

	if (IDirectSoundBuffer_Lock(m_dsound_buffer, 0, format.dwBufferBytes,
			(LPVOID *) & pvAudioPtr1, &dwAudioBytes1,
			(LPVOID *) & pvAudioPtr2, &dwAudioBytes2,
			DSBLOCK_ENTIREBUFFER) == DS_OK)
	{
		ZeroMemory(pvAudioPtr1, dwAudioBytes1);
		IDirectSoundBuffer_Unlock(m_dsound_buffer, (LPVOID) pvAudioPtr1, dwAudioBytes1,
			(LPVOID) pvAudioPtr2, dwAudioBytes2);
	}

	return true;
}

void DirectSoundWrapper::DirectSoundDestroyPCMBuffer()
{
	if( m_dsound_buffer )
    {
		IDirectSoundBuffer_Release( m_dsound_buffer );
		m_dsound_buffer = NULL;
    }
}

bool DirectSoundWrapper::DirectSoundFillBuffer(uint8_t *pcm_data, uint32_t data_size)
{
	DWORD	cursor			= 0;
	DWORD	junk			= 0;
    DWORD	raw_length		= 0;
	LPVOID	locked_buffer	= NULL;
	DWORD	status			= 0;

	if (data_size != m_buffer_length)
	{
		return false;
	}

	if (IDirectSoundBuffer_GetCurrentPosition(m_dsound_buffer, &junk, &cursor) == DSERR_BUFFERLOST)
	{
		IDirectSoundBuffer_Restore(m_dsound_buffer);
		if ( IDirectSoundBuffer_GetCurrentPosition(m_dsound_buffer, &junk, &cursor) != DS_OK )
		{
			return false;
		}
    }
    cursor /= m_buffer_length;
    m_last_buffer = cursor;
    cursor = (cursor + 1) % m_buffer_count;
    cursor *= m_buffer_length;

    if (IDirectSoundBuffer_Lock(m_dsound_buffer, cursor,
			m_buffer_length, &locked_buffer, &raw_length, NULL, &junk, 0) == DSERR_BUFFERLOST)
	{
        IDirectSoundBuffer_Restore(m_dsound_buffer);
        if ( IDirectSoundBuffer_Lock(m_dsound_buffer, cursor, m_buffer_length, 
			&locked_buffer, &raw_length, NULL, &junk, 0) != DS_OK )
		{
			return false;
		}
    }
	CopyMemory(locked_buffer, pcm_data, data_size);
	IDirectSoundBuffer_Unlock(m_dsound_buffer, locked_buffer, m_buffer_length, NULL, 0);
    IDirectSoundBuffer_GetStatus(m_dsound_buffer, &status);
    if ((status & DSBSTATUS_BUFFERLOST)) 
	{
        IDirectSoundBuffer_Restore(m_dsound_buffer);
        IDirectSoundBuffer_GetStatus(m_dsound_buffer, &status);
        if ((status & DSBSTATUS_BUFFERLOST))
		{
            return false;
        }
    }
	if (!(status & DSBSTATUS_PLAYING)) 
	{
        IDirectSoundBuffer_Play(m_dsound_buffer, 0, 0, DSBPLAY_LOOPING);
    }
    return true;
}

bool DirectSoundWrapper::SetVolume(uint32_t volume)
{
	return (IDirectSoundBuffer_SetVolume(m_dsound_buffer, volume) == DS_OK);
}