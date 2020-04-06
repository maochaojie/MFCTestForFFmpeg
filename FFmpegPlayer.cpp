#include "pch.h"
#include "FFmpegPlayer.h"
#define MAX_AUDIO_FRAME_SIZE 192000

int FFmpegPlayer::init() {

	av_register_all();
	avformat_network_init();//��ʼ����������
	pFormatCtx = avformat_alloc_context();//AVFormatContext�����ʼ��ΪNULL������avformat_alloc_context()���г�ʼ��
	
	if (SDL_Init(SDL_INIT_VIDEO)) {
		return -1;
	}

	return 0;


}

static Uint8  *audio_chunk;
static Uint32  audio_len;
static Uint8  *audio_pos;
void  fill_audio(void *udata, Uint8 *stream, int len) {
	SDL_memset(stream, 0, len);
	/*  Only  play  if  we  have  data  left  */
	if (audio_len == 0)
		return;
	/*  Mix  as  much  data  as  possible  */
	len = (len > audio_len ? audio_len : len);
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
	audio_pos += len;
	audio_len -= len;
}



int FFmpegPlayer::openVideo(string filepath) {


	if (avformat_open_input(&pFormatCtx, filepath.c_str(), NULL, NULL) != 0) {
	
		return -1;
	}

	return 0;
}






int FFmpegPlayer::getFormatInfo() {


	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
	
		return -1;
	}

	return 0;

}




int FFmpegPlayer::getStream() {

	return pFormatCtx->nb_streams;


}





int  FFmpegPlayer::getVideoStreamIndex() {

	int videoindex = -1;
	for (int i = 0; i < pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoindex = i;
			break;
		}
	if (videoindex == -1) {
		printf("Didn't find a video stream.\n");
		return -1;
	}

	return videoindex;



}



/*
int  FFmpegPlayer::decodeToYUV(string YUVFileName, int videoIndex) {

	pCodecCtx = pFormatCtx->streams[videoIndex]->codec;

	pCodec = avcodec_find_decoder(pFormatCtx->streams[videoIndex]->codec->codec_id);
	if (pCodec == NULL) {
		printf("Codec not found.\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		printf("Could not open codec.\n");
		return -1;
	}


	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();
	out_buffer = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));//����ռ������ͼƬ���ݡ�����Դ���ݺ�Ŀ������
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

	packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	FILE *fp_yuv = fopen(YUVFileName.c_str(), "wb+");


	frame_cnt = 0;
	while (av_read_frame(pFormatCtx, packet) >= 0) {
		if (packet->stream_index == videoIndex) {
ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
if (ret < 0) {

	return -1;
}
if (got_picture) {
	sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
		pFrameYUV->data, pFrameYUV->linesize);



	fwrite(pFrameYUV->data[0], 1, pCodecCtx->width*pCodecCtx->height, fp_yuv);
	fwrite(pFrameYUV->data[1], 1, pCodecCtx->width*pCodecCtx->height / 4, fp_yuv);
	fwrite(pFrameYUV->data[2], 1, pCodecCtx->width*pCodecCtx->height / 4, fp_yuv);

	frame_cnt++;

}
		}
		av_free_packet(packet);
	}

	fclose(fp_yuv);
	return 0;

}

*/



  

int  FFmpegPlayer::releaseFFmpeg() {

	sws_freeContext(img_convert_ctx);
	avcodec_close(pCodecCtx);
	avcodec_close(audioPCodecCtx);
	avformat_close_input(&pFormatCtx);
	return 0;

}







int  FFmpegPlayer::createWindow(CMFCTestDlg* dlg, int id) {


	//screen = SDL_CreateWindow("Simplest Video Play SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	//	pCodecCtx->width, pCodecCtx->height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	//if (!screen) {
	//	printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
	//	return -1;
	//}

	screen = SDL_CreateWindowFrom(dlg->GetDlgItem(id)->GetSafeHwnd());


	return 0;
}





int  FFmpegPlayer::sdlRender() {


	sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
	return 0;
}






