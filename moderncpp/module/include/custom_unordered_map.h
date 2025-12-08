#pragma once

#include <vector>
#include <list>
#include <utility> // For std::pair
#include <functional> // For std::hash, std::equal_to
#include <stdexcept> // For std::out_of_range
#include <iostream>

template<
    typename Key,
    typename Value,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>
>
class CustomUnorderedMap {
public:
    // Constructor: Initializes the map with a certain number of buckets.
    explicit CustomUnorderedMap(size_t bucket_count = 16)
        : buckets_(bucket_count), count_(0), max_load_factor_(0.75f) {
        if (bucket_count == 0) {
            buckets_.resize(1);
        }
    }

    // Access or insert an element.
    // If key exists, returns a reference to its value.
    // If not, it creates a new element with a default-constructed value.
    Value& operator[](const Key& key) {
        // Rehash if the load factor is too high
        if (load_factor() > max_load_factor_) {
            rehash(buckets_.size() * 2);
        }

        size_t bucket_index = get_bucket_index(key);
        auto& bucket = buckets_[bucket_index];

        // Find the key in the bucket's list
        for (auto& pair : bucket) {
            if (equal_fn_(pair.first, key)) {
                return pair.second;
            }
        }

        // If key not found, insert a new pair with a default value and return a reference to the new value.
        bucket.emplace_front(key, Value{});
        count_++;
        return bucket.front().second;
    }

    // Access an element with bounds checking (const version).
    const Value& at(const Key& key) const {
        size_t bucket_index = get_bucket_index(key);
        const auto& bucket = buckets_[bucket_index];

        for (const auto& pair : bucket) {
            if (equal_fn_(pair.first, key)) {
                return pair.second;
            }
        }

        throw std::out_of_range("Key not found in CustomUnorderedMap");
    }

    // Access an element with bounds checking (non-const version).
    Value& at(const Key& key) {
        // Re-use the const version to avoid code duplication
        return const_cast<Value&>(static_cast<const CustomUnorderedMap*>(this)->at(key));
    }

    // Erase an element by key. Returns true if an element was removed.
    bool erase(const Key& key) {
        size_t bucket_index = get_bucket_index(key);
        auto& bucket = buckets_[bucket_index];

        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (equal_fn_(it->first, key)) {
                bucket.erase(it);
                count_--;
                return true;
            }
        }
        return false;
    }

    // Returns the number of elements in the map.
    size_t size() const {
        return count_;
    }

    // Checks if the map is empty.
    bool empty() const {
        return count_ == 0;
    }

    // For debugging: prints the contents of the map.
    void print() const {
        std::cout << "Map content:" << std::endl;
        for (size_t i = 0; i < buckets_.size(); ++i) {
            const auto& bucket = buckets_[i];
            if (!bucket.empty()) {
                std::cout << "Bucket " << i << ": ";
                for (const auto& pair : bucket) {
                    std::cout << "[" << pair.first << ": " << pair.second << "] ";
                }
                std::cout << std::endl;
            }
        }
    }

private:
    using PairType = std::pair<const Key, Value>;
    using BucketType = std::list<PairType>;

    std::vector<BucketType> buckets_;
    size_t count_;
    float max_load_factor_;
    Hash hash_fn_;
    KeyEqual equal_fn_;

    // Calculates the current load factor.
    float load_factor() const {
        return static_cast<float>(count_) / buckets_.size();
    }

    // Gets the bucket index for a given key.
    size_t get_bucket_index(const Key& key) const {
        // Use the hash function and then modulo to fit into the bucket array size.
        return hash_fn_(key) % buckets_.size();
    }

    // Increases the number of buckets and re-inserts all elements.
    void rehash(size_t new_bucket_count) {
        if (new_bucket_count <= buckets_.size()) {
            return;
        }

        std::vector<BucketType> new_buckets(new_bucket_count);

        // Move all existing pairs to the new bucket vector
        for (auto& bucket : buckets_) {
            for (auto& pair : bucket) {
                size_t new_index = hash_fn_(pair.first) % new_bucket_count;
                new_buckets[new_index].push_front(std::move(pair));
            }
        }

        // Replace the old buckets with the new ones
        buckets_ = std::move(new_buckets);
    }
};

// --- Example Usage ---

// A custom struct to use as a key
struct Person {
    std::string name;
    int age;

    // We need to provide an equality operator for KeyEqual
    bool operator==(const Person& other) const {
        return name == other.name && age == other.age;
    }
};

// A custom hash function for our Person struct
struct PersonHash {
    std::size_t operator()(const Person& p) const {
        // A simple way to combine hashes
        std::size_t h1 = std::hash<std::string>{}(p.name);
        std::size_t h2 = std::hash<int>{}(p.age);
        return h1 ^ (h2 << 1); // Combine hashes
    }
};

// Overload operator<< for easy printing of Person
std::ostream& operator<<(std::ostream& os, const Person& p) {
    return os << p.name << " (" << p.age << ")";
}

int main() {
    std::cout << "--- Using std::string as key ---" << std::endl;
    CustomUnorderedMap<std::string, int> word_counts;
    word_counts["hello"] = 1;
    word_counts["world"] = 2;
    word_counts["hello"]++;

    std::cout << "Count of 'hello': " << word_counts.at("hello") << std::endl;
    std::cout << "Count of 'world': " << word_counts["world"] << std::endl;
    word_counts.print();

    word_counts.erase("world");
    std::cout << "\nAfter erasing 'world':" << std::endl;
    word_counts.print();

    std::cout << "\n--- Using custom struct as key ---" << std::endl;
    CustomUnorderedMap<Person, std::string, PersonHash> person_cities;

    Person alice = {"Alice", 30};
    Person bob = {"Bob", 42};

    person_cities[alice] = "New York";
    person_cities[bob] = "London";

    std::cout << alice << " lives in " << person_cities.at(alice) << std::endl;
    person_cities.print();

    return 0;
}