// Created on: 2008-06-30
// Created by: Alexander GRIGORIEV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#if (defined(_WIN32) || defined(__WIN32__)) && defined(HAVE_VIDEOCAPTURE)
  #include <windows.h>
  #include <OpenGl_AVIWriter.hxx>
#endif

#include <ViewerTest.hxx>

#include <Aspect_Window.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressSentry.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_DataMap.hxx>
#include <Image_PixMap.hxx>
#include <OSD_Timer.hxx>
#include <Precision.hxx>
#include <V3d_View.hxx>
#include <TCollection_AsciiString.hxx>

static Standard_Integer avi_record(Draw_Interpretor& /*di*/,
                                   Standard_Integer argc, const char** argv)
{
  if (argc < 2)
  {
    cout << "Syntax: " << argv[0] << " file | start | stop | save" << endl;
    return 1;
  }

  Standard_Integer aResult = 1;
#if (defined(_WIN32) || defined(__WIN32__))
  #ifdef HAVE_VIDEOCAPTURE
    Handle(V3d_View) aView = ViewerTest::CurrentView ();
    if (aView.IsNull())
    {
      std::cout << "Call vinit before!\n";
      return 1;
    }

    static OpenGl_AVIWriter * pAviWriter = 0L;

    if (strncmp(argv[1], "file", 5) == 0) {
      if (argc < 3) {
        cout << "Please define the name of AVI file to create ..." << endl;
      } else {
        const char * aFilename = argv[2];
        DWORD aFormat = mmioFOURCC('X','V','I','D');
        if (argc > 3) {
          const char * aFourcc = argv[3];
          aFormat = mmioFOURCC(aFourcc[0], aFourcc[1], aFourcc[2], aFourcc[3]);
        }
        pAviWriter = new OpenGl_AVIWriter (aFilename, aFormat, 25);
        aResult = 0;
      }
    } else if (pAviWriter == 0L) {
      cout << "AVI Writer instance has not been initialized. Use command "
           << argv[0] << " file ..." << endl;
    } else if (strncmp(argv[1], "start", 6) == 0) {
      pAviWriter->StartRecording (aView->Window()->NativeHandle());
      aResult = 0;
    } else if (strncmp(argv[1], "stop", 5) == 0) {
      pAviWriter->StopRecording();
      aResult = 0;
    } else if (strncmp(argv[1], "save", 5) == 0) {
      pAviWriter->StopRecording();
      delete pAviWriter;
      pAviWriter = 0L;
      aResult = 0;
    } else if (strncmp(argv[1], "status", 7) == 0) {
      cout << pAviWriter->GetLastErrorMessage() << endl;
      aResult = 0;
    }
  #else
    cout << "AVI writer capability was disabled\n";
  #endif
#else
  cout << "AVI writer is implemented only in Windows version\n";
#endif
  return aResult;
}

extern "C"
{
#ifdef _MSC_VER

  #pragma comment(lib, "avutil.lib")
  #pragma comment(lib, "avcodec.lib")
  #pragma comment(lib, "avformat.lib")
  #pragma comment(lib, "swscale.lib")

  // suppress some common warnings in FFmpeg headers
  #pragma warning(disable : 4244)
#endif

  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libswscale/swscale.h>

#ifdef _MSC_VER
  #pragma warning(default : 4244)
#endif
};

//! Video recording tool.
class ViewerTest_VideoRecorder : public Standard_Transient
{

public:

  //! Empty constructor.
  ViewerTest_VideoRecorder()
  : myAVContext   (NULL),
    myVideoStream (NULL),
    myVideoCodec  (NULL),
    myFrame       (NULL),
    myScaleCtx    (NULL),
    myPixFmtOut   (AV_PIX_FMT_YUV420P),
    myPixFmtSrc   (AV_PIX_FMT_RGBA), // AV_PIX_FMT_BGR0
    myFrameCount  (0)
  {
    myFrameRate.num = 1;
    myFrameRate.den = 30;

    // initialize libavcodec, and register all codecs and formats, should be done once
    av_register_all();
  }

  //! Destructor.
  ~ViewerTest_VideoRecorder()
  {
    Close();
  }

  //! Setup playback FPS. Should be set before Open().
  void SetFramerate (const Standard_Integer theNumerator,
                     const Standard_Integer theDenominator)
  {
    myFrameRate.num = theNumerator;
    myFrameRate.den = theDenominator;
  }