int  FFmpegPlayer::createSdlTexture() {

	Uint32 pixformat = 0;
	//IYUV: Y + U + V  (3 planes)
	//YV12: Y + V + U  (3 planes)
	pixformat = SDL_PIXELFORMAT_IYUV;

    sdlTexture = SDL_CreateTexture(sdlRenderer, pixformat, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);


	return 0;



}


int  FFmpegPlayer::decodeVideoToscreen(int videoIndex, int audioIndex, int* zanting) {

	pCodec = avcodec_find_decoder(pFormatCtx->streams[videoIndex]->codec->codec_id);
	if (pCodec == NULL) {
		printf("Codec not found.\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		printf("Could not open codec.\n");
		return -1;
	}

	AVFrame	* pFrame = av_frame_alloc();
	AVFrame	* pFrameYUV = av_frame_alloc();
	uint8_t *out_buffer;
	out_buffer = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));//����ռ������ͼƬ���ݡ�����Դ���ݺ�Ŀ������  
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

	AVPacket * packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);


	SDL_Rect sdlRect;

	frame_cnt = 0;
	int y_size;
	int ret, got_picture;
	
	//audio

	audioPCodecCtx = pFormatCtx->streams[audioIndex]->codec;


	// Find the decoder for the audio stream
	audioPCodec = avcodec_find_decoder(audioPCodecCtx->codec_id);
	if (audioPCodec == NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}

	// Open codec
	if (avcodec_open2(audioPCodecCtx, audioPCodec, NULL) < 0)
	{
		printf("Could not open codec.\n");
		return -1;
	}

	uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
	int out_nb_samples = 1152;
	AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
	int out_sample_rate = audioPCodecCtx->sample_rate;
	int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
	int iBufSize;
	iBufSize = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);
	uint8_t *uBuff;
	uBuff = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);

	SDL_AudioSpec pOutSpec;
	pOutSpec.freq = audioPCodecCtx->sample_rate;
	//pOutSpec.freq = 48000;
	pOutSpec.format = AUDIO_S16SYS;
	pOutSpec.channels = out_channels;
	pOutSpec.silence = 0;
	pOutSpec.samples = out_nb_samples;
	pOutSpec.callback = fill_audio;
	pOutSpec.userdata = audioPCodecCtx;

	SwrContext *pSwCtx;
	pSwCtx = swr_alloc();
	pSwCtx = swr_alloc_set_opts(pSwCtx, out_channel_layout, out_sample_fmt, out_sample_rate, audioPCodecCtx->channel_layout, audioPCodecCtx->sample_fmt, audioPCodecCtx->sample_rate, 0, NULL);
	swr_init(pSwCtx);


	if (SDL_OpenAudio(&pOutSpec, NULL) < 0)//���裨2������Ƶ�豸 
	{
		printf("can't open audio.\n");
		return 0;
	}

	while (true)
	{
		if (*zanting == 1)
		{
			continue;
		}

		if (av_read_frame(pFormatCtx, packet) < 0) {
			break;
		}
		if (packet->stream_index == videoIndex) {
			/*
			 * �ڴ˴�������H264�����Ĵ���
			 * ȡ����packet��ʹ��fwrite()
			 */


			//ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);

			//if (got_picture) {
			//	sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
			//		pFrameYUV->data, pFrameYUV->linesize);
			//	SDL_UpdateTexture(sdlTexture, NULL, pFrameYUV->data[0], pCodecCtx->width);
			//	sdlRect.x = 0;
			//	sdlRect.y = 0;
			//	sdlRect.w = pCodecCtx->width;
			//	sdlRect.h = pCodecCtx->height;

			//	SDL_RenderClear(sdlRenderer);
			//	SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
			//	SDL_RenderPresent(sdlRenderer);
			//	//Delay 40ms
			//	SDL_Delay(1);

			//	frame_cnt++;

			//}
		}
		else if (packet->stream_index == audioIndex) {
			ret = avcodec_decode_audio4(audioPCodecCtx, pFrame, &got_picture, packet);
			if (ret < 0)
			{
				printf("Error in decoding audio frame.\n");
				exit(0);
			}
			if (got_picture)
			{
				int data_size;
				data_size = av_samples_get_buffer_size(nullptr, audioPCodecCtx->channels, pFrame->nb_samples, audioPCodecCtx->sample_fmt, 1);

				memcpy(uBuff, pFrame->data[0], data_size);


				int dst_nb_samples = av_rescale_rnd(swr_get_delay(pSwCtx, pFrame->sample_rate) + pFrame->nb_samples,
					pOutSpec.freq, pOutSpec.format, AVRounding(1));

				swr_convert(pSwCtx, &uBuff, MAX_AUDIO_FRAME_SIZE, (const uint8_t **)pFrame->data, pFrame->nb_samples);
			}
			//---------------------------------------
			audio_chunk = (Uint8 *)uBuff;
			//������Ƶ���ݳ���
			audio_len = iBufSize;
			//audio_len = 4096;
			//����mp3��ʱ���Ϊaudio_len = 4096
			//���Ƚ���������������������MP3һ֡����4608
			//ʹ��һ�λص�������4096�ֽڻ��壩���Ų��꣬���Ի�Ҫʹ��һ�λص����������²��Ż���������
			//���ó�ʼ����λ��
			audio_pos = audio_chunk;
			//�ط���Ƶ���� 
			SDL_PauseAudio(0);
			//printf("don't close, audio playing...\n"); 
			while (audio_len > 0)
				SDL_Delay(1);
		}
		av_free_packet(packet);

	}
	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	SDL_CloseAudio();//�ر���Ƶ�豸 
	return 0;
}



