"""
Author  : Adam Zvara, xzvara01@stud.fit.vutbr.cz
Date    : 12.4.2022
Brief   : Instruction class containing arguments
"""

class Argument:
    """Argument class containing argument type, value and order"""

    def __init__(self, argument):
        """Create argument object from XML Element"""
        self.type = argument.attrib['type']
        if self.type == 'int':
            self.value = self.__to_int__(argument.text)
        elif self.type == 'float':
            self.value = self.__to_float__(argument.text)
        elif self.type == 'bool':
            self.value = self.__to_bool__(argument.text)
        else:
            if argument.text is None:
                self.value = ''
            else:
                self.value = argument.text
        self.order = int(argument.tag[3:]) # for ordering arguments in instruction

    def __to_int__(self, value):
        """Return argument value as integer"""
        try:
            return int(value)
        except ValueError:
            exit(32)

    def __to_float__(self, value):
        """Return argument value as float"""
        try:
            return float.fromhex(value)
        except ValueError:
            exit(32)

    def __to_bool__(self, value):
        """Return argument value as boolean"""
        if value == 'true':
            return True
        else:
            return False

    def __eq__(self, other):
        """Compare arguments"""
        if self.type != 'nil' and other.type != 'nil':
            if self.type == other.type and self.value == other.value:
                return True
            else:
                return False
        else:
            if self.type == 'nil' and other.type == 'nil':
                return True
            else:
                return False

class Instruction:
    """
    Class representing instruction which contains opcode, order, argument list and argument counter
    Arguments can be accessed with [] operators
    """

    __instructions = {('CREATEFRAME', 'PUSHFRAME', 'POPFRAME', 'RETURN', 'BREAK') : 0,
                      ('DEFVAR', 'LABEL', 'WRITE', 'CALL', 'PUSHS', 'POPS', 'JUMP',
                       'EXIT', 'DPRINT') : 1,
                      ('MOVE', 'READ', 'INT2CHAR', 'STRLEN', 'TYPE', 'NOT') : 2,
                      ('ADD', 'SUB', 'MUL', 'IDIV', 'LT', 'GT', 'EQ', 'AND',
                       'OR', 'STR2INT', 'CONCAT', 'GETCHAR', 'SETCHAR',
                       'JUMPIFEQ', 'JUMPIFNEQ') : 3}

    def __init__(self, opcode : str, order : str):
        try:
            order = int(order)
            if order < 1:
                exit(32)
        except ValueError:
            exit(32)
        self.__order = order
        self.__opcode = opcode.upper()
        self.__args = list()
        self.__arg_count = 0

    @property
    def order(self) -> int:
        return self.__order

    @property
    def opcode(self):
        return self.__opcode

    def __getitem__(self, pos) -> Argument:
        """Accessing arguments with [] operators"""
        try:
            return self.__args[pos]
        except KeyError:
            return None

    def newarg(self, argument):
        """Create new argument"""
        self.__args.append(Argument(argument))
        self.__arg_count += 1

    def sort(self):
        """Sort arguments based on order"""
        self.__args.sort(key=lambda x: x.order)

    def arg_valid_amount(self):
        """Check if instruction has correct amount of arguments"""
        for opcodes in self.__instructions.items():
            if self.__opcode in opcodes[0]:
                if self.__arg_count != opcodes[1]:
                    exit(32)

    def arg_valid(self):
        """Validate that sorted arguments begin from number 1 up to arg_count - 1"""
        for i in range(self.__arg_count):
            if self.__args[i].order != i+1:
                exit(32)

    @classmethod
    def max_args(cls):
        """Return maximum amount of instruction arguments"""
        return max(cls.__instructions.values())