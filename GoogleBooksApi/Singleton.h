#pragma once

#include <memory>
#include <mutex>

/**
 * @brief Singleton template pattern implementation.
 *
 * This class provides a thread-safe implementation of the Singleton design pattern.
 * It ensures that only one instance of the class is created and provides a global point of access to it.
 *
 * @tparam T The type of the singleton class.
 * @tparam Args The types of the arguments to be passed to the constructor of the singleton class.
 */
template <typename T, typename... Args>
class Singleton
{
private:
    /**
     * @brief Private default constructor to prevent instantiation.
     */
    Singleton() = default;

    /**
     * @brief Deleted copy constructor to prevent copying.
     */
    Singleton(const Singleton&) = delete;

    /**
     * @brief Deleted assignment operator to prevent assignment.
     *
     * @return Singleton&
     */
    Singleton& operator=(const Singleton&) = delete;

    /**
     * @brief Pointer to the singleton instance.
     */
    static std::unique_ptr<T> instance;

    /**
     * @brief Mutex to ensure thread safety.
     */
    static std::mutex mutex;

public:
    /**
     * @brief Provides access to the singleton instance.
     *
     * This method creates the singleton instance if it does not already exist.
     * It is thread-safe and ensures that only one instance is created.
     *
     * @param args Arguments to be passed to the constructor of the singleton class.
     * @return T& Reference to the singleton instance.
     */
    static T& getInstance(Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex);

        if (!instance) {
            instance = std::unique_ptr<T>(new T(std::forward<Args>(args)...));
        }
        return *instance;
    }

    /**
     * @brief Destroys the singleton instance.
     *
     * This method deletes the singleton instance and releases any resources held by it.
     * It is thread-safe.
     */
    static void destroyInstance() {
        std::lock_guard<std::mutex> lock(mutex);
        instance.reset();
    }

    /**
     * @brief Destructor.
     *
     * This destructor ensures that the singleton instance is destroyed when the Singleton object is destroyed.
     */
    ~Singleton()
    {
        if (instance)
            destroyInstance();
    }
};

/**
 * @brief Definition of the static instance pointer.
 */
template <typename T, typename... Args>
std::unique_ptr<T> Singleton<T, Args...>::instance = nullptr;

/**
 * @brief Definition of the static mutex.
 */
template <typename T, typename... Args>
std::mutex Singleton<T, Args...>::mutex;
