"""
Author  : Adam Zvara, xzvara01@stud.fit.vutbr.cz
Date    : 12.4.2022
Brief   : Program class implementing instruction methods
"""

from instruction import Instruction, Argument
from variable_manager import Variable_manager
from stack import Stack
from re import search
from sys import stderr

class Label:
    """Label class containing name and position in instruction list"""
    def __init__(self, name : str, pos : int):
        self.__name = name
        self.__pos = pos

    @property
    def name(self):
        return self.__name

    @property
    def pos(self):
        return self.__pos

class Program:
    """
    Class for interpreting program which contains instruction objects stored in instructions list
    and keeps position of currently interpreted instruction in internal counter instructions_pos
    """
    __instructions = list()
    __instructions_pos = 0
    __call_stack = list()
    __stack = Stack()
    __var_manager = Variable_manager()
    __types = ['int', 'bool', 'nil', 'string', 'float']

    def get_instruction(self):
        """Return next instruction to be interpreted or None if there are no more instructions"""
        if self.__instructions_pos < len(self.__instructions):
            return self.__instructions[self.__instructions_pos]
        else:
            return None

    def next_instr(self):
        """Increment instruction counter"""
        self.__instructions_pos += 1

    def add_instruction(self, instr : Instruction):
        """Add instruction object to instruction list"""
        for i in self.__instructions:
            if i.order == instr.order:
                exit(32)
        self.__instructions.append(instr)

    def sort(self):
        """Sort instructions by order and sort arguments for each instruction"""
        self.__instructions.sort(key=lambda x: x.order)
        for instruction in self.__instructions:
            instruction.sort()
            instruction.arg_valid()

    def create_labels(self):
        """Iterate thourgh instructions and create label objects"""
        self.__labels = list()
        for pos, instruction in enumerate(self.__instructions):
            if instruction.opcode == 'LABEL':
                name = instruction[0].value
                if self.label_find(name):
                    exit(52)
                self.__labels.append(Label(name, pos))

    def label_find(self, name : str):
        """Return label with given name or None if it does not exist"""
        for l in self.__labels:
            if l.name == name:
                return l
        return None

    # Private functions used for interpreting

    def __literal_or_variable(self, operand):
        """Returns value and type of given operand (can be variable or constant)"""
        if (operand.type == 'var'):
            value = self.__var_manager.get_value(operand.value)
            value_type = self.__var_manager.get_type(operand.value)
            if (value == None):
                exit(56)
            return value, value_type
        else:
            return operand.value, operand.type

    def __escape_sequences(self, string : str):
        """Escape all escape sequences in string"""
        escape_re = "\\\\[0-9][0-9][0-9]"
        while (matches := search(escape_re, string)) != None:
            escape = matches.regs[0]
            escaped = chr(int(string[escape[0]+1:escape[1]]))
            string = string[:escape[0]] + escaped + string[escape[1]:]
        return string

    # INSTRUCTION METHODS

    def defvar(self, arg):
        self.__var_manager.add(arg)

    def move(self, src, arg):
        arg, arg_t = self.__literal_or_variable(arg)
        self.__var_manager.insert_value(src, arg, arg_t)

    # FRAMES MANIPULATION

    def createframe(self):
        self.__var_manager.TF_create()

    def pushframe(self):
        self.__var_manager.TF_push()

    def popframe(self):
        self.__var_manager.TF_pop()

    # DEBUG

    def dprint(self, arg):
        value, _ = self.__literal_or_variable(arg)
        print(value, end='', file=stderr)

    def exit(self, arg):
        value, value_t = self.__literal_or_variable(arg)
        if value_t != 'int':
            exit(53)
        if value < 0 or value > 49:
                exit(57)
        exit(value)

    def type(self, var, symbol):
        for type in self.__types:
            if symbol.type == type:
                self.__var_manager.insert_value(var, type, 'string')
        if symbol.type == 'var':
            self.__var_manager.insert_value(var, self.__var_manager.get_type(symbol.value), 'string')

    # INPUT OUTPUT FUNCTIONS

    def write(self, arg):
        if arg.type == 'var':
            (result,type) = self.__var_manager.print(arg)
            if type == 'string':
                result = self.__escape_sequences(result)
        else:
            if arg.type == 'string':
                result = self.__escape_sequences(str(arg.value))
            elif arg.type == 'float':
                result = float.hex(arg.value)
            elif arg.type == 'nil':
                result = ''
            elif arg.type == 'bool':
                if arg.value == True:
                    result='true'
                else:
                    result='false'
            else:
                result = arg.value
        print(result, end='')

    def read(self, var, type, user_input):
        self.__var_manager.get_value(var.value)
        if type.value not in ['int', 'string', 'bool', 'float']:
            exit(53)
        try:
            if user_input is None:
                raise Exception
            if type.value == 'bool':
                if user_input.lower() == 'true':
                    self.__var_manager.insert_value(var, True, 'bool')
                else:
                    self.__var_manager.insert_value(var, False, 'bool')
            elif type.value == 'int':
                self.__var_manager.insert_value(var, int(user_input), 'int')
            elif type.value == 'string':
                self.__var_manager.insert_value(var, user_input, 'string')
            elif type.value == 'float':
                self.__var_manager.insert_value(var, float.fromhex(user_input), 'float')
        except Exception:
            self.__var_manager.insert_value(var, 'nil', 'nil')

    # JUMP FUNCTIONS

    def jump(self, label : Argument):
        if not (label := self.label_find(label.value)):
            exit(52)
        self.__instructions_pos = label.pos

    def call(self, label : Argument):
        self.__call_stack.append(self.__instructions_pos)
        self.jump(label)

    def returnfunction(self):
        if len(self.__call_stack) > 0:
            self.__instructions_pos = self.__call_stack.pop()
        else:
            exit(56)

    # STACK BASIC FUNCTIONS

    def pushs(self, arg) -> None:
        value, type = self.__literal_or_variable(arg)
        if value == None:
            exit(56)
        if type == 'string':
            value = self.__escape_sequences(value)
        self.__stack.push(value, type)

    def pops(self, var : Argument) -> None:
        if var.type != 'var':
            exit(56)
        stack_item = self.__stack.pop()
        self.__var_manager.insert_value(var, stack_item.value, stack_item.type)

    # ARITHMETIC FUNCTIONS

    def __set_args_arithmetics(self, op1, op2):
        """Check type compatibilty of operands before arithmetic operation, return their values and result type"""
        allowed = ['int', 'var', 'float']
        if op1.type not in allowed or op2.type not in allowed:
            exit(53)
        val1, val1_t = self.__literal_or_variable(op1)
        val2, val2_t = self.__literal_or_variable(op2)
        if val1 is None or val2 is None:
            exit(56)
        if val1_t != val2_t:
            exit(53)
        if type(val1) is float:
            result_type = 'float'
        else:
            result_type = 'int'
        return val1, val2, result_type

    def __arithmetics(self, opcode, var, value1, value2, result_type):
        """Execute arithmetic operation with given values and store result in given variable"""
        if opcode == 'ADD':
            self.__var_manager.insert_value(var, value1+value2, result_type)
        elif opcode == 'SUB':
            self.__var_manager.insert_value(var, value1-value2, result_type)
        elif opcode == 'MUL':
            self.__var_manager.insert_value(var, value1*value2, result_type)
        elif opcode == 'IDIV':
            if value2 == 0:
                exit(57)
            self.__var_manager.insert_value(var, value1//value2, result_type)
        elif opcode == 'DIV':
            if type(value1) is not float or type(value2) is not float:
                exit(53)
            if value2 == 0:
                exit(57)
            self.__var_manager.insert_value(var, value1//value2, result_type)

    def __set_args_arithmetics_stack(self):
        """Pop oprands from stack and return their values with result type (only integer supported)"""
        allowed = ['int', 'var']
        value2 = self.__stack.pop()
        value1 = self.__stack.pop()
        if (value1.type not in allowed) or (value2.type not in allowed) or (type(value1.value) != type(value2.value)):
            exit(53)
        return value1.value, value2.value, 'int'

    def __arithmetics_stack(self, opcode, value1, value2):
        """Execute arithmetic operation with given values and push result on stack"""
        if opcode == 'ADDS':
            self.__stack.push(value1+value2)
        elif opcode == 'SUBS':
            self.__stack.push(value1-value2)
        elif opcode == 'MULS':
            self.__stack.push(value1*value2)
        elif opcode == 'IDIVS':
            if value2 == 0:
                exit(57)
            self.__stack.push(value1//value2)

    def arithmetic_functions(self, instruction):
        if instruction.opcode[-1] == 'S':
            value1, value2, result_type = self.__set_args_arithmetics_stack()
            self.__arithmetics_stack(instruction.opcode, value1, value2)
        else:
            var, operand1, operand2 = instruction
            (value1, value2, result_type) = self.__set_args_arithmetics(operand1, operand2)
            self.__arithmetics(instruction.opcode, var, value1, value2, result_type)

    # CONVERSION METHODS

    def __pre_conversion(self, arg, desired_type, value_from_stack = False):
        """Check type compatibility of value before conversion to desired type, return its value"""
        if value_from_stack == False:
            val, val_t = self.__literal_or_variable(arg)
        else:
            val, val_t = arg.value, arg.type
        if val == None:
            exit(56)
        elif val_t != desired_type:
            exit(53)
        return val

    def float2int(self, dst, arg):
        val = self.__pre_conversion(arg, 'float')
        try:
            self.__var_manager.insert_value(dst, int(val), 'int')
        except Exception:
            exit(58)

    def int2float(self, dst, arg):
        val = self.__pre_conversion(arg, 'int')
        try:
            self.__var_manager.insert_value(dst, float(val), 'float')
        except Exception:
            exit(58)

    def int2char(self, dst, arg):
        val = self.__pre_conversion(arg, 'int')
        try:
            self.__var_manager.insert_value(dst, chr(val), 'string')
        except Exception:
            exit(58)

    def stri2int(self, dst, arg, pos):
        val = self.__pre_conversion(arg, 'string')
        pos = self.__pre_conversion(pos, 'int')
        try:
            if pos < 0:
                raise Exception
            self.__var_manager.insert_value(dst, ord(val[pos]), 'int')
        except Exception:
            exit(58)

    def int2chars(self):
        val = self.__pre_conversion(self.__stack.pop(), 'int', True)
        try:
            self.__stack.push(chr(val), 'string')
        except Exception:
            exit(58)

    def stri2ints(self):
        val = self.__pre_conversion(self.__stack.pop(), 'int', True)
        dest = self.__stack.pop()
        if dest.type != 'string':
            exit(53)
        try:
            if val < 0:
                raise Exception
            self.__stack.push(ord(dest.value[val]), 'int')
        except Exception:
            exit(58)

    # MISC

    def concat(self, dest, str1, str2):
        str1, str1_t = self.__literal_or_variable(str1)
        str2, str2_t = self.__literal_or_variable(str2)
        if str1_t != 'string' or str2_t != 'string':
            exit(53)
        self.__var_manager.insert_value(dest, str1+str2, 'string')

    def strlen(self, dest, string):
        string, string_t = self.__literal_or_variable(string)
        if string_t != 'string':
            exit(53)
        self.__var_manager.insert_value(dest, len(string), 'int')

    def getchar(self, dest, string, pos):
        string, string_t = self.__literal_or_variable(string)
        pos, pos_t = self.__literal_or_variable(pos)
        if string_t != 'string' or pos_t != 'int':
            exit(53)
        try:
            self.__var_manager.insert_value(dest, string[pos], 'string')
            if pos < 0:
                raise IndexError
        except IndexError:
            exit(58)

    def setchar(self, dest, pos, new_symbol):
        dest_string, dest_type = self.__literal_or_variable(dest)
        if dest_string is None:
            exit(56)
        if dest_type != 'string':
            exit(53)
        dest_string = list(dest_string)
        pos, pos_t = self.__literal_or_variable(pos)
        new_symbol, ns_t = self.__literal_or_variable(new_symbol)

        if pos_t != 'int' or ns_t != 'string':
            exit(53)

        new_symbol = self.__escape_sequences(new_symbol)

        if pos_t != 'int' or ns_t != 'string':
            exit(53)

        pos = int(pos)
        if (pos < 0) or (pos >= len(dest_string)) or (len(new_symbol) == 0):
            exit(58)
        dest_string[pos] = new_symbol[0]
        self.__var_manager.insert_value(dest, ''.join(dest_string), 'string')

    # LOGIC FUNCTIONS

    def __pre_logic(self, bool1, bool2):
        bool1, bool1_t = self.__literal_or_variable(bool1)
        bool2, bool2_t = self.__literal_or_variable(bool2)
        if bool1_t != 'bool' or bool2_t != 'bool':
            exit(53)
        return bool1, bool2

    def andfunction(self, dest, bool1, bool2):
        bool1, bool2 = self.__pre_logic(bool1, bool2)
        self.__var_manager.insert_value(dest, bool1 and bool2, 'bool')

    def orfunction(self, dest, bool1, bool2):
        bool1, bool2 = self.__pre_logic(bool1, bool2)
        self.__var_manager.insert_value(dest, bool1 or bool2, 'bool')

    def notfunction(self, dest, bool1):
        bool1, bool1_t = self.__literal_or_variable(bool1)
        if bool1_t != 'bool':
            exit(53)
        self.__var_manager.insert_value(dest, not bool1, 'bool')

    # COMPARISONS

    def __pre_comparison(self, arg1, arg2):
        """Return values and types of given arguments and escape sequences"""
        value1, value1_t = self.__literal_or_variable(arg1)
        value2, value2_t = self.__literal_or_variable(arg2)
        if value1_t == 'string':
            value1 = self.__escape_sequences(value1)
        if value2_t == 'string':
            value2 = self.__escape_sequences(value2)
        return value1, value1_t, value2, value2_t

    def __set_args(self, arg1, arg2):
        value1, value1_t, value2, value2_t = self.__pre_comparison(arg1, arg2)
        if value1_t != 'nil' and value2_t != 'nil':
            if value1_t != value2_t:
                exit(53)
        return value1, value2

    def jumpifeq(self, target, arg1, arg2):
        value1, value2 = self.__set_args(arg1, arg2)
        if value1 == value2:
            self.jump(target)

    def jumpifneq(self, target, arg1, arg2):
        value1, value2 = self.__set_args(arg1, arg2)
        if value1 != value2:
            self.jump(target)

    def eq(self, dest, arg1, arg2):
        value1, value1_t, value2, value2_t = self.__pre_comparison(arg1, arg2)
        if value1_t != 'nil' and value2_t != 'nil':
            if value1_t != value2_t:
                exit(53)
        self.__var_manager.insert_value(dest, value1 == value2, 'bool')

    def gt(self, dest, arg1, arg2):
        value1, value1_t, value2, value2_t = self.__pre_comparison(arg1, arg2)
        if value1_t == 'nil' or value2_t == 'nil' or value1_t != value2_t:
            exit(53)
        self.__var_manager.insert_value(dest, value1 > value2, 'bool')

    def lt(self, dest, arg1, arg2):
        value1, value1_t, value2, value2_t = self.__pre_comparison(arg1, arg2)
        if value1_t == 'nil' or value2_t == 'nil' or value1_t != value2_t:
            exit(53)
        self.__var_manager.insert_value(dest, value1 < value2, 'bool')

    def __jump_comparison_stack(self):
        value2 = self.__stack.pop()
        value1 = self.__stack.pop()
        if value1.type != 'nil' and value2.type != 'nil':
            if value1.type != value2.type:
                exit(53)
        if value1.type == 'string':
            value1 = self.__escape_sequences(value1.value)
        else:
            value1 = value1.value
        if value2.type == 'string':
            value2 = self.__escape_sequences(value2.value)
        else:
            value2 = value2.value
        return value1, value2

    def jumpifeqs(self, target):
        value1, value2 = self.__jump_comparison_stack()
        if value1 == value2:
            self.jump(target)

    def jumpifneqs(self, target):
        value1, value2 = self.__jump_comparison_stack()
        if value1 != value2:
            self.jump(target)

    # STACK BONUS

    def __pre_logic_stack(self):
        """__pre_logic equivalent for stack functions"""
        val2 = self.__stack.pop()
        val1 = self.__stack.pop()
        if val2.type != 'bool' or val1.type != 'bool':
            exit(53)
        return val1.value, val2.value

    def ands(self):
        val1, val2 = self.__pre_logic_stack()
        self.__stack.push(val1 and val2, 'bool')

    def ors(self):
        val1, val2 = self.__pre_logic_stack()
        self.__stack.push(val1 or val2, 'bool')

    def nots(self):
        val = self.__stack.pop()
        if val.type != 'bool':
            exit(53)
        self.__stack.push(not val.value, 'bool')

    def eqs(self):
        value2 = self.__stack.pop()
        value1 = self.__stack.pop()
        if value1.type != 'nil' and value2.type != 'nil':
            if value1.type != value2.type:
                exit(53)
        self.__stack.push(value1.value == value2.value, 'bool')

    def __pre_comparison_stack(self):
        value2 = self.__stack.pop()
        value1 = self.__stack.pop()
        if (value1.type == 'nil' and value2.type == 'nil') or (value1.type != value2.type):
            exit(53)
        return value1.value, value2.value

    def gts(self):
        value1, value2 = self.__pre_comparison_stack()
        self.__stack.push(value1 > value2, 'bool')

    def lts(self):
        value1, value2 = self.__pre_comparison_stack()
        self.__stack.push(value1 < value2, 'bool')

    def clears(self):
        while self.__stack.is_empty() is not True:
            self.__stack.pop()