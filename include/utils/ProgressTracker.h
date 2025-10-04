#ifndef PROGRESSTRACKER_H
#define PROGRESSTRACKER_H

#include <atomic>

/**
 * @brief Thread-safe progress tracking utility
 */
class ProgressTracker {
public:
    /**
     * @brief Constructor
     */
    ProgressTracker()
        : current(0)
        , total(0)
    {}

    /**
     * @brief Set the total number of items
     * @param t Total count
     */
    void setTotal(int t) {
        total = t;
    }

    /**
     * @brief Set the current progress
     * @param c Current count
     */
    void setCurrent(int c) {
        current = c;
    }

    /**
     * @brief Increment the current progress by 1
     */
    void increment() {
        current++;
    }

    /**
     * @brief Increment the current progress by a specific amount
     * @param amount Amount to increment
     */
    void incrementBy(int amount) {
        current += amount;
    }

    /**
     * @brief Reset progress to zero
     */
    void reset() {
        current = 0;
        total = 0;
    }

    /**
     * @brief Get the current progress count
     * @return Current count
     */
    int getCurrent() const {
        return current;
    }

    /**
     * @brief Get the total count
     * @return Total count
     */
    int getTotal() const {
        return total;
    }

    /**
     * @brief Get the progress percentage (0-100)
     * @return Percentage as double
     */
    double getPercentage() const {
        int t = total;
        int c = current;
        return (t > 0) ? (c * 100.0) / t : 0.0;
    }

    /**
     * @brief Check if progress is complete
     * @return True if current >= total
     */
    bool isComplete() const {
        return current >= total && total > 0;
    }

    /**
     * @brief Get remaining items
     * @return Number of remaining items
     */
    int getRemaining() const {
        int t = total;
        int c = current;
        return (t > c) ? (t - c) : 0;
    }

private:
    std::atomic<int> current;
    std::atomic<int> total;
};

#endif // PROGRESSTRACKER_H
