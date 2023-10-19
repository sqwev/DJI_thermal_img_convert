# encoding: utf-8
# @author: Shenzhou Liu
# @email: 913737515@qq.com

import os
import shutil
import platform
import subprocess
import piexif
import numpy as np
from tqdm import tqdm
from PIL import Image


def get_platform():
    '''
    获取当前系统平台
    :return: Windows or Linux
    '''
    return platform.system()

def mkdir(path):
    '''
    创建指定的文件夹
    :param path: 文件夹路径，字符串格式
    :return: True(新建成功) or False(文件夹已存在，新建失败)
    '''
    isExists = os.path.exists(path)
    if not isExists:
        os.makedirs(path)
        print(f"Successfully create path: {path}")
        return True
    else:
        shutil.rmtree(path)
        os.makedirs(path)
        print(f"Path: {path} already exists, delete and create again.")
        return False


def jpg2tiff(inputpath, outpath, **kwargs):
    if kwargs is not None: 
        legal_fields = ["distance", "humidity", "emissivity", "reflection"]
        input_kwargs = []
        for key in kwargs:
            if key in legal_fields:
                input_kwargs.append(key)
            else:
                raise ValueError(f"{key} not in legal fields: {legal_fields}")
        command = "dji_thermal_sdk_v1.4_20220929/utility/bin/windows/release_x64/dji_irp.exe -s " + \
              inputpath + " -a measure -o " + outpath
        for key in input_kwargs:
            para_cmd = " --"+key+" "+str(kwargs[key])
            command += para_cmd

    else:
        command = "dji_thermal_sdk_v1.4_20220929/utility/bin/windows/release_x64/dji_irp.exe -s " + \
              inputpath + " -a measure -o " + outpath
        
    PLATFORM = get_platform()
    if PLATFORM == "Windows":
        args = [r"C:\WINDOWS\system32\WindowsPowerShell\v1.0\powershell.exe", command]
        ps = subprocess.Popen(args, stdout=subprocess.PIPE)
        result = ps.stdout.read()
    elif PLATFORM == "Linux":
        result = subprocess.run(command, shell=True, capture_output=True, text=True)
    return result

def print_help():
    print(f"=====================================================\n")
    Chinese_note = """
    注意事项：输入的文件名不能含有空格，Tab等异常字符，否则会报错！
可选输入参数：测量距离，环境湿度，发射率，反射温度
测温距离：待测目标的距离。红外热像仪接收物体自身辐射的红外线生成热像图，距离越远，
红外辐射衰减越多。红外热像仪出厂时默认的标定距离一般是固定的，在这个距离测温最精
确，距离过近或过远都会增大测温误差。
相对湿度：目标测量环境下的空气相对湿度。请根据实际测量环境配置，默认 70 即表示相
对湿度为 70%，取值范围 [20~100]。湿度值配置是否准确将影响测温精度，但影响度较小。
发射率：被测物体表面以辐射的形式释放能量相对强弱的能力。可参考“常见物质发射率表”
进行配置，由于被测物体表面可能存在腐蚀、氧化等情况，实际发射率值与参考值可能存在
一定差异。发射率配置是否准确将影响测温精度，且影响度较大。
反射温度：周围环境物体辐射的能量可能被待测目标表面反射，与待测目标辐射一同被相机
接收，造成测温误差。如果周围环境没有特别的高温或低温物体，反射温度配置为环境温度
即可。反射温度是否配置准确会影响测温精度，数值与环境温度偏差越大影响度越大。

"""    
    English_note = """Notes: The input file name should not contain spaces, tabs, 
    or other abnormal characters, or it may cause errors!
    Optional input parameters: Measurement distance, ambient humidity, 
    emissivity, reflected temperature.
    Measurement distance: The distance to the target being measured. 
    The infrared thermal imager receives the infrared radiation emitted 
    by the object to generate a thermal image. 
    The farther the distance, the more the infrared radiation attenuates. 
    The default calibration distance of the infrared thermal imager is generally fixed, 
    and the temperature measurement is most accurate at this distance. 
    Measuring too close or too far will increase measurement errors. 
    Relative humidity: The ambient air relative humidity in the target measurement environment. 
    Please configure it according to the actual measurement environment, 
    with a default value of 70, which represents a relative humidity of 70%. 
    The range is [20~100]. The accuracy of humidity configuration will affect 
    temperature measurement accuracy, but the impact is relatively small.
    Emissivity: The ability of the object being measured to release energy 
    in the form of radiation. You can refer to the "Common Emissivity Table" 
    for configuration. Due to possible corrosion or oxidation on the surface 
    of the measured object, the actual emissivity value may differ from the 
    reference value. The accuracy of emissivity configuration will affect 
    temperature measurement accuracy, and the impact is relatively large.
    Reflected temperature: The energy radiated by surrounding objects in 
    the environment may be reflected by the surface of the target being 
    measured and received by the camera together with the target\'s radiation, 
    resulting in temperature measurement errors. If there are no particularly high 
    or low-temperature objects in the surrounding environment, the reflected 
    temperature should be configured as the ambient temperature. The accuracy 
    of reflected temperature configuration will affect temperature measurement 
    accuracy, and the larger the deviation from the ambient temperature, 
    the greater the impact."""
    print(Chinese_note)
    print(English_note)
    print(f"=====================================================\n")




def run(input_dir, output_dir, **kwargs):
    print_help()

    temp_dir = "temp_dir"
    mkdir(temp_dir)
    mkdir(output_dir)

    # ----------------- get input file path list -----------------
    input_file_path_list = []
    for root, dirs, files in os.walk(input_dir):
        for file in files:
            if file.endswith(".JPG") or file.endswith(".png") or file.endswith(".jpg") or file.endswith(".PNG"):
                input_file_path_list.append(os.path.join(root, file))

    print(f"Program detect {len(input_file_path_list)} raw files in {input_dir}.")
    if len(input_file_path_list) == 0:
        raise ValueError(f"Program detect 0 raw files in {input_dir}.")
    # ----------------- convert jpg to tiff -----------------
    print(f"Start to convert..")
    for input_file_path in tqdm(input_file_path_list):
        img_name = os.path.basename(input_file_path)
        raw_file_path = os.path.join(temp_dir, img_name.split(".")[0]+".raw")
        tiff_file_path = os.path.join(
            output_dir, img_name.split(".")[0] + ".tiff")
        psReturn = jpg2tiff(input_file_path, raw_file_path, **kwargs)

        # get rows and cols in jpg file
        image = Image.open(input_file_path)
        width, height = image.size
        rows, cols = width, height
        img = np.fromfile(raw_file_path, dtype='int16')
        img = img / 10 # raw存储的温度值是实际的10倍
        img = img.reshape(cols, rows)
        im = Image.fromarray(img)
        exif_dict = piexif.load(input_file_path)
        new_exif = {
            '0th': {},
            'Exif': {},
            'GPS': exif_dict['GPS'],
            'Interop': {},
            '1st': {},
            'thumbnail': exif_dict['thumbnail']
        }
        exif_bytes = piexif.dump(new_exif)
        im.save(tiff_file_path, exif=exif_bytes)

    print(f"Program convert {len(input_file_path_list)} raw files to tiff files in {output_dir}.")
    shutil.rmtree(temp_dir)
    print(f"Delete temp dir: {temp_dir}")


if __name__ == "__main__":
    current_dir = os.getcwd()
    os.chdir(current_dir)
    input_dir = "input_dir"
    output_dir = 'out_dir'
    # "distance", "humidity", "emissivity", "reflection"
    run(input_dir, output_dir, distance=10, humidity=70,emissivity=0.95,reflection=40)
