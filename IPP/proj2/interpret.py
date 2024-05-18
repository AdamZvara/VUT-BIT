"""
Author  : Adam Zvara, xzvara01@stud.fit.vutbr.cz
Date    : 12.4.2022
Brief   : Main interpreter file
"""

from program import Program, Instruction
from argparse import ArgumentParser
from instruction import Instruction
from sys import exit, stdin
import xml.etree.ElementTree as ET

class InputSource:
    """Class for handling source and input files given with --source and --input options"""

    def __init__(self):
        """Set private input and source variables"""
        args = self.__parse_args()
        if args.source is None and args.input is None:
            exit(10)

        if args.input is not None:
            try:
                self.__input = open(args.input, "r")
            except FileNotFoundError:
                exit(11)
        else:
            self.__input = None

        self.__source = args.source if args.source is not None else stdin

    def __parse_args(self):
        """Parse source and input options"""
        arg_parser = ArgumentParser(description='Interpret code given in XML format')
        arg_parser.add_argument('--source', help='source XML file')
        arg_parser.add_argument('--input', help='input to interpreted program')
        return arg_parser.parse_args()

    def get_input_line(self):
        """Return single line from user input or None if EOF has been reached"""
        if self.__input is None:
            # reading input from console
            try:
                return input()
            except EOFError:
                return None
        else:
            # reading input from file
            line = self.__input.readline()
            # if readline returns empty string EOF has been reached and return None
            # readline represents empty line with '\n'
            return line.rstrip('\n') if line != '' else None

    def get_source(self):
        """Return source filename"""
        return self.__source

class XML_representation:
    """Class to encapsulate working with XML representation"""

    def __init__(self, source : str):
        """Parse source file using ElementTree"""
        try:
            self.tree = ET.parse(source)
            # list containing all possible 'arg'+number values
            self.__max_args = ['arg'+str(i) for i in range(1, Instruction.max_args()+1)]
        except ET.ParseError:
            exit(31)
        # Set root and check if program element tags are valid
        self.root = self.tree.getroot()
        if self.root.tag != 'program' or ('language','IPPcode22') not in self.root.attrib.items():
            exit(32)

    def get_root(self):
        """Return program root"""
        return self.root

    def instruction_valid(self, instruction : ET.Element):
        """Check if instruction is in valid format"""
        if instruction.tag != 'instruction':
            exit(32)
        keys = instruction.attrib.keys()
        if 'order' not in keys or 'opcode' not in keys:
            exit(32)

    def argument_valid(self, argument : ET.Element):
        """Check if argument is in valid range"""
        if argument.tag not in self.__max_args or 'type' not in argument.attrib.keys():
            exit(32)

input_files = InputSource()
xml = XML_representation(input_files.get_source())
program = Program()

# Parse XML file into program object
for instruction_element in xml.get_root():
    xml.instruction_valid(instruction_element)
    attribute = instruction_element.attrib
    # create new instruction object to be inserted into program instructions
    instr = Instruction(attribute['opcode'], attribute['order'])
    for argument in instruction_element:
        xml.argument_valid(argument)
        instr.newarg(argument)
    instr.arg_valid_amount()
    program.add_instruction(instr)

# Sort program instruction and arguments
program.sort()

# Scan instructions and create labels
program.create_labels()

# Interpret instructions
while instruction := program.get_instruction():
    opcode = instruction.opcode

    # instruction arguments can be accessed with [] operator, passing *instruction expands argument list

    # instructions with same function name as instruction name that take multiple arguments
    if opcode in ['DEFVAR', 'WRITE', 'MOVE', 'JUMP', 'DPRINT', 'EXIT', 'TYPE', 'CALL', 'PUSHS', 'POPS', 'FLOAT2INT', 'INT2FLOAT',
    'INT2CHAR', 'STRI2INT', 'CONCAT', 'STRLEN', 'GETCHAR', 'SETCHAR', 'EQ', 'GT', 'LT', 'JUMPIFEQS', 'JUMPIFNEQS', 'JUMPIFEQ', 'JUMPIFNEQ']:
        getattr(program, opcode.lower())(*instruction)

    # instructions with same function name as instruction name + "function" that take multiple arguments
    elif opcode in ['AND', 'OR', 'NOT', 'RETURN']:
        getattr(program, opcode.lower() + 'function')(*instruction)

    # instructions with same function name as instruction name that take no arguments
    elif opcode in ['CREATEFRAME', 'PUSHFRAME', 'POPFRAME', 'STRI2INTS', 'ANDS', 'ORS', 'NOTS', 'EQS', 'GTS', 'LTS', 'INT2CHARS', 'CLEARS']:
        getattr(program, opcode.lower())()

    # arithmetic functions
    elif opcode in ['ADD', 'SUB', 'IDIV', 'MUL', 'DIV', 'ADDS', 'SUBS', 'IDIVS', 'MULS']:
        program.arithmetic_functions(instruction)

    # read instruction
    elif opcode == 'READ':
        user_input = input_files.get_input_line()
        program.read(*instruction, user_input)

    # label instruction
    elif opcode == 'LABEL':
        pass

    else:
        exit(32)

    program.next_instr()