#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>


std::atomic<int> client_count(0);
bool done = false;  

void client_thread(int max_clients) {
    while (client_count.load(std::memory_order_relaxed) < max_clients) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        client_count.fetch_add(1, std::memory_order_relaxed); 
        std::cout << "Новый клиент. Текущее количество: " 
            << client_count.load(std::memory_order_relaxed) << std::endl;
    }
    done = true;
}

void operator_thread() {
    while (!done || client_count.load(std::memory_order_relaxed) > 0) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        if (client_count.load(std::memory_order_relaxed) > 0) {
            client_count.fetch_sub(1, std::memory_order_relaxed); 
            std::cout << "Клиент обслужен. Оставшееся количество: " << client_count.load(std::memory_order_relaxed) << std::endl;
        }
    }
}

int main() {

    setlocale(LC_ALL, "Russian");

    int max_clients = 10;

    std::thread client(client_thread, max_clients);
    std::thread oper(operator_thread);

    client.join();
    oper.join();

    std::cout << "Все клиенты обслужены." << std::endl;

    return 0;
}
