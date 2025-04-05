/*
 * ESP8266红外遥控主控程序
 * 功能：通过物理按键或手机APP控制家电，支持红外信号学习功能
 */

// ################### 头文件与宏定义 ###################
// 启用Blinker的SmartConfig和WiFi功能
#define BLINKER_ESP_SMARTCONFIG				// 一键配网功能
#define BLINKER_WIFI						// 启用WiFi连接

// 引入必要的库文件
#include <Arduino.h>						// Arduino核心库
#include <IRremoteESP8266.h>				// 红外接收库
#include <IRsend.h>							// 红外发送库
#include <Blinker.h>						// 点灯科技物联网库
#include <IRrecv.h>							// 红外接收库
#include <IRutils.h>						// 红外工具库

// 功能切换宏：1 表示启用，0 表示关闭
#define FUNCTION_WIFI 1			// WiFi功能开关（1启用）
#define FUNCTION_ASR 0			// 语音识别功能开关（0关闭）

// ################### 硬件配置 ###################
// 引脚定义
#define SELECT_PIN 12  			// 设备选择引脚
#define TRIGGER_PIN 10 			// 开关触发引脚
const uint16_t kRecvPin = 13; 	// 红外接收引脚
const uint16_t kIrLed = 5;	  	// 使用的GPIO引脚

// 参数配置
#define LONG_PRESS_TIME 1000	 // 长按判定时间(ms)
#define LEARN_BLINK_INTERVAL 200 // LED闪烁间隔
#define LEARN_TIMEOUT 10000		 // 学习超时时间(ms) // NEW
const int khz = 38;				 // NEC协议的载波频率为38kHz

// ################### 设备定义 ###################
// 设备类型枚举
enum DeviceType
{
	AC,
	TV,
	FAN
};
DeviceType currentDevice = AC;  // 初始选择空调

// 红外信号原始数据
#define ACON_LEN (sizeof(ACon) / sizeof(ACon[0]))
#define ACOFF_LEN (sizeof(ACoff) / sizeof(ACoff[0]))
#define TVON_LEN (sizeof(TVon) / sizeof(TVon[0]))

