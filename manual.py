import serial
import time


def movement(q):
    if q == '0':
        st_num = 'm'
        dir = 'b'
    elif q == '1':
        st_num = 'm'
        dir = 'f'
    elif q == '2':
        st_num = 'n'
        dir = 'r'
    elif q == '3':
        st_num = 'n'
        dir = 'l'
    table_hc6.write(dir.encode('utf-8'))
    time.sleep(0.05)
    table_hc6.write(st_num.encode('utf-8'))


if __name__ == '__main__':
    table_hc6 = serial.Serial('COM3')
    while True:
        print('way = ')
        way = input()
        if way == 'q':
            break
        movement(way)


