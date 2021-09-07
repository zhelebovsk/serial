import serial

if __name__ == '__main__':
    table_hc6 = serial.Serial('COM3')
    table_hc6.write('f'.encode('utf-8'))
