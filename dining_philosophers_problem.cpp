#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <string>
#include <algorithm>

std::mutex cout_mutex; // Do synchronizacji wypisywania
std::mutex stop_mutex; // Do ochrony flagi stopu
bool stop_flag = false;

class Philosopher {
    const int id;
    const int total_philosophers;
    std::mutex& left_fork;
    std::mutex& right_fork;

    void print(const std::string& msg) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Philosopher " << id << " " << msg << "\n";
    }

    void think() {
        print("is thinking...");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    void eat() {
        // Aby uniknąć zakleszczenia, różne kolejności blokad
        if (id % 2 == 0) {
            std::unique_lock<std::mutex> lock1(left_fork);
            std::unique_lock<std::mutex> lock2(right_fork);
            print("is eating (using forks " + std::to_string(id) +
                  " and " + std::to_string((id + 1) % total_philosophers) + ")");
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        } else {
            std::unique_lock<std::mutex> lock1(right_fork);
            std::unique_lock<std::mutex> lock2(left_fork);
            print("is eating (using forks " + std::to_string(id) +
                  " and " + std::to_string((id + 1) % total_philosophers) + ")");
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

public:
    Philosopher(int id, std::mutex& left, std::mutex& right, int total)
        : id(id), total_philosophers(total), left_fork(left), right_fork(right) {}

    void operator()() {
        while (true) {
            {
                std::lock_guard<std::mutex> lock(stop_mutex);
                if (stop_flag) break;
            }
            think();
            eat();
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <number_of_philosophers>\n";
        return 1;
    }

    const int num_philosophers = std::max(2, std::stoi(argv[1]));
    std::vector<std::mutex> forks(num_philosophers);
    std::vector<std::thread> philosophers;

    // Tworzenie filozofów
    for (int i = 0; i < num_philosophers; ++i) {
        philosophers.emplace_back(
            Philosopher(i, forks[i], forks[(i + 1) % num_philosophers], num_philosophers)
        );
    }

    std::cout << "Press Enter to stop the simulation...\n";
    std::cin.get();

    {
        std::lock_guard<std::mutex> lock(stop_mutex);
        stop_flag = true;
    }

    for (auto& t : philosophers) {
        t.join();
    }

    std::cout << "Simulation ended successfully.\n";
    return 0;
}
