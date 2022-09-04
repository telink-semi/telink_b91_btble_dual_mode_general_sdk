## V5.1.1.0(FR)

### Dependency Updates
 * N/A

### Features
* Support two BT ACL connections (One master and one slave)
* Support one BT SCO connection
* Support one BLE ACL connection
* Support A2DP Sink and Source
* Support HFP HF and AG
* Support PBAP-Client
* Support BT-SPP transmission
* Support File System (fat16)
* Support battery detection and low-voltage protection
* Support system monitoring (stack detection, watchdog)
* Support USB Device (Audio MIC and SPK,HID,CDC,MSC)
* Support MP3 decoding
* Support MSBC decoding
* Support CVSD decoding
* Support bluetooth modular management and application management
* Support Audio modular management and application management
* Support Voice Noise Reduction
* Support Acoustic Echo Cancellation
* Support BT-IAP transmission (new)
* Support BT-ATT transmission (new)
* Support BT-HID transmission (new)
* Support three-way conversation (new)
* Support low power consumption (BT has only one connection and serves as a slave role) (new)
* Support OTA function (data channel: BT-ATT, BT-SPP, UART) (new)

### Bug Fixs
* Fix logic bugs related to the SDP Server
* Fix problems related to connections
* Fix repeated log output over the serial port

### BREAKING CHANGES
* Support low power consumption (BT has only one connection and serves as a slave role)
* Support OTA function (data channel: BT-ATT, BT-SPP, UART)
* Optimize part of the code logic
* Enriched interfaces for some functions

### CodeSize
* Flash: 569KB with log and 455KB without log
* RAM: I-RAM is 96640B and D-RAM is 89088B (with log);I -RAM is 94608B and D-RAM is 85664B (without log)

### Dependency Updates
* N/A

### Features
* 支持两路BT ACL连接 (一路master，一路slave)
* 支持一路BT SCO连接
* 支持一路BLE ACL连接
* 支持A2DP Sink和Source
* 支持HFP HF和AG
* 支持PBAP-Client
* 支持BT-SPP传输
* 支持文件系统 (fat16)
* 支持电池检测和低压保护
* 支持系统监测（堆栈检测，看门狗）
* 支持USB设备（Audio MIC和SPK,HID,CDC,MSC）
* 支持MP3解码
* 支持MSBC编解码
* 支持CVSD编解码
* 支持蓝牙模块化管理和应用管理
* 支持音频模块化管理和应用管理
* 支持语音降噪
* 支持回音消除
* 支持BT-IAP传输（新增）
* 支持BT-ATT传输（新增）
* 支持BT-HID传输（新增）
* 支持三方会话（新增）
* 支持低功耗（BT只有一路连接，并且作为slave角色）（新增）
* 支持OTA功能（数据通道：BT-ATT、BT-SPP、UART）（新增）

### Bug Fixs
* 修复SDP服务处理相关的问题
* 修复已知连接相关的问题
* 修复串口输出重复log的问题

### BREAKING CHANGES
* 支持低功耗（BT只有一路连接，并且作为slave角色
* 支持OTA功能（数据通道：BT-ATT、BT-SPP、UART）
* 优化部分代码逻辑
* 丰富部分功能接口

### CodeSize
* Flash: 带log版本为569KB，不带log版本为455KB
* RAM: 带log版本，I-RAM为96640B，D-RAM为89088B；不带log版本，I-RAM为94608B，D-RAM为85664B。
