#include<thread>
#include"2048Game.h"

int main()
{
    char continuePlaying;
    do {
        init2048Game();

        thread t1(handleInput);
        thread t2(updateGame);
        thread t3(displayGame);
        t1.join();
        t2.join();
        t3.join();

        printScore();

        cout << "Play Again? (Y/N)" << endl;
        cin >> continuePlaying;
    } while (continuePlaying=='Y' || continuePlaying=='y');
    
    return 0;
}