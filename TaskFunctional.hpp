#ifndef EXPERIMENTAL_SYSTEM_TASKFUNCTIONAL_HPP
#define EXPERIMENTAL_SYSTEM_TASKFUNCTIONAL_HPP

#include <queue>
#include <condition_variable>
#include <mutex>

#include "FunctionalClient.hpp"


#define CUSTOM_FUNCTIONAL_PART

namespace communication {

    class Task
    {
    private:
        std::string itsMeta;
        std::string itsData;

    public:
        Task(std::string meta, std::string data)
                : itsMeta(std::move(meta)),
                  itsData(std::move(data))
        {
        }

        void do_work()
        {
            std::cout << "Do work Meta: " << itsMeta << "\n"
                    << "Data: " << itsData
                    << std::endl;
        }

        bool its_cancelling()
        {
            return itsMeta == "cancel";
        }

    };


    class TaskFunctionalModule
    {
    private:
        std::shared_ptr<Client> itsClient;
        std::vector<std::shared_ptr<std::thread>> itsThreads;

        std::queue<Task> itsTaskQueue;
        std::mutex itsMutex;
        std::condition_variable itsCondVariable;

    public:
        explicit TaskFunctionalModule(std::shared_ptr<Client> client)
                : itsClient(std::move(client))
        {
        }

        ~TaskFunctionalModule()
        {
            for (auto & thread : itsThreads){
                thread->join();
            }
        }

        void push(std::string param1, std::string param2)
        {
            std::unique_lock<std::mutex> lock(itsMutex);
            std::cout << "Puhsed to Custom functional part: " << param1 << " " << param2 << std::endl;
            itsTaskQueue.emplace(param1, param2);
            itsCondVariable.notify_one();
        }

        void run_threads()
        {
            auto some_thread = std::make_shared<std::thread>(
                    [this](){

                        bool cycle_running(true);

                        while (cycle_running){

                            std::unique_lock<std::mutex> lock(itsMutex);

                            while (itsTaskQueue.empty()){
                                std::cout << "Task q empty, sleeping..." << std::endl;
                                itsCondVariable.wait(lock);
                                std::cout << "After sleep." << std::endl;
                            }

                            Task task = itsTaskQueue.front();
                            itsTaskQueue.pop();
                            std::cout << "@ Queue size: " << itsTaskQueue.size() << std::endl;

                            lock.unlock();

                            task.do_work();

                            if (task.its_cancelling()){
                                std::cout << "This task is make cancel" << std::endl;
                                break;
                            }
                        }

                        u_int sleep_value = 15;
                        std::cout << "Running some thread, sleep value: " << sleep_value << std::endl;
                        std::this_thread::sleep_for(std::chrono::seconds(sleep_value));
                        std::cout << "Ending sleeping for some thread" << std::endl;
                        itsClient->send_data_to("module_two", prtcl::Body());
                    }
                    );
            itsThreads.push_back(some_thread);
        }
    };


    using CustomFunctionalPart = TaskFunctionalModule;
}


#endif //EXPERIMENTAL_SYSTEM_TASKFUNCTIONAL_HPP
