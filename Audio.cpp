#include <highgui.h>
#include <cv.h>

extern "C"
{
#include <SDL.h>
#include <SDL_thread.h>
#include <avcodec.h>
#include <avformat.h>
}

#include <iostream>
#include <stdio.h>
//#include <malloc.h>

using namespace cv;    

#define SDL_AUDIO_BUFFER_SIZE 1024

typedef struct PacketQueue 
{
  AVPacketList *first_pkt, *last_pkt;
  int nb_packets;
  int size;
  SDL_mutex *mutex;
  SDL_cond *cond;
} PacketQueue;
PacketQueue audioq;

int audioStream = -1;
int videoStream = -1;
int quit = 0;

SDL_Surface* screen = NULL; 
SDL_Surface* surface = NULL;

AVFormatContext* pFormatCtx = NULL;
AVCodecContext* aCodecCtx = NULL;
AVCodecContext* pCodecCtx = NULL;  

void show_frame(IplImage* img)
{
    if (!screen) 
    {
        screen = SDL_SetVideoMode(img->width, img->height, 0, 0);
        if (!screen) 
        {
            fprintf(stderr, "SDL: could not set video mode - exiting\n");
            exit(1);
        }
    }

    // Assuming IplImage packed as BGR 24bits
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*)img->imageData,
                img->width,
                img->height,
                img->depth * img->nChannels,
                img->widthStep,
                0xff0000, 0x00ff00, 0x0000ff, 0
                );

    SDL_BlitSurface(surface, 0, screen, 0);

    SDL_Flip(screen);
}

void packet_queue_init(PacketQueue *q) 
{
    memset(q, 0, sizeof(PacketQueue));
    q->mutex = SDL_CreateMutex();
    q->cond = SDL_CreateCond();
}

int packet_queue_put(PacketQueue *q, AVPacket *pkt) 
{
    AVPacketList *pkt1;
    if (av_dup_packet(pkt) < 0) 
    {
        return -1;
    }

    //pkt1 = (AVPacketList*) av_malloc(sizeof(AVPacketList));
    pkt1 = (AVPacketList*) malloc(sizeof(AVPacketList));
    if (!pkt1) return -1;
    pkt1->pkt = *pkt;
    pkt1->next = NULL;

    SDL_LockMutex(q->mutex);

    if (!q->last_pkt)
        q->first_pkt = pkt1;
    else
        q->last_pkt->next = pkt1;

    q->last_pkt = pkt1;
    q->nb_packets++;
    q->size += pkt1->pkt.size;
    SDL_CondSignal(q->cond);

    SDL_UnlockMutex(q->mutex);
    return 0;
}

static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block)
{
    AVPacketList *pkt1;
    int ret;

    SDL_LockMutex(q->mutex);

    for (;;) 
    {      
        if( quit) 
        {
            ret = -1;
            break;
        }

        pkt1 = q->first_pkt;
        if (pkt1) 
        {
            q->first_pkt = pkt1->next;
            if (!q->first_pkt)
                q->last_pkt = NULL;

                q->nb_packets--;
            q->size -= pkt1->pkt.size;
            *pkt = pkt1->pkt;
            //av_free(pkt1);
            free(pkt1);
            ret = 1;
            break;
        } 
        else if (!block) 
        {
            ret = 0;
            break;
        } 
        else 
        {
            SDL_CondWait(q->cond, q->mutex);
        }
    }

    SDL_UnlockMutex(q->mutex);
    return ret;
}

int audio_decode_frame(AVCodecContext *aCodecCtx, uint8_t *audio_buf, int buf_size) 
{
    static AVPacket pkt;
    static uint8_t *audio_pkt_data = NULL;
    static int audio_pkt_size = 0;

    int len1, data_size;

    for (;;) 
    {
        while (audio_pkt_size > 0) 
        {
            data_size = buf_size;
            len1 = avcodec_decode_audio2(aCodecCtx, (int16_t*)audio_buf, &data_size, 
                                         audio_pkt_data, audio_pkt_size);
            if (len1 < 0) 
            {
                /* if error, skip frame */
                audio_pkt_size = 0;
                break;
            }
            audio_pkt_data += len1;
            audio_pkt_size -= len1;
            if (data_size <= 0) 
            {
                /* No data yet, get more frames */
                continue;
            }
            /* We have data, return it and come back for more later */
            return data_size;
        }

            if (pkt.data)
            av_free_packet(&pkt);

        if (quit) return -1;

        if (packet_queue_get(&audioq, &pkt, 1) < 0) return -1;

        audio_pkt_data = pkt.data;
        audio_pkt_size = pkt.size;
    }
}

void audio_callback(void *userdata, Uint8 *stream, int len) 
{
    AVCodecContext *aCodecCtx = (AVCodecContext *)userdata;
    int len1, audio_size;

    static uint8_t audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
    static unsigned int audio_buf_size = 0;
    static unsigned int audio_buf_index = 0;

    while (len > 0) 
    {
        if (audio_buf_index >= audio_buf_size) 
        {
            /* We have already sent all our data; get more */
            audio_size = audio_decode_frame(aCodecCtx, audio_buf, sizeof(audio_buf));
            if(audio_size < 0) 
            {
                /* If error, output silence */
                audio_buf_size = 1024; // arbitrary?
                memset(audio_buf, 0, audio_buf_size);
            } 
            else 
            {
                audio_buf_size = audio_size;
            }
            audio_buf_index = 0;
        }

        len1 = audio_buf_size - audio_buf_index;
        if (len1 > len)
            len1 = len;

            memcpy(stream, (uint8_t *)audio_buf + audio_buf_index, len1);
        len -= len1;
        stream += len1;
        audio_buf_index += len1;
    }
}

