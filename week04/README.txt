HW04 SOLUTION - STM32F103 BLUE PILL

Task3_LED_Blink/main.c

PC13 Output Push-Pull.
LED sáng 500 ms, tắt 500 ms.

Task4_Button_Debounce/main.c

PC13: LED onboard.
PA0: Input Pull-up.
Nút nhấn nối giữa PA0 và GND.
Debounce 20 ms.
Chỉ toggle LED sau khi nhấn rồi thả.
Nhấn giữ không toggle liên tục.

Task5_Keypad_2x2/main.c

Row1 = PA0
Row2 = PA1
Col1 = PB0
Col2 = PB1
LED  = PC13

Phím 1 (Row1-Col1) là phím mục tiêu và dùng để toggle LED.

CMakeLists_snippet.txt

Đấu Task 4:
PA0 ---- nút nhấn ---- GND

Đấu Task 5:
PA0 = Row1
PA1 = Row2
PB0 = Col1
PB1 = Col2
PC13 = LED onboard