  //! Setup playback FPS. Should be set before Open().
  //! For fixed-fps content, timebase should be 1/framerate and timestamp increments should be identical to 1.
  void SetFramerate (const Standard_Integer theValue)
  {
    myFrameRate.num = int(theValue);
    myFrameRate.den = 1;
  }

  //! Close the stream - stop recorder.
  void Close()
  {
    if (myScaleCtx != NULL)
    {
      sws_freeContext (myScaleCtx);
    }

    if (myAVContext == NULL)
    {
      return;
    }

    // Write the trailer, if any. The trailer must be written before you close the CodecContexts open when you wrote the header;
    // otherwise av_write_trailer() may try to use memory that was freed on av_codec_close().
    av_write_trailer (myAVContext);

    // close each codec
    if (myVideoStream != NULL)
    {
      avcodec_close (myVideoStream->codec);
      myVideoStream = NULL;
    }
    if (myFrame != NULL)
    {
      av_free (myPicDst.data[0]);
      av_frame_free (&myFrame);
      myFrame = NULL;
    }

    if (!(myAVContext->oformat->flags & AVFMT_NOFILE))
    {
      // close the output file
      avio_close (myAVContext->pb);
    }

    // free the stream
    avformat_free_context (myAVContext);
    myAVContext = NULL;
  }

  //! Open output stream - start recorder.
  Standard_Boolean Open (const char*            theFileName,
                         const Standard_Integer theWidth,
                         const Standard_Integer theHeight)
  {
    Close();
    myFrameCount = 0;
    if (theWidth <= 0
     || theHeight <= 0)
    {
      return Standard_False;
    }

    // allocate the output media context
    avformat_alloc_output_context2 (&myAVContext, NULL, NULL, theFileName);
    if (myAVContext == NULL)
    {
      ::Message::DefaultMessenger()->Send ("ViewerTest_VideoRecorder, could not deduce output format from file extension - using MPEG.", Message_Warning);
      avformat_alloc_output_context2 (&myAVContext, NULL, "mpeg", theFileName);
    }
    if (myAVContext == NULL)
    {
      return Standard_False;
    }

    // add the audio stream using the default format codecs and initialize the codecs
    if (!addVideoStream (myAVContext->oformat->video_codec, theWidth, theHeight))
    {
      Close();
      return Standard_False;
    }

    // open video codec and allocate the necessary encode buffers
    if (!openVideoCodec())
    {
      Close();
      return Standard_False;
    }

av_dump_format (myAVContext, 0, theFileName, 1); /// debug

    // open the output file, if needed
    if ((myAVContext->oformat->flags & AVFMT_NOFILE) == 0)
    {
      const int aResAv = avio_open (&myAVContext->pb, theFileName, AVIO_FLAG_WRITE);
      if (aResAv < 0)
      {
        const TCollection_AsciiString aMsg = TCollection_AsciiString ("Error: could not open '") + theFileName + "', " + formatAvError (aResAv);
        ::Message::DefaultMessenger()->Send (aMsg, Message_Fail);
        Close();
        return Standard_False;
      }
    }

    // write the stream header, if any
    const int aResAv = avformat_write_header (myAVContext, NULL);
    if (aResAv < 0)
    {
      const TCollection_AsciiString aMsg = TCollection_AsciiString ("Error: can not open output file '") + theFileName + "', " + formatAvError (aResAv);
      ::Message::DefaultMessenger()->Send (aMsg, Message_Fail);
      Close();
      return Standard_False;
    }
    return Standard_True;
  }

  //! Access RGBA frame, should NOT be re-initialized outside.
  Image_PixMap& ChangeFrame()
  {
    return myImgSrcRgba;
  }

  //! Push new frame, should be called after Open().
  Standard_Boolean PushFrame()
  {
    return writeVideoFrame (Standard_False);
  }

protected:

  //! Wrapper for av_strerror().
  TCollection_AsciiString formatAvError (const int theError) const
  {
    char anErrBuf[AV_ERROR_MAX_STRING_SIZE] = {};
    av_strerror (theError, anErrBuf, AV_ERROR_MAX_STRING_SIZE);
    return anErrBuf;
  }

