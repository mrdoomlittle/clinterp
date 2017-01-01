# include "clinterp.hpp"
# include <vector>
mdl::clinterp::clinterp() {
    
   
}
# define SEP ' '
# define EQULS '='
char ** mdl::clinterp::filter(bool ignore) {
    //printf("size_of_term_input: %ld\n", this-> term_input.size()); 
    if (this-> term_input.size() == 0) return nullptr;
    
    if (this-> current.base_instruct.size() != 0) {
        this-> current.base_instruct.clear();
        this-> current.base_instruct.resize(0);

        this-> current.bi_arguments.clear();
        this-> current.bi_arguments.resize(0);
    }
   
    if (this-> current.bi_args_value.size() != 0) {
        this-> current.bi_args_value.clear();
        this-> current.bi_args_value.resize(0);

    }
 
 
    bool * seporator_mapping = static_cast<bool *>(malloc(this-> term_input.size() * sizeof(bool)));
    memset(seporator_mapping, false, this-> term_input.size() * sizeof(bool));
    bool str_seporator = false;
    for (size_t i = 0; i != term_input.size(); i ++) {
        if (this-> term_input[i] == '"' || str_seporator) {
            if (!str_seporator) { 
                seporator_mapping[i] = true;
                str_seporator = true;
            } else {
                if (this-> term_input[i] == '"') {
                    seporator_mapping[i] = true;
                    str_seporator = false;
                } else {
                    seporator_mapping[i] = false;
                }
            }    
        } else {
            if (this-> term_input[i] == SEP) 
                seporator_mapping[i] = true;
            else
                seporator_mapping[i] = false;
            str_seporator = false;
        }
    }
   
    ublas::vector<char> tmp;
    for (size_t i = 0; i != this-> term_input.size(); i ++) {
        if (seporator_mapping[i] == true) break;
        tmp.resize(tmp.size() + 1); 
        tmp(i) = this-> term_input[i];
    }

    char * base_instruct = static_cast<char *>(malloc(tmp.size() * sizeof(char)));

    for (size_t i = 0; i != tmp.size(); i ++) {
        base_instruct[i] = tmp[i];
    }

    //printf("check is base instruct exists\n");   
    if (this-> does_mcommand_exist(base_instruct) == false) return nullptr;
    //printf("check is base instruct exists: pass\n");

    this-> current.base_instruct.resize(tmp.size());

    for (std::size_t i = 0 ; i != tmp.size() ; i ++)
        this-> current.base_instruct[i] = tmp[i];

    std::size_t arg_point = 0;
    bool begin_counting = false;
    
    ublas::vector<boost::array<std::size_t, 3>> arg_len;
     
    bool first_time = true, skip = false;
    for (size_t i = 0; i != this-> term_input.size(); i ++) {
        if (begin_counting == true) {
            if (seporator_mapping[i])  {
                begin_counting = false;
                skip = false;
            } else {
                if (this-> term_input[i] == EQULS) skip = true;
                if (!skip) arg_len[arg_point][1] += 1; 
                if (skip) arg_len[arg_point][2] ++;
            }
        }

        if (seporator_mapping[i] && !seporator_mapping[i + 1]) {
            arg_len.resize(arg_len.size() + 1);
            begin_counting = true;
          
            if (!first_time) arg_point ++;
            arg_len[arg_point][0] = (i + 1);

            first_time = false;
        }
    }
           
    ublas::vector<char *> args;
       
    this-> current.bi_args_value.resize(arg_len.size());
    std::size_t adr = this-> get_bi_addr(base_instruct);
    for (std::size_t o = 0 ; o != arg_len.size(); o ++) {
        std::size_t k = 0, g = 0;
        bool found_val = false;
        
        char * __tmp = static_cast<char *>(malloc(arg_len[o][1] * sizeof(char) + 1));
        memset(__tmp, '\0', arg_len[o][1] * sizeof(char) + 1);
        for (std::size_t i = arg_len[o][0] ; i != (arg_len[o][0] + arg_len[o][1] + arg_len[o][2]); i ++) {
            
            if (this-> term_input[i] == EQULS || found_val) {
                if (found_val) {
                    this-> current.bi_args_value[o].resize(
                        this-> current.bi_args_value[o].size() + 1);
                    //std::cout << "&> " << this-> term_input[i] << std::endl;                        
                    this-> current.bi_args_value[o][g] = this-> term_input[i];
                    g ++;
                }

                found_val = true;
            } else __tmp[k] = this-> term_input[i];
                     
 
            k ++; 
        }
        if (! this-> does_bi_arg_exist(base_instruct, __tmp)
            && (!this-> db[adr].settings.bypass_arg_checking && !ignore) ) {
            for (std::size_t l = 0; l != args.size(); l ++)
                std::free(args[l]);
            std::free(__tmp);
            return nullptr; 
        }
      
        args.resize(args.size() + 1);
        args(o) = __tmp;
    }
 
    
    this-> current.bi_arguments.resize(arg_len.size());
    for (std::size_t o = 0 ; o != arg_len.size(); o ++) {
        this-> current.bi_arguments[o].resize(arg_len[o][1]);
          
        for (std::size_t l = 0; l != arg_len[o][1]; l ++) {
            this-> current.bi_arguments[o][l] = args[o][l];
            //std::cout << "@ " << this-> current.bi_arguments[o][l] << "," << arg_len[o][1] << std::endl;
        }

    }
    std::free(base_instruct);
    std::free(seporator_mapping);
}

