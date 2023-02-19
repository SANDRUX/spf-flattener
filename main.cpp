#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>

enum class Mechanism
{
    ip4,
    ip6,
    a,
    mx,
    ptr,
    exists,
    include,
    redirect,
    exp,
};

struct parsedRecord
{
    std::string address;
    Mechanism mechanism;
};

void parse_txt_record(const std::string & record, std::vector<parsedRecord> & parsed)
{
    std::vector<std::string> temporary;
    char * ptr = NULL;

    ptr = std::strtok(const_cast<char *>(record.c_str()), " ");
    while (ptr != NULL)
    {
        temporary.push_back(std::string(ptr));
        ptr = std::strtok(NULL, " ");
    }

    for (int i = 0; i < temporary.size() - 1; i++)
    {
        if (temporary[i].find("ip4") != std::string::npos)
        {
            parsed.push_back({std::string(temporary[i].c_str() + 4), Mechanism::ip4});
        }

        else if (temporary[i].find("ip6") != std::string::npos)
        {
            parsed.push_back({std::string(temporary[i].c_str() + 4), Mechanism::ip6});
        }

        else if (temporary[i][0] == 'a')
        {
            if (temporary[i].size() == 1)
            {
                parsed.push_back({std::string(" "), Mechanism::a});
            }
            else
            {
                parsed.push_back({std::string(temporary[i].c_str() + 2), Mechanism::a});
            }
        }

        else if (temporary[i].find("mx") != std::string::npos)
        {
            if (temporary[i].size() == 2)
            {
                parsed.push_back({std::string(" "), Mechanism::mx});
            }
            else
            {
                parsed.push_back({std::string(temporary[i].c_str() + 3), Mechanism::mx});
            }
        }

        else if (temporary[i].find("ptr") != std::string::npos)
        {
            if (temporary[i].size() == 3)
            {
                parsed.push_back({std::string(" "), Mechanism::ptr});
            }
            else
            {
                parsed.push_back({std::string(temporary[i].c_str() + 4), Mechanism::ptr});
            }
        }

        else if (temporary[i].find("exists") != std::string::npos)
        {
            parsed.push_back({std::string(temporary[i].c_str() + 7), Mechanism::exists});
        }

        else if (temporary[i].find("include") != std::string::npos)
        {
            parsed.push_back({std::string(temporary[i].c_str() + 8), Mechanism::include});
        }

        else if (temporary[i].find("redirect") != std::string::npos)
        {
            parsed.push_back({std::string(temporary[i].c_str() + 9), Mechanism::redirect});
        }

        else if (temporary[i].find("exp") != std::string::npos)
        {
            parsed.push_back({std::string(temporary[i].c_str() + 4), Mechanism::exp});
        }
    }
}

void flatten(const std::string & domain, std::vector<std::string> & address)
{
    unsigned char buf[4096];
    char dispbuf[4096];
    ns_rr rr;

    int size = res_query(domain.c_str(), C_IN, T_TXT, buf, sizeof(buf)/sizeof(char));

    ns_msg answer;

    ns_initparse(buf, size, &answer);

    size = ns_msg_count(answer, ns_s_an);

    std::string resolve_output;

    for (int i = 0; i < size; i++)
    {
        ns_parserr(&answer, ns_s_an, i, &rr);
        ns_sprintrr(&answer, &rr, NULL, NULL, dispbuf, sizeof(dispbuf));

        char * ptr = NULL;

        if ((ptr = std::strstr(dispbuf, "v=spf")) != NULL)
        {
            ptr += 7;

            while (*ptr != '"')
            {
                resolve_output += *ptr;
                ptr++;
            }

        }
    }
    std::vector <parsedRecord> record;

    parse_txt_record(resolve_output, record);

    for (int i = 0; i < record.size(); i++)
    {
        if (record[i].mechanism == Mechanism::ip4)
        {
            address.push_back(record[i].address);
        }
        else if (record[i].mechanism == Mechanism::ip6)
        {
            address.push_back(record[i].address);
        }
        else if(record[i].mechanism == Mechanism::redirect || record[i].mechanism == Mechanism::include)
        {
            flatten(record[i].address, address);
        }
    }
}

int main()
{
    std::vector <std::string> output;
    flatten("optime.dev", output);

    for (int i = 0; i < output.size(); i++)
    {
        std::cout << output[i] << std::endl;
    }

    return 0;
}