  //! Append video stream.
  Standard_Boolean addVideoStream (const AVCodecID        theCodecId,
                                   const Standard_Integer theWidth,
                                   const Standard_Integer theHeight)
  {
    // find the encoder
    myVideoCodec = avcodec_find_encoder (theCodecId);
    if (myVideoCodec == NULL)
    {
      const TCollection_AsciiString aMsg = TCollection_AsciiString ("Error: can not find encoder for ") + avcodec_get_name (theCodecId);
      ::Message::DefaultMessenger()->Send (aMsg, Message_Fail);
      return Standard_False;
    }

    myVideoStream = avformat_new_stream (myAVContext, myVideoCodec);
    if (myVideoStream == NULL)
    {
      const TCollection_AsciiString aMsg = TCollection_AsciiString ("Error: can not allocate stream!");
      ::Message::DefaultMessenger()->Send (aMsg, Message_Fail);
      return Standard_False;
    }
    myVideoStream->id = myAVContext->nb_streams - 1;

    AVCodecContext* aCodecCtx = myVideoStream->codec;
    aCodecCtx->codec_id = theCodecId;
    // resolution must be a multiple of two
    aCodecCtx->width    = theWidth;
    aCodecCtx->height   = theHeight;
    // Timebase is the fundamental unit of time (in seconds) in terms of which frame timestamps are represented.
    // For fixed-fps content, timebase should be 1/framerate and timestamp increments should be identical to 1.
    aCodecCtx->time_base.den = myFrameRate.num;
    aCodecCtx->time_base.num = myFrameRate.den;
    aCodecCtx->gop_size      = 12; // emit one intra frame every twelve frames at most
    aCodecCtx->pix_fmt       = myPixFmtOut;

    // some formats want stream headers to be separate
    if (myAVContext->oformat->flags & AVFMT_GLOBALHEADER)
    {
      aCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }
    return Standard_True;
  }

  //! Open video codec.
  Standard_Boolean openVideoCodec()
  {
    // open the codec
    AVCodecContext* aCodecCtx = myVideoStream->codec;

    AVDictionary* anOptions = NULL;
    //av_dict_set (&anOptions, "threads", "auto", 0);
    switch (aCodecCtx->codec_id)
    {
      case AV_CODEC_ID_MPEG2VIDEO:
      {
        // just for testing, we also add B frames
        aCodecCtx->max_b_frames = 2;
        aCodecCtx->bit_rate = 6000000;
        break;
      }
      case AV_CODEC_ID_H264:
      {
        // use CRF (Constant Rate Factor) as best single-pass compressing method
        av_dict_set (&anOptions, "crf",     "20",        0); // quality 18-28, 23 is default (normal), 18 is almost lossless
        av_dict_set (&anOptions, "preset",  "slow",      0); // good compression (see also "veryslow", "ultrafast")

        // live-capturing
        //av_dict_set (&anOptions, "qp",     "0",         0); // instead of crf
        //av_dict_set (&anOptions, "preset", "ultrafast", 0);

        // compatibility with devices
        //av_dict_set (&anOptions, "profile", "baseline",  0);
        //av_dict_set (&anOptions, "level",   "3.0",       0);
        break;
      }
      /*case AV_CODEC_ID_VP8:
      case AV_CODEC_ID_VP9:
      {
        av_dict_set (&anOptions, "crf",     "20",        0); // quality 4–63, 10 is normal
        break;
      }*/
    }

    int aResAv = avcodec_open2 (aCodecCtx, myVideoCodec, &anOptions);
    if (anOptions != NULL)
    {
      av_dict_free (&anOptions);
    }
    if (aResAv < 0)
    {
      const TCollection_AsciiString aMsg = TCollection_AsciiString ("Error: can not open video codec, ") + formatAvError (aResAv);
      ::Message::DefaultMessenger()->Send (aMsg, Message_Fail);
      return Standard_False;
    }

    // allocate and init a re-usable frame
    myFrame = av_frame_alloc();
    if (myFrame == NULL)
    {
      const TCollection_AsciiString aMsg = TCollection_AsciiString ("Error: can not allocate video frame!");
      ::Message::DefaultMessenger()->Send (aMsg, Message_Fail);
      return Standard_False;
    }

    // allocate the encoded raw picture
    aResAv = avpicture_alloc (&myPicDst, aCodecCtx->pix_fmt, aCodecCtx->width, aCodecCtx->height);
    if (aResAv < 0)
    {
      const TCollection_AsciiString aMsg = TCollection_AsciiString ("Error: can not allocate picture ")
                                         + aCodecCtx->width+ "x" + aCodecCtx->height + ", " + formatAvError (aResAv);
      ::Message::DefaultMessenger()->Send (aMsg, Message_Fail);
      return Standard_False;
    }
    // copy data and linesize picture pointers to frame
    myFrame->format = aCodecCtx->pix_fmt;
    myFrame->width  = aCodecCtx->width;
    myFrame->height = aCodecCtx->height;
    *((AVPicture* )myFrame) = myPicDst;

    const Standard_Size aStride = aCodecCtx->width + 16 - (aCodecCtx->width % 16);
    if (!myImgSrcRgba.InitZero (Image_PixMap::ImgRGBA, aCodecCtx->width, aCodecCtx->height, aStride))
    {
      const TCollection_AsciiString aMsg = TCollection_AsciiString ("Error: can not allocate RGBA32 picture ")
                                         + aCodecCtx->width+ "x" + aCodecCtx->height;
      ::Message::DefaultMessenger()->Send (aMsg, Message_Fail);
      return Standard_False;
    }
    myImgSrcRgba.SetTopDown(true);

    myScaleCtx = sws_getContext (aCodecCtx->width, aCodecCtx->height, myPixFmtSrc,
                                 aCodecCtx->width, aCodecCtx->height, aCodecCtx->pix_fmt,
                                 SWS_BICUBIC, NULL, NULL, NULL);
    if (myScaleCtx == NULL)
    {
      const TCollection_AsciiString aMsg = TCollection_AsciiString ("Error: can not initialize the conversion context!");
      ::Message::DefaultMessenger()->Send (aMsg, Message_Fail);
      return Standard_False;
    }

    return Standard_True;
  }

