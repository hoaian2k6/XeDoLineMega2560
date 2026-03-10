#include <Arduino.h>
#include <QTRSensors.h>
#include <Servo.h>

QTRSensors qtr;
Servo steeringServo;

const uint8_t pwmRight = 10; // PB4
const uint8_t dirRight = 21; // PD0
const uint8_t pwmLeft = 13;  // PB7
const uint8_t dirLeft = 20;  // PD1

const uint8_t servoPin = 5;  // PE3
const uint8_t led1 = 41;     // PG3
const uint8_t led0 = 40;     // PG4
const uint8_t button = 4;    // PE2

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
const uint8_t SensorPins[SensorCount] = {A0, A1, A2, A3, A4, A5, A6, A7};

// --- HÀM ĐIỀU KHIỂN ĐỘNG CƠ (DÙNG ĐỂ TEST) ---
void setMotors(int leftSpeed, int rightSpeed) {
  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  if (leftSpeed >= 0) {
    digitalWrite(dirLeft, LOW);
    analogWrite(pwmLeft, leftSpeed);
  } else {
    digitalWrite(dirLeft, HIGH);
    analogWrite(pwmLeft, -leftSpeed);
  }

  if (rightSpeed >= 0) {
    digitalWrite(dirRight, LOW);
    analogWrite(pwmRight, rightSpeed);
  } else {
    digitalWrite(dirRight, HIGH);
    analogWrite(pwmRight, -rightSpeed);
  }
}

// --- HÀM XỬ LÝ NÚT NHẤN ---
void waitButton() {
  while (digitalRead(button) == HIGH) { delay(10); } // Chờ nhấn nút
  while (digitalRead(button) == LOW)  { delay(10); } // Chờ nhả nút ra
  delay(300); // Chống dội phím
}

void setup() {
  Serial.begin(9600);
  
  // Thiết lập I/O
  pinMode(pwmRight, OUTPUT); pinMode(dirRight, OUTPUT);
  pinMode(pwmLeft, OUTPUT);  pinMode(dirLeft, OUTPUT);
  pinMode(led0, OUTPUT);     pinMode(led1, OUTPUT);
  pinMode(button, INPUT_PULLUP);

  // Khởi tạo Servo
  steeringServo.attach(servoPin);
  steeringServo.write(90); // Cho góc đi thẳng mặc định

  // Khởi tạo Cảm biến QTR
  qtr.setTypeAnalog();
  qtr.setSensorPins(SensorPins, SensorCount);

  setMotors(0, 0);

  Serial.println("=====================================");
  Serial.println("  CHUONG TRINH TEST PHAN CUNG XE");
  Serial.println("=====================================\n");

  // ==========================================
  // BÀI TEST 1: KIỂM TRA SERVO LÁI
  // ==========================================
  Serial.println("1. Nhan nut de TEST SERVO (Banh lai)");
  digitalWrite(led0, HIGH); // Bật LED báo chờ nhấn nút
  waitButton();
  digitalWrite(led0, LOW);
  
  Serial.println("-> Dang test Servo...");
  steeringServo.write(50);  // Bẻ trái
  delay(1000);
  steeringServo.write(130); // Bẻ phải
  delay(1000);
  steeringServo.write(90);  // Về lại giữa
  delay(1000);
  Serial.println("-> Test Servo xong. (Neu chieu quay bi nguoc, hay ghi chu lai)\n");

  // ==========================================
  // BÀI TEST 2: KIỂM TRA ĐỘNG CƠ TRÁI
  // ==========================================
  Serial.println("2. Nhan nut de TEST DONG CO TRAI (Banh trai)");
  digitalWrite(led1, HIGH);
  waitButton();
  digitalWrite(led1, LOW);
  
  Serial.println("-> Dong co trai TIEN (2 giay)...");
  setMotors(120, 0); 
  delay(2000);
  
  Serial.println("-> Dong co trai LUI (2 giay)...");
  setMotors(-120, 0); 
  delay(2000);
  
  setMotors(0, 0); // Dừng
  Serial.println("-> Test Dong co trai xong. (Neu banh xe quay lui truoc, hay dao day cap)\n");

  // ==========================================
  // BÀI TEST 3: KIỂM TRA ĐỘNG CƠ PHẢI
  // ==========================================
  Serial.println("3. Nhan nut de TEST DONG CO PHAI (Banh phai)");
  digitalWrite(led0, HIGH);
  digitalWrite(led1, HIGH);
  waitButton();
  digitalWrite(led0, LOW);
  digitalWrite(led1, LOW);
  
  Serial.println("-> Dong co phai TIEN (2 giay)...");
  setMotors(0, 120);
  delay(2000);
  
  Serial.println("-> Dong co phai LUI (2 giay)...");
  setMotors(0, -120);
  delay(2000);
  
  setMotors(0, 0); // Dừng
  Serial.println("-> Test Dong co phai xong.\n");

  // ==========================================
  // BÀI TEST 4: CHUYỂN SANG ĐỌC CẢM BIẾN DÒ LINE
  // ==========================================
  Serial.println("4. Nhan nut de TEST CAM BIEN (Xem so lieu duoi day)");
  digitalWrite(led0, HIGH);
  waitButton();
  digitalWrite(led0, LOW);
  Serial.println("-> Dang doc gia tri cam bien (0 -> 1023). Dua vach den vao de thu:");
  delay(1000);
}

void loop() {
  // Ở chế độ test, ta dùng hàm read() thay vì readLineBlack() 
  // để đọc giá trị thô (raw) xem có mắt đọc nào bị cháy hỏng không.
  qtr.read(sensorValues);

  // In 8 giá trị ra Serial Monitor
  for (uint8_t i = 0; i < SensorCount; i++) {
    Serial.print(sensorValues[i]);
    Serial.print("\t"); // Phím tab để tạo khoảng cách giữa các số
  }
  Serial.println(); // Xuống dòng

  // Test chức năng phản hồi LED: 
  // Nếu mắt ngoài cùng bên trái (A0) thấy vạch đen -> Bật LED0
  // Nếu mắt ngoài cùng bên phải (A7) thấy vạch đen -> Bật LED1
  // (Giả sử giá trị > 500 là vạch đen, tùy chỉnh số này nếu cần)
  if (sensorValues[0] > 500) { digitalWrite(led0, HIGH); } 
  else { digitalWrite(led0, LOW); }

  if (sensorValues[7] > 500) { digitalWrite(led1, HIGH); } 
  else { digitalWrite(led1, LOW); }

  delay(250); // Delay 0.25s để số in ra không bị quá nhanh, dễ nhìn
}