#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <algorithm>
#include <string>

// Custom SpinLock implementation (mutex replacement)
class SpinLock {
    std::atomic_flag locked = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (locked.test_and_set(std::memory_order_acquire)) {
            std::this_thread::yield(); // Reduce CPU contention
        }
    }
    void unlock() {
        locked.clear(std::memory_order_release);
    }
};

class Philosopher {
    const int id;
    const int total_philosophers;
    SpinLock& left_fork;
    SpinLock& right_fork;
    std::atomic<bool>& stop_flag;

    void think() {
        std::cout << "Philosopher " << id << " is thinking...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    void eat() {
        // Different fork acquisition order based on philosopher ID
        if (id % 2 == 0) {
            left_fork.lock();
            right_fork.lock();
        } else {
            right_fork.lock();
            left_fork.lock();
        }

        std::cout << "Philosopher " << id << " is eating (using forks "
                  << id << " and " << (id + 1) % total_philosophers << ")\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Always unlock in reverse order
        right_fork.unlock();
        left_fork.unlock();
    }

public:
    Philosopher(int id, SpinLock& left, SpinLock& right, std::atomic<bool>& stop, int total)
        : id(id), total_philosophers(total), left_fork(left), right_fork(right), stop_flag(stop) {}

    void operator()() {
        while (!stop_flag.load(std::memory_order_relaxed)) {
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

    const int num_philosophers = std::max(2, std::stoi(argv[1])); // Minimum 2 philosophers
    std::vector<SpinLock> forks(num_philosophers);
    std::atomic<bool> stop_flag(false);
    std::vector<std::thread> philosophers;

    // Create philosopher threads
    for (int i = 0; i < num_philosophers; ++i) {
        philosophers.emplace_back(
            Philosopher(i, forks[i], forks[(i + 1) % num_philosophers], 
            std::ref(stop_flag), num_philosophers)
        );
    }

    std::cout << "Press Enter to stop the simulation...\n";
    std::cin.get();
    stop_flag.store(true, std::memory_order_relaxed);

    // Cleanup threads
    for (auto& t : philosophers) {
        t.join();
    }

    std::cout << "Simulation ended successfully.\n";
    return 0;
}