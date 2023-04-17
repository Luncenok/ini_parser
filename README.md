# INI Parser
This is a project for "Computer Architecture with Low Level Programming" classes
[link](https://www.cs.put.poznan.pl/tzok/public/cawllp-02-ini.html)

## INI files
- INI files contain data in basic text format
- The structure of INI file is the following:
```ini
[section1]
key1 = value1
key2 = value2
key3 = value3

; comment
[section2]
key4 = value4
key5 = value5
key6 = value6
```
- There are:
  - Sections with names between brackets
  - Pairs of keys and values
  - Empty lines
  - Comments

## Project
Write a C program to parse INI files using only C standard library
- The program should accept two command line parameters in the format:
```
$ ./program PATH-TO-INI-FILE.ini section.key
```
- For example:
```
$ ./program test.ini data.velocity
```
- Program must print out the value of key in the given section
- For the example above, the program has to print out value under velocity key in [data] section
- Program must detect a missing section and print out dedicated message e.g. Failed to find section [data]
- Program must detect a missing key in a present section e.g. Failed to find key "velocity" in section [data]
- Parse the data into well defined structures e.g. struct section { ... };
- Program must detect invalid identifiers in INI file (section names and keys) i.e. those which contain characters other than letters, digits and dashes (the - character)
- Program must accept arbitrary INI files with section name, key or value of any length (without compile-time limit)
- Program must distinguish types of values between strings and numbers
- Program must be able to understand simple expressions given by the following command line parameters:
```
$ ./program PATH-TO-INI-FILE.ini expression "distance.velocity * travel.time"
```
- The rules for expression evaluation are the following:
  - For numbers, support addition (+ operator), subtraction (- operator), multiplication (* operator) and division (/ operator)
  - For strings, support concatenation (+ operator)
  - Expression involving operands of different types is invalid (detect it and print out a message) e.g. distance.velocity + text.message is invalid
  - Usage of operators -, * and / for string type operands is also invalid e.g. text.hello * text.world
Example files
example-3.0.ini
example-4.5.ini
