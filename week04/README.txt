HW04 SOLUTION - STM32F103 BLUE PILL
=====================================

answers.txt
-----------
Chứa đáp án Task 1, Task 2 và ghi rõ Task 5 chọn Cách 2.
Sau khi quay video, thay dòng [DÁN LINK...] bằng link YouTube hoặc Google Drive thật.

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

Chọn LED phản hồi Cách 2.
Phím 1 (Row1-Col1) là phím mục tiêu và dùng để toggle LED.

CMakeLists_snippet.txt

Đoạn cần thêm vào CMakeLists.txt hiện tại.

LƯU Ý
-----
Mỗi Task có một main.c riêng.
Khi test Task nào, dùng main.c của Task đó.
Không đưa cả 3 main.c vào cùng một target vì sẽ lỗi multiple definition of main.

Project STM32F103 vẫn cần các file startup, system, CMSIS, linker script và cấu hình HAL
của project mẫu hiện tại.

Một số phiên bản STM32CubeF1 còn cần stm32f1xx_hal_conf.h, stm32f1xx_hal_def.h và
CMSIS device headers. Nếu project mẫu đã có thì giữ nguyên.

Đấu Task 4:
PA0 ---- nút nhấn ---- GND

Đấu Task 5:
PA0 = Row1
PA1 = Row2
PB0 = Col1
PB1 = Col2
PC13 = LED onboard
