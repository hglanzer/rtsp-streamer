#include<stdio.h>
#include<gst/rtsp-server/rtsp-server.h>
#include<glib.h>


/*
	gst-rtsp-server documentation: https://github.com/GStreamer/gst-rtsp-server/tree/master/docs

	media pipeline: gst-launch-1.0 filesrc location=/media/Common/big_buck_bunny_1080p_surround.avi ! decodebin ! vaapisink

	connect to rtsp server from command line:
		gst-launch-1.0 rtspsrc location=rtsp://10.0.1.145:6667/test ! decodebin ! vaapisink

	
	*******************************
	*** AUDIO FROM PULSE SERVER ***
	*******************************

		* this just creates mp3 from local pulse audio / modify for streaming

			1) get proper pulse sink
				pactl list sinks
			2) unmute sink
				pactl set-sink-mute alsa_output.pci-0000_00_1b.0.iec958-stereo 0
			3) gst pipeline
				gst-launch-1.0 pulsesrc device=alsa_output.pci-0000_00_1b.0.iec958-stereo.monitor ! lamemp3enc ! filesink location=test.mp3
		
*/

static GMainLoop *loop = NULL;

static void new_client(GstRTSPServer *rtsp, GstRTSPClient *client, gpointer user_data)
{
	printf("new client\n\n");
}

static gboolean janitor(gpointer user_data)
{
	printf("janitor() here\n");
	return 1;
}

int main(int argc, char **argv)
{
	GstRTSPServer *rtsp;
	GstRTSPSessionPool *pool;
	GstRTSPMediaFactory *factory;
	GstRTSPMountPoints *mounts;
	guint timeoutSrc;

	gst_init(&argc, &argv);
	timeoutSrc = g_timeout_add_seconds(2, janitor, NULL);

	rtsp = gst_rtsp_server_new();
	if(!rtsp)
	{	
		printf("gst_rtsp_server_new() failed\n");
		return -1;
	}
	g_signal_connect(rtsp, "client-connected", (GCallback)new_client, NULL);
	gst_rtsp_server_set_service(rtsp, "6667");
	gst_rtsp_server_set_backlog(rtsp, 10);
	pool = gst_rtsp_server_get_session_pool(rtsp);

	factory = gst_rtsp_media_factory_new ();	
	gst_rtsp_media_factory_set_launch(factory,  "( audiotestsrc ! rtpmp4apay pt=96 name=pay0 )");

	// this works together with client
	//
	//	gst-launch-1.0 rtspsrc location=rtsp://10.0.1.145:6667/test ! decodebin ! vaapisink
	//
	//gst_rtsp_media_factory_set_launch(factory,  "( videotestsrc ! x264enc ! rtph264pay pt=96 name=pay0 )");

	/* get the default mount points from the server */
	mounts = gst_rtsp_server_get_mount_points(rtsp);
	/* attach the video test signal to the "/test" URL */
	gst_rtsp_mount_points_add_factory (mounts, "/test", factory);
	g_object_unref (mounts);


	loop = g_main_loop_new(NULL, FALSE);
	gst_rtsp_server_attach (rtsp, NULL);
	g_main_loop_run(loop);

	printf("exit\n\n");
	return 0;
}