void setup_ffmpeg(char* filename)
{
    if (av_open_input_file(&pFormatCtx, filename, NULL, 0, NULL) != 0)
    {
        fprintf(stderr, "FFmpeg failed to open file %s!\n", filename);
        exit(-1); 
    }

    if (av_find_stream_info(pFormatCtx) < 0)
    {
        fprintf(stderr, "FFmpeg failed to retrieve stream info!\n");
        exit(-1); 
    }

    // Dump information about file onto standard error
    dump_format(pFormatCtx, 0, filename, 0);

    // Find the first video stream
    int i = 0;
    for (i; i < pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO && videoStream < 0) 
        {
            videoStream = i;
        }

        if (pFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO && audioStream < 0) 
        {
            audioStream = i;
        }
    }

    if (videoStream == -1)
    {
        fprintf(stderr, "No video stream found in %s!\n", filename);
        exit(-1); 
    }  

    if (audioStream == -1)
    {
        fprintf(stderr, "No audio stream found in %s!\n", filename);
        exit(-1); 
    }  

    // Get a pointer to the codec context for the audio stream
    aCodecCtx = pFormatCtx->streams[audioStream]->codec;

    // Set audio settings from codec info
    SDL_AudioSpec wanted_spec;
    wanted_spec.freq = aCodecCtx->sample_rate;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = aCodecCtx->channels;
    wanted_spec.silence = 0;
    wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
    wanted_spec.callback = audio_callback;
    wanted_spec.userdata = aCodecCtx;

    SDL_AudioSpec spec;
    if (SDL_OpenAudio(&wanted_spec, &spec) < 0) 
    {
        fprintf(stderr, "SDL_OpenAudio: %s\n", SDL_GetError());
        exit(-1);
    }

    AVCodec* aCodec = avcodec_find_decoder(aCodecCtx->codec_id);
    if (!aCodec) 
    {
        fprintf(stderr, "Unsupported codec!\n");
        exit(-1);
    }
    avcodec_open(aCodecCtx, aCodec);

    // audio_st = pFormatCtx->streams[index]
    packet_queue_init(&audioq);
    SDL_PauseAudio(0); 

    // Get a pointer to the codec context for the video stream
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;

    // Find the decoder for the video stream
    AVCodec* pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) 
    {
        fprintf(stderr, "Unsupported codec!\n");
        exit(-1); // Codec not found
    }

    // Open codec
    if (avcodec_open(pCodecCtx, pCodec) < 0)
    {
        fprintf(stderr, "Unsupported codec!\n");
        exit(-1); // Could not open codec
    }
}


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <video>" << std::endl;
        return -1;
    }

    av_register_all();

    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) 
    {
        fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
        return -1;
    }

    // Init ffmpeg and setup some SDL stuff related to Audio
    setup_ffmpeg(argv[1]);

    VideoCapture cap(argv[1]); // open the default camera
    if (!cap.isOpened())  // check if we succeeded
    {
        std::cout << "Failed to load file!" << std::endl;
            return -1;
    }

    AVPacket packet;
    while (av_read_frame(pFormatCtx, &packet) >= 0) 
    {
        if (packet.stream_index == videoStream) 
        {
              // Actually this is were SYNC between audio/video would happen.
              // Right now I assume that every VIDEO packet contains an entire video frame, and that's not true. A video frame can be made by multiple packets!
              // But for the time being, assume 1 video frame == 1 video packet,
              // so instead of reading the frame through ffmpeg, I read it through OpenCV. 

              Mat frame;
              cap >> frame; // get a new frame from camera

          // do some processing on the frame, either as a Mat or as IplImage.
              // For educational purposes, applying a lame grayscale conversion
          IplImage ipl_frame = frame;
          for (int i = 0; i < ipl_frame.width * ipl_frame.height * ipl_frame.nChannels; i += ipl_frame.nChannels)
          {
                ipl_frame.imageData[i] = (ipl_frame.imageData[i] + ipl_frame.imageData[i+1] + ipl_frame.imageData[i+2])/3;   //B
                ipl_frame.imageData[i+1] = (ipl_frame.imageData[i] + ipl_frame.imageData[i+1] + ipl_frame.imageData[i+2])/3; //G
                ipl_frame.imageData[i+2] = (ipl_frame.imageData[i] + ipl_frame.imageData[i+1] + ipl_frame.imageData[i+2])/3; //R
            }

            // Display it on SDL window
            show_frame(&ipl_frame); 

            av_free_packet(&packet);
        }   
        else if (packet.stream_index == audioStream) 
        {
            packet_queue_put(&audioq, &packet);
        } 
        else 
        {
            av_free_packet(&packet);
        }

        SDL_Event event;
        SDL_PollEvent(&event);
        switch (event.type) 
        {
            case SDL_QUIT:
                SDL_FreeSurface(surface);
                SDL_Quit();
                break;

            default:
                break;  
        }
    }
    // the camera will be deinitialized automatically in VideoCapture destructor

    // Close the codec
    avcodec_close(pCodecCtx);

    // Close the video file
    av_close_input_file(pFormatCtx);

    return 0;
}