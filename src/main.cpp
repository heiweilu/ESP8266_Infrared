// 定义宏以启用SmartConfig和WiFi功能
#define BLINKER_ESP_SMARTCONFIG
#define BLINKER_WIFI

// 引入必要的库文件
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <Blinker.h>

// 功能切换宏：1 表示启用，0 表示关闭
#define FUNCTION_AC 0
#define FUNCTION_TV 0
#define FUNCTION_FIR 0
#define FUNCTION_WIFI 0
#define FUNCTION_ASR 1

// 红外相关定义
const uint16_t kIrLed = 4;  // 使用的GPIO引脚，推荐使用D2（GPIO4）
int khz = 38;               // NEC协议的载波频率为38kHz
IRsend irsend(kIrLed);      // 初始化红外发送对象

// 红外信号编码数组
// ACon: 空调开机信号
// ACoff: 空调关机信号
// TVon: 电视开机信号
uint16_t ACon[279] = {8988, 4488, 652, 556, 648, 554, 652, 1656, 650, 1656, 650, 1656, 652, 1656, 650, 1656, 650, 556, 652, 554, 650, 1656, 652, 554, 650, 1658, 650, 554, 650, 556, 650, 554, 650, 554, 652, 554, 650, 554, 652, 554, 652, 554, 650, 556, 650, 554, 650, 556, 650, 556, 650, 556, 650, 554, 650, 556, 650, 554, 650, 1656, 650, 554, 650, 1656, 652, 554, 650, 554, 650, 1656, 650, 554, 650, 19990, 650, 554, 650, 554, 652, 554, 652, 554, 650, 554, 650, 554, 652, 554, 650, 554, 650, 554, 650, 554, 650, 556, 650, 554, 650, 556, 650, 554, 652, 554, 652, 554, 650, 554, 650, 556, 650, 554, 652, 554, 650, 556, 650, 556, 650, 556, 650, 554, 652, 554, 652, 556, 650, 554, 650, 556, 650, 556, 650, 556, 650, 556, 650, 556, 650, 40000, 8990, 4488, 650, 556, 650, 556, 650, 1656, 650, 1656, 652, 1656, 650, 1656, 650, 1656, 652, 554, 650, 554, 650, 1656, 650, 556, 650, 1656, 650, 554, 650, 556, 650, 556, 650, 556, 650, 554, 652, 554, 650, 556, 648, 556, 650, 554, 650, 556, 652, 554, 650, 554, 650, 556, 650, 554, 652, 554, 650, 556, 652, 1654, 652, 1656, 652, 1656, 652, 554, 650, 554, 652, 1656, 652, 554, 650, 19990, 650, 556, 650, 554, 652, 556, 650, 554, 650, 556, 652, 554, 650, 554, 650, 554, 652, 554, 650, 554, 652, 554, 652, 554, 652, 552, 652, 554, 652, 554, 650, 554, 652, 554, 650, 554, 650, 554, 652, 554, 650, 1656, 652, 1656, 652, 554, 650, 556, 650, 554, 650, 554, 652, 556, 652, 554, 652, 1656, 650, 1658, 650, 554, 650, 554, 652}; // COOLIX B2BF80
uint16_t ACoff[279] = {8988, 4490, 650, 556, 650, 556, 650, 1658, 650, 556, 650, 1656, 650, 1656, 650, 1658, 650, 556, 650, 554, 650, 1658, 650, 554, 650, 1656, 650, 554, 652, 554, 650, 556, 650, 554, 650, 556, 650, 554, 650, 556, 650, 554, 652, 554, 650, 554, 652, 554, 652, 554, 652, 554, 652, 554, 652, 554, 652, 554, 652, 1656, 652, 552, 652, 1654, 654, 552, 678, 528, 676, 1630, 676, 528, 678, 19960, 684, 520, 684, 522, 684, 520, 684, 522, 684, 522, 682, 522, 684, 522, 682, 522, 682, 524, 680, 524, 682, 524, 682, 524, 682, 524, 682, 524, 682, 524, 680, 526, 680, 524, 680, 526, 680, 526, 680, 526, 678, 526, 680, 526, 678, 526, 678, 526, 678, 526, 678, 528, 678, 526, 656, 550, 678, 528, 654, 552, 652, 552, 652, 1656, 652, 40000, 8988, 4490, 650, 552, 652, 554, 652, 1656, 650, 554, 652, 1656, 650, 1658, 650, 1658, 650, 552, 652, 554, 652, 1656, 650, 554, 652, 1658, 648, 554, 652, 554, 650, 556, 650, 554, 652, 554, 650, 554, 652, 554, 650, 554, 650, 556, 650, 556, 650, 554, 652, 554, 650, 554, 650, 554, 652, 554, 650, 556, 650, 1680, 628, 1678, 628, 1678, 628, 556, 650, 556, 648, 1680, 628, 556, 648, 20012, 628, 556, 650, 556, 650, 556, 648, 556, 650, 556, 648, 556, 648, 558, 648, 558, 646, 558, 648, 558, 648, 558, 648, 558, 646, 558, 648, 558, 646, 580, 626, 560, 646, 580, 626, 580, 626, 580, 626, 580, 626, 1682, 624, 1680, 626, 580, 626, 578, 626, 580, 626, 580, 624, 580, 626, 580, 626, 1682, 626, 1682, 624, 580, 624, 1684, 622}; // GREE uint8_t state[8] = {0x74, 0x0A, 0x00, 0x50, 0x00, 0x00, 0x00, 0x80}; // COOLIX B27BE0
uint16_t TVon[67] = {9042, 4450, 604, 546, 602, 526, 602, 526, 602, 526, 602, 526, 602, 546, 602, 1636, 604, 526, 602, 1636, 604, 1636, 602, 1636, 602, 1638, 604, 1634, 602, 1634, 602, 546, 602, 1636, 602, 526, 602, 1658, 602, 526, 600, 526, 602, 1634, 602, 544, 602, 528, 602, 526, 602, 1636, 602, 546, 602, 1636, 602, 1634, 604, 526, 602, 1636, 604, 1634, 602, 1636, 602}; // NEC 2FD48B7

