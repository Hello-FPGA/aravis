一、背景说明

1、调试相机设备描述

*   MV-CA060-11GM-00D91249143（GIGE network）
*   MV-CB060-10UM-C-00E17643445（USB）

2、编码工具及配置描述

*   编码工具：VSCode
*   环境包管理工具：[MSYS2](https://www.msys2.org) ,下载链接：<https://github.com/msys2/msys2-installer/releases/download/2024-05-07/msys2-x86_64-20240507.exe>
*   调试工具：gdb
*   其他工具：meson（负责构建项目依赖关系）、ninja（负责编译代码）

3、项目和系统描述

*   Aravis 0.10.0
*   OS: Windows10(X86\_64)

4、编译及调试配置

*   安装MSYS2软件，安装说明指导：<https://www.msys2.org/docs/installer/>，安装完成后打开MSYS2 MINGW64（PS:如果系统是32位，请选择相应的32位依赖的库），使用"pacam -s"命令进行相关依赖库安装。
    ```bash
    Laptop@DESKTOP-TQBS0MT MINGW64 ~
    # pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-gccmingw-w64-x86_64-gobject-introspection mingw-w64-x86_64-meson mingw-w64-x86_64-cmake mingw-w64-x86_64-libnotify mingw-w64-x86_64-gst-plugins-good mingw-w64-x86_64-gst-plugins-bad mingw-w64-x86_64-gstreamer mingw-w64-x86_64-gtk3 mingw-w64-x86_64-libxml2 mingw-w64-x86_64-zlib mingw-w64-x86_64-libusb mingw-w64-x86_64-gobject-introspection-runtime mingw-w64-x86_64-python-gobject mingw-w64-x86_64-gst-plugin-gtk
    ```

*   编译Aravis 0.10.0
    ```bash
    Laptop@DESKTOP-TQBS0MT MINGW64 /d/projects/order/20240620/aravis-main
    # mkdir build

    Laptop@DESKTOP-TQBS0MT MINGW64 /d/projects/order/20240620/aravis-main
    # meson --buildtype=debug -Ddocumentation=disabled -Dgst-plugin=enabled -Dintrospection=enabled -Dusb=enabled -Dviewer=enabled -Dgv-n-buffers=1 . ./build

    Laptop@DESKTOP-TQBS0MT MINGW64 /d/projects/order/20240620/aravis-main
    # ninja -C ./build --verbose install

    Laptop@DESKTOP-TQBS0MT MINGW64 /d/projects/order/20240620/aravis-main
    # meson test -C ./build

    ```

*   VSCode 安装插件C/C++、Meson

    &#x20;Meson插件需要在Workspace中配置Build Folder为build

*   进入项目，创建目录.vscode,在其目录下创建launch.json、tasks.json两个文件。

    launch.json:

    ```json
    {
      "version": "0.2.0",
      "configurations": [
        {
           // 配置Title
          "name": "Aravis Viewer Runner: Debug Session",
          "type": "cppdbg",
          "request": "launch",
          // 运行系统架构
          "targetArchitecture": "x86_64",
          // 添加日志数据输出
          "args": ["-d","all"],
          "stopAtEntry": false,
          // 是否外挂控制台
          "externalConsole": true,
          // 调试代码路径
          "cwd": "${workspaceFolder}/viewer",
          // 目标编译程序
          "program": "${workspaceFolder}/build/viewer/arv-viewer-0.10.exe",
          "MIMode": "gdb",
          // gdb的安装路径(千万不可以中文目录)
          "miDebuggerPath": "D:\\soft\\msys64\\mingw64\\bin\\gdb.exe",
          "setupCommands": [
            {
              "description": "Enable pretty-printing for gdb",
              "text": "-enable-pretty-printing",
              "ignoreFailures": true
            }
          ],
          // 指定启动的task，需要与task.json 里面定义的label一样的名称
          "preLaunchTask": "Meson:build"
        }
      ]
    }
    ```

    task.json:

    ```json
    {
        "version": "2.0.0",
        "tasks": [
            {
                "label": "Meson:build", // build 模式下需要运行ninja -C ./build --verbose install（PS:这种命令会将相关dll放到MINGW64的环境中）
                "type": "meson",
                "problemMatcher": [
                    "$meson-gcc",
                ],
                "mode": "build",
                "group": "build"
            },
            {
                "label": "Meson:install",// install 模式会重新将相关dll放到MINGW64的环境中
                "type": "meson",
                "problemMatcher": [
                    "$meson-gcc",
                ],
                "mode": "install",
                "group": "build"
            },
            {
                "label": "Meson:test",//会进行单元测试，然后通过后启动程序
                "type": "meson",
                "problemMatcher": [
                    "$meson-gcc",
                ],
                "mode": "test",
                "group": "build"
            }
        ]
    }
    ```



5、范例代码描述：**以下测试范例源码均都在项目test目录下，执行程序文件（PS\:exe文件）在build/test目录下**

```markdown
arvacquisitiontest.c 测试采集相机buffer。
使用cmd 执行：arv-acquisition-test.exe Hikvision-2BDF17643445-00E17643445
/ arv-acquisition-test.exe Hikvision-MV-CA060-11GM-00D91249143

arvautopacketsizetest.c 测试Gige相机自动确定可用于数据流的最大数据包大小，并设置ArvGevSCPSPacketSize值。
使用cmd 执行：arv-auto-packet-size-test.exe Hikvision-2BDF17643445-00E17643445
/ arv-auto-packet-size-test.exe Hikvision-MV-CA060-11GM-00D91249143

arvchunkparsertest.c 测试区块数据解析
使用cmd 执行：arv-chunk-parser-test.exe

arvdevicescantest.c 测试扫描出相关相机设备
使用cmd 执行：arv-device-scan-test.exe

arvdevicetest.c 测试获取相机设备相关信息
使用cmd 执行：arv-device-test.exe
    
arvevaluatortest.c 暂未知道测试的功能，貌似是用来通过数据设置然后进行评估某方面的结果
使用cmd 执行：arv-evaluator-test.exe

arvexample.c 测试设置相机帧速率、像素，并获取图片stream
使用cmd 执行：arv-example.exe 

arvgenicamtest.c 通过读取相机的xml文件信息，可以解析相关相机的配置信息
使用cmd 执行：arv-genicam-test.exe MV-CB060-10UM-C.xml/ arv-genicam-test.exe MV-CA060-11GM.xml

arvheartbeattest.c 通过指定设备名称，通过定时获取stream buffer来判定设备是否离线
使用cmd 执行：arv-heartbeat-test.exe -n Hikvision-2BDF17643445-00E17643445/
arv-heartbeat-test.exe -n Hikvision-MV-CA060-11GM-00D91249143

arvnetworktest.c 获取本机网络信息
使用cmd 执行：arv-network-test.exe 

arvroitest.c 获取相机设备相关信息，vendor、model、device id等，并对其设置width、height(目前没有调通，有报错)
使用cmd 执行：arv-roi-test.exe Hikvision-2BDF17643445-00E17643445 / arv-roi-test.exe Hikvision-MV-CA060-11GM-00D91249143

arvziptest.c 功能未知
使用cmd 执行：arv-zip-test.exe MV-CB060-10UM-C.xml/ arv-zip-test.exe MV-CA060-11GM.xml
```

6、注意事项：

&#x20;  1）**Window10环境USB相机驱动问题，需要手动自己切换，可以使用zading切换，帮助链接：<https://blog.csdn.net/joyopirate/article/details/138067528>**

&#x9;2）**如果发现view无法正常采集流数据，检查mingw-w64-x86\_64-gst-plugin-gtk、mingw-w64-x86\_64-gtk3是否安装**

7、相关文献

```markdown
* Forum: https://aravis-project.discourse.group
* Github repository: https://github.com/AravisProject/aravis
* Releases: https://github.com/AravisProject/aravis/releases
* Release notes: https://github.com/AravisProject/aravis/blob/master/NEWS.md
* Genicam standard: http://www.genicam.org
```

&#x20;  &#x20;

