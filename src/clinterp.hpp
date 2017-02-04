# ifndef __clinterp__hpp
# define __clinterp__hpp
# include <boost/cstdint.hpp>
# include <boost/cstdlib.hpp>
# include <cstdio>
# include <iostream>
# include <string.h>
# include <stdarg.h>
# define CLINTERP_VERSION "0.0.01"
# define CLINTERP_SEPORATOR ' '
# include <boost/array.hpp>
# include <initializer_list>
# include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
namespace ublas = boost::numeric::ublas;
namespace mdl { class clinterp
{
    public:
    clinterp();
    /* return the starting point of the base intruster */
    std::size_t get_bi_addr(char const * __bi_name);

    /* return the starting point of a bi argument */ 
    std::size_t get_bi_arg_addr(char const *__bi_arg_name) {
        for (std::size_t i = 0 ; i != this-> current.bi_arguments.size() ; i ++)
			if (this-> compare_strings(this-> current.bi_arguments[i], __bi_arg_name)) return i;

		return 0;
    }

    bool does_bi_arg_exist(char const *__bi_name, char const *__bi_arg_name);

    bool is_base_instruct(char const *__bi_name);
    bool is_bi_argument(char const *__bi_arg_name);

    char * get_bi_argument(std::size_t __arg_point) {
        if (__arg_point >= this-> current.bi_arguments.size()) return nullptr;

        std::size_t len = this-> current.bi_arguments[__arg_point].size();

        char *tmp = static_cast<char *>(malloc((len + 1) * sizeof(char)));
        memset(tmp, '\0', (len + 1) * sizeof(char));

        for (std::size_t o = 0; o != len; o ++)
            tmp[o] = this-> current.bi_arguments[__arg_point][o];

        return tmp;
    }

    void add_mcommand(char const *__name);

    void add_base_instruct(char const *__name) {
        this-> add_mcommand(__name);
    }

    char *get_bi_arg_value(char const *__bi_arg_name) {
        std::size_t addr = this-> get_bi_arg_addr(__bi_arg_name);

        std::size_t len_of_val = this-> current.bi_args_value[addr].size();

        char *tmp = static_cast<char *>(malloc((len_of_val + 1) * sizeof(char)));
        memset(tmp, '\0', (len_of_val + 1) * sizeof(char));

        for (std::size_t o = 0; o != len_of_val; o ++)
			tmp[o] = this-> current.bi_args_value[addr][o];

        return tmp;
    }

    void add_bi_argument(char const *__bi_name, char const *__bi_arg_name);

    bool does_mcommand_exist(char const *__name);

    bool compare_strings(ublas::vector<char> __value_0, char const *__value_1);
    bool compare_strings(char const *__value_0, char const *__value_1);

    char **filter(bool ignore);

    char *read_from_term(boost::uint16_t __buff_len, bool __update);
    void write_to_term(char const *__out, ...);

    private:
    typedef struct {
        bool bypass_arg_checking = false;
    } __settings;

    typedef struct {
        ublas::vector<char> base_instruct;
        ublas::vector<ublas::vector<char>> bi_arguments;
        ublas::vector<ublas::vector<char>> bi_args_value;
        __settings settings;
    } i;

    ublas::vector<i> db;
    i current;

    ublas::vector<char> term_input;
} ;
}

# endif /*__clinterp__hpp*/
