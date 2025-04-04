// 定义宏以启用SmartConfig和WiFi功能
#define BLINKER_ESP_SMARTCONFIG
#define BLINKER_WIFI

// 引入必要的库文件
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <Blinker.h>

// 功能切换宏：1 表示启用，0 表示关闭
#define FUNCTION_WIFI 0  	// WiFi
#define FUNCTION_ASR 0   	// 语音识别

#define ACON_LEN (sizeof(ACon) / sizeof(ACon[0]))
#define ACOFF_LEN (sizeof(ACoff) / sizeof(ACoff[0]))
#define TVON_LEN (sizeof(TVon) / sizeof(TVon[0]))

#define SELECT_PIN 12   // 设备选择引脚
#define TRIGGER_PIN 10 // 开关触发引脚

// 新增设备状态变量
enum DeviceType
{
	AC,
	TV,
	FAN
};
DeviceType currentDevice = AC;  // 初始选择空调
bool isSelectPressed = false;
bool isTriggerPressed = false;


// 红外相关定义
const uint16_t kIrLed = 5;  // 使用的GPIO引脚，推荐使用D2（GPIO4）
const int khz = 38;			// NEC协议的载波频率为38kHz
IRsend irsend(kIrLed);      // 初始化红外发送对象

// 红外信号编码数组
// ACon: 空调开机信号
// ACoff: 空调关机信号
// TVon: 电视开关机信号
// FAN: 风扇开关机信号
uint16_t ACon[279] = {8988, 4488, 652, 556, 648, 554, 652, 1656, 650, 1656, 650, 1656, 652, 1656, 650, 1656, 650, 556, 652, 554, 650, 1656, 652, 554, 650, 1658, 650, 554, 650, 556, 650, 554, 650, 554, 652, 554, 650, 554, 652, 554, 652, 554, 650, 556, 650, 554, 650, 556, 650, 556, 650, 556, 650, 554, 650, 556, 650, 554, 650, 1656, 650, 554, 650, 1656, 652, 554, 650, 554, 650, 1656, 650, 554, 650, 19990, 650, 554, 650, 554, 652, 554, 652, 554, 650, 554, 650, 554, 652, 554, 650, 554, 650, 554, 650, 554, 650, 556, 650, 554, 650, 556, 650, 554, 652, 554, 652, 554, 650, 554, 650, 556, 650, 554, 652, 554, 650, 556, 650, 556, 650, 556, 650, 554, 652, 554, 652, 556, 650, 554, 650, 556, 650, 556, 650, 556, 650, 556, 650, 556, 650, 40000, 8990, 4488, 650, 556, 650, 556, 650, 1656, 650, 1656, 652, 1656, 650, 1656, 650, 1656, 652, 554, 650, 554, 650, 1656, 650, 556, 650, 1656, 650, 554, 650, 556, 650, 556, 650, 556, 650, 554, 652, 554, 650, 556, 648, 556, 650, 554, 650, 556, 652, 554, 650, 554, 650, 556, 650, 554, 652, 554, 650, 556, 652, 1654, 652, 1656, 652, 1656, 652, 554, 650, 554, 652, 1656, 652, 554, 650, 19990, 650, 556, 650, 554, 652, 556, 650, 554, 650, 556, 652, 554, 650, 554, 650, 554, 652, 554, 650, 554, 652, 554, 652, 554, 652, 552, 652, 554, 652, 554, 650, 554, 652, 554, 650, 554, 650, 554, 652, 554, 650, 1656, 652, 1656, 652, 554, 650, 556, 650, 554, 650, 554, 652, 556, 652, 554, 652, 1656, 650, 1658, 650, 554, 650, 554, 652}; // COOLIX B2BF80
uint16_t ACoff[279] = {8988, 4490, 650, 556, 650, 556, 650, 1658, 650, 556, 650, 1656, 650, 1656, 650, 1658, 650, 556, 650, 554, 650, 1658, 650, 554, 650, 1656, 650, 554, 652, 554, 650, 556, 650, 554, 650, 556, 650, 554, 650, 556, 650, 554, 652, 554, 650, 554, 652, 554, 652, 554, 652, 554, 652, 554, 652, 554, 652, 554, 652, 1656, 652, 552, 652, 1654, 654, 552, 678, 528, 676, 1630, 676, 528, 678, 19960, 684, 520, 684, 522, 684, 520, 684, 522, 684, 522, 682, 522, 684, 522, 682, 522, 682, 524, 680, 524, 682, 524, 682, 524, 682, 524, 682, 524, 682, 524, 680, 526, 680, 524, 680, 526, 680, 526, 680, 526, 678, 526, 680, 526, 678, 526, 678, 526, 678, 526, 678, 528, 678, 526, 656, 550, 678, 528, 654, 552, 652, 552, 652, 1656, 652, 40000, 8988, 4490, 650, 552, 652, 554, 652, 1656, 650, 554, 652, 1656, 650, 1658, 650, 1658, 650, 552, 652, 554, 652, 1656, 650, 554, 652, 1658, 648, 554, 652, 554, 650, 556, 650, 554, 652, 554, 650, 554, 652, 554, 650, 554, 650, 556, 650, 556, 650, 554, 652, 554, 650, 554, 650, 554, 652, 554, 650, 556, 650, 1680, 628, 1678, 628, 1678, 628, 556, 650, 556, 648, 1680, 628, 556, 648, 20012, 628, 556, 650, 556, 650, 556, 648, 556, 650, 556, 648, 556, 648, 558, 648, 558, 646, 558, 648, 558, 648, 558, 648, 558, 646, 558, 648, 558, 646, 580, 626, 560, 646, 580, 626, 580, 626, 580, 626, 580, 626, 1682, 624, 1680, 626, 580, 626, 578, 626, 580, 626, 580, 624, 580, 626, 580, 626, 1682, 626, 1682, 624, 580, 624, 1684, 622}; // COOLIX B27BE0
uint16_t TVon[67] = {9042, 4450, 604, 546, 602, 526, 602, 526, 602, 526, 602, 526, 602, 546, 602, 1636, 604, 526, 602, 1636, 604, 1636, 602, 1636, 602, 1638, 604, 1634, 602, 1634, 602, 546, 602, 1636, 602, 526, 602, 1658, 602, 526, 600, 526, 602, 1634, 602, 544, 602, 528, 602, 526, 602, 1636, 602, 546, 602, 1636, 602, 1634, 604, 526, 602, 1636, 604, 1634, 602, 1636, 602}; // NEC 2FD48B7

