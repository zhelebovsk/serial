import serial
import time

def serial_ports_list():
    ports = ['COM%s' % (i + 1) for i in range(11)]
    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result


if __name__ == '__main__':

    print(serial_ports_list())
    table_hc6 = serial.Serial('COM10')
    while True:
        table_hc6.write('l'.encode('utf-8'))
        table_hc6.write('4'.encode('utf-8'))
        time.sleep(1)
        table_hc6.write('f'.encode('utf-8'))
        table_hc6.write('4'.encode('utf-8'))
        time.sleep(1)
        table_hc6.write('r'.encode('utf-8'))
        table_hc6.write('4'.encode('utf-8'))
        time.sleep(1)
        table_hc6.write('b'.encode('utf-8'))
        table_hc6.write('4'.encode('utf-8'))
        time.sleep(1)