  //! Write new video frame.
  Standard_Boolean writeVideoFrame (const Standard_Boolean theToFlush)
  {
    int aResAv = 0;
    AVCodecContext* aCodecCtx = myVideoStream->codec;
    if (!theToFlush)
    {
      uint8_t* aSrcData[4]     = { (uint8_t*)myImgSrcRgba.ChangeData(),   NULL, NULL, NULL };
      int      aSrcLinesize[4] = { (int     )myImgSrcRgba.SizeRowBytes(), 0,    0,    0    };
      sws_scale (myScaleCtx,
                 aSrcData, aSrcLinesize,
                 0, aCodecCtx->height,
                 myPicDst.data, myPicDst.linesize);
    }

    AVPacket aPacket = { 0 };
    av_init_packet (&aPacket);
    if ((myAVContext->oformat->flags & AVFMT_RAWPICTURE) != 0
     && !theToFlush)
    {
      // raw video case - directly store the picture in the packet
      aPacket.flags        |= AV_PKT_FLAG_KEY;
      aPacket.stream_index  = myVideoStream->index;
      aPacket.data          = myPicDst.data[0];
      aPacket.size          = sizeof(AVPicture);

      aResAv = av_interleaved_write_frame (myAVContext, &aPacket);
    }
    else
    {
      // encode the image
      myFrame->pts = myFrameCount;
      int isGotPacket = 0;
      aResAv = avcodec_encode_video2 (aCodecCtx, &aPacket, theToFlush ? NULL : myFrame, &isGotPacket);
      if (aResAv < 0)
      {
        const TCollection_AsciiString aMsg = TCollection_AsciiString ("Error: can not encode video frame, ") + formatAvError (aResAv);
        ::Message::DefaultMessenger()->Send (aMsg, Message_Fail);
        return Standard_False;
      }

      // if size is zero, it means the image was buffered
      if (isGotPacket)
      {
        const AVRational& aTimeBase = aCodecCtx->time_base;

        // rescale output packet timestamp values from codec to stream timebase
        aPacket.pts          = av_rescale_q_rnd (aPacket.pts,      aTimeBase, myVideoStream->time_base, AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        aPacket.dts          = av_rescale_q_rnd (aPacket.dts,      aTimeBase, myVideoStream->time_base, AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        aPacket.duration     = av_rescale_q     (aPacket.duration, aTimeBase, myVideoStream->time_base);
        aPacket.stream_index = myVideoStream->index;

        // write the compressed frame to the media file
        aResAv = av_interleaved_write_frame (myAVContext, &aPacket);
      }
      else
      {
        aResAv = 0;
      }
    }

    if (aResAv < 0)
    {
      const TCollection_AsciiString aMsg = TCollection_AsciiString ("Error: can not write video frame, ") + formatAvError (aResAv);
      ::Message::DefaultMessenger()->Send (aMsg, Message_Fail);
      return Standard_False;
    }
    ++myFrameCount;
    return Standard_True;
  }

protected:

  AVFormatContext* myAVContext;
  AVStream*        myVideoStream;
  AVCodec*         myVideoCodec;
  AVFrame*         myFrame;
  AVPicture        myPicDst;
  SwsContext*      myScaleCtx;
  AVRational       myFrameRate;
  AVPixelFormat    myPixFmtOut;

  Image_PixMap     myImgSrcRgba;
  AVPixelFormat    myPixFmtSrc;

  int64_t          myFrameCount;

public:

  DEFINE_STANDARD_RTTIEXT(ViewerTest_VideoRecorder, Standard_Transient) // Type definition

};

//DEFINE_STANDARD_HANDLE(ViewerTest_VideoRecorder, Standard_Transient)
//IMPLEMENT_STANDARD_HANDLE (ViewerTest_VideoRecorder, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(ViewerTest_VideoRecorder, Standard_Transient)
//DEFINE_STANDARD_HANDLE(ViewerTest_Animation, Standard_Transient)

//! Structure defining animation sequence
class ViewerTest_Animation : public Standard_Transient
{

public:

  //! Connected animation slice
  struct Slice
  {

    Standard_Real                StartTime; //!< presentation timestamp within parent animation slice
    Standard_Real                Duration;  //!< animation duration
    Handle(ViewerTest_Animation) Animation; //!< connected animation to be executed

    Slice() : StartTime (0.0), Duration (0.0) {}

  };

public:

  TCollection_AsciiString     OnStart;  //!< procedure to be evaluated at the beggining of this animation slice
  TCollection_AsciiString     OnEnd;    //!< procedure to be evaluated at the end       of this animation slice
  TCollection_AsciiString     OnRedraw; //!< procedure to be evaluated at every frame during this animation slice
  NCollection_Sequence<Slice> Slices;   //!< sequence of child animation slices

public:

  //! Empty constructor
  ViewerTest_Animation() {}

  //! Reset the animation definition
  void Reset()
  {
    OnStart.Clear();
    OnEnd.Clear();
    OnRedraw.Clear();
    Slices.Clear();
  }

  //! Estimated duration
  Standard_Real Duration() const
  {
    Standard_Real aDuration = -1.0;
    for (NCollection_Sequence<Slice>::Iterator aSliceIter (Slices);
         aSliceIter.More(); aSliceIter.Next())
    {
      Standard_Real aSliceDuration = aSliceIter.Value().Duration >= 0.0
                                   ? aSliceIter.Value().Duration
                                   : aSliceIter.Value().Animation->Duration();
      aDuration = Max (aDuration, aSliceIter.Value().StartTime + aSliceDuration);
    }
    return aDuration;
  }

public:

  DEFINE_STANDARD_RTTIEXT(ViewerTest_Animation,Standard_Transient) // Type definition

};

//IMPLEMENT_STANDARD_HANDLE (ViewerTest_Animation, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(ViewerTest_Animation, Standard_Transient)

static NCollection_DataMap<TCollection_AsciiString, Handle(ViewerTest_Animation)> ViewerTest_TheAnimationsMap;

//! Evaluate animation sequence
static void VEvalAnimation (Draw_Interpretor&           theDI,
                            const ViewerTest_Animation& theAnim,
                            const Standard_Real         theStartPts,
                            const Standard_Real         theEndPts,
                            const Standard_Real         thePrevPts,
                            const Standard_Real         thePts)
{
  const Standard_Real aLocPts = thePts - theStartPts;
  if ( thePts     >= theStartPts
   &&  thePrevPts <  theStartPts
   && !theAnim.OnStart.IsEmpty())
  {
    const TCollection_AsciiString aCmd = theAnim.OnStart + " " + thePts + " " + aLocPts + " onStart";
    theDI.Eval (aCmd.ToCString());
  }

  for (NCollection_Sequence<ViewerTest_Animation::Slice>::Iterator aSliceIter (theAnim.Slices);
       aSliceIter.More(); aSliceIter.Next())
  {
    VEvalAnimation (theDI,
                    *aSliceIter.Value().Animation,
                    aSliceIter.Value().StartTime,
                    aSliceIter.Value().StartTime + aSliceIter.Value().Duration,
                    thePrevPts,
                    thePts);
  }

  if (!theAnim.OnRedraw.IsEmpty()
   && thePts >= theStartPts
   && thePts <  theEndPts)
  {
    const TCollection_AsciiString aCmd = theAnim.OnRedraw + " " + thePts + " " + aLocPts + " onRedraw";
    theDI.Eval (aCmd.ToCString());
  }

  if ( thePts     >= theEndPts
   &&  thePrevPts <  theEndPts
   && !theAnim.OnEnd.IsEmpty())
  {
    const TCollection_AsciiString aCmd = theAnim.OnEnd + " " + thePts + " " + aLocPts + " onEnd";
    theDI.Eval (aCmd.ToCString());
  }
}

//==============================================================================
//function : VAnimation
//purpose  :
//==============================================================================
static Standard_Integer VAnimation (Draw_Interpretor& theDI,
                                    Standard_Integer  theArgNb,
                                    const char**      theArgVec)
{
  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (theArgNb < 2)
  {
    for (NCollection_DataMap<TCollection_AsciiString, Handle(ViewerTest_Animation)>::Iterator anAnimIter (ViewerTest_TheAnimationsMap);
         anAnimIter.More(); anAnimIter.Next())
    {
      theDI << anAnimIter.Key() << " " << anAnimIter.Value()->Duration() << " sec\n";
    }
    return 0;
  }

  Standard_Boolean toPlay        = Standard_False;
  Standard_Boolean isInteractive = Standard_False;
  Standard_Real    aPlayFrom     =  0.0;
  Standard_Real    aPlayDuration = -1.0;
  Standard_Integer aPlayFpsNum   = 0;
  Standard_Integer aPlayFpsDen   = 1;
  Standard_CString aRecordFile   = NULL;
  Standard_Integer aRecWidth     = 0;
  Standard_Integer aRecHeight    = 0;
  TCollection_AsciiString aName;
  Handle(ViewerTest_Animation) anAnim;
  Standard_Integer anArgIter = 1;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg = theArgVec[anArgIter];
    TCollection_AsciiString anArgCase (anArg);
    anArgCase.LowerCase();
    if (anArgCase == "-onstart")
    {
      if (++anArgIter >= theArgNb
       || anAnim.IsNull())
      {
        std::cout << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }
      anAnim->OnStart = theArgVec[anArgIter];
    }
    else if (anArgCase == "-onend")
    {
      if (++anArgIter >= theArgNb
       || anAnim.IsNull())
      {
        std::cout << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }
      anAnim->OnEnd = theArgVec[anArgIter];
    }
    else if (anArgCase == "-onredraw"
          || anArgCase == "-ondraw")
    {
      if (++anArgIter >= theArgNb
       || anAnim.IsNull())
      {
        std::cout << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }
      anAnim->OnRedraw = theArgVec[anArgIter];
    }
    else if (anArgCase == "-reset")
    {
      if (anAnim.IsNull())
      {
        std::cout << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }
      anAnim->Reset();
    }
    else if (anArgCase == "-delete")
    {
      if (++anArgIter < theArgNb
       || anAnim.IsNull())
      {
        std::cout << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }
      ViewerTest_TheAnimationsMap.UnBind (aName);
    }
    else if (anArgCase == "-addslice"
          || anArgCase == "-slice")
    {
      if (anArgIter + 3 >= theArgNb
       || anAnim.IsNull())
      {
        std::cout << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }

      ViewerTest_Animation::Slice aSlice;
      aSlice.StartTime = Draw::Atof (theArgVec[++anArgIter]);
      aSlice.Duration  = Draw::Atof (theArgVec[++anArgIter]);
      if (aSlice.StartTime < 0.0)
      {
        std::cout << "Error: PTS should not be negative (specified " << aSlice.StartTime << ")\n";
        return 1;
      }
      TCollection_AsciiString anOtherName (theArgVec[++anArgIter]);
      if (!ViewerTest_TheAnimationsMap.Find (anOtherName, aSlice.Animation))
      {
        std::cout << "Error: animation with name '" << anOtherName << "' is not registered!\n";
        return 1;
      }
      Standard_Integer anIndex = 1;
      for (NCollection_Sequence<ViewerTest_Animation::Slice>::Iterator aSliceIter (anAnim->Slices);
           aSliceIter.More(); aSliceIter.Next(), ++anIndex)
      {
        if (aSlice.StartTime < aSliceIter.Value().StartTime)
        {
          anAnim->Slices.InsertBefore (anIndex, aSlice);
          anIndex = -1;
          break;
        }
      }
      if (anIndex != -1)
      {
        anAnim->Slices.Append (aSlice);
      }
    }
    else if (anArgCase == "-play")
    {
      toPlay = Standard_True;
    }
    else if (anArgCase == "-playfrom")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }
      aPlayFrom = Draw::Atof (theArgVec[anArgIter]);
    }
    else if (anArgCase == "-playduration"
          || anArgCase == "-playdur")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }
      aPlayDuration = Draw::Atof (theArgVec[anArgIter]);
    }
    else if (anArgCase == "-playfps"
          || anArgCase == "-fps")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }

      TCollection_AsciiString aFpsStr (theArgVec[anArgIter]);
      Standard_Integer aSplitIndex = aFpsStr.FirstLocationInSet ("/", 1, aFpsStr.Length());
      if (aSplitIndex == 0)
      {
        aPlayFpsNum = aFpsStr.IntegerValue();
        aPlayFpsDen = 1;
      }
      else
      {
        const TCollection_AsciiString aDenStr = aFpsStr.Split (aSplitIndex);
        aFpsStr.Split (aFpsStr.Length() - 1);
        const TCollection_AsciiString aNumStr = aFpsStr;
        aPlayFpsNum = aNumStr.IntegerValue();
        aPlayFpsDen = aDenStr.IntegerValue();
        if (aPlayFpsDen < 1)
        {
          std::cout << "Error: wrong syntax at '" << anArg << "'\n";
          return 1;
        }
      }
    }
    else if (anArgCase == "-inter"
          || anArgCase == "-interactive")
    {
      isInteractive = Standard_True;
    }
    else if (anArgCase == "-rec"
          || anArgCase == "-record")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }
      aRecordFile = theArgVec[anArgIter];
    }
    else if (anArgCase == "-recwidth"
          || anArgCase == "-recordwidth")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }
      aRecWidth = Draw::Atoi (theArgVec[anArgIter]);
    }
    else if (anArgCase == "-recheight"
          || anArgCase == "-recordheight")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }
      aRecHeight = Draw::Atoi (theArgVec[anArgIter]);
    }
    else if (aName.IsEmpty())
    {
      aName = anArg;
      if (!ViewerTest_TheAnimationsMap.Find (aName, anAnim))
      {
        anAnim = new ViewerTest_Animation();
        ViewerTest_TheAnimationsMap.Bind (aName, anAnim);
      }
    }
    else
    {
      std::cout << "Error: wrong syntax at '" << anArg << "'\n";
      return 1;
    }
  }

  if (!toPlay)
  {
    return 0;
  }

  // play animation
  if (aPlayDuration < 0.0)
  {
    aPlayDuration = anAnim->Duration();
  }
  Standard_Real aPtsTo = aPlayFrom + aPlayDuration;
  OSD_Timer aTimer;
  aTimer.Start();
  Standard_Real aPrevPts = aPlayFrom - Precision::Confusion();
  if (aPlayFpsNum > 0)
  {
    const Handle(V3d_View)&          aView = ViewerTest::CurrentView();
    Handle(ViewerTest_VideoRecorder) aRecorder;
    if (aRecordFile != NULL)
    {
      if (aView.IsNull())
      {
        std::cout << "Error: no active view!\n";
        return 1;
      }

      Standard_Integer aWinWidth = 0, aWinHeight = 0;
      aView->Window()->Size (aWinWidth, aWinHeight);
      if (aRecWidth <= 0)
      {
        aRecWidth = aWinWidth;
      }
      if (aRecHeight <= 0)
      {
        aRecHeight = aWinHeight;
      }

      aRecorder = new ViewerTest_VideoRecorder();
      aRecorder->SetFramerate (aPlayFpsNum, aPlayFpsDen);
      if (!aRecorder->Open (aRecordFile, aRecWidth, aRecHeight))
      {
        return 0;
      }
    }

    // fixed FPS, bad formula - to be corrected
    const Standard_Real aFrameDur = Standard_Real(aPlayFpsDen) / Standard_Real(aPlayFpsNum);
    for (Standard_Real aPts = aPlayFrom; aPts <= aPtsTo; aPts += aFrameDur)
    {
      VEvalAnimation (theDI, *anAnim,
                      aPlayFrom, aPtsTo,
                      aPrevPts,  aPts);
      aPrevPts = aPts;
      if (!aRecorder.IsNull())
      {
        if (!aView->ToPixMap (aRecorder->ChangeFrame(),
                              aRecorder->ChangeFrame().SizeX(), aRecorder->ChangeFrame().SizeY(),
                              Graphic3d_BT_RGBA, Standard_True, V3d_SDO_MONO))
        {
          std::cout << "Error: view dump is failed!\n";
          return 0;
        }
        if (!aRecorder->PushFrame())
        {
          return 0;
        }
      }

      if (isInteractive)
      {
        // handle user events
        theDI.Eval ("after 1 set waiter 1");
        theDI.Eval ("vwait waiter");
      }
    }
  }
  else
  {
    // live playback
    for (Standard_Real aPts = aPlayFrom; aPts <= aPtsTo; aPts = aTimer.ElapsedTime())
    {
      VEvalAnimation (theDI, *anAnim,
                      aPlayFrom, aPtsTo,
                      aPrevPts,  aPts);
      aPrevPts = aPts;
      if (isInteractive)
      {
        // handle user events
        theDI.Eval ("after 1 set waiter 1");
        theDI.Eval ("vwait waiter");
      }
    }
  }
  aTimer.Show();
  //theDI << "Duration: " << aTimer.ElapsedTime() << " sec.\n";
  return 0;
}

