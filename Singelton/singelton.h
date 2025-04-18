class Singleton
{
public:
    static Singleton *getInstance()
    {
        Singleton *tmp = instance.load(std::memory_order_relaxed);
        if (tmp == nullptr)
        { // First check (no lock)
            std::lock_guard<std::mutex> lock(mutex_);
            tmp = instance.load(std::memory_order_relaxed);
            if (tmp == nullptr)
            { // Second check (with lock)
                tmp = new Singleton();
                instance.store(tmp, std::memory_order_release);
            }
        }
        return tmp;
    }

private:
    Singleton() {}
    static std::atomic<Singleton *> instance;
    static std::mutex mutex_;
};
std::atomic<Singleton *> Singleton::instance{nullptr};