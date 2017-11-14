#ifndef __DIRECT_SOUND_WRAPPER_H__
#define __DIRECT_SOUND_WRAPPER_H__


#include <Windows.h>
#include <MMSystem.h>
#include <dsound.h>
#include <stdint.h>

class DirectSoundWrapper
{
public:
	DirectSoundWrapper(void);
	~DirectSoundWrapper(void);
	/*
		��ʼ��
		channels:������
		samples:������������1024
		sample_rate:������
		bits_per_sample:sampleλ��
	*/
	bool Initialize(uint8_t channels, uint32_t samples, uint32_t sample_rate, uint8_t bits_per_sample);

	/*
		�ͷ���Դ
	*/
	void CleanUp();
	/*
		��ȡSample��С
		channels * samples * bits_per_sample / 8
	*/
	uint32_t GetSampleSize();

	/*
		���Sample
		data_size = channels * samples * bits_per_sample / 8
	*/
	bool FillSample(uint8_t *pcm_data, uint32_t data_size);

	/*
		�ȴ�Sample���Ž���
	*/
	void WaitDone();

	/*
		��������(�ֱ�)
	*/
	bool SetVolume(uint32_t volume);
private:
	bool DirectSoundCreate();
	void DirectSoundDestroy();

	bool DirectSoundCreatePCMBuffer(uint8_t channels, uint32_t samples, uint32_t sample_rate, uint8_t bits_per_sample, uint16_t buffer_count);
	void DirectSoundDestroyPCMBuffer();
	bool DirectSoundFillBuffer(uint8_t *pcm_data, uint32_t data_size);

	HMODULE					m_dsound_dll;
	LPDIRECTSOUND8			m_dsound_context;
    LPDIRECTSOUNDBUFFER		m_dsound_buffer;

    uint32_t				m_buffer_count;
    uint32_t				m_buffer_length;
    uint32_t				m_last_buffer;
};

#endif

/*
	std::ifstream file;
	file.open("music.pcm", ios::binary);
	DirectSoundWrapper s;
	s.Initialize(2, 1024, 44100, 16);
	uint8_t *audio_buffer = new uint8_t[s.GetSampleSize()];
	while (!file.eof())
	{
		file.read((char *)audio_buffer, s.GetSampleSize());
		s.FillSample(audio_buffer, s.GetSampleSize());
		s.WaitDone();
	}
	delete [] audio_buffer;
	s.CleanUp();
*/