#include <bits/stdc++.h>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <SQLiteCpp/SQLiteCpp.h>

namespace po = boost::program_options;
namespace bfs = boost::filesystem;

const std::string CREATE_TABLE_CASES = "CREATE TABLE IF NOT EXISTS cases (id INTEGER PRIMARY KEY, tag TEXT, case_input TEXT, case_output TEXT);";
const std::string GET_MAX_ID = "SELECT MAX(id) FROM cases WHERE tag = ?;";
const std::string INSERT_ONE_CASE = "INSERT INTO cases VALUES (?, ?, ?, ?);";
const std::string LS_CASES = "SELECT * FROM cases WHERE tag = ?;";
const std::string DEL_CASES = "DELETE FROM cases WHERE tag = ? AND id = ?;";
const std::string SELECT_CASES = "SELECT case_input, case_output FROM cases WHERE tag = ?;";

int new_contest(std::string contest_name) {
  // create contest dir
  bfs::path p("./" + contest_name);
  if (!bfs::create_directory(p)) {
    std::cout<<"create dir for " + contest_name + " failed!"<<std::endl;
    return 1;
  }
  // create db table
  //
  try {
    SQLite::Database db("./" + contest_name + "/cf.db3", SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
    SQLite::Transaction trans(db);
    db.exec(CREATE_TABLE_CASES.c_str());
    trans.commit();
  } catch (std::exception& e) {
    std::cout<<"exception: "<<e.what()<<std::endl;
    return 2;
  }
  return 0;
}

int new_file(std::string file_name) {
  std::string cmd_str = "cp /home/vagrant/tpl.cpp ./" + file_name + ".cpp";
  try {
    std::system(cmd_str.c_str());
  } catch (std::exception& e) {
    std::cout<<"exception: "<<e.what()<<std::endl;
    return 1;
  }

  return 0;
}

int add_case(std::string source_name) {
  // get case_input/case_output from user input
  // use empty lines to represent end of input
  std::string input, output;
  std::string line;

  std::cout<<"Please enter case input, end by empty line/lines:"<<std::endl;

  // eliminate preceding empty lines
  getline(std::cin, line);
  while(line.empty()) {
    getline(std::cin, line);
  }
  
  std::vector<std::string> lines; 
  while(!line.empty()) {
    lines.push_back(line);
    getline(std::cin, line);
  }

  input = boost::algorithm::join(lines, "\n");

  std::cout<<"Please enter case output, end by empty line/lines:"<<std::endl;

  // eliminate preceding empty lines
  getline(std::cin, line);
  while(line.empty()) {
    getline(std::cin, line);
  }

  lines.clear();
  while(!line.empty()) {
    lines.push_back(line);
    getline(std::cin, line);
  }

  output = boost::algorithm::join(lines, "\n");

  // get max id, insert into cases
  //
  int max_id = 0;
  SQLite::Database db("./cf.db3", SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
  try {
    SQLite::Statement query(db, GET_MAX_ID.c_str());
    query.bind(1, source_name);
    while (query.executeStep()) { 
      max_id = query.getColumn(0).getInt();
    }
  } catch (std::exception& e) {
    std::cout<<"exception: "<<e.what()<<std::endl;
    std::cout<<GET_MAX_ID<<std::endl;
    return 2;
  }

  try {
    SQLite::Statement query(db, INSERT_ONE_CASE.c_str());
    query.bind(1, max_id + 1);
    query.bind(2, source_name);
    query.bind(3, input);
    query.bind(4, output);
    
    query.exec();
  } catch (std::exception& e) {
    std::cout<<"exception: "<<e.what()<<std::endl;
    std::cout<<INSERT_ONE_CASE<<std::endl;
    return 2;
  }

  return 0;
}

int ls_case(std::string source_name) {
  SQLite::Database db("./cf.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
  try {
    SQLite::Statement query(db, LS_CASES.c_str());
    query.bind(1, source_name);
    
    while(query.executeStep()) {
      int id = query.getColumn(0).getInt();
      std::string tag = query.getColumn(1).getText();
      std::string input = query.getColumn(2).getText();
      boost::replace_all(input, "\n", "\\n");
      std::string output = query.getColumn(3).getText();
      boost::replace_all(output, "\n", "\\n");
      std::cout<<id<<" "<<tag<<" "<<input<<" "<<output<<std::endl; 
    }
  } catch (std::exception& e) {
    std::cout<<"exception: "<<e.what()<<std::endl;
    std::cout<<LS_CASES<<std::endl;
    return 2;
  }

  return 0;
}

int del_case(std::vector<std::string>& case_ids) {
  // first case_id is source_name
  //
  if (case_ids.size() < 2) {
    std::cout<<"Wrong format of arguments!"<<std::endl;
    return 1;
  } 

  std::string source_name = case_ids[0];
  size_t n = case_ids.size();

  SQLite::Database db("./cf.db3", SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);

  for (size_t i = 1; i < n; i++) {
    int case_id = boost::lexical_cast<int>(case_ids[i]);

    try {
      SQLite::Statement query(db, DEL_CASES.c_str());
      query.bind(1, source_name);
      query.bind(2, case_id);
      
      query.exec();
    } catch (std::exception& e) {
      std::cout<<"exception: "<<e.what()<<std::endl;
      std::cout<<DEL_CASES<<std::endl;
      return 2; 
    }
  }

  return 0;
}

int test_case(std::string source_name) {
  // compile source file under current directory.
  //
  int shell_ret = -1;
  std::string cmd_str = "g++ -std=c++11 " + source_name + ".cpp > /tmp/cf_compile_out";
  try {
    shell_ret = std::system(cmd_str.c_str());
  } catch (std::exception& e) {
    std::cout<<"exception: "<<e.what()<<std::endl;
    return 1;
  }
  if (shell_ret != 0) {
    std::cout<<"exception: shell_ret is not 0, something bad happened during compilationi."<<std::endl;
    return 2;
  }

  // query all cases by source name, write to file under /tmp/{source_name}_cases_input.data,/tmp/{source_name}_cases_ans.data.
  //
  SQLite::Database db("./cf.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
  std::vector<std::string> inputs, anses; 
  try {
    SQLite::Statement query(db, SELECT_CASES.c_str());
    query.bind(1, source_name);
    
    while(query.executeStep()) {
      std::string case_input = query.getColumn(0).getText();
      std::string case_output = query.getColumn(1).getText();
      
      inputs.push_back(case_input);
      anses.push_back(case_output); 
    }
  } catch (std::exception& e) {
    std::cout<<"exception: "<<e.what()<<std::endl;
    std::cout<<SELECT_CASES<<std::endl;
    return 3;
  }
  if (inputs.empty() || anses.empty()) {
    std::cout<<"Strange! Didn't get cases from the database, why?"<<std::endl;
    return 4;
  } 

  // iterate every case, and check.
  for (size_t i = 0; i < inputs.size(); i++) {
    int case_num = i + 1;
    // write case_input to /tmp/{source_name}_case_{case_num}.in
    // write case_ans to /tmp/{source_name}_case_{case_num}.ans
    // run the program with case_input and write output to /tmp/{source_name}_case_{case_num}.out
    std::string fn_case_in = "/tmp/" + source_name + "_case_" + boost::lexical_cast<std::string>(case_num) + ".in";
    std::string fn_case_ans = "/tmp/" + source_name + "_case_" + boost::lexical_cast<std::string>(case_num) + ".ans";
    std::string fn_case_out = "/tmp/" + source_name + "_case_" + boost::lexical_cast<std::string>(case_num) + ".out";

    std::ofstream fh_case_in(fn_case_in);
    std::ofstream fh_case_ans(fn_case_ans);
    fh_case_in<<inputs[i];
    fh_case_ans<<anses[i];
    fh_case_in.close();
    fh_case_ans.close();

    std::string cmd_str = "cat " + fn_case_in + " | ./a.out > " + fn_case_out;
    int shell_ret = -1;
    try {
      shell_ret = std::system(cmd_str.c_str());
    } catch (std::exception& e) {
      std::cout<<"exception: "<<e.what()<<std::endl;
      return 5;
    }
    if (shell_ret != 0) {
      std::cout<<"cmd_str: "<<cmd_str<<std::endl;
      std::cout<<"bash failed!"<<std::endl;
      return 6;
    }

    shell_ret = -1;
    cmd_str = "diff -Z -B " + fn_case_ans + " " + fn_case_out;
    try {
      shell_ret = std::system(cmd_str.c_str());
    } catch (std::exception& e) {
      std::cout<<"exception: "<<e.what()<<std::endl;
      return 7;
    }
    if (shell_ret != 0) {
      std::cout<<"case num: "<< case_num << " failed!"<<std::endl;
      std::cout<<"please compare the diff between following two files:"<<std::endl;
      std::cout<<fn_case_out<<std::endl;
      std::cout<<fn_case_ans<<std::endl; 
    } else {
      std::cout<<"case "<<case_num<<" Pass!"<<std::endl;
    }
  }
  
  return 0;
}

int main(int argc, char* argv[]) {
  std::ios_base::sync_with_stdio(false);

  po::options_description desc("Support options");
  std::string dir;
  desc.add_options()
    ("help,h", "show help message.")
    ("new,n", po::value<std::string>(), "new a contest.")
    ("file-add,f", po::value<std::string>(), "new a src file, arg is without the type suffix.")
    // case management
    ("case-add,a", po::value<std::string>(), "add A test case,!!!!don't add multiple cases once!!!!! argument is source file name without type suffix.")
    ("case-ls,l", po::value<std::string>(), "list all test cases, argument is source file name without type suffix.")
    ("case-del,d", po::value< std::vector<std::string> >(), "delete test cases, first one is source name.")
    // run test
    ("test,t", po::value<std::string>(), "run a test suite.");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm); 

  if (vm.count("help")) {
    std::cout<<desc<<std::endl;
    return 0;
  }

  if (vm.count("new")) {
    std::string contest_name = vm["new"].as<std::string>();
    return new_contest(contest_name);
  }

  if (vm.count("file-add")) {
    std::string src_name = vm["file-add"].as<std::string>();
    return new_file(src_name);
  }

  if (vm.count("case-add")) {
    std::string source_name = vm["case-add"].as<std::string>();
    return add_case(source_name);
  }

  if (vm.count("case-ls")) {
    std::string source_name = vm["case-ls"].as<std::string>();
    return ls_case(source_name);
  }

  if (vm.count("case-del")) {
    std::vector<std::string> case_ids = vm["case-del"].as<std::vector<std::string> >();
    return del_case(case_ids);
  }

  if (vm.count("test")) {
    std::string source_name = vm["test"].as<std::string>();
    return test_case(source_name);
  }

  return 0;
}
