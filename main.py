# encoding: utf-8
# @author: Shenzhou Liu
# @email: 913737515@qq.com

import os
import shutil
import subprocess
import piexif
import numpy as np
from tqdm import tqdm
from PIL import Image

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


def jpg2tiff(inputpath, outpath):
    command = "dji_thermal_sdk_v1.4_20220929/utility/bin/windows/release_x64/dji_irp.exe -s " + \
              inputpath + " -a measure -o " + outpath
    args = [r"C:\WINDOWS\system32\WindowsPowerShell\v1.0\powershell.exe", command]
    ps = subprocess.Popen(args, stdout=subprocess.PIPE)
    psReturn = ps.stdout.read()
    return psReturn


def run(input_dir, output_dir):
    print(f"=====================================================\n")
    print(f"注意事项：输入的文件名不能含有空格，Tab等异常字符，否则会报错！\n\
Attention: The input file name cannot contain any abnormal \n\
characters such as spaces or tabs, otherwise an error will be reported")
    print(f"=====================================================\n")
    temp_dir = "temp_dir"
    mkdir(temp_dir)
    mkdir(output_dir)


    input_file_path_list = []
    for root, dirs, files in os.walk(input_dir):
        for file in files:
            if file.endswith(".JPG") or file.endswith(".png") or file.endswith(".jpg") or file.endswith(".PNG"):
                input_file_path_list.append(os.path.join(root, file))

    print(f"Program detect {len(input_file_path_list)} raw files in {input_dir}.")

    print(f"Start to convert..")
    for input_file_path in tqdm(input_file_path_list):
        img_name = os.path.basename(input_file_path)
        raw_file_path = os.path.join(temp_dir, img_name.split(".")[0]+".raw")
        tiff_file_path = os.path.join(
            output_dir, img_name.split(".")[0] + ".tiff")
        psReturn = jpg2tiff(input_file_path, raw_file_path)

        # get rows and cols in jpg file
        image = Image.open(input_file_path)
        width, height = image.size

        rows, cols = width, height
        img = np.fromfile(raw_file_path, dtype='int16')
        img = img / 10
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
    run(input_dir, output_dir)
