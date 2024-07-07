#include <arv.h>
#include <stdio.h>
/**
 * arv_save_camera_xml_test: 通过扫描在线设备保存camera的xml数据
 *
 */
int main(int argc, char** argv)
{   
	ArvCamera *camera;
	ArvDevice *device;
    guint n_devices;
	const char *genicam;
	const char *filename;
	g_autoptr (GError) error = NULL;
	gboolean success = FALSE;	
	size_t size;
	int i;

    printf ("Get Device List\n");
	// 扫描当前在线设备
	arv_update_device_list();
	// 获取设备数量
	n_devices = arv_get_n_devices();
	printf ("Found Devices: %d\n", n_devices);

	for (i = 0; i < n_devices; i++){
		camera = arv_camera_new (arv_get_device_id (i), NULL);
		if(!ARV_IS_CAMERA(camera)){
			g_print("Camera:%s open xml data fail-1\n",arv_get_device_id (i));
			continue;
		}
		device = arv_camera_get_device (camera);
		if(!ARV_IS_CAMERA(camera)){
			continue;
		}
		// 通过索引获取camera
		genicam = arv_device_get_genicam_xml (device, &size);

		if (NULL == genicam) {
			g_print("Camera:%s open xml data fail-2\n",arv_get_device_id (i));
			continue;
		}

		// 打开已收集到设备
		printf ("Camera:%s is opened\n", arv_get_device_id (i));
		filename = g_strdup_printf("D:\\%s.xml",arv_get_device_id (i));
		// 保存文件
		success = g_file_set_contents (filename, genicam, size, &error);
		if (success){
			g_print ("Camera:%s save xml data success\n", arv_get_device_id (i));
		}else{
			g_print ("Camera:%s save xml data fail\n", arv_get_device_id (i));
		}
	}
	g_clear_error(&error);
    return 0;
}