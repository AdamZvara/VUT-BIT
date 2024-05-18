"""
Author  : Adam Zvara, xzvara01@stud.fit.vutbr.cz
Date    : 12.4.2022
Brief   : Classes used for variable manipulation in interpret
"""

from instruction import Argument

class Variable():
    """Class representing variable containing frame, name, value and type"""

    def __init__(self, whole_name : str):
        self.__frame = whole_name[:2]   # GF/TF/LF without @
        self.__name = whole_name[3:]    # actual name of variable
        self.__value = None
        self.__type = ''

    @property
    def frame(self):
        return self.__frame

    @frame.setter
    def frame(self, value):
        self.__frame = value

    @property
    def name(self):
        return self.__name

    @property
    def value(self):
        return self.__value

    @value.setter
    def value(self, val):
        self.__value = val

    @property
    def type(self):
        return self.__type

    @type.setter
    def type(self, value):
        self.__type = value

class Variable_manager():
    """Manager class to work with variables in interpreted program"""

    def __init__(self):
        self.__frames = dict({'GF':list(), 'TF':list(), 'LF':list()})
        self.__local_frames_stack = list()
        self.__temp_frame_active = False
        self.__local_frame_active = False

    def add(self, variable : Argument):
        """Create new variable and store it in frame given by its name"""
        var = Variable(variable.value)
        try:
            if not self.find(variable.value):
                if var.frame == 'TF' and not self.__temp_frame_active:
                    exit(55)
                self.__frames[var.frame].append(var)
            else:
                exit(52)
        except KeyError:
            exit(55)

    def find(self, name : str):
        """Return variable with given name or None if variable does not exist"""
        frame = name[:2]
        if (frame == 'TF' and not self.__temp_frame_active) or (frame == 'LF' and self.__local_frame_active == False):
            exit(55)
        for var in self.__frames[frame]:
            if var.name == name[3:]:
                return var
        return None

    def insert_value(self, dest : Argument, value : any, type : str) -> None:
        """Insert value into variable if variable exists"""
        if var := self.find(dest.value):
            var.value = value
            var.type = type
        else:
            exit(54)

    def get_type(self, var_name : str):
        """Get type of variable if variable exists"""
        if var := self.find(var_name):
            return var.type
        else:
            exit(54)

    def get_value(self, var_name : str):
        """Get value of variable if variable exists"""
        if var := self.find(var_name):
            return var.value
        else:
            exit(54)

    def print(self, arg : Argument):
        """Return variable valu to be printed"""
        if not (var := self.find(arg.value)):
            exit(54)
        if var.type == 'string':
            result = str(var.value)
        elif var.type == 'float':
            result = float.hex(var.value)
        elif var.type == 'nil':
            result = ''
        elif var.type == 'bool':
            if (var.value is True):
                result='true'
            else:
                result='false'
        else:
            if var.value is None:
                exit(56)
            result = var.value
        return result, var.type

    def TF_create(self):
        """Empty temporary frame if it exists and create new one"""
        if self.__temp_frame_active:
            self.__frames.update({'TF':list()})
        self.__temp_frame_active = True

    def TF_push(self):
        """Promote created temporary frame to local frame, save previous local frame in local_frames_stack"""
        if not self.__temp_frame_active:
            exit(55)
        if len(self.__frames['LF']) != 0:
            self.__local_frames_stack.append(self.__frames['LF'])
        self.__frames.update({'LF':self.__frames['TF']})
        for arg in self.__frames['LF']:
            arg.frame = 'LF'
        self.__frames.update({'TF':list()})
        self.__temp_frame_active = False
        self.__local_frame_active = True

    def TF_pop(self):
        """
        Move active local frame with its variables into temporary frame,
        pop frame from local frames stack and place its variables in local frame
        """
        if self.__local_frame_active == False:
            exit(55)
        self.__frames.update({'TF': self.__frames['LF']})
        if len(self.__local_frames_stack) > 0:
            LF_stack_top = self.__local_frames_stack.pop()
        else:
            LF_stack_top = list()
            self.__local_frame_active = False
        self.__frames.update({'LF': LF_stack_top})
        for arg in self.__frames['TF']:
            arg.frame = 'TF'
        self.__temp_frame_active = True