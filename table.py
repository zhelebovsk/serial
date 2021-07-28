import serial
import time
import cv2 as cv
import os


def nothing(a):
    pass

if __name__ == '__main__':
    os.chdir('data')
    vid = cv.VideoCapture(0)
    m = 4
    n = 6
    num = 0
    pos = []
    table_hc6 = serial.Serial('COM10')
    for j in range(n):
        for i in range(m):
            if j % 2 == 0:
                pos.append([i, j])
            else:
                pos.append([m - 1 - i, j])

    current_pos = pos.pop(0)
    move = [0, 0]
    ret, image = vid.read()
    cv.imwrite(str(num) + '.' + str(current_pos) + '.bmp', image)
    for i in pos:
        num += 1
        move[0] = i[0] - current_pos[0]
        move[1] = i[1] - current_pos[1]
        if move[0] != 0:
            if move[0] == 1:
                dir = 'b'
            else:
                dir = 'f'
        else:
            if move[1] == 1:
                dir = 'r'
            else:
                dir = 'l'
        table_hc6.write(dir.encode('utf-8'))
        time.sleep(0.05)
        table_hc6.write('4'.encode('utf-8'))
        time.sleep(1)
        for s in range(5):
            ret, image = vid.read()
            cv.imwrite(str(num) + '.' + str(i) + str(s) + '.bmp', image)
        time.sleep(1)
        current_pos = i


#vid = cv.VideoCapture(0)
#dir = 'b'
#st = '4'

#window = 'original'
#cv.namedWindow(window)
#cv.createTrackbar('Gauss', window, 5, 30, nothing)
