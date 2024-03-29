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
//    char all;
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

//    char all;

//    for (int i = 0; i < temporary.size(); i++)
//    {
//        if (temporary[i].find("all") != std::string::npos)
//        {
//            all = temporary[i][0];
//        }
//    }

    for (int i = 0; i < temporary.size(); i++)
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

            char * end = std::strstr(dispbuf, "all");

            while (ptr < end - 1)
            {
                if (*ptr != '"')
                {
                    resolve_output += *ptr;
                }
                ptr++;
            }
        }
    }

    std::vector <parsedRecord> record;

    parse_txt_record(resolve_output, record);

    for (int i = 0; i < record.size(); i++)
    {
        if(record[i].mechanism == Mechanism::redirect || record[i].mechanism == Mechanism::include)
        {
            flatten(record[i].address, address);            
        }
        else
        {
            std::string recording;

            switch (record[i].mechanism)
            {
                case Mechanism::ip4:
                    recording += "ip4:";
                    break;

                case Mechanism::ip6:
                    recording += "ip6:";
                    break;

                case Mechanism::a:
                    recording += "a:";
                    break;

                case Mechanism::mx:
                    recording += "mx:";
                    break;

                case Mechanism::ptr:
                    recording += "ptr:";
                    break;

                case Mechanism::exists:
                    recording += "exists:";
                    break;

                case Mechanism::exp:
                    recording += "exp=";
                    break;
            }

            recording += record[i].address;

            address.push_back(recording);
        }
    }
}

int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        std::cerr << "spf: missing argument\n Try: spf <domain>";
        return 0;
    }

    int size = 0;

    std::vector <std::string> output;

    flatten(std::string(argv[1]), output);

    std::cout << std::endl << std::endl;

    std::string resolved_spf = "v=spf1 ";

    for (int i = 0; i < output.size(); i++)
    {
        if (size + output[i].size() > 255)
        {
            resolved_spf += "\b\" \"";
            size = 0;
        }
        else
        {
            resolved_spf += output[i] += " ";
        }
        size += output[i].size();        
    }

    resolved_spf += "~all\"";

    std::cout << resolved_spf;

    return 0;
}
