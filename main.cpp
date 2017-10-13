#include <bits/stdc++.h>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  po::options_description desc("Support options");
  desc.add_options()
    ("help,H", "show help message")
    ("new,N", po::value<std::string>(), "new a contest")
    ("case-add,A", po::value<std::string>(), "add a test case")
    ("case-ls,L", po::value<std::string>(), "list all test cases")
    ("case-del,D", po::value< std::vector<std::string> >(), "delete test cases");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm); 

  if (vm.count("help")) {
    std::cout<<desc<<std::endl;
    return 0;
  }

  if (vm.count("new")) {
    std::cout<<"new"<<std::endl; 
  }


  return 0;
}
