#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <functional>

#include <nodec/observers.hpp>

TEST_CASE("Check to notify on not reentrant") {
    class IObserver {
    public:
        virtual void on_notify() = 0;
    };

    class TestObserver : public IObserver {
    public:
        void on_notify() override {
            notified = true;
        }

        bool notified = false;
    };

    SUBCASE("add and remove") {
        nodec::Observers<IObserver> observers;

        TestObserver observer1;

        observers.add_observer(&observer1);
        CHECK(observer1.notified == false);

        observers.each([](IObserver *observer) {
            observer->on_notify();
        });
        CHECK(observer1.notified == true);

        observers.remove_observer(&observer1);

        observer1.notified = false;
        observers.each([](IObserver *observer) {
            observer->on_notify();
        });
        CHECK(observer1.notified == false);
    }
}

TEST_CASE("Check to notify on reentrant") {
    class IObserver {
    public:
        virtual void on_notify() = 0;
    };

    class TestObserver : public IObserver {
    public:
        void on_notify() override {
            callback();
        }

        std::function<void()> callback;
    };

    SUBCASE("add while notify") {
        nodec::Observers<IObserver> observers;

        TestObserver observer1;
        TestObserver observer2;
        bool notified_1 = false;
        bool notified_2 = false;
        observer1.callback = [&]() {
            observers.add_observer(&observer2);
            notified_1 = true;
        };
        observer2.callback = [&]() {
            notified_2 = true;
        };

        observers.add_observer(&observer1);

        observers.each([](IObserver *observer) {
            observer->on_notify();
        });

        CHECK(notified_1 == true);
        CHECK(notified_2 == false);
    }
}