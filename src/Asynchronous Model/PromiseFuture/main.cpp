#include <iostream>
#include <future>
#include <string>
#include <Windows.h>

void query_data(std::promise<std::string>& prom, std::promise<int>& prom2) {
	Sleep(1000);
	std::string data = "hello";
	prom.set_value(data);
    Sleep(1000);
	prom2.set_value(100);

}

void async_future_promise() {
	std::promise<std::string> prom;
    std::promise<int> prom2;
    /**
     * @brief 
     * 当你直接将 prom 作为参数传递给 std::thread 时，默认情况下会进行复制。
     * 这意味着 query_data 函数接收到的是 prom 的一个拷贝，而不是对原始对象的引用。
     * 在这种情况下，query_data 无法修改原始的 std::promise 对象，导致无法成功设置值。
     * 使用 std::ref 可以避免这种拷贝，确保 query_data 函数能操作原始的 prom 对象：
     */
	std::thread(query_data, std::ref(prom),std::ref(prom2)).detach();
	std::future<std::string> result = prom.get_future();
    std::future<int> result2 = prom2.get_future();
    /**
     * @brief 
     * 是一个用于获取异步操作结果的对象。
     * 在调用 result.get() 时，如果结果尚未准备好，主线程会被阻塞，直到结果可用。
     */
	std::cout << "async_future_promise data: " << result.get()<<result2.get() << "\n";
}

int main()
{
	async_future_promise();
}