int  FFmpegPlayer::decodeToscreen(int videoIndex,int* zanting) {

	pCodec = avcodec_find_decoder(pFormatCtx->streams[videoIndex]->codec->codec_id);
	if (pCodec == NULL) {
		printf("Codec not found.\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		printf("Could not open codec.\n");
		return -1;
	}

	AVFrame* pFrame = av_frame_alloc();
	AVFrame* pFrameYUV = av_frame_alloc();
	uint8_t *out_buffer;
	out_buffer = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));//����ռ������ͼƬ���ݡ�����Դ���ݺ�Ŀ������  
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

	AVPacket* packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	



	SDL_Rect sdlRect;

	frame_cnt = 0;
	int y_size;
	int ret, got_picture;

	while (true)
	{
		if (*zanting == 1)
		{
			continue;
		}

		if (av_read_frame(pFormatCtx, packet) < 0) {
			break;
		}
		if (packet->stream_index == videoIndex) {
			/*
			 * �ڴ˴�������H264�����Ĵ���
			 * ȡ����packet��ʹ��fwrite()
			 */

			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);

			if (got_picture) {
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
					pFrameYUV->data, pFrameYUV->linesize);
				SDL_UpdateTexture(sdlTexture, NULL, pFrameYUV->data[0], pCodecCtx->width);
				sdlRect.x = 0;
				sdlRect.y = 0;
				sdlRect.w = pCodecCtx->width;
				sdlRect.h = pCodecCtx->height;
				SDL_RenderClear(sdlRenderer);
				SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
				SDL_RenderPresent(sdlRenderer);
				//Delay 40ms
				SDL_Delay(40);
				frame_cnt++;
			}
		}
		av_free_packet(packet);

	}
	av_frame_free(&pFrame);
	return 0;


}

int FFmpegPlayer::getAudioStreamIndex() {
	int audioStream = -1;
	for (int i = 0; i < pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audioStream = i;
			break;
		}

	if (audioStream == -1)
	{
		printf("Didn't find a audio stream.\n");
		return -1;
	}

	return audioStream;
}

