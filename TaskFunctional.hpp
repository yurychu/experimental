#ifndef EXPERIMENTAL_SYSTEM_TASKFUNCTIONAL_HPP
#define EXPERIMENTAL_SYSTEM_TASKFUNCTIONAL_HPP


#include "FunctionalClient.hpp"


#define CUSTOM_FUNCTIONAL_PART

namespace communication {

    class TaskFunctionalModule
    {
    private:
        std::shared_ptr<Client> itsClient;
        std::vector<std::shared_ptr<std::thread>> itsThreads;

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
            std::cout << "Puhsed to Custom functional part: " << param1 << " " << param2 << std::endl;
        }

        void run_threads()
        {
            int i = 0;
            std::cout << i << std::endl;
            auto some_thread = std::make_shared<std::thread>(
                    [this](){
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
