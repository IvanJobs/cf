#include <bits/stdc++.h>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string.hpp>

#include <SQLiteCpp/SQLiteCpp.h>

namespace po = boost::program_options;
namespace bfs = boost::filesystem;

const std::string CREATE_TABLE_CASES = "CREATE TABLE IF NOT EXISTS cases (id INTEGER PRIMARY KEY, tag TEXT, case_input TEXT, case_output TEXT);";
const std::string GET_MAX_ID = "SELECT MAX(id) FROM cases WHERE tag = ?;";
const std::string INSERT_ONE_CASE = "INSERT INTO cases VALUES (?, ?, ?, ?);";
const std::string LS_CASES = "SELECT * FROM cases WHERE tag = ?;";

void new_contest(std::string contest_name) {
  // create contest dir
  bfs::path p("./" + contest_name);
  if (!bfs::create_directory(p)) {
    std::cout<<"create dir for " + contest_name + " failed!"<<std::endl;
    return ;
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
    return ;
  }
  std::cout<<"Done!"<<std::endl;
}

void add_case(std::string source_name) {
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
    return ;
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
    return ;
  }

  std::cout<<"Done!"<<std::endl;
}

void ls_case(std::string source_name) {
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
    return ;
  }
  
  std::cout<<"Done!"<<std::endl;
}

void del_case(std::vector<std::string>& case_ids) {

}

int main(int argc, char* argv[]) {
  po::options_description desc("Support options");
  std::string dir;
  desc.add_options()
    ("help,H", "show help message")
    ("dir", po::value<std::string>(&dir)->default_value("./"))
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
    std::string contest_name = vm["new"].as<std::string>();
    new_contest(contest_name);
    return 0;
  }

  if (vm.count("case-add")) {
    std::string source_name = vm["case-add"].as<std::string>();
    add_case(source_name);
    return 0;
  }

  if (vm.count("case-ls")) {
    std::string source_name = vm["case-ls"].as<std::string>();
    ls_case(source_name);
    return 0;
  }

  if (vm.count("case-del") || vm.count("D")) {
    std::vector<std::string> case_ids = vm["case-del"].as<std::vector<std::string> >();
    del_case(case_ids);
    return 0;
  }

  return 0;
}
