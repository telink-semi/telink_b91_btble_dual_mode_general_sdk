## V5.1.1.2(FR)

### Dependency Updates
* N/A

### Features
* N/A

### Bug Fixs
* N/A

### BREAKING CHANGES
* N/A

### CodeSize
* N/A

### Dependency Updates
* N/A

### Features
* 增加重采样算法
* 增加LED灯管理 （包括PWM）

### Bug Fixs
* 修复Controller工程编译错误的问题；

### BREAKING CHANGES
* 重构codec管理，增加外部codec挂载接口
* 重构debug逻辑，解决库中代码调试困难的问题
* debug与非debug使用同一个库

### CodeSize
* N/A




## V5.1.1.0(FR)

### Dependency Updates
 * N/A

### Features
* Support BT-IAP transmission
* Support BT-ATT transmission
* Support BT-HID transmission
* Support three-way conversation
* Support low power consumption (BT has only one connection and serves as a slave role)
* Support OTA function (data channel: BT-ATT, BT-SPP, UART)

### Bug Fixs
* Fix logic bugs related to the SDP Server
* Fix problems related to connections
* Fix repeated log output over the serial port

### BREAKING CHANGES
* N/A

### CodeSize
* Flash: 569KB with log and 455KB without log
* RAM: I-RAM is 94KB and D-RAM is 87KB (with log);I -RAM is 92KB and D-RAM is 83KB (without log)

### Dependency Updates
* N/A

### Features
* 支持BT-IAP传输
* 支持BT-ATT传输
* 支持BT-HID传输
* 支持三方会话
* 支持低功耗（BT只有一路连接，并且作为slave角色）
* 支持OTA功能（数据通道：BT-ATT、BT-SPP、UART）

### Bug Fixs
* 修复SDP服务处理相关的问题
* 修复已知连接相关的问题
* 修复串口输出重复log的问题

### BREAKING CHANGES
* N/A

### CodeSize
* Flash: 带log版本为569KB，不带log版本为455KB
* RAM: 带log版本，I-RAM为94KB，D-RAM为87KB；不带log版本，I-RAM为92KB，D-RAM为83KB。






## V5.1.0.0(FR)

### Dependency Updates
 * N/A

### Features
* Supports two BT ACL connections
* Support one BT SCO connection
* Support one BLE ACL connection
* Support A2DP Sink and Source
* Support HFP HF and AG
* Support PBAP-Client
* Support SPP transmission
* Support File System
* Support battery detection and low-voltage protection
* Support system monitoring (stack detection, watchdog)
* Support USB Device (Audio,HID,CDC,MSC)
* Support MP3 decoding
* Support MSBC decoding
* Support CVSD decoding
* Support bluetooth modular management and application management
* Support Audio modular management and application management
* Support Voice Noise Reduction
* Support Acoustic Echo Cancellation

### Bug Fixs
* N/A

### BREAKING CHANGES
* N/A

### CodeSize
* Flash: 488KB with log and 397KB without log
* RAM: I-RAM is 92112B and D-RAM is 86824B (without log); I-RAM is 88792B and D-RAM is 83968B (with log)

### Dependency Updates
* N/A

### Features
* 支持两路BT ACL连接
* 支持一路BT SCO连接
* 支持一路BLE ACL连接
* 支持A2DP Sink和Source
* 支持HFP HF和AG
* 支持PBAP-Client
* 支持SPP传输
* 支持文件系统
* 支持电池检测和低压保护
* 支持系统监测（堆栈检测，看门狗）
* 支持USB设备（Audio,HID,CDC,MSC）
* 支持MP3解码
* 支持MSBC编解码
* 支持CVSD编解码
* 支持蓝牙模块化管理和应用管理
* 支持音频模块化管理和应用管理
* 支持语音降噪
* 支持回音消除

### Bug Fixs
* N/A

### BREAKING CHANGES
* N/A

### CodeSize
* Flash: 带log版本为488KB，不带log版本为397KB
* RAM: 带log版本，I-RAM为92112B，D-RAM为86824B；不带log版本，I-RAM为88792B，D-RAM为83968B。



