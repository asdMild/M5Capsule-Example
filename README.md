# M5Capsule-Example

arduino esp32s3 M5Capsule IMU platformio Example

include Serial.print
include export velocity

require rcmags/imuFilter@^1.6.3


官方的Capsule的代码会出现在arduino上  serial无法print出来的情况
这个是需要在setup里面手动设置一下 serial.begin

如果需要再platformio下使用
因为board的种类没有覆盖
可以针对 m5stack-stamps3 加几个参数来使用
加
build_flags = 
	-D ARDUINO_USB_MODE=0
	-D ARDUINO_USB_CDC_ON_BOOT=1
可以保证serial.print在platformio下正常使用

推荐全程使用M5而不是M5Capsule，这两个的instance不是同一个，交叉使用会出现问题