//=======================================================================
//function : AviCommands
//purpose  :
//=======================================================================

void ViewerTest::AviCommands(Draw_Interpretor& theCommands)
{
  const char* aGroup = "ViewerTest AVI commands";

  theCommands.Add("vrecord", "vrecord [option]\n"
                  "where [option] can be:\n"
                  "\tfile <filename.avi> <FOURCC=VIDX': Create AVI file "
                                         "for recording,\n"
                  "\tstart              : begin/restart recording,\n"
                  "\tstop               : stop recording,\n"
                  "\tstatus             : log error message,\n"
                  "\tsave               : close the AVI file\n",
		  __FILE__,
		  &avi_record, aGroup); //Draft_Modification

  theCommands.Add("vanimation", "Alias for vanimation",
    __FILE__, VAnimation, aGroup);

  theCommands.Add("vanim",
            "List existing animations:"
    "\n\t\t:  vanim"
    "\n\t\t: Animation playback:"
    "\n\t\t:  vanim -play AnimName [-interactive] [-fps FPS=0/1]"
    "\n\t\t:       [-playFrom Sec=0] [-playDuration Sec=-1.0]"
    "\n\t\t:       [-record VideoFileName]"
    "\n\t\t:       [-recordWidth Width] [-recordHeight Height]"
    "\n\t\t: where"
    "\n\t\t:   -interactive  handle user events on idle"
    "\n\t\t:   -playFrom     animation start PTS"
    "\n\t\t:   -playDuration animation duration"
    "\n\t\t:   -playFps      constant framerate for recording (e.g. 30/1),"
    "\n\t\t:                 disables live time"
    "\n\t\t:   -record       record to the file"
    "\n\t\t:   -recordWidth  width of recorded video"
    "\n\t\t:                (window width if not set)"
    "\n\t\t:   -recordHeight height of recorded video"
    "\n\t\t:                (window height if not set)"
    "\n\t\t: Animation definition:"
    "\n\t\t:  vanim AnimName"
    "\n\t\t:        -delete"
    "\n\t\t:        -onStart  StartProcName"
    "\n\t\t:        -onEnd    EndProcName"
    "\n\t\t:        -onRedraw AnimProcName"
    "\n\t\t:        -addSlice FromPts Duration OtherAnimName"
    "\n\t\t:        -reset"
    "\n\t\t: where"
    "\n\t\t:   -delete   remove animation with specified name"
    "\n\t\t:   -onStart  procedure to be called once"
    "\n\t\t:             at the beginning"
    "\n\t\t:   -onEnd    procedure to be called once at the end"
    "\n\t\t:   -onRedraw procedure to be called at each frame"
    "\n\t\t:   -addSlice call another animation within specified duration"
    "\n\t\t:   -reset    clear list of linked slices"
    "\n\t\t: Callback procedure should has the following prototype:"
    "\n\t\t:  callback thePts theLocalPts theEvent"
    "\n\t\t: where"
    "\n\t\t:   -thePts       current presentation timestamp"
    "\n\t\t:   -theLocalPts  presentation timestamp within slice"
    "\n\t\t:   -theEvent     event from the list onStart,onEnd,onRedraw",
    __FILE__, VAnimation, aGroup);
}
