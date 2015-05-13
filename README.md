
rScreen
=============================
A screen recoder for Windows based on ffmpeg

What it is
--------------------------
This software is created to record Windows screen into video files. 
It is built with [Visual Studio](https://www.visualstudio.com/) and depends on [ffmpeg](https://www.ffmpeg.org/).

Usage
------------------------
Basic Usage:
```bash
rScreen --time [time_in_secs] --file [file_path_to_save]
```
This would create a video file named **file_path_to_save** (you can use anything like a.avi, or b.mp4, etc.). And the video records **time_in_secs** seconds of video stream of the screen.

Build
-----------------------
You can checkout the source code and directly open the **rScreen.sln** file with your Visual Studio.
We use [Zeranoe FFmpeg](http://ffmpeg.zeranoe.com/) instead of building ffmpeg from source.