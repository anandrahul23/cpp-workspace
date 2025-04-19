module;
#include <atomic>
export module leonrahul.LockFreeSharedPtr;

using namespace std;

export namespace ThreadSafeWorld
{

    class ControlBlock
    {

    private:
        atomic<int> count;

    public:
        ControlBlock() : count{0} {};
        int getCount()
        {
            return count.load(std::memory_order_relaxed);
        }
        int addRef()
        {
            return count.fetch_add(1, std::memory_order_relaxed);
        }
        int removeRef()
        {
            return count.fetch_sub(1, std::memory_order_release);
        }
    };

    class LockFreeSharedPtr
    {
    private:
        ControlBlock *cb;

    public:
        LockFreeSharedPtr() : cb{nullptr} {};
    };
}