// WiFi相关定义
BlinkerButton Button1("btn-led");
BlinkerButton Button2("btn-acon");
BlinkerButton Button3("btn-acoff");
BlinkerButton Button4("btn-tv");
BlinkerButton Button5("btn-fan");

char auth[] = "3496f0d8897e"; // 设备密钥，用于连接点灯Blinker APP

// 全局变量
bool isProcessing = false;               // 防止重复执行的标志位
unsigned long lastCommandTime = 0;       // 上次指令执行时间
const unsigned long MIN_INTERVAL = 2000; // 最小间隔时间（毫秒）

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

//LED状态
void button1_callback(const String &state)
{
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
	Button1.print("LED切换");
}
// 空调开
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


void setup()
{
	// 初始化红外发送模块
	irsend.begin();

	// 初始化串口通信，设置波特率
	Serial.begin(115200);
	BLINKER_DEBUG.stream(Serial); // 将调试信息输出到串口
	
	// 配置内置LED引脚为输出模式，并初始化为低电平
	pinMode(LED_BUILTIN, OUTPUT);
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


void loop()
{
	// 设备选择逻辑（去抖处理）
	bool selectPressed = digitalRead(SELECT_PIN) == LOW;
	if (selectPressed && !isSelectPressed)
	{
		currentDevice = static_cast<DeviceType>((currentDevice + 1) % 3);
        BLINKER_LOG("切换到: ", currentDevice == AC ? "空调" : currentDevice == TV ? "电视": "风扇");
    }

	isSelectPressed = selectPressed;
	// 开关触发逻辑（去抖处理）
	bool triggerPressed = digitalRead(TRIGGER_PIN) == LOW;
	if (triggerPressed && !isTriggerPressed)
	{
		switch (currentDevice)
		{
		case AC:
            Serial.println("触发空调信号");   
            sendIRSignal(ACon, ACON_LEN, khz); // 发送空调开机信号
			break;
		case TV:
            Serial.println("触发电视信号");
            sendIRSignal(TVon, TVON_LEN, khz); // 发送电视开机信号
			break;
		case FAN:
            Serial.println("触发风扇信号");
            // sendIRSignal(FCon, FCON_LEN, khz); // 需补充风扇信号数组
			break;
		}
	}
	isTriggerPressed = triggerPressed;

#if FUNCTION_WIFI
	Blinker.run();
#endif

#if FUNCTION_ASR
	// 语音识别功能
	if (Serial.available())
	{
		uint8_t incomingByte = Serial.read();

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


}