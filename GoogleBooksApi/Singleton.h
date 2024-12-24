#pragma once

/**
 * Singleton template pattern implementation.
 */

#include <mutex>
#include <memory>
#include <utility>

template <typename T, typename... Args>
class Singleton 
{
private:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static std::unique_ptr<T> instance;
    static std::mutex mutex;

public:
    static T& getInstance(Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex);

        if (!instance) {
            instance = std::unique_ptr<T>(new T(std::forward<Args>(args)...));
        }
        return *instance;
    }

    // Método para destruir explicitamente a instância (útil em alguns cenários de teste)
    static void destroyInstance() {
        std::lock_guard<std::mutex> lock(mutex);
        instance.reset();
    }

    ~Singleton()
    {
        if (instance)
            destroyInstance();
    }
};

template <typename T, typename... Args>
std::unique_ptr<T> Singleton<T, Args...>::instance = nullptr;

template <typename T, typename... Args>
std::mutex Singleton<T, Args...>::mutex;
