// Written by Sh1n3zZ on July 1, 2023.
// Visit https://log.lightxi.com/Health.html to learn more.

#include <SoftwareSerial.h>
 
SoftwareSerial mySerial(13, 12); // RX, TX  通过软串口连接esp8266，


/******************************************************************************/
String ssid ="class01";                                       //WIFI名称
String password="12345678";                                  //WIFI密码
String uid = "c07cda929236221df33d6b90d73b35db";             // 用户私钥，巴法云控制台获取
String topic = "Health";                                    //推送消息的主题，即往哪个主题推送



void setup() {
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  // 初始化 A0 A1 两个端口输入
  // A0 端口输入为 MQ-135 / A1 端口输入为 MQ-7
  // MQ-135 用于检测空气中有害气体浓度，如二氧化碳、甲醛、苯等
  // MQ-7 是一种测量空气中一氧化碳（CO）浓度的传感器
  
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
 
  mySerial.begin(115200);
  mySerial.println("AT+RST");   // 初始化重启一次esp8266
  delay(1500);
  echo();
  mySerial.println("AT");
  echo();
  delay(500);
  mySerial.println("AT+CWMODE=3");  // 设置Wi-Fi模式
  echo();
  mySerial.println("AT+CWJAP=\"" +ssid+"\",\"" + password + "\"");  // 连接Wi-Fi
  echo();
  delay(10000);
}
 
void loop() {
 
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
  post();
}
 
void echo(){
  delay(50);
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  }
}
 
void post(){


  /*****************获取到的传感器数值*****************/
    //为了演示，定义了多种类型的数据，可根据自己传感器自行选择
    int data1 = analogRead(A0);
    int data2 = analogRead(A1);
    unsigned int data3 = 45;
    unsigned char data4 = 26;
    double data5 = 99.12;
    String data6 = "ON";
    /*********************数据上传*******************/
    String msg = "";
    //数据用#号包裹，方便app端根据#号做字符串切割，不理解的百度=C语言split分割字符串
    msg = "#"+String(data1)+"#"+String(data2)+"#"+String(data3)+"#"+String(data4)+"#"+String(data5)+"#"+data6+"#";


      
  String postData;
  //Post Data
  postData = "uid="+uid+"&topic="+topic+"&msg="+msg;
  mySerial.println("AT+CIPMODE=1");
  echo();
  mySerial.println("AT+CIPSTART=\"TCP\",\"api.bemfa.com\",80");  // 连接服务器的80端口
  delay(1000);
  echo();
  mySerial.println("AT+CIPSEND"); // 进入TCP透传模式，接下来发送的所有消息都会发送给服务器
  echo();
  mySerial.print("POST /api/device/v1/data/1/"); // 开始发送post请求
  mySerial.print(" HTTP/1.1\r\nHost: api.bemfa.com\r\nContent-Type: application/x-www-form-urlencoded\r\nConnection:close\r\nContent-Length:"); // post请求的报文格式
  mySerial.print(postData.length()); // 需要计算post请求的数据长度
  mySerial.print("\r\n\r\n"); 
  mySerial.println(postData); // 结束post请求
  delay(3000);
  echo();
  mySerial.print("+++"); // 退出tcp透传模式，用println会出错
  postData = "";
  msg="";
  delay(2000);
}