// 红外信号编码数组
// ACon: 空调开机信号
// ACoff: 空调关机信号
// TVon: 电视开关机信号
// FAN: 风扇开关机信号
uint16_t ACon[279] = {8988, 4488, 652, 556, 648, 554, 652, 1656, 650, 1656, 650, 1656, 652, 1656, 650, 1656, 650, 556, 652, 554, 650, 1656, 652, 554, 650, 1658, 650, 554, 650, 556, 650, 554, 650, 554, 652, 554, 650, 554, 652, 554, 652, 554, 650, 556, 650, 554, 650, 556, 650, 556, 650, 556, 650, 554, 650, 556, 650, 554, 650, 1656, 650, 554, 650, 1656, 652, 554, 650, 554, 650, 1656, 650, 554, 650, 19990, 650, 554, 650, 554, 652, 554, 652, 554, 650, 554, 650, 554, 652, 554, 650, 554, 650, 554, 650, 554, 650, 556, 650, 554, 650, 556, 650, 554, 652, 554, 652, 554, 650, 554, 650, 556, 650, 554, 652, 554, 650, 556, 650, 556, 650, 556, 650, 554, 652, 554, 652, 556, 650, 554, 650, 556, 650, 556, 650, 556, 650, 556, 650, 556, 650, 40000, 8990, 4488, 650, 556, 650, 556, 650, 1656, 650, 1656, 652, 1656, 650, 1656, 650, 1656, 652, 554, 650, 554, 650, 1656, 650, 556, 650, 1656, 650, 554, 650, 556, 650, 556, 650, 556, 650, 554, 652, 554, 650, 556, 648, 556, 650, 554, 650, 556, 652, 554, 650, 554, 650, 556, 650, 554, 652, 554, 650, 556, 652, 1654, 652, 1656, 652, 1656, 652, 554, 650, 554, 652, 1656, 652, 554, 650, 19990, 650, 556, 650, 554, 652, 556, 650, 554, 650, 556, 652, 554, 650, 554, 650, 554, 652, 554, 650, 554, 652, 554, 652, 554, 652, 552, 652, 554, 652, 554, 650, 554, 652, 554, 650, 554, 650, 554, 652, 554, 650, 1656, 652, 1656, 652, 554, 650, 556, 650, 554, 650, 554, 652, 556, 652, 554, 652, 1656, 650, 1658, 650, 554, 650, 554, 652}; // COOLIX B2BF80
uint16_t ACoff[279] = {8988, 4490, 650, 556, 650, 556, 650, 1658, 650, 556, 650, 1656, 650, 1656, 650, 1658, 650, 556, 650, 554, 650, 1658, 650, 554, 650, 1656, 650, 554, 652, 554, 650, 556, 650, 554, 650, 556, 650, 554, 650, 556, 650, 554, 652, 554, 650, 554, 652, 554, 652, 554, 652, 554, 652, 554, 652, 554, 652, 554, 652, 1656, 652, 552, 652, 1654, 654, 552, 678, 528, 676, 1630, 676, 528, 678, 19960, 684, 520, 684, 522, 684, 520, 684, 522, 684, 522, 682, 522, 684, 522, 682, 522, 682, 524, 680, 524, 682, 524, 682, 524, 682, 524, 682, 524, 682, 524, 680, 526, 680, 524, 680, 526, 680, 526, 680, 526, 678, 526, 680, 526, 678, 526, 678, 526, 678, 526, 678, 528, 678, 526, 656, 550, 678, 528, 654, 552, 652, 552, 652, 1656, 652, 40000, 8988, 4490, 650, 552, 652, 554, 652, 1656, 650, 554, 652, 1656, 650, 1658, 650, 1658, 650, 552, 652, 554, 652, 1656, 650, 554, 652, 1658, 648, 554, 652, 554, 650, 556, 650, 554, 652, 554, 650, 554, 652, 554, 650, 554, 650, 556, 650, 556, 650, 554, 652, 554, 650, 554, 650, 554, 652, 554, 650, 556, 650, 1680, 628, 1678, 628, 1678, 628, 556, 650, 556, 648, 1680, 628, 556, 648, 20012, 628, 556, 650, 556, 650, 556, 648, 556, 650, 556, 648, 556, 648, 558, 648, 558, 646, 558, 648, 558, 648, 558, 648, 558, 646, 558, 648, 558, 646, 580, 626, 560, 646, 580, 626, 580, 626, 580, 626, 580, 626, 1682, 624, 1680, 626, 580, 626, 578, 626, 580, 626, 580, 624, 580, 626, 580, 626, 1682, 626, 1682, 624, 580, 624, 1684, 622}; // COOLIX B27BE0
uint16_t TVon[67] = {9042, 4450, 604, 546, 602, 526, 602, 526, 602, 526, 602, 526, 602, 546, 602, 1636, 604, 526, 602, 1636, 604, 1636, 602, 1636, 602, 1638, 604, 1634, 602, 1634, 602, 546, 602, 1636, 602, 526, 602, 1658, 602, 526, 600, 526, 602, 1634, 602, 544, 602, 528, 602, 526, 602, 1636, 602, 546, 602, 1636, 602, 1634, 604, 526, 602, 1636, 604, 1634, 602, 1636, 602}; // NEC 2FD48B7

// ################### 物联网配置 ###################
BlinkerButton Button1("btn-led");			 // LED控制按钮
BlinkerButton Button2("btn-acon");			 // 空调开
BlinkerButton Button3("btn-acoff");			 // 空调关
BlinkerButton Button4("btn-tv");			 // 电视开关
BlinkerButton Button5("btn-fan");			 // 风扇开关
char auth[] = "3496f0d8897e"; // 设备密钥，用于连接点灯Blinker APP

// ################### 全局变量 ###################
const uint16_t kCaptureBufferSize = 1024;
decode_results results;						 // 解码结果存储
IRrecv irrecv(kRecvPin, kCaptureBufferSize); // NEW
IRsend irsend(kIrLed);						 // 发射对象
unsigned long lastButtonPressTime = 0;

// 状态标志
bool isProcessing = false;              	 // 防止重复执行的标志位
bool isLearningMode = false;				 // 学习模式标志
bool isTriggerPressed = false;
bool isSelectPressed = false;

unsigned long lastCommandTime = 0;      	 // 上次指令执行时间
const unsigned long MIN_INTERVAL = 2000; 	 // 最小间隔时间（毫秒）
unsigned long buttonPressTime = 0; 			// 按键按下时间记录
uint16_t learnedRawData[555];     			 // 学习到的红外数据
uint16_t learnedDataLength = 0;    			// 学习数据长度

