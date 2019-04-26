#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

using namespace std;

class TBuffer
{
	int in, out, n;	
	char Buf[5];
	pthread_cond_t readCV;  
	pthread_cond_t writeCV; 
	pthread_mutex_t mutex;  
	
	public:
	TBuffer()
	{
		in = 0; out = 0; n = 0;
		pthread_cond_init(&readCV, NULL);
		pthread_cond_init(&writeCV, NULL);
		pthread_mutex_init(&mutex, NULL);
	};
	~TBuffer() 
	{
		pthread_cond_destroy(&readCV);
		pthread_cond_destroy(&writeCV);
		pthread_mutex_destroy(&mutex);
	};
	void	Write(char m);
	char	Read();
};

void TBuffer::Write(char m)
{
	//вход в критический участок;
	pthread_mutex_lock(&mutex);

	while (n == 5)
	{//буфер полный
		//перейти к ожиданию записи с одновременным освобождением критического участка;
		pthread_cond_wait(&writeCV, &mutex);	
	}
	n++;
	Buf[in] = m;
	//cout << "Buffer: " << Buf << endl;
	in = (in + 1) % 5;

	//сигнализировать о возможности чтения;
	pthread_cond_signal(&readCV);

	//выход из критического участка;
	pthread_mutex_unlock(&mutex);
}	

char TBuffer::Read()
{
	//вход в критический участок;
	
	pthread_mutex_lock(&mutex);

	while (n == 0) 
	{//буфер пустой 
		//перейти к ожиданию чтения с одновременным освобождением критического участка;
		pthread_cond_wait(&readCV, &mutex);
	}
	n--;
	char m = Buf[out];
	out = (out + 1) % 5;

	//сигнализировать о возможности записи;
	pthread_cond_signal(&writeCV);

	//выход из критического участка;
	pthread_mutex_trylock(&mutex);
	return m;
}

struct Args {
	int flag_1;
	int flag_2;
	TBuffer buf;
};

void* func_1(void* args)
{
	Args *arg=(Args*) args;	

	char s_num = 'a';

	while (arg->flag_1 == 0) 
	{
		s_num ++; 
		cout << "Записал " << s_num << endl;
		arg->buf.Write(s_num);
		sleep(1);
	}	
}

void* func_2(void* args)
{
	Args *arg=(Args*) args;
	while (arg->flag_2 == 0) 
	{
		char m = arg->buf.Read() + ('A' - 'a');
		cout << "Прочитал: " << m << " " << endl;
		sleep(1);
	}
}


int main()
{
	Args pthread_arg; 
	pthread_arg.flag_1 = 0;
	pthread_arg.flag_2 = 0;

	pthread_t pthread_1, pthread_2;	

	pthread_create(&pthread_1, NULL, func_1, (void*) &pthread_arg); 
	pthread_create(&pthread_2, NULL, func_2, (void*) &pthread_arg);
 	
	getchar();

	pthread_arg.flag_1 = 1;
	pthread_arg.flag_2 = 1;		

	pthread_join(pthread_1, NULL);	
 	pthread_join(pthread_2, NULL);
 
    return 0;
}
