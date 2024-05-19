# PayBoxA2-ESP32-Project
project for paybox A2 box

### 配置:
* flash: 16MB
* ram: 4MB

### PlatformIO bin merge:
* 参考文档:https://github.com/platformio/platform-espressif32/issues/1078
* 常用分区表:
```
# 0x1000 bootloader.bin
# 0x8000 partitions.bin
# 0xe000 boot_app0.bin
# 0x10000 firmware.bin
```
* 简单代码:
```ini
[env:esp32dev]
...

extra_scripts =
	misc/merge.py
```
```python
Import('env')
import os

OUTPUT_DIR = "$BUILD_DIR{}".format(os.path.sep)
APP_BIN = "$BUILD_DIR/${PROGNAME}.bin"


def copy_merge_bins(source, target, env):
    firmware_src = str(target[0])
    flash_images = env.Flatten(env.get("FLASH_EXTRA_IMAGES", [])) + ["$ESP32_APP_OFFSET", APP_BIN]
    name = firmware_src.split(os.path.sep)[2]
    flash_size = env.GetProjectOption("board_upload.flash_size")
    board = env.BoardConfig()
    f_flash = board.get("build.f_flash", "40000000L")
    flash_freq = '40m'
    if (f_flash == '80000000L'):
        flash_freq = '80m'
    mcu = board.get("build.mcu", "esp32")
    firmware_dst = "{}{}_{}_{}_0x0.bin".format(OUTPUT_DIR, mcu, name, flash_size)
    if os.path.isfile(firmware_dst):
        os.remove(firmware_dst)
    cmd = " ".join(
        ["$PYTHONEXE", "$OBJCOPY", '--chip', mcu, 'merge_bin', '--output', firmware_dst, '--flash_mode', 'dio',
         '--flash_size', flash_size, '--flash_freq', flash_freq] + flash_images)
    env.Execute(cmd)


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", [copy_merge_bins])

```

### Details
* framework: https://registry.platformio.org/tools/platformio/framework-arduinoespressif32
* Flash分区配置：https://github.com/espressif/arduino-esp32/tree/master/tools/partitions
* 各种板子说明：https://docs.platformio.org/en/latest/platforms/espressif32.html
* 各种板子配置：https://github.com/platformio/platform-espressif32/tree/master/boards