// ################### 函数实现 ###################
// 启动学习模式
void startLearningMode()
{
	isLearningMode = true;
	learnedDataLength = 0;
	irrecv.enableIRIn();
	Serial.println("\n>>> 进入学习模式 <<<");
}
void stopLearningMode()
{
	isLearningMode = false;
	digitalWrite(LED_BUILTIN, LOW);
	Serial.println("\n>>> 退出学习模式 <<<");
}

void processLearning()
{
	static unsigned long learnStartTime = 0;

	if (learnStartTime == 0)
	{
		learnStartTime = millis();
		Serial.println("请对准红外接收头按下遥控器...");
	}

	if (irrecv.decode(&results))
	{
		// 成功接收信号
		learnedDataLength = results.rawlen - 1;
		for (uint16_t i = 1; i <= learnedDataLength; i++)
		{
			learnedRawData[i - 1] = results.rawbuf[i] * kRawTick;
		}
		Serial.printf("已学习 %d 个时序数据\n", learnedDataLength);
		irrecv.resume();
		stopLearningMode();
		learnStartTime = 0;
	}
	else if (millis() - learnStartTime > LEARN_TIMEOUT)
	{
		// 10秒超时
		Serial.println("\n!!! 学习超时 !!!");
		stopLearningMode();
		learnStartTime = 0;
	}
}

// 发送红外信号封装函数
void sendIRSignal(const uint16_t *signal, size_t length, int khz)
{
	if (!isProcessing && (millis() - lastCommandTime >= MIN_INTERVAL))
	{
		isProcessing = true;
		irsend.sendRaw(signal, length, khz);
		lastCommandTime = millis();
		isProcessing = false;
	}
}

// ################### 回调函数 ###################
// 按钮1回调：切换板载LED
void button1_callback(const String &state)
{
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
	Button1.print("LED切换");
}
// 按钮2回调：发送空调开机信号
void button2_callback(const String &state)
{
	Button2.print("空调开");
	irsend.sendRaw(ACon, sizeof(ACon) / sizeof(ACon[0]), khz); // 发送原始红外信号										   // 延时2秒，确保信号发送完成
}
// 空调关
void button3_callback(const String &state)
{
	Button3.print("空调关");
	irsend.sendRaw(ACoff, sizeof(ACoff) / sizeof(ACoff[0]), khz);
}
// 电视开关
void button4_callback(const String &state)
{
	// 发送电视开机信号
	irsend.sendRaw(TVon, sizeof(TVon) / sizeof(TVon[0]), khz);
	Button4.print("电视开关");
}
// 风扇开关
void button5_callback(const String &state)
{
	digitalWrite(LED_BUILTIN, HIGH);
	Button5.print("风扇开关");
}



// 数据读取回调函数，打印接收到的数据
void dataRead(const String &data)
{
	BLINKER_LOG("Blinker readString: ", data);
}

// ################### 主程序 ###################
void setup()
{

	irsend.begin();		 			 // 初始化红外发送模块
	irrecv.enableIRIn(); 			 // 初始化irrecv接收的这个库，必须要有
	pinMode(kRecvPin, INPUT);

	Serial.begin(115200);		 	 // 初始化串口通信，设置波特率
	BLINKER_DEBUG.stream(Serial); 	 // 将调试信息输出到串口

	pinMode(LED_BUILTIN, OUTPUT); 	 // 配置内置LED引脚为输出模式，并初始化为低电平
	digitalWrite(LED_BUILTIN, LOW);
	pinMode(SELECT_PIN, INPUT_PULLUP);  // 内部上拉电阻
	pinMode(TRIGGER_PIN, INPUT_PULLUP);

	// 初始化Blinker库并绑定回调函数
	// Blinker.begin(auth, ssid, pswd);
	Blinker.begin(auth);
	Button1.attach(button1_callback);
	Button2.attach(button2_callback);
	Button3.attach(button3_callback);
	Button4.attach(button4_callback);
	Button5.attach(button5_callback);
	Blinker.attachData(dataRead);
}