bool mdl::clinterp::is_base_instruct(char const * __bi_name) {
    if (this-> current.base_instruct.size() == 0) return false;

    return this-> compare_strings(this-> current.base_instruct, __bi_name);    
}

bool mdl::clinterp::is_bi_argument(char const * __bi_arg_name) {
    for (std::size_t o = 0; o != this-> current.bi_arguments.size(); o ++) {
        bool res = this-> compare_strings(this-> current.bi_arguments[o], __bi_arg_name);
        if (res) return true;
    }

    return false;
}

void mdl::clinterp::add_mcommand(char const * __name)
{
    if (this-> does_mcommand_exist(__name)) return;
    this-> db.resize(this-> db.size() + 1);

    size_t length_of_name = strlen(__name);

    this-> db[this-> db.size() -1].base_instruct.resize(length_of_name);
    for (size_t i = 0; i != length_of_name; i ++)
        this-> db[this-> db.size() -1].base_instruct(i) = __name[i];
}
 

bool mdl::clinterp::does_mcommand_exist(char const * __name)
{
    if (this-> db.size() == 0) return false;
   
    for (size_t i = 0; i != this-> db.size(); i ++) {
        if (this-> compare_strings(this-> db[i].base_instruct, __name)) return true; 
    }
  
    return false; 
}

std::size_t mdl::clinterp::get_bi_addr(char const * __bi_name) {
    for (std::size_t i = 0 ; i != this-> db.size() ; i ++) {
        if (this-> compare_strings(this-> db[i].base_instruct, __bi_name)) return i;
    }
}

bool mdl::clinterp::does_bi_arg_exist(char const * __bi_name, char const * __bi_arg_name)
{
    std::size_t bi_addr = this-> get_bi_addr(__bi_name);
    for (std::size_t c = 0 ; c != this-> db[bi_addr].bi_arguments.size() ; c ++) {
        bool res = this-> compare_strings(this-> db[bi_addr].bi_arguments[c], __bi_arg_name); 
       
        if (res) return true;
    }

    return false;
}

void mdl::clinterp::add_bi_argument(char const * __bi_name, char const * __bi_arg_name)
{
    if (this-> does_bi_arg_exist(__bi_name, __bi_arg_name)) return;

    std::size_t bi_addr = this-> get_bi_addr(__bi_name);

    this-> db[bi_addr].bi_arguments.resize(
        this-> db[bi_addr].bi_arguments.size() + 1);

    std::size_t length_of_arg = strlen(__bi_arg_name);
    std::size_t addr = (this-> db[bi_addr].bi_arguments.size() - 1);
    
    this-> db[bi_addr].bi_arguments[addr].resize(length_of_arg);

    for (std::size_t i = 0 ; i != length_of_arg ; i ++) {
        this-> db[bi_addr].bi_arguments[addr][i] = __bi_arg_name[i];
    }
}

bool mdl::clinterp::compare_strings(ublas::vector<char> __value_0, char const * __value_1)
{
    bool res = false;
    // the + 1 is because i was having underflow error/s
    char * tmp = static_cast<char *>(malloc(__value_0.size() * sizeof(char) + 1));
    memset(tmp, '\0', __value_0.size() * sizeof(char) + 1);
    for (size_t i = 0; i != __value_0.size(); i ++)
        tmp[i] = __value_0[i];
    res = compare_strings(tmp, __value_1);
    std::free(tmp);
    return res;
}

