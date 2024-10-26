#include <iostream>
#include <string>
#include <vector>
#include <functional>

class Observable {
public:
    void subscribe(std::function<void(const std::string&)> observer) {
        observers.push_back(observer);
    }

    void emit(const std::string& data) {
        for (const auto& observer : observers) {
            observer(data);  // 每次 emit 都通知所有观察者
        }
    }

private:
    std::vector<std::function<void(const std::string&)>> observers;
};

int main() {
    Observable dataService;

    // 观察者1
    dataService.subscribe([](const std::string& data) {
        std::cout << "Observer 1 received data: " << data << std::endl;
    });

    // 观察者2
    dataService.subscribe([](const std::string& data) {
        std::cout << "Observer 2 received data: " << data << std::endl;
    });

    // 模拟数据推送
    dataService.emit("Hello, Reactive!");
    dataService.emit("Next data in the stream!");

    return 0;
}
