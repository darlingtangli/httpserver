#include <iostream>
#include <string>
#include <cstdio>
#include <util/inv_httpclient.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

using namespace std;
using namespace inv;
using namespace boost;

void doget(char* url, int n)
{
    uint32_t tid = pthread_self();
    fprintf(stderr, "%u start\n", tid);
    while (n-- >0 )
    {
        string s;
        int ret = INV_HttpClient::get(url, s, 100);
        if (ret==0)
            fprintf(stderr, "%u : %s", tid, s.c_str());
        else
            fprintf(stderr, "%u : timeout\n", tid);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        cout << argv[0] << " <url> <threanum> <cycle/thread>\n";
    }

    thread_group tg;
    for (int i = 0; i < atoi(argv[2]); i++)
    {
        tg.create_thread(bind(doget, argv[1], atoi(argv[3])));
    }
    tg.join_all();


    return 0;
}