// WiFi相关定义
#define BUTTON_1 "btn-abc" // 按钮名称
BlinkerButton Button1(BUTTON_1); // 创建按钮对象
char auth[] = "aa0df37ab04a"; // 设备密钥，用于连接点灯Blinker APP

// 全局变量
bool isProcessing = false;               // 防止重复执行的标志位
unsigned long lastCommandTime = 0;       // 上次指令执行时间
const unsigned long MIN_INTERVAL = 2000; // 最小间隔时间（毫秒）


// 按钮回调函数，切换内置LED状态
void button1_callback(const String &state)
{
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
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

    // 初始化串口通信，设置波特率为9600
    Serial.begin(9600);
    BLINKER_DEBUG.stream(Serial); // 将调试信息输出到串口
    
    // 配置内置LED引脚为输出模式，并初始化为低电平
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
    // 初始化Blinker库并绑定回调函数
    Blinker.begin(auth);
    Blinker.attachData(dataRead);
    Button1.attach(button1_callback);
}


void loop()
{
#ifdef FUNCTION_AC
    // 发送空调开机信号
    Serial.println("On");
    irsend.sendRaw(ACon, sizeof(ACon) / sizeof(ACon[0]), khz); // 发送原始红外信号
    delay(2000);            // 延时2秒，确保信号发送完成

    // 延时10秒后发送空调关机信号
    delay(10000);
    Serial.println("Off");
    irsend.sendRaw(ACoff, sizeof(ACoff) / sizeof(ACoff[0]), khz); 
    delay(2000); 
#endif

#ifdef FUNCTION_TV
    // 发送电视开机信号
    Serial.println("On");
    irsend.sendRaw(TVon, sizeof(TVon) / sizeof(TVon[0]), khz); 
    delay(2000);
#endif

#ifdef FUNCTION_WIFI
    Blinker.run();
#endif

#ifdef FUNCTION_ASR
    // 语音识别功能
    if (Serial.available())
    {
        uint8_t incomingByte = Serial.read();

        // 检查是否接收到0x11
        switch (incomingByte)
        {
            case 0x11:
                if (!isProcessing && (millis() - lastCommandTime >= MIN_INTERVAL))
                {
                    isProcessing = true;
                    Serial.println("Received 0x10: AC On");
                    irsend.sendRaw(ACon, sizeof(ACon) / sizeof(ACon[0]), khz);
                    delay(500);                // 发送信号需要的时间
                    lastCommandTime = millis(); // 更新时间戳
                    isProcessing = false;
                }
                break;
            case 0x10:
                if (!isProcessing && (millis() - lastCommandTime >= MIN_INTERVAL))
                {
                    isProcessing = true;
                    Serial.println("Received 0x11: AC Off");
                    irsend.sendRaw(ACoff, sizeof(ACoff) / sizeof(ACoff[0]), khz);
                    delay(500);
                    lastCommandTime = millis();
                    isProcessing = false;
                }
                break;
            case 0x21:
                if (!isProcessing && (millis() - lastCommandTime >= MIN_INTERVAL))
                {
                    isProcessing = true;
                    Serial.println("Received 0x20: TV On");
                    irsend.sendRaw(TVon, sizeof(TVon) / sizeof(TVon[0]), khz);
                    delay(500);
                    lastCommandTime = millis();
                    isProcessing = false;
                }
                break;
            case 0x20:
                if (!isProcessing && (millis() - lastCommandTime >= MIN_INTERVAL))
                {
                    isProcessing = true;
                    Serial.println("Received 0x21: TV Off");                   // 修正原代码错误
                    irsend.sendRaw(TVon, sizeof(TVon) / sizeof(TVon[0]), khz); // 若需关机信号，需替换为对应数组
                    delay(500);
                    lastCommandTime = millis();
                    isProcessing = false;
                }
                break;
            case 0x31:
                if (!isProcessing && (millis() - lastCommandTime >= MIN_INTERVAL))
                {
                    isProcessing = true;
                    Serial.println("Received 0x30");
                    // 添加对应逻辑
                    lastCommandTime = millis();
                    isProcessing = false;
                }
                break;
            case 0x30:
                if (!isProcessing && (millis() - lastCommandTime >= MIN_INTERVAL))
                {
                    isProcessing = true;
                    Serial.println("Received 0x31");
                    // 添加对应逻辑
                    lastCommandTime = millis();
                    isProcessing = false;
                }
                break;
            default:
                break;
        }
    }

#endif
}