/**
 * @brief 主循环函数，持续处理设备交互逻辑
 *
 * 本函数负责处理以下核心功能：
 * 1. 设备选择按钮的状态检测及模式切换
 * 2. 触发按钮的短按/长按动作处理
 * 3. 学习模式下的LED状态指示和信号处理
 * 4. WiFi功能模块的运行维护（条件编译）
 * 5. 语音识别指令处理（条件编译）
 * 6. 全局延时去抖处理
 *
 * 注：本函数在Arduino框架下自动循环调用，无参数和返回值
 */
void loop()
{ 
	/*---- 设备选择按钮处理 ----*/
	// 设备选择逻辑（去抖处理）
	bool selectPressed = digitalRead(SELECT_PIN) == LOW;

	// 按下瞬间记录时间
	if (selectPressed && !isSelectPressed)
	{
		buttonPressTime = millis();
	}

	// 处理按钮释放动作，根据按压时长切换模式或设备
	if (!selectPressed && isSelectPressed)
	{
		unsigned long duration = millis() - buttonPressTime;

		// 长按触发学习模式
		if (duration > LONG_PRESS_TIME)
		{
			startLearningMode();
		}
		else // 短按循环切换设备
		{
			currentDevice = static_cast<DeviceType>((currentDevice + 1) % 3);
			String deviceName = currentDevice == AC ? "空调" : currentDevice == TV ? "电视"
																				   : "风扇";
			BLINKER_LOG("切换到: ", deviceName);
			Serial.println("当前设备: " + deviceName);

			// 新增LED反馈逻辑
			digitalWrite(LED_BUILTIN, HIGH); // 点亮LED
			delay(100);						 // 保持100ms
			digitalWrite(LED_BUILTIN, LOW);	 // 关闭LED
		}
	}
	isSelectPressed = selectPressed;

	/*---- 触发按钮处理 ----*/
	bool triggerPressed = digitalRead(TRIGGER_PIN) == LOW;
	// 检测触发按钮按下动作
	if (triggerPressed && !isTriggerPressed)
	{
		buttonPressTime = millis(); // 记录按下时刻
		isTriggerPressed = true;
		Serial.println("按键按下");
	}
	// 处理触发按钮释放动作
	else if (!triggerPressed && isTriggerPressed)
	{
		unsigned long duration = millis() - buttonPressTime;
		isTriggerPressed = false;

		// 长按发送学习记录的红外信号
		if (duration > LONG_PRESS_TIME)
		{
			if (learnedDataLength > 0)
			{
				Serial.printf("[长按] 发送学习信号（时长:%lums)\n", duration);
				sendIRSignal(learnedRawData, learnedDataLength, khz);
			}
		}
		else // 短按发送当前设备预设信号
		{
			Serial.printf("[短按] 设备操作（时长:%lums)\n", duration);
			switch (currentDevice)
			{
			case AC:
				sendIRSignal(ACon, ACON_LEN, khz);
				break;
			case TV:
				sendIRSignal(TVon, TVON_LEN, khz);
				break;
			case FAN: /* 风扇逻辑 */
				break;
			}
		}
	}
	isTriggerPressed = triggerPressed;

	/*---- 学习模式状态处理 ----*/
	if (isLearningMode)
	{
		// LED指示灯闪烁逻辑
		static unsigned long lastBlink = 0;
		if (millis() - lastBlink > LEARN_BLINK_INTERVAL)
		{
			digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
			lastBlink = millis();
		}
		// 红外信号学习处理
		processLearning();
	}

#if FUNCTION_WIFI
	// 维护WiFi连接和云服务通信
	Blinker.run();
#endif

#if FUNCTION_ASR
	/*---- 语音指令处理 ----*/
	if (Serial.available())
	{
		uint8_t incomingByte = Serial.read();

		// 根据协议字节执行对应设备操作
		switch (incomingByte)
		{
			case 0x11:  // 空调开机
				sendIRSignal(ACon, ACON_LEN, khz);
				break;
			case 0x10:  // 空调关机
				sendIRSignal(ACoff, ACOFF_LEN, khz);
				break;
			case 0x21:  // 电视开关
			case 0x20:
				sendIRSignal(TVon, TVON_LEN, khz);
				break;		
			case 0x31: // 风扇开关
			case 0x30:
				break;
			default:
				break;
		}
	}

#endif
	// 全局延时用于按钮去抖处理
	delay(20);
}