# OS_4_2019
# INTERACTION OF THREADS THROUGH A BUFFER, REALIZED BY CONDITIONAL VARIABLES
## Mục đích bài lab:  làm quen với kỹ cơ chế tương tác các thread thông qua buffer (bộ đệm) được xây dựng trên các biến điều kiện.  

## Lý thuyết tổng quan  
* Bộ đệm (buffering) là công cụ phù hợp với tốc độ ghi (write) dữ liệu của một thread này và tốc độ đọc (read) của một thread khác. Trong trường hợp này thì bộ đệm dùng chung, chia đối tượng ra thành thread đọc và thread ghi.
* Vài yêu cầu đối với thuật toán hoạt động của buffer:
    - Không được ghi (write) tin nhắn vào trong buffer full; thread nào mà cố gắng làm điều đó thì  phải bị lock, cho đến khi thực hiện sự giải phóng bộ nhớ vào bộ đệm.
    - Không được đọc (read) tin nhắn từ buffer empty; thread nào mà cố gắng làm điều đó thì phải bị lock, cho đến khi một tin nhắn có sẵn trong vùng đệm.
 * Theo quy tắc, cơ chế đồng bộ hóa ghi vào buffer và đọc từ buffer gọi là ẩn với lập trình viên. Lập trình viên chỉ được cấp các nguyên hàm để Creat Buffer, Destroy Buffer, Write to Buffer, Read from Buffer; trông giống như làm việc với các files.
## Mẫu thread ghi dữ liệu vào buffer  
Mẫu thread, ghi dữ liệu vào buffer theo mẫu sau:  
While (điều kiện dừng luồng không được đáp ứng) {  
generate data;  
write data to buffer;  
delay for a while;  
}  
## Mẫu thread đọc dữ liệu từ buffer  
Mẫu thread, đọc dữ liệu từ buffer theo mẫu sau:  
While (điều kiện dừng luồng không được đáp ứng) {    
read data from buffer;  
process data;    
delay for a while;    
}    
## Cấu trúc của buffer  
* Buffer là một mảng N phần tử có chung 1 kiểu xác định. Trạng thái của buffer được mô tả bằng số lượng tin nhắn n, chứa trong buffer, và bằng 2 index - index out để đọc (read) và index in để ghi (write).  
* Việc ghi (write) vào buffer được dự tính trước bằng cách kiểm tra điều kiện "buffer đã đầy", nghĩa là  n==N, và việc đọc (read) từ buffer - kiểm tra điều kiện "buffer đã rỗng" : nghĩa là tốc độ read cao hơn write. Trong trạng thái thông thường index write có một chút cao hơn giá trị index read, được mô tả trong mô hình bản vẽ sau:  
![Capture](https://user-images.githubusercontent.com/24553030/56429992-86bf1c00-62cd-11e9-8627-148f384cc8a6.PNG)

