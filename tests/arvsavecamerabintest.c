#include <arv.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>


static void
write_buffer(char *path, char *buf, int size){
    FILE *outfile;
 
    if ((outfile = fopen(path, "ab+")) == NULL)
    {
        printf("\nCan not open the path: %s \n", path);
        exit(-1);
    }
    fwrite(buf, sizeof(char), size, outfile);
    fclose(outfile);
}

/**
 * arv_save_camera_bin_test: 保存camera的Buffer数据到bin文件中
 *
 */
int
main(int argc, char const *argv[])
{
    guint n_devices;
	int i;
    printf("Get Device List\n");
	// 扫描当前在线设备
	arv_update_device_list();
	
	n_devices = arv_get_n_devices();

	printf ("Found Devices: %d\n", n_devices);

	for (i = 0; i < n_devices; i++){
        ArvCamera *camera;
        g_autoptr (GError) gerror = NULL;
        ArvStream *stream;
        GTimer *timer;
        ArvBuffer *buffer;
        size_t payload_size;
        size_t buffer_size;
        gboolean is_start_timer = TRUE;
        gboolean read_buffer_success = FALSE;
        char *buffer_data;
        unsigned buffer_count = 0;
        unsigned buffer_total = 0;

		// 打开已收集到设备
		camera = arv_camera_new (arv_get_device_id (i), NULL);

        if (!ARV_IS_CAMERA (camera)) {
            printf("%s Camera not found,error:%s\n", arv_get_device_id (i), gerror->message);
            continue;
		}

        printf ("%s is open camera\n", arv_get_device_id (i));

        /* Set region of interrest to a 1024X1024 pixel area */
        arv_camera_set_region (camera, 0, 0, 1024, 1024, NULL);
        /* Set frame rate to 10 Hz */
        arv_camera_set_frame_rate (camera, 10, NULL);

        /* Create a new stream object */
        stream = arv_camera_create_stream (camera, NULL, NULL, &gerror);

        if (!ARV_IS_STREAM (stream)) {
            printf("stream error:%s\n", gerror->message);
            return EXIT_FAILURE;
        }

        if (gerror == NULL)
            payload_size = arv_camera_get_payload (camera, &gerror);

        if (gerror == NULL) {
            for (i = 0 ; i < 100; i++){
                arv_stream_push_buffer (stream, arv_buffer_new (payload_size, NULL));
            }  
        }

        arv_camera_set_acquisition_mode (camera, ARV_ACQUISITION_MODE_CONTINUOUS, &gerror);

        if (gerror == NULL){
            /* Start the video stream */
            arv_stream_set_emit_signals (stream, TRUE);
            arv_camera_start_acquisition (camera, &gerror);
        }

        // 创建计时器
        timer = g_timer_new ();
        if(is_start_timer){
            // 开始计时
            g_timer_start (timer);
            is_start_timer = FALSE;
        }
        
        do {
            
            buffer = arv_stream_timeout_pop_buffer(stream, 2000000);
		    if (ARV_IS_BUFFER (buffer)){
                if (arv_buffer_get_status (buffer) == ARV_BUFFER_STATUS_SUCCESS) {
                    printf ("Acquired %d*%d buffer\n",
							arv_buffer_get_image_width (buffer),
							arv_buffer_get_image_height (buffer));
                    buffer_data = arv_buffer_get_image_data (buffer, &buffer_size);
                    write_buffer("D:\\2.bin",buffer_data,buffer_size);
                    buffer_count++;
                    buffer_total++;
                    if (buffer_count == 10) {
                        g_usleep(10);
                        buffer_count = 0;
                    }
                    arv_stream_push_buffer (stream, buffer);
                    read_buffer_success = TRUE;
                }else{
                    read_buffer_success = FALSE;
                }
		    }else{
                read_buffer_success = FALSE;
            }

            if(!read_buffer_success){
                printf("buffer has no data\n");
            }
        }while (g_timer_elapsed (timer, NULL) < 31);
        // 设置30s g_timer_elapsed (timer, NULL) < 30

        if (gerror == NULL){
            arv_camera_stop_acquisition (camera, &gerror);
            arv_stream_set_emit_signals (stream, FALSE);
        }

        g_clear_object (&stream);
        g_clear_error (&gerror);
        // 销毁计时器
        g_timer_destroy(timer);
        printf ("Frames number= %d \n", buffer_total);   
    }
    return 0;
}
