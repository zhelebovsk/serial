import serial
import time
import cv2 as cv
import os


def nothing(a):
    pass

if __name__ == '__main__':
    os.chdir('data')
    vid = cv.VideoCapture(0)
    m = 21
    n = 5
    num = 0
    pos = []
    table_hc6 = serial.Serial('COM3')
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
        time.sleep(8)
        #for s in range(5):
        ret, image = vid.read()
        cv.imwrite(str(num) + '.' + str(i) + '.bmp', image)
        #cv.imwrite(str(num) + '.' + str(i) + str(s) + '.bmp', image)
        #time.sleep(1)
        current_pos = i


#vid = cv.VideoCapture(0)
#dir = 'b'
#st = '4'

#window = 'original'
#cv.namedWindow(window)
#cv.createTrackbar('Gauss', window, 5, 30, nothing)
