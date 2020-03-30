#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <ctime>
#include <chrono>
#include <thread>
 
 
class Command
{
public:
    Command(std::string commandName):
        commandName(commandName)
    {
        time = std::time(nullptr);
        // std::cout << std::to_string(time) <<std::endl;
    }
    std::string commandName;
    int time;
};
 
class Observer
{
public:
    virtual void update(std::list<Command *>& commands) = 0;
};
 
class Printer
{
    int n;  //Размер блока. Задается в конструкторе
    int nCounter = 0;  //счетчик вложенных скобок
    int historySize = 0;  //Размер буфера
    std::vector<Observer*> subs; //Подписчики
    std::list<Command*> buffer; //Список команд
public:
    Printer(int n):n(n){}
    Printer() = delete;
    void subscribe(Observer *obs)
    {
        subs.push_back(obs);
    }
 
    void printAll(std::list<Command *>& commands)
    {
        for(auto &s: subs)
        {
            s->update(commands);
        }
        historySize = 0;
        buffer.clear();
    }
 
    void print(std::string cmd)
    {
        if (cmd[0] == '{')
        {
            if (!nCounter && historySize) //Вывести буфер, если открылась ПЕРВАЯ скобка
            {
                printAll(buffer);
            }
            ++nCounter;
        }
        else if (cmd[0] == '}')
        {
            --nCounter;
            if (nCounter <= 0)  //Вывести буфер, если закрылась ПЕРВАЯ скобка
            {
                printAll(buffer);
            }
           
        }
        else if (!std::cin.eof())
        {
            buffer.push_back(new Command(cmd));
            if (!nCounter)
            {
                ++historySize;
                if (historySize >=n)
                {
                    printAll(buffer);
                }
            }
        }
        else if (historySize && !nCounter)
        {
            //std::cout <<"historySize = " << historySize <<std::endl;
            printAll(buffer);
        }
    }
};
 
class Console: public Observer
{
public:
    Console(Printer& print)
    {
        print.subscribe(this);
    }
 
    void update(std::list<Command *>& commands) override
    {
        std::string text = "";
        for (auto command : commands)
        {
            text+=command->commandName + " ";
        }
        std::cout << "bulk: " << text <<std::endl;
    }
};
 
class File: public Observer
{
public:
    File(Printer& print)
    {
        print.subscribe(this);
    }
 
    void update(std::list<Command *>& commands) override
    {
        //std::this_thread::sleep_for(std::chrono::seconds(1));
        std::string text = "";
        std::string fileName = "bulk" + std::to_string(commands.front()->time);
        std::ofstream outfile ("./" + fileName + ".txt",std::ofstream::binary);
        for (auto command : commands)
        {
            text+=command->commandName + "\n";
        }
        outfile.write(text.c_str(),text.size());
        outfile.close();
 
        std::cout << fileName + ".txt created\n" << std::endl;
    }
};
 
int main(int argc, char** argv)
{
   
    int n; //Размер блока команд
   
    //std::cout << "Введите размер блока: ";
    //std::cin >> n;
    n = std::stoi(argv[1]);
 
    //std::cout<<"The argument is"<<argv[1]<<std::endl;
 
    std::string str;
    //std::getline(std::cin, str); //После std::cin первый getline не срабатывает
 
    Printer printer{n}; //Класс вывода на экран
    Console console{printer}; //Подписать консольно на вывод
    File file{printer}; //Подписать файл на вывод
 
    while (!std::cin.eof()) //Cntr + D -конец файла
    {
        std::getline(std::cin, str);
        printer.print(str);
    }
    return 0;
}