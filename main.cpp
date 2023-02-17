#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>

void parse_txt_record(const std::string & record, std::vector<std::string> & parsed)
{
    char * ptr = NULL;

    ptr = std::strtok(const_cast<char *>(record.c_str()), " ");
    while (ptr != NULL)
    {
        parsed.push_back(std::string(ptr));
        ptr = std::strtok(NULL, " ");
    }
}


int main()
{
    unsigned char buf[4096];
    char dispbuf[4096];
    ns_rr rr;

    int size = res_query("optime.dev", C_IN, T_TXT, buf, sizeof(buf)/sizeof(char));

    ns_msg answer;

    ns_initparse(buf, size, &answer);

    size = ns_msg_count(answer, ns_s_an);

    std::string resolve_output;

    for (int i = 0; i < size; i++)
    {
        ns_parserr(&answer, ns_s_an, i, &rr);
        ns_sprintrr(&answer, &rr, NULL, NULL, dispbuf, sizeof(dispbuf));

        char * ptr = NULL;

        if ((ptr = std::strstr(dispbuf, "spf")) != NULL)
        {
            ptr += 5;

            while (*ptr != '"')
            {
                resolve_output += *ptr;
                ptr++;
            }

        }
    }

    std::vector<std::string> output;

    parse_txt_record(resolve_output, output);

    for (auto & a : output)
    {
        std::cout << a << std::endl;
    }

    return 0;
}
