# Bài tập lớn cuối kỳ môn học Vi Xử Lý

## Mục tiêu

Xây dựng hệ thống nhập mật khẩu thông qua UART trên máy tính, hiển thị phản hồi lên màn hình LCD 16x2, lưu mật khẩu vào EEPROM 24C02 giao tiếp I2C (bit-banging), có chức năng đổi mật khẩu và khóa nếu nhập sai quá 3 lần.

## Kiến thức sử dụng

- Lập trình trực tiếp bằng thanh ghi (không dùng HAL)
- UART thủ công: gửi/nhận dữ liệu qua USART1
- I2C bit-banging: điều khiển EEPROM 24C02
- LCD 16x2 giao tiếp 4-bit (PB10–PB15)
- Hiển thị dạng ẩn mật khẩu `******`
- Cơ chế cập nhật mật khẩu với lệnh `update` → `yes`/`no`
- Tạm khóa hệ thống 15 giây sau 3 lần nhập sai, có thể thay đổi thời gian khóa

## Phần cứng

- Vi điều khiển: STM32F103C8T6 (Blue Pill)
- LCD 1602, giao tiếp 4-bit
- EEPROM AT24C02
- USB to UART (dùng với Tera Term)

## Cấu trúc thư mục
```
Project_vi_xu_ly_20242/
│
├── Application/
│ ├── uart.c/h
│ ├── lcd.c/h
│ ├── eeprom.c/h
│ ├── password.c/h
│ ├── delay.c/h
│
├── Drivers
│ ├── main.c
├── README.md
```
## Cách sử dụng

1. Biên dịch và nạp code bằng Keil v5 + ST-Link Utility
2. Mở Tera Term (baudrate 9600)
3. Khi nhập:
   - Nhập đúng mật khẩu → hiện “MẬT KHẨU ĐÚNG”
   - Sai quá 3 lần → hiện "Sai 3 lần!" + đếm ngược khóa 15s
   - Gõ `update` → chọn `yes` để đổi mật khẩu

## Công cụ sử dụng

- Keil uVision5
- ST-Link Utility
- Tera Term
- EasyEDA (phần mềm mô phỏng mạch)

## Nhóm tác giả
```
Nguyễn Thành Đạt – 20223688
Nguyễn Hoàng Tiến - 20223741
Đỗ Mai Phương - 20224349  
Lớp ETTN - K67 
Bộ môn Kỹ thuật vi xử lý - 20242
GVHD: PGS.TS Hoàng Mạnh Thắng
```

