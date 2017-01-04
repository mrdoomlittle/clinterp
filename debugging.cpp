# include <clinterp.hpp>

int main() {
    mdl::clinterp cl;

  
    cl.add_base_instruct("qgdb");
 
    cl.add_bi_argument("qgdb", "--set");
    cl.add_bi_argument("qgdb", "--get");
    cl.add_bi_argument("qgdb", "--add");   
    //std::cout << cl.does_bi_arg_exist("hello", "pop") << std::endl;
    //return 0;
    //std::cout << cl.does_mcommand_exist("hello") << std::endl;    
    for (;;) {
        char * tmp = cl.read_from_term(128, true);
        cl.filter(true);
        std::free(tmp);
 
        std::cout << "base state: " << cl.is_base_instruct("qgdb") << std::endl;
        if (cl.is_base_instruct("qgdb") && cl.is_bi_argument("you")) {
            cl.write_to_term("hello & & nice to\\nmeet you. &\n", "my name", "is mrdoomlittle", "12");
            std::cout << cl.get_bi_arg_value("you") << std::endl; 
        }
return 0;
        //char *

      //  std::cout << cl.get_bi_argument(2) << "," << cl.get_bi_arg_value("--set") << "," << cl.get_bi_arg_value("name") << ", " << cl.get_bi_arg_value("value")<< std::endl;
   }
}
