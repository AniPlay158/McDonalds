#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <windows.h>

using namespace std;

struct Order
{
    string dishName;
    int cookTime;
};

class OrderProcessor
{
private:
    queue<Order> orderQueue;
    mutex mtx;
    vector<string> availableDishes = { "Чизбургер", "Чикен рол", "кола", "Дабл чизбургер", "Апельсиновий сiк", "Роял Делюкс", "Биг тейсті" };
    const int maxCookingCapacity = 10;

public:

    void addOrder(const Order& order)
    {
        mtx.lock();
        orderQueue.push(order);
        mtx.unlock();
    }


    void processOrders()
    {

        while (!orderQueue.empty())
        {

            mtx.lock();
            if (orderQueue.empty())
            {

                mtx.unlock();
                continue;

            }

            Order order = orderQueue.front();
            orderQueue.pop();
            mtx.unlock();


            auto it = find(availableDishes.begin(), availableDishes.end(), order.dishName);

            if (it != availableDishes.end())
            {

                cout << "Приготовлено: " << order.dishName << "\n";

                this_thread::sleep_for(chrono::seconds(order.cookTime));
                cout << "Страва '" << order.dishName << "' готова, приємного апетиту!\n";
                Sleep(3000); 
                system("cls");
            }
            else
            {

                cout << "Страву '" << order.dishName << "' не можна приготувати.\n";
            }
        }
    }
};

int main()
{
    setlocale(LC_ALL, "");
    OrderProcessor processor;

    processor.addOrder({ "Чизбургер", 3 });
    processor.addOrder({ "кола", 2 });
    processor.addOrder({ "Чикен рол", 4 });
    processor.addOrder({ "Дабл чизбургер", 3 });
    processor.addOrder({ "Апельсиновий сiк", 2 });
    processor.addOrder({ "Роял Делюкс", 5 });
    processor.addOrder({ "Биг тейсті", 5 });

    thread orderThread(&OrderProcessor::processOrders, &processor);

    orderThread.join();

    return 0;
}
