# Universal Planning Validator

Software that can be used to validate:
* Classical planning problems.
* Planning programs.

_The software is still under heavy development_. You can take a look to the [issues](https://github.com/aig-upf/universal-planning-validator/issues) to know about current bugs. In the future we plan to add support for multiagent and temporal planning problems.

## Installation

Firstly, make sure you have the `scons` tool installed in your computer. You will need it to compile the software.

Then, you have to either clone or download this repository. To clone it, you can use the following command:

```
git clone https://github.com/aig-upf/universal-planning-validator.git
```

This repository references the `universal-pddl-parser` repository. There are two ways of referencing that repository:

1. You use the `universal-pddl-parser` submodule inside this repository.
1. You use the `PDDL_PARSER_PATH` environment variable, which should contain the path to the `universal-pddl-parser` repository.

Finally, to compile the repository tools, run the following command:

```
cd universal-planning-validator
./build.sh
```

Once the software is built, you can run the tests to check that the validator
is running:

```
./tests/test.bin
```

## Running the validator

Currently, the software is used as follows:

```
./validate [options] <domain.pddl> <task.pddl> <plan>
```

where the possible options are:

* `-h` to show help.
* `-c` to specify that the input is a classical planning problem.
* `-p` to specify that the input is a planning program.
* `-v` to print verbose information while validating (added and deleted fluents).
