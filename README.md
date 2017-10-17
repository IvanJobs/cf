# cf
Command Line Tool For Codeforces.

# why to develop this project 
Time limit is important in competitive programming contests. 
As to Codeforces, I have felt like to develop a cli tool to do cases management and run test cases for a long time.
I once developed a version of this project in bash, but deleted it in the end because bash is not so convenient in someway.
At the end, I choose c++ to implement this project, hope more guys will come to contribute it. 

# features 
1. cases management.
2. test running.

# cli schema 
```
Support options:
  -h [ --help ]         show help message.
  -n [ --new ] arg      new a contest.
  -a [ --case-add ] arg add A test case,!!!!don't add multiple cases once!!!!!
                        argument is source file name without type suffix.
  -l [ --case-ls ] arg  list all test cases, argument is source file name
                        without type suffix.
  -d [ --case-del ] arg delete test cases, first one is source name.
  -t [ --test ] arg     run a test suite.
```

# todo list
1. change GUN option style to subcommand style.
