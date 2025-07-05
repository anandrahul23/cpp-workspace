#include <coroutine>
#include <optional>  // For std::optional
#include <stdexcept> // For std::logic_error, std::runtime_error
#include <utility>   // For std::exchange, std::move
#include <exception> // For std::current_exception, std::rethrow_exception, std::exception_ptr
#include <iostream>  // For example output (std::cout, std::cerr)
#include <string>    // For std::to_string (in example)

// Forward declaration
struct lazy_int;

// Promise type for lazy_int
struct lazy_int_promise_type
{
    std::optional<int> current_value;
    std::exception_ptr exception_ptr_;
    std::coroutine_handle<> continuation_ = std::noop_coroutine(); // The coroutine awaiting this lazy_int

    lazy_int get_return_object(); // Implementation after lazy_int is defined

    std::suspend_always initial_suspend() noexcept
    {
        // Coroutine starts suspended, making it lazy.
        return {};
    }

    auto final_suspend() noexcept
    {
        // This awaiter is entered when the lazy_int's coroutine (the one producing the int) co_returns or finishes.
        // Its job is to resume the coroutine that was co_awaiting the lazy_int.
        struct final_awaiter
        {
            lazy_int_promise_type *promise_ptr; // Pointer to the promise of the lazy_int's coroutine

            bool await_ready() noexcept
            {
                // We always want to suspend the lazy_int's coroutine here and potentially switch to the continuation.
                return false;
            }

            // This await_suspend is called with the handle of the lazy_int's coroutine itself.
            // It should return the handle of the coroutine to be resumed next.
            std::coroutine_handle<> await_suspend(std::coroutine_handle<lazy_int_promise_type> /*this_coroutine_handle*/) noexcept
            {
                // Resume the stored continuation (the coroutine that co_awaited the lazy_int).
                return promise_ptr->continuation_;
            }

            void await_resume() noexcept
            {
                // Nothing to do here for this specific final_awaiter.
            }
        };
        return final_awaiter{this};
    }

    void unhandled_exception()
    {
        exception_ptr_ = std::current_exception();
    }

    void return_value(int value)
    {
        current_value = value;
    }

    // Helper for lazy_int's await_suspend to store the awaiting coroutine's handle.
    void set_continuation(std::coroutine_handle<> cont) noexcept
    {
        continuation_ = cont;
    }
};

// The lazy_int coroutine type. It is both a coroutine return type and an awaitable.
struct lazy_int
{
public:
    using promise_type = lazy_int_promise_type;

private:
    std::coroutine_handle<promise_type> coro_h_;

public:
    // Constructor is typically called by the coroutine framework via promise_type::get_return_object.
    explicit lazy_int(std::coroutine_handle<promise_type> h) : coro_h_(h) {}

    // Coroutines are generally not copyable due to unique ownership of the coroutine state.
    lazy_int(const lazy_int &) = delete;
    lazy_int &operator=(const lazy_int &) = delete;

    // Movable
    lazy_int(lazy_int &&other) noexcept : coro_h_(std::exchange(other.coro_h_, nullptr)) {}

    lazy_int &operator=(lazy_int &&other) noexcept
    {
        if (this != &other)
        {
            if (coro_h_)
            {
                coro_h_.destroy();
            }
            coro_h_ = std::exchange(other.coro_h_, nullptr);
        }
        return *this;
    }

    ~lazy_int()
    {
        if (coro_h_)
        {
            coro_h_.destroy();
        }
    }

    // Awaitable interface: these methods make `lazy_int` itself an awaiter.
    bool await_ready() const noexcept
    {
        if (!coro_h_)
        {
            // Moved-from state: consider it "ready" to throw in await_resume.
            return true;
        }
        // The value is ready if the coroutine has already computed it (set current_value)
        // or if it has thrown an exception (exception_ptr_ is set).
        return coro_h_.promise().current_value.has_value() || coro_h_.promise().exception_ptr_;
    }

