/* Copyright (c) 2012 Polytechnic Institute of New York University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *
 * Contact: Fraida Fund, ffund01@students.poly.edu
 *
 *************************************************************************
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#define OML_FROM_MAIN
#include "../../vlc-dash_oml.h"
#include <stdlib.h>

/* VLC core API headers */
#include <vlc_common.h>
#include <vlc_plugin.h>
#include <vlc_interface.h>
#include <vlc_vout.h>
#include <vlc_aout.h>
#include <vlc_charset.h>
#include <vlc_input.h>
#include <vlc_es.h>
#include <vlc_playlist.h>
#include <vlc_meta.h>
#include <vlc_fs.h>
#include <vlc_playlist.h>
#include <vlc_input.h>
#include <stddef.h>
#include <oml2/omlc.h>



/* Forward declarations */
static int Open( vlc_object_t * );
static void Close( vlc_object_t * );
static void Run( intf_thread_t * );

extern oml_mps_t* g_oml_mps_vlc;


struct intf_sys_t
{
    input_thread_t *p_input;
    playlist_t     *p_playlist;
    playlist_item_t *p_node;   
    vlc_thread_t thread; 
};

/* Module descriptor */
vlc_module_begin()
    set_shortname(N_("omlstats"))
    set_description(N_("OML Stats"))
    set_capability("interface", 0)
    set_callbacks(Open, Close)
    set_category(CAT_INTERFACE)
vlc_module_end ()


/* Starts the omlstats module, then calls the Run function. */
static int Open(vlc_object_t *p_this)
{
    fprintf(stderr,"Opened omlstats interface\n");
    intf_thread_t *p_intf = (intf_thread_t *)p_this;
    intf_sys_t *p_sys  = calloc( 1, sizeof( intf_sys_t ) );


    if( unlikely(!p_sys) )
        return VLC_ENOMEM;

    p_sys->p_node = NULL;
    p_sys->p_input = NULL;

    p_intf->p_sys = p_sys;
    
    if (vlc_clone(&p_sys->thread, Run, p_intf, VLC_THREAD_PRIORITY_LOW))
        return VLC_ENOMEM;

    return VLC_SUCCESS;

}

/* Stops the module  */
static void Close(vlc_object_t *p_this)
{
    intf_thread_t *p_intf = (intf_thread_t *)p_this;
    intf_sys_t    *p_sys = p_intf->p_sys;


    if( p_sys->p_input )
    {
        vlc_object_release( p_sys->p_input );
    }
    free( p_sys );
    omlc_close(); 
}


/* Run main thread  */
static void Run( intf_thread_t *p_intf )
{
    intf_sys_t    *p_sys = p_intf->p_sys;

    playlist_t    *p_playlist = pl_Get( p_intf );
    p_sys->p_playlist = p_playlist;

    
    oml_register_mps();

    
    int result = omlc_start();

    if (result == -1) {
      fprintf (stderr, "Error starting up OML measurement streams\n");
      exit (1);
    }
   
    

    while( vlc_object_alive( p_intf ) )
    {
      msleep(1000000);
        if( p_sys->p_input == NULL )
        {
            p_sys->p_input = playlist_CurrentInput( p_playlist );
        }
        else if( p_sys->p_input->b_dead )
        {
            vlc_object_release( p_sys->p_input );
            p_sys->p_input = NULL;
        }
        if (p_sys->p_input)
        {

          input_item_t *p_item = input_GetItem( p_sys->p_input );

	  oml_inject_audio(g_oml_mps_vlc->audio, (int32_t) p_item->p_stats->i_decoded_audio, 
			(int32_t) p_item->p_stats->i_played_abuffers, (int32_t) p_item->p_stats->i_lost_abuffers);
	  oml_inject_video(g_oml_mps_vlc->video, (int32_t) p_item->p_stats->i_decoded_video, (int32_t) p_item->p_stats->i_displayed_pictures,
			(int32_t) p_item->p_stats->i_lost_pictures);
	  oml_inject_input(g_oml_mps_vlc->input, (int32_t) p_item->p_stats->i_read_packets, (int32_t) p_item->p_stats->i_read_bytes,
			(double) p_item->p_stats->f_input_bitrate, (int32_t) p_item->p_stats->i_demux_read_bytes,
			(double) p_item->p_stats->f_demux_bitrate, (int32_t) p_item->p_stats->i_demux_corrupted, 
			(int32_t) p_item->p_stats->i_demux_discontinuity);
	  oml_inject_output(g_oml_mps_vlc->output, (int32_t) p_item->p_stats->i_sent_packets,  (int32_t) p_item->p_stats->i_sent_bytes, 
			(double) p_item->p_stats->f_send_bitrate);
        }
    }
}

