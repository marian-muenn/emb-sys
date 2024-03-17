import builtins
import serial
import time

#MPPT Tester values
pid = b'PID\t0xA04B\r\n'
fw = b'FW\t139\r\n'
ser = b'SER#\tHQ1540TUFJ5\r\n'
v = b'V\t13310\r\n'
i = b'I\t1000\r\n'
vpv = b'VPV\t79530\r\n'
ppv = b'PPV\t26\r\n'
cs = b'CS\t0\r\n'
mppt = b'MPPT\t0\r\n'
err = b'ERR\t0\r\n'
load = b'LOAD\tON\r\n'
il = b'IL\t0\r\n'
h19 = b'H19\t22700\r\n'
h20 = b'H20\t236\r\n'
h21 = b'H21\t734\r\n'
h22 = b'H22\t302\r\n'
h23 = b'H23\t875\r\n'
hsds = b'HSDS\t193\r\n'
checksum = b'Checksum\t\t'
frame = b'\r\n' + pid  + fw  + ser + v + i + vpv + ppv + cs + mppt + err + load\
    + h19 + h20 + h21 + h22 + h23 + hsds + il + checksum 
text = frame.decode()
print(text)
sum = 0
for b in frame:
    sum += b
print(sum % 256)

with serial.Serial('/dev/ttyUSB0', 19200, timeout=1) as serial:
    while(True):
        v = b'V\t13310\r\n'
        frame = b'\r\n' + pid  + fw  + ser + v + i + vpv + ppv + cs + mppt + err + load\
            + h19 + h20 + h21 + h22 + h23 + hsds + il + checksum 
        for x in range(10):
            serial.write(frame)
            time.sleep(1)
            print(serial.read_all())
        v = b'V\t10310\r\n'
        frame = b'\r\n' + pid  + fw  + ser + v + i + vpv + ppv + cs + mppt + err + load\
            + h19 + h20 + h21 + h22 + h23 + hsds + il + checksum 
        for x in range(10):
            serial.write(frame)
            time.sleep(1)
            print(serial.read_all())