int FFmpegPlayer::decodeAudioToDevice(int audioIndex, int* zanting) {
	//audio
	int ret, got_picture;
	AVFrame* pFrame = av_frame_alloc();
	AVPacket* packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	audioPCodecCtx = pFormatCtx->streams[audioIndex]->codec;


	// Find the decoder for the audio stream
	audioPCodec = avcodec_find_decoder(audioPCodecCtx->codec_id);
	if (audioPCodec == NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}

	// Open codec
	if (avcodec_open2(audioPCodecCtx, audioPCodec, NULL) < 0)
	{
		printf("Could not open codec.\n");
		return -1;
	}

	uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
	int out_nb_samples = 1152;
	AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
	int out_sample_rate = audioPCodecCtx->sample_rate;
	int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
	int iBufSize;
	iBufSize = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);
	uint8_t *uBuff;
	uBuff = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);

	SDL_AudioSpec pOutSpec;
	pOutSpec.freq = audioPCodecCtx->sample_rate;
	//pOutSpec.freq = 48000;
	pOutSpec.format = AUDIO_S16SYS;
	pOutSpec.channels = out_channels;
	pOutSpec.silence = 0;
	pOutSpec.samples = out_nb_samples;
	pOutSpec.callback = fill_audio;
	pOutSpec.userdata = audioPCodecCtx;

	SwrContext *pSwCtx;
	pSwCtx = swr_alloc();
	pSwCtx = swr_alloc_set_opts(pSwCtx, out_channel_layout, out_sample_fmt, out_sample_rate, audioPCodecCtx->channel_layout, audioPCodecCtx->sample_fmt, audioPCodecCtx->sample_rate, 0, NULL);
	swr_init(pSwCtx);


	//  
	//
	if (SDL_OpenAudio(&pOutSpec, NULL) < 0)//���裨2������Ƶ�豸 
	{
		printf("can't open audio.\n");
		return 0;
	}

	while (true)
	{
		if (*zanting == 1) {
			continue;
		}
		if (av_read_frame(pFormatCtx, packet) < 0) {
			break;
		}
		if (packet->stream_index == audioIndex)
		{
			ret = avcodec_decode_audio4(audioPCodecCtx, pFrame, &got_picture, packet);
			if (ret < 0)
			{
				printf("Error in decoding audio frame.\n");
				exit(0);
			}
			if (got_picture)
			{
				int data_size;
				data_size = av_samples_get_buffer_size(nullptr, audioPCodecCtx->channels, pFrame->nb_samples, audioPCodecCtx->sample_fmt, 1);

				memcpy(uBuff, pFrame->data[0], data_size);


				int dst_nb_samples = av_rescale_rnd(swr_get_delay(pSwCtx, pFrame->sample_rate) + pFrame->nb_samples,
					pOutSpec.freq, pOutSpec.format, AVRounding(1));

				swr_convert(pSwCtx, &uBuff, MAX_AUDIO_FRAME_SIZE, (const uint8_t **)pFrame->data, pFrame->nb_samples);
			}
			//---------------------------------------
			audio_chunk = (Uint8 *)uBuff;
			//������Ƶ���ݳ���
			audio_len = iBufSize;
			//audio_len = 4096;
			//����mp3��ʱ���Ϊaudio_len = 4096
			//���Ƚ���������������������MP3һ֡����4608
			//ʹ��һ�λص�������4096�ֽڻ��壩���Ų��꣬���Ի�Ҫʹ��һ�λص����������²��Ż���������
			//���ó�ʼ����λ��
			audio_pos = audio_chunk;
			//�ط���Ƶ���� 
			SDL_PauseAudio(0);
			//printf("don't close, audio playing...\n"); 
			while (audio_len > 0)
				SDL_Delay(1);
		}
		av_free_packet(packet);
	}
	av_frame_free(&pFrame);
	SDL_CloseAudio();//�ر���Ƶ�豸 
	return 0;
}

int  FFmpegPlayer::releaseSdl() {
	
	SDL_Quit();

	return 0;

}


int FFmpegPlayer::getPictureSize(int videoIndex) {

	pCodecCtx = pFormatCtx->streams[videoIndex]->codec;

	return 0;

}