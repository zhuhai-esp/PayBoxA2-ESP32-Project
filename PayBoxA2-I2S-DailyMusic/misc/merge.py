Import('env')
import os
import subprocess

buildFlags = env.ParseFlags(env['BUILD_FLAGS'])
OUTPUT_DIR = ".pio/build{}".format(os.path.sep)

platform = env.PioPlatform()
FRAMEWORK_DIR = platform.get_package_dir("framework-arduinoespressif32")
FRAMEWORK_DIR = "{}{}".format(FRAMEWORK_DIR, os.path.sep)


def copy_merge_bins(source, target, env):
    name = str(target[0]).split(os.path.sep)[2]
    flash_size = env.GetProjectOption("board_upload.flash_size")
    board = env.BoardConfig()
    flash_mode = board.get("build.flash_mode", "dio")
    f_flash = board.get("build.f_flash", "40000000L")
    flash_freq = '40m'
    if (f_flash == '80000000L'):
        flash_freq = '80m'
    mcu = board.get("build.mcu", "esp32")
    bootloader = "{}tools{}sdk{}{}{}bin{}bootloader_{}_{}.bin".format(FRAMEWORK_DIR, os.path.sep, os.path.sep, mcu,
                                                                      os.path.sep, os.path.sep, flash_mode, flash_freq)
    bootloader = str(target[0]).replace('firmware.bin', 'bootloader.bin')
    bootloader_location = '0x1000'
    if (mcu == 'esp32s3'):
        bootloader_location = '0x0000'
    partitions = "{}{}partitions.bin".format(env.subst("$BUILD_DIR"), os.path.sep)
    boot_app0 = "{}tools{}partitions{}boot_app0.bin".format(FRAMEWORK_DIR, os.path.sep, os.path.sep, os.path.sep)
    firmware_dst = "{}{}{}{}_{}_{}_0x0.bin".format(OUTPUT_DIR, name, os.path.sep, mcu, name, flash_size)
    firmware_src = str(target[0])
    # print('pa[%s]'%partitions)
    # print('bo[%s]'%boot_app0)
    # print('fr[%s]'%firmware_src)
    # print('de[%s]'%firmware_dst)
    if os.path.isfile(firmware_dst):
        os.remove(firmware_dst)
    esptool = '{}{}esptool.py'.format(platform.get_package_dir("tool-esptoolpy"), os.path.sep)
    process = subprocess.Popen(
        ['python', esptool, '--chip', mcu, 'merge_bin', '--output', firmware_dst, '--flash_mode', 'dio', '--flash_size',
         flash_size, '--flash_freq', flash_freq, bootloader_location, bootloader, '0x8000', partitions, '0xe000',
         boot_app0, '0x10000', firmware_src],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    # print('RESS: %s'%stdout.decode("utf-8") )
    # print('ERR: %s'%stderr.decode("utf-8") )


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", [copy_merge_bins])
