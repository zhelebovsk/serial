import serial
import time
import cv2 as cv
import os
import pytesseract
import csv
import numpy as np


def read_data(vid, x0, x1, y0, y1, gauss, kernel, custom_config, lang, num, current_pos):
    velocity = []
    i = 0
    while len(velocity) < 12:
        i += 1
        details = []
        ch = 1
        while ch:
            ret, image = vid.read()
            image = image[x0:x1, y0:y1]
            gray = cv.cvtColor(image, cv.COLOR_BGR2GRAY)
            if gauss > 0:
                gray = cv.blur(gray, (gauss, gauss))
            # провека оттенков серого !!! подобрать диапазон по cam.py
            if np.mean(gray) < 100:
                continue
            if np.mean(gray) > 150:
                continue
            # конец проверки
            thresh = cv.threshold(gray, 0, 255,
                                  cv.THRESH_BINARY | cv.THRESH_OTSU)[1]
            if kernel > 0:
                k = cv.getStructuringElement(cv.MORPH_ELLIPSE, (kernel, kernel))
                thresh = cv.erode(thresh, k, iterations=1)
            # проверка чб изображения !!! подобрать диапазон по cam.py
            if np.mean(thresh) < 150:
                continue
            if np.mean(thresh) > 220:
                continue
            # конец проверки
            details = pytesseract.image_to_string(thresh,
                                                  config=custom_config,
                                                  lang=lang)
            ch = 0
        cv.putText(image, details[0:3],
                   (50, 50),
                   cv.FONT_HERSHEY_SIMPLEX,
                   1,
                   (0, 255, 0),
                   2)
        cv.imwrite(str(num) + '.' + str(current_pos) + str(i) + '.bmp', image)
        velocity.append(details)
        time.sleep(1.1)
    print(velocity)
    return velocity


if __name__ == '__main__':
    table_hc6 = serial.Serial('COM3')
    # tesseract params
    lang = 'lets'
    custom_config = r'--oem 3 --psm 6 -c tessedit_char_whitelist=0123456789'
    os.chdir('data')
    # cam init
    vid = cv.VideoCapture(0)
    time.sleep(3)
    vid.set(cv.CAP_PROP_FRAME_WIDTH, 1920)
    vid.set(cv.CAP_PROP_FRAME_HEIGHT, 1080)
    # check correct params with cam.py
    gauss = 3
    kernel = 3
    x0 = 440
    x1 = 538
    y0 = 827
    y1 = 1015
    # field and points
    #m = 21
    #n = 5
    m = 3
    n = 3
    num = 0
    pos = []
    # open new csv file
    f = open('text.txt', 'w')
    for j in range(n):
        for i in range(m):
            if j % 2 == 0:
                pos.append([i, j])
            else:
                pos.append([m - 1 - i, j])
    current_pos = pos.pop(0)
    move = [0, 0]

    vel = read_data(vid, x0, x1, y0, y1, gauss, kernel, custom_config, lang, num, current_pos)
    f.write(str(num) + '\t' + str(current_pos[1:-1]) + '\t')
    for v in vel:
        f.write(v[0] + '.' + v[1:3] + '\t')
    f.write('\n')

    for i in pos:
        num += 1
        move[0] = i[0] - current_pos[0]
        move[1] = i[1] - current_pos[1]
        if move[0] != 0:
            st_num = 'm'
            if move[0] == 1:
                dir = 'b'
            else:
                dir = 'f'
        else:
            st_num = 'n'
            if move[1] == 1:
                dir = 'r'
            else:
                dir = 'l'
        table_hc6.write(dir.encode('utf-8'))
        time.sleep(0.05)
        table_hc6.write(st_num.encode('utf-8'))
        time.sleep(7)
        vel = read_data(vid, x0, x1, y0, y1, gauss, kernel, custom_config, lang, num, i)
        f.write(str(num) + '\t' + str(i[1:-1]) + '\t')
        for v in vel:
            f.write(v[0] + '.' + v[1:3] + '\t')
            #f.write(v[0:3] + '\t')
        f.write('\n')
        time.sleep(0.1)
        current_pos = i
    f.close()
