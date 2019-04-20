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
* Thường là buffer thực thi giống kiểu "vòng", nghĩa là sau khi write vào cell cuối cùng của buffer thì việc write đó sẽ tiếp tục với cell đầu tiên, và sau khi read từ cell cuối cùng sẽ tiếp tục đọc từ cell đầu tiên.  
## Mô tả buffer  
````class TBuffer  
{  
int in, out, n;  
char Buf[N];  
public:  
TBuffer(){in = 0; out = 0; n = 0;};  
~TBuffer();  
void	Write(char M);  
char	Read();  
}  
````
Ghi dữ liệu theo phương thức sau:  
````void TBuffer::Write(char M)
{
вход в критический участок;   // Vào critical section
while (n == N){//буфер полный (buffer is full)
перейти к ожиданию записи с одновременным освобождением критического участка;  //đi đến việc chờ ghi đồng thời
}
n++;
Buf[in] = M;
in = (in + 1) % N;
сигнализировать о возможности чтения;  //khả năng đọc tín hiệu
выход из критического участка; // ra khỏi CS
}
````
Đọc dữ liệu theo phương thức sau:  
````char TBuffer::Read()
{
вход в критический участок;  //đi vào CS
while (n == 0) {//буфер пустой //buffer is empty
перейти к ожиданию чтения с одновременным освобождением критического участка; //đi với việc chờ đọc cùng sự giải phóng đồng thời của CS
}
n--;
char М = Buf[out];
out = (out + 1) % N;
сигнализировать о возможности записи; // khả năng đọc tín hiệu
выход из критического участка; //đi khỏi CS
return M;
}
````
Số lượng tin nhắn hiện tại trong buffer (n) chính là tài nguyên quan trọng  (critical resource), bởi vì bao nhiêu threads write và read có thể đồng thời kiểm tra và thiết lập lại giá trị đó. Vì thế hành động write vào buffer và read từ buffer phải chạy trong chế độ loại trừ lẫn nhau.  
Để thực hiện việc loại trừ lẫn nhau, sử dụng mutex.  
Để thực hiện lock thread đồng thời giải phóng mutex, sử dụng biến điều kiện.  
*Biến điều kiện* là một công cụ đồng bộ hóa, trên cơ sở để thực hiện các hành động.  
___Tạo biến điều kiện:___  
````int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);````  
````pthread_cond_t *cond```` – con trỏ đến «biến điều kiện» - biến có kiểu pthread_cond_t;  
````const pthread_condattr_t *attr```` – struct, mô tả thuộc tính của biến điều kiện.  
___Phá hủy biến điều kiện:___  
````int pthread_cond_destroy(pthread_cond_t *cond).````  
__Chờ trên biến điều kiện.___    
1.Nếu thread gọi lệnh:  
````int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);````  
````pthread_cond_t *cond```` – con trỏ đến «biến điều kiện» - biến có kiểu pthread_cond_t;  
````pthread_mutex_t *mutex```` – con trỏ đến mutex,    
thì luồng bị lock và release mutex, được mô tả trong hành động.  
Để lock và release mutex thực hiện một hành động "hạt nhân".  
___Hành động đưa tín hiệu trên biến điều kiện:___  
2.Nếu thread gọi lệnh:
    ````int pthread_cond_signal(pthread_cond_t *cond);```` sau đó nó bị chặn trên biến điều kiện, nó tiếp tục thực hiện chương trình từ chính điểm mà nó bị chặn; và cùng với điều đó, mutex bị capture lại.  
    
    
  Hai hành vi trên có thể được thực hiện với các khả năng bổ sung sau:  
    - Nếu tồn tại vài threads, bị khóa trong biến điều kiện, chúng có thể đồng thời active, gọi hàm:  
        ````int pthread_cond_broadcast(pthread_cond_t *cond).````  
        Vòng lặp while() để lặp lại sự kiểm tra lại trạng thái buffer cho phép trong kết quả của các "cuộc đua" chỉ 1 thread trong số các threads tiếp tục thực hiện trong CS. Các thread còn lại sẽ lặp lại bị lock.
    - Nếu có 1 thread, bị lock trong biến điều kiện, nhưng không có luồng nào có thể active lại nó (ví dụ: luồng đã bị kết thúc bất thường); thì thay vì lock nó trong "thời gian vô hạn", ta có thể chặn nó trong thời gian nhất định, bằng cách gọi hàm:  
    ````int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime````  
    ````pthread_cond_t *cond```` – con trỏ đến «biến điều kiện»;  
````pthread_mutex_t *mutex```` – con trỏ đến mutex,  
````const struct timespec *abstime```` – thời gian chờ tuyệt đối.Nếu hết thời gian chờ và tín hiệu kích hoạt chưa được nhận, chức năng sẽ trả về lỗi [ETIMEDOUT].  


Vì thế, mô tả class buffer Tbuffer có thể được bổ sung bởi các phần tử sau:    
````pthread_cond_t readCV```` - biến điều kiện để chặn luồng chờ đọc;  
````pthread_cond_t writeCV```` - biến điều kiện để chặn luồng chờ bản ghi;  
````pthread_mutex_t mutex```` - mutex để cung cấp loại trừ lẫn nhau khi gọi các thao tác ghi vào bộ đệm và đọc từ bộ đệm.  
## Quá trình thực hiện chương trình
- Triển khai đối tượng đệm Buffer trong mã nguồn.  
- Lập trình nhiệm vụ tương tác giữa hai luồng bằng cách sử dụng đối tượng Bộ đệm đã triển khai.  
- Phân tích các tình huống khi tốc độ ghi dữ liệu cao hơn tốc độ đọc và khi tốc độ ghi dữ liệu thấp hơn tốc độ đọc.  
- Để có thể kết thúc chính xác một chương trình để chặn các luồng, hãy sử dụng hàm pthread_cond_cond_med ().  
# Câu hỏi:
## 1.	Дайте определение понятия «условная переменная».    
Условная переменная — примитив синхронизации, обеспечивающий блокирование одного или нескольких потоков до момента поступления сигнала от другого потока о выполнении некоторого условия или до истечения максимального промежутка времени ожидания. Условные переменные используются вместе с ассоциированным мьютексом и являются элементом некоторых видов мониторов.  
*Biến điều kiện là một cơ chế đồng bộ hóa đảm bảo chặn một hoặc một số luồng cho đến khi nhận được tín hiệu từ luồng khác về việc thực hiện một điều kiện nhất định hoặc cho đến khi hết thời gian chờ tối đa. Các biến có điều kiện được sử dụng cùng với mutex liên quan và là một phần của một số loại màn hình nhất định.*  
## 2.	Какие действия выполняются над мьютексом, адрес которого передается в операцию ожидания условной переменной? Какова цель этих действий?  

    





