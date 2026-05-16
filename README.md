PS4 Controller

 　　↓
      
ESP32
  - stick演算
  - mecanum混合
  - CAN送信
    
      ↓
    
STM32
  - CAN受信
  - PWM出力
  - モータ駆動
  - encoder計測
    
      ↓
ESP32へfeedback


　canID : <0x100> ESP → STM1 (4輪オムニ用pwm, dir)  
 &emsp;
 &emsp;
 &emsp;
 &nbsp;
 <0x101> STM1 → ESP (ロータリーエンコーダーの値)
