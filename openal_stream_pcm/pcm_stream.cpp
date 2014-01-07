#include "al.h"
#include "alc.h"
#include <stdio.h>
#include <Windows.h>
#include <iostream>
 
using namespace std;

#pragma  comment(lib,"OpenAL32.lib")
ALCcontext  *m_Context=NULL;
ALCdevice  *m_Device=NULL;
ALuint   m_sourceID;
ALuint bufferID=0;


FILE *pcmfile;
long numframe=0;

bool initOpenAL();
void updataQueueBuffer();
void OpenAudio();
void PlaySound();
void PlayStop();
void clearOpenAL();
bool initOpenAL()
{
	if(m_Device==NULL)
	{
		m_Device=alcOpenDevice(NULL);

	}
	else
		return false;
	if(m_Context==NULL)
	{
		if(m_Device)
		{
			m_Context=alcCreateContext(m_Device,NULL);
			alcMakeContextCurrent(m_Context);
		}
	}

	alGenSources(1,&m_sourceID);
	alSourcei(m_sourceID,AL_LOOPING,AL_FALSE);
	alSourcef(m_sourceID,AL_SOURCE_TYPE,AL_STREAMING);
	alSourcef(m_sourceID,AL_GAIN,1.0f);
	alDopplerVelocity(1.0);
	//alSpeedOfSound(2.0);


	return true;

}
bool updataBuffer()
{
	ALint state;
	int processed ,queued;


	


	/*
	if(state!=AL_PLAYING)
	{
		PlaySound();
		return false;
	}
	*/
	alGetSourcei(m_sourceID,AL_BUFFERS_PROCESSED,&processed);
	alGetSourcei(m_sourceID,AL_BUFFERS_QUEUED,&queued);
	alGetSourcei(m_sourceID,AL_SOURCE_STATE,&state);
	if(state==AL_STOPPED||state==AL_PAUSED||state==AL_INITIAL)
	{
		if(queued<processed||queued==0||(queued==1&&processed==1))
		{
			PlayStop();
			clearOpenAL();

		}
		 PlaySound();

	}
	while(processed--)
	{
		ALuint buffer;
		alSourceUnqueueBuffers(m_sourceID,1,&buffer);
		alDeleteBuffers(1,&buffer);

	}
	return true;
}

void openAudioFromQueue(unsigned char* data,long length)
{
	if(data==NULL)
		return;

	

	alGenBuffers(1,&bufferID);
	alBufferData(bufferID,AL_FORMAT_MONO16,data,length,44100);
	alSourceQueueBuffers(m_sourceID,1,&bufferID);
	
	updataBuffer();
	alGenSources(1,&m_sourceID);
	 
	/*
	ALint value;
	alGetSourcei(m_sourceID,AL_SOURCE_STATE,&value);
	if(value!=AL_PLAYING)
	{
		alSourcePlay(m_sourceID);
	}
	int state;
	do
	{
		
		Sleep(1);
		alGetSourcei(m_sourceID,AL_SOURCE_STATE,&state);

	}while(state==AL_PLAYING);
	*/
}
void OpenAudio()
{

	alGenBuffers(1,&bufferID);


	//unsigned char *data=new unsigned char[pLength];
	// int framesize=8802;
	// short int *pBuf=new short int[framesize];

	fseek(pcmfile,0L,SEEK_END);
	long length=ftell(pcmfile); //6618950
	//int len=fread(pBuf,1,framesize,pcmfile);
	unsigned char *outdata=new unsigned char[length];
	//	fseek(pcmfile,0L,SEEK_SET);
	rewind(pcmfile);

	/*
	while(1)
	{
		fread(outdata,1,length,pcmfile);

		alBufferData(bufferID,AL_FORMAT_MONO16,outdata,length,44100);

	}
	*/

	fread(outdata,1,length,pcmfile);

	alBufferData(bufferID,AL_FORMAT_MONO16,outdata,length,44100);

	alGenSources(1,&m_sourceID);

	alSourceQueueBuffers(m_sourceID,1,&bufferID);
	alSourcei(m_sourceID,AL_BUFFER,bufferID);
	//	alSourcei(m_sourceID,AL_LOOPING,AL_TRUE);
	/*
	if(outdata)
	{

	delete outdata[];
	}	
	*/

}
void PlaySound()
{
	ALint state;
	alGetSourcei(m_sourceID,AL_SOURCE_STATE,&state);
	if(state!=AL_PLAYING)
	{
	alSourcePlay(m_sourceID);
	}

	do
	{



    	Sleep(1);
		alGetSourcei(m_sourceID,AL_SOURCE_STATE,&state);

	}while(state==AL_PLAYING);
	/*
	while(1)
	{
		Sleep(1);
		int  number=0;
	alGetSourcei(m_sourceID,AL_BUFFERS_QUEUED,&number);
	cout<<"queued number="<<number<<endl;
	if(number==0)
		break;

	}
	*/

}
void PlayStop()
{
	ALint state;
	alGetSourcei(m_sourceID,AL_SOURCE_STATE,&state);
	if(state!=AL_STOPPED)
	{
		alSourceStop(m_sourceID);
	}
}
void clearOpenAL()
{
	alDeleteSources(1,&m_sourceID);
	if(m_Context!=NULL)
	{
		alcDestroyContext(m_Context);
		m_Context=NULL;

	}
	if(m_Device)
	{
		alcCloseDevice(m_Device);
		m_Device=NULL;
	}

	alDeleteBuffers(1,&m_sourceID);
	alDeleteBuffers(1,&bufferID);
}
int main(int argc ,char* argv[])
{
	pcmfile=fopen("44100_1_16.pcm","rb");
	if(pcmfile==NULL)
	{
		cout<<"open error"<<endl;
	}

	 /*
	initOpenAL();

	OpenAudio();

    PlaySound();
	clearOpenAL();
	 */


	 
	 
 
	initOpenAL();
	//long framesize=6618950;//文件总大小
	long framesize=8802;//一帧数据大小
	while(1)
	{
	
	unsigned char *pcmdata=new unsigned char[framesize+50];
    Sleep(1);
	 
    LONG len=fread(pcmdata,1,framesize,pcmfile);
	if(len<framesize)
	{
		break;
	}

	openAudioFromQueue(pcmdata,framesize);


	 
	numframe++;

	delete []pcmdata;

	}



	//fclose(pcmfile);
	//clearOpenAL();
//	cout<<"numframe="<<numframe<<endl;
	 
	return 0;
}

