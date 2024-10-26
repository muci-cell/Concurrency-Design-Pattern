#include <iostream>
#include <string>
#include <Windows.h>
#include <thread>

// 函数指针
typedef void (*Callback)(const std::string& str);
void query_data(Callback cb) {
	Sleep(5000);
	std::string data = "hello";
	cb(data);
}

void show_data(const std::string& data) {
	std::cout << "show_data data: " << data << "\n";
}

void async_callback() {
	// 在 C++ 中，函数名本身可以被视为一个指向该函数的指针
	std::thread td = std::thread(query_data, show_data);
	td.join(); // 堵塞当前运行线程
	//td.detach();  // 主线程不等待新线程，直接继续运行
}


int main()
{
	async_callback();
	std::cout <<"main thread continue!"<< "\n";
}