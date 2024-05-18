"""
Author  : Adam Zvara, xzvara01@stud.fit.vutbr.cz
Date    : 12.4.2022
Brief   : Stack used for storing constants
"""

class Stack_item:
    """Stack item containing value and type"""
    def __init__(self, value, type):
        self.__value = value
        self.__type = type

    @property
    def value(self):
        return self.__value

    @property
    def type(self):
        return self.__type

class Stack:
    """Class used for for stack instructions"""
    def __init__(self):
        self.__stack = list()

    def pop(self):
        if len(self.__stack) == 0:
            exit(56)
        return self.__stack.pop()

    def push(self, value, type='int'):
        self.__stack.append(Stack_item(value, type))

    def is_empty(self):
        return len(self.__stack) == 0