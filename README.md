# 前言
帮朋友做的，基于ESP的红外遥控器，语音模块采用天问，整个项目比较简单。软件利用arduino库，红外遥控库，联网使用Blinker库

支持以下功能：
1. 按键遥控红外设备。
2. 语音遥控红外设备。
3. 连接WIFI，手机APP遥控红外设备。
4. 模式切换。
5. 支持红外学习模式。

# 器材采购
| 部件     | 型号                 | 用途     | 价格|链接|
| ------ | ------------------ | ------ |-------|-----|
| 主控   | ESP8266（CP2102）    | 系统核心控制 |¥10.5|[link](https://item.taobao.com/item.htm?abbucket=15&id=619991323193) |
| 红外收发模块 | 数字38KHz红外接收发射头     | 红外信号收发 |¥4.14|[link](https://detail.tmall.com/item.htm?id=720024442200)  |
| 语音识别模块 | 天问语音识别模块(ASR_PRO) | 语音指令识别    |¥29.1|[link](https://detail.tmall.com/item.htm?abbucket=15\&id=683860993579) |
| 锂电池x2      | 3.7V锂电池（PH2.0-500mAh）| 系统供电         | ¥15.0   | [Link](https://item.taobao.com/item.htm?id=714971603325&skuId=4997690252351)       |
| 按键   | 轻触开关6x6x6_smd              | 切换遥控对象 |¥2.97| [link](https://item.jd.com/10064231060192.html#crumb-wrap)| 
| 电源芯片   | pw5100-5v             | 锂电池升压 | ¥5+8 | [link](https://i-item.jd.com/10102424115848.html)| 
| 功率电感     |  0420-3.3uH     | 滤波   |¥5 |[link](https://ic-item.jd.com/10133696057167.html#none)| 
| 电源电容     |  22uF     | 滤波   | ¥3.8|[link](https://ic-item.jd.com/10101684718599.html#crumb-wrap)| 
| 电源电阻    |  0.5R     | 反馈   | ¥4.14 |[link](https://item.jd.com/10145360412447.html)| 

---

# 系统框图
![image](https://tuchuang.heiweilu.top/picture/20250306212307.png)

# 原理图设计
![image](https://tuchuang.heiweilu.top/picture/20250401014402.png)
![image](https://tuchuang.heiweilu.top/picture/20250401014252.png)

# 引脚分配
![image](https://tuchuang.heiweilu.top/picture/20250401014552.png)

# 设计流程图

```mermaid
graph TD
A[需求确认与器件选型] --> B[绘制原理图]
B --> C[PCB制板]
C --> D[元器件采购]
D --> E[元器件焊接]
E --> F[硬件调试]

```

# 软件架构框图:

```mermaid
graph TD

    subgraph 红外遥控模块
        B1[开源红外遥控库集成] --> B2[红外信号接收/发送]
        B3[预设红外编码库] --> B2
        B4[自定义学习功能] --> B2
        B5[按键控制接口] --> B2
    end

    subgraph 语音识别模块
        C1[语音识别硬件接口] --> C2[语音指令解析]
        C2 --> C3[指令执行接口]
    end

    subgraph WIFI功能模块
        D1[WIFI连接管理] --> D2[云端通信接口]
        D3[手机APP控制界面] --> D2
    end
    
    A[中央控制单元<br>] --> B[红外遥控模块]
    A --> C[语音识别模块]
    A --> D[WIFI功能模块]
    A --> E[独立按键]
```

---

# 博客链接
[https://github.com/heiweilu/ESP8266_Infrared](https://www.heiweilu.top/index.php/diyesphongwaiyaokongqi/)