bool mdl::clinterp::compare_strings(char const * __value_0, char const * __value_1)
{
    size_t len_of_v0 = strlen(__value_0);
    size_t len_of_v1 = strlen(__value_1);
    size_t matching_char_c = 0;

//    std::cout << __value_0 << ", " << __value_1  << std::endl;

    if (len_of_v0 != len_of_v1) return false;

    for (size_t i = 0; i != len_of_v0; i ++)
        if (__value_0[i] == __value_1[i]) matching_char_c ++;

    if (matching_char_c == len_of_v0) return true;
    return false;
}

char * mdl::clinterp::read_from_term(boost::uint16_t __buff_len, bool __update)
{

    char * tmp = static_cast<char *>(malloc(__buff_len * sizeof(char))); 
    memset(tmp, '\0', __buff_len * sizeof(char));

    std::cin.getline(tmp, __buff_len * sizeof(char));

  
    if (__update) {
        this-> term_input.clear(); this-> term_input.resize(0);
        for (size_t i = 0; i != strlen(tmp); i ++) {
            this-> term_input.resize(this-> term_input.size() + 1);
            this-> term_input(i) = tmp[i]; 
        }
    }

  
    return tmp;
}
# define TAG '&'
void mdl::clinterp::write_to_term(char const * __out, ...)
{
    va_list args;
    va_start(args, __out);
  
    std::vector<char const *> tmp;
    std::vector<size_t> paddr;
    
    size_t osize = strlen(__out);
    size_t length_of_ = osize;
    size_t c = 0;
    for (size_t i = 0; i != osize; i ++)
    {
        if (__out[i] == TAG) {
            if (i != 0) if (__out[i-1] == TAG) break;
            if (i != (osize - 1)) if (__out[i+1] == TAG) break;
            if (i != (osize - 1)) if (__out[i] == '\\' && __out[i+1] == 'n') length_of_ --;
          
            paddr.push_back(i);
            //length_of_ --;
            c ++;
            //std::cout << "------>" << std::endl;
        }
    }
    

    for (size_t i = 0; i != c; i ++) {
        char const * dat = va_arg(args, char const *);
        tmp.push_back(dat); 
   }     
    
    std::vector<size_t> plen;
   
    size_t red = 0;
    for (size_t i = 0; i != tmp.size(); i ++) {
        size_t sz = strlen(tmp[i]);
        for (size_t o = 0; o != sz; o ++) {
            if (i != (sz - 1)) if (tmp[i][o] == '\\' && tmp[i][o+1] == 'n') red ++;
        }
        length_of_ += (sz - red) - 1;
        plen.push_back(sz);
        red = 0; 
    }

    char * out = static_cast<char *>(malloc(length_of_ * sizeof(char) + 12));
    
    memset(out, '\0', length_of_ * sizeof(char)+12);

    bool no_action = false;
    size_t k = 0, p = 0, tk = 0;
    for (size_t i = 0; i <= length_of_; i ++) {
  
 //       if (paddr.size() != 0) {
            if (paddr.size() != 0 && k == paddr.at(p)) {
              
                for (size_t o = 0; o != plen.at(p); o ++) {
                    if (no_action) o += tk;
                    no_action = false;
                    if (o != (plen.at(p) - 1))
                    {
                        if (tmp[p][o] == '\\' && tmp[p][o + 1] == 'n') {
                            out[i] = '\n';
                            tk ++;
                            
                            no_action = true;
                        }
                    }

                    if (no_action == false)
                        out[i] = tmp[p][o];
                   
                    //std::cout << "---->" << std::endl;
                    if (o != (plen.at(p) - 1)) i ++;
                }
                tk = 0;
                if (paddr.size() != p+1) { p ++; }
           } else {
        if (no_action) k += tk;
        tk = 0; no_action = false;

        if (i != (length_of_ - 1)) {
            if (__out[k] == '\\' && __out[k + 1] == 'n') {
                out[i] = '\n';
                tk ++;
                no_action = true;            
            }
        }         
      
        if (no_action == false) 
            out[i] = __out[k];
}
   
        if (k != (osize - 0)) k ++;
    }

    
  
    va_end(args);

    printf("%s", out);
    std::free(out); 
}
