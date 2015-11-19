
/*
 * This file is automatically generated by oml2-scaffold. Please do not edit.
 */

#ifndef VLC_OML_H
#define VLC_OML_H

#ifdef __cplusplus
extern "C" {
#endif

/* Get size_t and NULL from <stddef.h>.  */
#include <stddef.h>
#include <stdio.h>
#include <oml2/omlc.h>

typedef struct {
  OmlMP* audio;
  OmlMP* video;
  OmlMP* input;
  OmlMP* output;
  OmlMP* dashDlSession;
  OmlMP* dashRateAdaptation;
} oml_mps_t;


#ifdef OML_FROM_MAIN
/*
 * Only declare storage once, usually from the main
 * source, where OML_FROM_MAIN is defined
 */


static OmlMPDef oml_audio_def[] = {
  {"i_decoded_audio_blocks", OML_INT32_VALUE},
  {"i_played_audio_buffers", OML_INT32_VALUE},
  {"i_lost_audio_buffers", OML_INT32_VALUE},
  {NULL, (OmlValueT)0}
};

static OmlMPDef oml_video_def[] = {
  {"i_decoded_video_blocks", OML_INT32_VALUE},
  {"i_played_video_frames", OML_INT32_VALUE},
  {"i_lost_video_frames", OML_INT32_VALUE},
  {NULL, (OmlValueT)0}
};

static OmlMPDef oml_input_def[] = {
  {"i_read_packets", OML_INT32_VALUE},
  {"i_read_bytes", OML_INT32_VALUE},
  {"f_input_bitrate", OML_DOUBLE_VALUE},
  {"i_demux_read_bytes", OML_INT32_VALUE},
  {"f_demux_bitrate", OML_DOUBLE_VALUE},
  {"i_demux_corrupted", OML_INT32_VALUE},
  {"i_demux_discontinuity", OML_INT32_VALUE},
  {NULL, (OmlValueT)0}
};

static OmlMPDef oml_output_def[] = {
  {"i_sent_packets", OML_INT32_VALUE},
  {"i_sent_bytes", OML_INT32_VALUE},
  {"f_send_bitrate", OML_DOUBLE_VALUE},
  {NULL, (OmlValueT)0}
};


static OmlMPDef oml_dashRateAdaptation_def[] = {
  {"chosenRate_bps", OML_UINT64_VALUE},
  {"empiricalRate_bps", OML_UINT64_VALUE},
  {NULL, (OmlValueT)0}
};

static OmlMPDef oml_dashDlSession_def[] = {
  {"chunkCount", OML_INT32_VALUE},
  {"readSession_B", OML_INT32_VALUE},
  {"readChunk_B", OML_INT32_VALUE},
  {"timeSession_s", OML_DOUBLE_VALUE},
  {"timeChunk_s", OML_DOUBLE_VALUE},
  {NULL, (OmlValueT)0}
};


static oml_mps_t g_oml_mps_storage;
oml_mps_t* g_oml_mps_vlc = &g_oml_mps_storage;

static void
oml_register_mps()

{
  g_oml_mps_vlc->audio = omlc_add_mp("audio", oml_audio_def);
  g_oml_mps_vlc->video = omlc_add_mp("video", oml_video_def);
  g_oml_mps_vlc->input = omlc_add_mp("input", oml_input_def);
  g_oml_mps_vlc->output = omlc_add_mp("output", oml_output_def);
  //g_oml_mps_vlc->dashDlSession = omlc_add_mp("dashDlSession", oml_dashDlSession_def);
  //g_oml_mps_vlc->dashRateAdaptation = omlc_add_mp("dashRateAdaptation", oml_dashRateAdaptation_def);

}



#else
/*
 * Not included from the main source,
 * only declare the global pointer to the MPs
 */

extern oml_mps_t* g_oml_mps_vlc;


#endif /* OML_FROM_MAIN */

inline void oml_inject_audio(OmlMP* mp, int32_t i_decoded_audio_blocks, int32_t i_played_audio_buffers, int32_t i_lost_audio_buffers)
{
  OmlValueU v[3];

  omlc_zero_array(v, 3);

  omlc_set_int32(v[0], i_decoded_audio_blocks);
  omlc_set_int32(v[1], i_played_audio_buffers);
  omlc_set_int32(v[2], i_lost_audio_buffers);

  omlc_inject(mp, v);

}

inline void oml_inject_video(OmlMP* mp, int32_t i_decoded_video_blocks, int32_t i_played_video_frames, int32_t i_lost_video_frames)
{
  OmlValueU v[3];

  omlc_zero_array(v, 3);

  omlc_set_int32(v[0], i_decoded_video_blocks);
  omlc_set_int32(v[1], i_played_video_frames);
  omlc_set_int32(v[2], i_lost_video_frames);

  omlc_inject(mp, v);

}

inline void oml_inject_input(OmlMP* mp, int32_t i_read_packets, int32_t i_read_bytes, double f_input_bitrate, int32_t i_demux_read_bytes, double f_demux_bitrate, int32_t i_demux_corrupted, int32_t i_demux_discontinuity)
{
  OmlValueU v[7];

  omlc_zero_array(v, 7);

  omlc_set_int32(v[0], i_read_packets);
  omlc_set_int32(v[1], i_read_bytes);
  omlc_set_double(v[2], f_input_bitrate);
  omlc_set_int32(v[3], i_demux_read_bytes);
  omlc_set_double(v[4], f_demux_bitrate);
  omlc_set_int32(v[5], i_demux_corrupted);
  omlc_set_int32(v[6], i_demux_discontinuity);

  omlc_inject(mp, v);

}

inline void oml_inject_output(OmlMP* mp, int32_t i_sent_packets, int32_t i_sent_bytes, double f_send_bitrate)
{
  OmlValueU v[3];

  omlc_zero_array(v, 3);

  omlc_set_int32(v[0], i_sent_packets);
  omlc_set_int32(v[1], i_sent_bytes);
  omlc_set_double(v[2], f_send_bitrate);

  omlc_inject(mp, v);

}


inline void oml_inject_dashRateAdaptation(OmlMP* mp, uint64_t chosenRate_bps, uint64_t empiricalRate_bps)
{
  OmlValueU v[2];

  omlc_zero_array(v, 2);

  omlc_set_uint64(v[0], chosenRate_bps);
  omlc_set_uint64(v[1], empiricalRate_bps);

  omlc_inject(mp, v);

}


inline void oml_inject_dashDlSession(OmlMP* mp, int32_t chunkCount, int32_t readSession_B, int32_t readChunk_B, double timeSession_s, double timeChunk_s)
{
  OmlValueU v[5];

  omlc_zero_array(v, 5);

  omlc_set_int32(v[0], chunkCount);
  omlc_set_int32(v[1], readSession_B);
  omlc_set_int32(v[2], readChunk_B);
  omlc_set_double(v[3], timeSession_s);
  omlc_set_double(v[4], timeChunk_s);

  omlc_inject(mp, v);


}

/* Compatibility with old applications */
#ifndef g_oml_mps
# define g_oml_mps	g_oml_mps_vlc
#endif

#ifdef __cplusplus
}
#endif

#endif /* VLC_OML_H */