    void await_suspend(std::coroutine_handle<> awaiting_coroutine) noexcept
    {
        if (!coro_h_)
        {
            // This case should be prevented by await_ready() returning true for a null coro_h_ (moved-from state).
            // If somehow reached, resuming the awaiter is a safe fallback to prevent deadlock.
            awaiting_coroutine.resume();
            return;
        }

        // Store the handle of the awaiting coroutine in our promise.
        // This is so that when our coroutine (`coro_h_`) finishes, its `final_suspend`
        // knows whom to resume.
        coro_h_.promise().set_continuation(awaiting_coroutine);

        // Resume our own coroutine (`coro_h_`) to start/continue the lazy computation.
        coro_h_.resume();
        // The `awaiting_coroutine` remains suspended. It will be resumed by `coro_h_`'s `final_suspend` logic.
    }

    int await_resume()
    {
        if (!coro_h_)
        {
            // Attempting to co_await a moved-from lazy_int.
            throw std::logic_error("co_await on a moved-from lazy_int");
        }

        if (coro_h_.promise().exception_ptr_)
        {
            // An exception occurred during the lazy computation. Rethrow it.
            std::rethrow_exception(coro_h_.promise().exception_ptr_);
        }

        if (!coro_h_.promise().current_value.has_value())
        {
            // This should ideally not happen if the coroutine logic is correct (i.e., it co_returned or threw).
            throw std::logic_error("lazy_int: value not available after coroutine completion without exception.");
        }
        return coro_h_.promise().current_value.value();
    }
};

// Definition of lazy_int_promise_type::get_return_object (needs lazy_int to be fully defined)
inline lazy_int lazy_int_promise_type::get_return_object()
{
    return lazy_int{std::coroutine_handle<lazy_int_promise_type>::from_promise(*this)};
}

// --- Example Usage ---

// A coroutine function that returns a lazy_int
lazy_int calculate_square_lazily(int id, int input)
{
    std::cout << "[lazy_calc #" << id << "] Coroutine body starts. Simulating work for input: " << input << "...\n";
    // Simulate some work.
    int result = input * input;
    std::cout << "[lazy_calc #" << id << "] Work done. Result: " << result << "\n";
    co_return result;
}

lazy_int generate_value_or_throw(int id, bool should_throw)
{
    std::cout << "[lazy_throw #" << id << "] Coroutine body starts.\n";
    if (should_throw)
    {
        std::cout << "[lazy_throw #" << id << "] Intentionally throwing exception.\n";
        throw std::runtime_error("Exception from lazy_throw #" + std::to_string(id));
    }
    int result = 100 + id;
    std::cout << "[lazy_throw #" << id << "] Work done. Result: " << result << "\n";
    co_return result;
}

// A simple fire-and-forget task type to run the top-level consumer coroutine.
struct simple_task
{
    struct promise_type
    {
        simple_task get_return_object() noexcept { return {}; }
        std::suspend_never initial_suspend() noexcept { return {}; } // Start immediately
        std::suspend_never final_suspend() noexcept { return {}; }   // Complete immediately when co_return void; is hit
        void return_void() noexcept {}
        void unhandled_exception() noexcept
        { // Or handle more gracefully
            std::cerr << "Unhandled exception in simple_task's promise!\n";
            if (std::current_exception())
            {
                try
                {
                    std::rethrow_exception(std::current_exception());
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Exception: " << e.what() << std::endl;
                }
            }
            std::terminate();
        }
    };
};

