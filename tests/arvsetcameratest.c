#include <arv.h>
#include <arvcamera.h>
#include <stdlib.h>
#include <stdio.h>


static double arv_option_exposure = -1;
static double arv_option_frame_rate = -1;
static char *arv_option_debug_domains = "device";
static int arv_option_width = -1;
static int arv_option_height = -1;

static const GOptionEntry arv_option_entries[] =
{
	{ "exposure", 	'e', 0, G_OPTION_ARG_DOUBLE,
		&arv_option_exposure, "Exposure", NULL },
	{ "frame_rate", 'f', 0, G_OPTION_ARG_DOUBLE,
		&arv_option_frame_rate,	"Frame rate", NULL },
	{ "width", 		'w', 0, G_OPTION_ARG_INT,
		&arv_option_width,	"Width", NULL },
	{ "height", 	'h', 0, G_OPTION_ARG_INT,
		&arv_option_height, "Height", NULL },
	{ "debug", 		'd', 0, G_OPTION_ARG_STRING,
		&arv_option_debug_domains, 	"Debug mode", NULL },
	{ NULL }
};
/**
 * fake test : 测试fake设置曝光时间
 *
 */
int 
fakemain(int argc, char** argv)
{
	ArvDevice *device;
	ArvCamera *camera;
	GError *gerror = NULL;
	const char *genicam;
	gsize size;
	// 传入设备ID
	device = arv_fake_device_new ("TEST0", &gerror);
	g_assert (ARV_IS_FAKE_DEVICE (device));
	g_assert (gerror == NULL);
	// 传入设备指针，获取camera的数据
	genicam = arv_device_get_genicam_xml (device, &size);
	g_assert (genicam != NULL);  

	printf("genicam:%s\n",genicam);

	camera = arv_camera_new_with_device (device, NULL);

	if (!ARV_IS_CAMERA (camera)) {
		printf("Camera not found,error\n");
	}
	
	arv_camera_set_exposure_time(camera, 16000,  &gerror);
    if(gerror != NULL)
		printf("Camera set exposure time is fail,error:%s\n", gerror->message);

	g_clear_error (&gerror);

	g_object_unref (device); 
}


/**
 * arv_device_save_test : 通过传入曝光时间、帧率、width、height设置camera属性：arv_device_save_test.exe -e 16000.00 -f 10.0 -h 1024 -w 1024
 *
 */
int 
main(int argc, char** argv)
{   
	ArvDevice *device;
	GOptionContext *context;
	GError *error = NULL;
	ArvCamera *camera;
	guint n_devices;
	int i;
	g_autoptr (GError) gerror = NULL;
	double exposure_time;
	double frame_rate;

	context = g_option_context_new (NULL);
	g_option_context_add_main_entries (context, arv_option_entries, NULL);

	if (!g_option_context_parse (context, &argc, &argv, &error)) {
		g_option_context_free (context);
		g_print ("Option parsing failed: %s\n", error->message);
		g_error_free (error);
		return EXIT_FAILURE;
	}

	g_option_context_free (context);

	arv_debug_enable (arv_option_debug_domains);

    printf("Get Device List\n");
	// 扫描当前在线设备
	arv_update_device_list();
	
	n_devices = arv_get_n_devices();

	printf ("Found Devices: %d\n", n_devices);

	for (i = 0; i < n_devices; i++){
	
		camera = arv_camera_new (arv_get_device_id (i), &gerror);

		if (!ARV_IS_CAMERA (camera)) {
			//printf("Camera not found,error:%s\n", gerror->message);
			continue;
		}

		// 打开已收集到设备
		printf ("Camera:%s is open\n", arv_get_device_id (i));

		// 设置尺寸
		if(arv_option_width > 0 && arv_option_height > 0){
			arv_camera_set_region (camera, 0, 0, arv_option_width, arv_option_height, NULL);
			gint width = -1;
			gint height = -1;
			arv_camera_get_region (camera, NULL, NULL, &width, &height, NULL);
			//g_assert (arv_option_height == height ||arv_option_width == width);  
			if(arv_option_height == height && arv_option_width == width){
				printf ("arv_camera_set_region set success, image size set to %dx%d\n", width, height);
			}else{
				printf ("arv_camera_set_region set fail\n");
			}
			
		}
		
		// 设置曝光
		if (arv_option_exposure > 0 ){
			//printf("arv_camera_is_exposure_time_available:%d\n",arv_camera_is_exposure_time_available(camera, NULL));
			arv_camera_set_exposure_time(camera, arv_option_exposure,  NULL);
			exposure_time = arv_camera_get_exposure_time(camera,  NULL);
			//g_assert (exposure_time == arv_option_exposure);  
			if(exposure_time == arv_option_exposure){
				printf("arv_camera_set_exposure_time success,exposure_time:%g\n",exposure_time);
			}else{
				printf ("arv_camera_set_exposure_time set fail\n");
			}
			
		}

		// 设置帧率
		if (arv_option_frame_rate > 0 ){
			//printf("arv_camera_is_frame_rate_available:%d\n",arv_camera_is_frame_rate_available(camera, NULL));
			arv_camera_set_frame_rate (camera, arv_option_frame_rate,  NULL);
			frame_rate =arv_camera_get_frame_rate(camera, NULL);
			//g_assert (frame_rate == arv_option_frame_rate);  
			if(frame_rate == arv_option_frame_rate){
				printf("arv_camera_set_frame_rate success,frame_rate:%g\n",frame_rate);
			}else{
				printf ("arv_camera_set_frame_rate set fail\n");
			}
		}
	}		
    return 0;
}