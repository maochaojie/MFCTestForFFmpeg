#include <stdio.h>
#include <iostream>
#include <string>
#include <stdio.h>
using namespace std;
#include <MFCTestDlg.h>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "sdl/SDL.h"
#include "sdl/SDL_main.h"
};







class FFmpegPlayer
{
private:
	AVFormatContext	*pFormatCtx;

	AVCodecContext	*pCodecCtx, *audioPCodecCtx;
	AVCodec			*pCodec, *audioPCodec;
	struct SwsContext *img_convert_ctx;
	//输入文件路径

	SDL_Window *screen;
	SDL_Renderer* sdlRenderer;
	SDL_Texture* sdlTexture;



	int frame_cnt;
public:
	int init();
	int openVideo(string filepath);
	int getFormatInfo();
	int getStream();
	int getVideoStreamIndex();
	int getAudioStreamIndex();
	int decodeToYUV(string YUVFileName,int videoIndex);
	int releaseFFmpeg();
	int createWindow(CMFCTestDlg* dlg, int id);
	int sdlRender();
	int createSdlTexture();
	int decodeToscreen(int videoIndex,int* zanting);
	int decodeAudioToDevice(int audioIndex, int* zanting);
	int releaseSdl();
	int getPictureSize(int videoIndex);
	int decodeVideoToscreen(int videoIndex, int audioIndex, int* zanting);
};