// Consumer coroutine demonstrating the use of lazy_int
simple_task consumer_example()
{
    std::cout << "--- Consumer Coroutine: Started ---\n";

    std::cout << "\n[Consumer] Creating lazy_int_A (for 5*5)...\n";
    lazy_int lazy_val_A = calculate_square_lazily(1, 5);
    std::cout << "[Consumer] lazy_int_A created. Computation should be deferred.\n";

    std::cout << "\n[Consumer] Creating lazy_int_B (for 10*10)...\n";
    lazy_int lazy_val_B = calculate_square_lazily(2, 10);
    std::cout << "[Consumer] lazy_int_B created. Computation should be deferred.\n";

    std::cout << "\n[Consumer] About to co_await lazy_val_A...\n";
    int val_A = co_await lazy_val_A;
    std::cout << "[Consumer] co_await lazy_val_A finished. Value: " << val_A << " (Expected 25)\n";

    std::cout << "\n[Consumer] About to co_await lazy_val_A again (should be instant)...\n";
    int val_A_again = co_await lazy_val_A; // Should use memoized value
    std::cout << "[Consumer] co_await lazy_val_A again finished. Value: " << val_A_again << " (Expected 25)\n";

    std::cout << "\n[Consumer] About to co_await lazy_val_B...\n";
    int val_B = co_await lazy_val_B;
    std::cout << "[Consumer] co_await lazy_val_B finished. Value: " << val_B << " (Expected 100)\n";

    std::cout << "\n[Consumer] Testing exception propagation (lazy_throw_C)...\n";
    lazy_int lazy_throw_C = generate_value_or_throw(3, true);
    try
    {
        std::cout << "[Consumer] About to co_await lazy_throw_C...\n";
        int val_throw = co_await lazy_throw_C; // This line should throw
        std::cout << "[Consumer] co_await lazy_throw_C finished. Value: " << val_throw << " (ERROR: Should have thrown)\n";
    }
    catch (const std::runtime_error &e)
    {
        std::cout << "[Consumer] Caught expected exception: " << e.what() << "\n";
    }

    std::cout << "\n[Consumer] Testing no-throw case (lazy_no_throw_D)...\n";
    lazy_int lazy_no_throw_D = generate_value_or_throw(4, false);
    try
    {
        std::cout << "[Consumer] About to co_await lazy_no_throw_D...\n";
        int val_no_throw = co_await lazy_no_throw_D;
        std::cout << "[Consumer] co_await lazy_no_throw_D finished. Value: " << val_no_throw << " (Expected 104)\n";
    }
    catch (const std::exception &e)
    { // Should not catch here
        std::cout << "[Consumer] Caught UNEXPECTED exception: " << e.what() << "\n";
    }

    std::cout << "\n[Consumer] Testing move semantics...\n";
    lazy_int lazy_move_source = calculate_square_lazily(5, 7);
    std::cout << "[Consumer] lazy_move_source created.\n";
    lazy_int lazy_move_dest = std::move(lazy_move_source);
    std::cout << "[Consumer] lazy_move_source moved to lazy_move_dest.\n";

    std::cout << "[Consumer] About to co_await lazy_move_dest...\n";
    int val_moved = co_await lazy_move_dest;
    std::cout << "[Consumer] co_await lazy_move_dest finished. Value: " << val_moved << " (Expected 49)\n";

    std::cout << "[Consumer] Attempting to co_await moved-from lazy_move_source (should throw std::logic_error)...\n";
    try
    {
        // co_await on a moved-from object.
        // lazy_move_source.coro_h_ is nullptr.
        // await_ready() will return true.
        // await_resume() will be called and should throw.
        int val_from_moved = co_await lazy_move_source;
        std::cout << "[Consumer] co_await lazy_move_source finished. Value: " << val_from_moved << " (ERROR: Should have thrown)\n";
    }
    catch (const std::logic_error &e)
    {
        std::cout << "[Consumer] Caught expected std::logic_error: " << e.what() << "\n";
    }

    std::cout << "\n--- Consumer Coroutine: Finished ---\n";
    // co_return; // Implicit for void-returning coroutines if simple_task::promise_type::return_void() exists
}

int main()
{
    std::cout << "--- Main: Program Starting ---\n";
    consumer_example(); // This call starts the coroutine.
                        // Because simple_task uses suspend_never for initial_suspend,
                        // consumer_example() executes until its first true suspension point.
                        // The chain of resumptions will then drive it to completion.
    std::cout << "--- Main: consumer_example() invoked and completed. ---\n";
    std::cout << "--- Main: Program Finished ---\n";
    return 0;
}
