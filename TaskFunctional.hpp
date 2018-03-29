#ifndef EXPERIMENTAL_SYSTEM_TASKFUNCTIONAL_HPP
#define EXPERIMENTAL_SYSTEM_TASKFUNCTIONAL_HPP


#define CUSTOM_FUNCTIONAL_PART


namespace communication {

    class TaskFunctionalModule
    {
    private:
    public:
        void push(std::string param1, std::string param2)
        {
            std::cout << "Puhsed to Custom functional part: " << param1 << " " << param2 << std::endl;
        }
    };


    using CustomFunctionalPart = TaskFunctionalModule;
}


#endif //EXPERIMENTAL_SYSTEM_TASKFUNCTIONAL_HPP
