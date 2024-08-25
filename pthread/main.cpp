#include <iostream>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <chrono>

class Event {
public:
    std::mutex mutex;
    std::condition_variable condition;

    bool eventTriggered = false;

    void notify(void) {
        std::unique_lock<std::mutex> lock(mutex);

        eventTriggered = true;
        condition.notify_one(); // Trigger the event

        lock.unlock();
    }

    void wait(void) {
        std::unique_lock<std::mutex> lock(mutex);

        while (false == eventTriggered) {
            condition.wait(lock);
        }

        eventTriggered = false;

        lock.unlock();
    }
};

#include <iostream>
#include <pthread.h>
#include <unistd.h>

class PULSE_GEN {
public:
    PULSE_GEN()
        : counter(0), a(0), inN(0), inNx(0), acc_x(0),
          temp_flag(0), clk1(0), pin_x_out(0), dir_x_out(0),
          enable_handler(0), preset_handler(0) {
        for (int i = 0; i < 10; i++)
            buffer_x[i] = 0;

        pthread_mutex_init(&mutex, nullptr);
        pthread_cond_init(&fulled, nullptr);




    }

    ~PULSE_GEN() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&fulled);
    }

    void init() {
        // catch_value();
        acc_x = inNx;
    }

    static void* mainProcessThreadWrapper(void* arg) {
        PULSE_GEN* pulseGen = static_cast<PULSE_GEN*>(arg);
        pulseGen->mainProcessThread();
        return nullptr;
    }

    // static void* resetThreadWrapper(void* arg) {
    //     PULSE_GEN* pulseGen = static_cast<PULSE_GEN*>(arg);
    //     pulseGen->handleResetMethod();
    //     return nullptr;
    // }

    static void* writeThreadWrapper(void* arg) {
        PULSE_GEN* pulseGen = static_cast<PULSE_GEN*>(arg);
        pulseGen->writeMethod();
        return nullptr;
    }

    void mainProcessThread() {
        while (true) {
            count();
            mainPulse();
            usleep(1000000); // Simulating clock cycle
        }
    }

    void handleResetMethod() {
        pthread_mutex_lock(&mutex);
        counter = 0;
        temp_flag = 0;

        inNx = 0;
        acc_x = 0;

        clk1 = 0;
        pin_x_out = 0;
        dir_x_out = 0;

        for (int i = 0; i < 10; i++) {
            buffer_x[i] = 0;
        }
            
        pthread_mutex_unlock(&mutex);
    }

    void writeMethod() {
        pthread_mutex_lock(&mutex);
        a++;
        if (a > 10) {
            a = 10;
        }
            
        buffer_x[a - 1] = Nx;
        pthread_mutex_unlock(&mutex);
    }

    void catchValue() {
        pthread_mutex_lock(&mutex);
        inN = N;
        inNx = buffer_x[0];

        if (inNx > 10) {
            inNx = 10;
        }

        for (int i = 0; i < a; i++) {
            buffer_x[i] = buffer_x[i + 1];
        }
            
        if (--a < 0) {
            a = 0;
        }
            

        buffer_x[a] = 0;
        pthread_mutex_unlock(&mutex);
    }

private:
    int counter;
    int a;
    int buffer_x[10];

    int acc_x;
    int temp_flag;
    int clk1;
    int pin_x_out;
    int dir_x_out;
    int enable_handler;
    int preset_handler;

    pthread_mutex_t mutex;
    pthread_cond_t fulled;

    // Simulated signals
    int CLK1;
    int Nx;
    int N;

    void count() {
        pthread_mutex_lock(&mutex);
        enable_handler = ENABLE;

        if (PRESET) preset_handler = false;
        else preset_handler = true;

        if (enable_handler && preset_handler) {
            counter++;

            if (counter > 100) {
                counter = 0;
                pthread_cond_signal(&fulled);
            }

            if (counter <= 50)
                clk1 = 1;
            else
                clk1 = 0;

            if (counter == 50)
                CLK1 = 1;
            else if (counter == 100)
                CLK1 = 0;

            PULSE_X_OUT = ~clk1 & pin_x_out;
            DIR_X = dir_x_out;
        } else {
            PULSE_X_OUT = 0;
            DIR_X = 0;
            FLAG = 0;
            CLK1 = 0;
        }
        pthread_mutex_unlock(&mutex);
    }

    void mainPulse() {
        pthread_mutex_lock(&mutex);
        temp_flag++;

        if (temp_flag <= inN) {
            FLAG = 1;
            std::cout << "FLAG = 1" << std::endl;
        }
        else {
            FLAG = 0;
            std::cout << "FLAG = 0" << std::endl;
        }
            

        if (temp_flag == inN || temp_flag == 2 * inN)
            catchValue();

        if (temp_flag >= 2 * inN)
            temp_flag = 0;

        acc_x += inNx;

        if (acc_x >= inN) {
            acc_x -= inN;
            pin_x_out = 1;
        } else
            pin_x_out = 0;
        pthread_mutex_unlock(&mutex);
    }

public:
    int CLK;
    int inN;
    int inNx;
    int ENABLE;
    int WR;
    int PRESET;
    int PULSE_X_OUT;
    int DIR_X;
    int FLAG;
};

int main() {
    PULSE_GEN pulseGen;
    pthread_t mainProcessThreadHandle, writeThreadHandle;

    // int x[2];
    // x[2] = 13413; // for test sanitizer tool

    // Create threads
    pthread_create(&mainProcessThreadHandle, nullptr, &PULSE_GEN::mainProcessThreadWrapper, &pulseGen);
    // pthread_create(&resetThreadHandle, nullptr, &PULSE_GEN::resetThreadWrapper, &pulseGen);
    pthread_create(&writeThreadHandle, nullptr, &PULSE_GEN::writeThreadWrapper, &pulseGen);

    // Simulate events and signals
    pulseGen.ENABLE = 1;
    pulseGen.PRESET = 0;

    pulseGen.inN = 10;
    pulseGen.inNx = 5;

    // Let the program run for a while
    sleep(5);

    // Stop threads
    pthread_cancel(mainProcessThreadHandle);
    // pthread_cancel(resetThreadHandle);
    pthread_cancel(writeThreadHandle);

    // Join threads
    pthread_join(mainProcessThreadHandle, nullptr);
    // pthread_join(resetThreadHandle, nullptr);
    pthread_join(writeThreadHandle, nullptr);

    return 0;
}
