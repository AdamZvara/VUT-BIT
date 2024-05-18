<?php
/**
 * @brief   Class instruction
 * @date    6.3.2022
 * @author  xzvara01, xzvara01@stud.fit.vutbr.cz
 */

include_once 'error_codes.php';

/*
 * Class representing single instruction, which is being parsed
 */
class Instruction
{
    private static $_order = 0;
    private $_opcode;
    private $_arg_counter = 1; # starts at one because XML attribues 'argX' start at one

    # REGEXes to match arguments
    const VAR_FRAME_R = '(LF|GF|TF)@';
    const VAR_IDENT_R = '[_\-$&%*!?a-zA-Z][_\-$&%*!?a-zA-Z0-9]*';
    const VAR_R = '/^'. self::VAR_FRAME_R . self::VAR_IDENT_R.'$/';
    const TYPE_R = '/^(int|string|bool)$/';
    const BOOL_R = '/^bool@(?i:true|false)$/';
    const INT_R  = '/^int@[-|+]?[0-9]+$/';
    const STRING_R = '/^string@([^\\\]|\\\[0-9][0-9][0-9])*$/';
    const NIL_R = '/^nil@nil$/';

    function __construct($opcode)
    {
        # for every new instruction increase order by one for the whole class
        self::$_order++;
        $this->_opcode = $opcode;
    }

    # start writing instruction element with attributes order and opcode
    public function start_element($xml)
    {
        $xml->startElement('instruction');
        $xml->writeAttribute('order', self::$_order);
        $xml->writeAttribute('opcode', $this->_opcode);
    }

    # end currently opened element
    public function end_element($xml) { $xml->endElement(); }

    # try to match variable regex on argument
    private function var_match($arg)
    {
        if (!preg_match(self::VAR_R, $arg))
            exit(E_OTHER);
    }

    # try to match symbol regex on argument, which consists of bool, string and variable regex check
    private function symb_match($arg)
    {
        if (!preg_match(self::BOOL_R, $arg)   && !preg_match(self::INT_R, $arg) &&
            !preg_match(self::STRING_R, $arg) && !preg_match(self::NIL_R, $arg) &&
            !preg_match(self::VAR_R, $arg))
            exit(E_OTHER);
    }

    # try to match label regex on arument
    private function label_match($arg)
    {
        if (!preg_match('/^' . self::VAR_IDENT_R . '$/', $arg))
            exit(E_OTHER);
    }

    # try to match type regex on argument
    private function type_match($arg)
    {
        if (!preg_match(self::TYPE_R, $arg))
            exit(E_OTHER);
    }

    # return array (type, value) from symbol argument
    # for example: GF@abc       -> ('var', 'GF@abc')
    #              string@abc   -> ('string', 'abc')
    private function get_symbol_type($symbol)
    {
        if (str_starts_with($symbol, 'GF@') ||
            str_starts_with($symbol, 'LF@') ||
            str_starts_with($symbol, 'TF@')) {
                return array('var', $symbol);
        }

        if (str_starts_with($symbol, 'string@')) {
            $type = 'string';
        } elseif (str_starts_with($symbol, 'int@')) {
            $type = 'int';
        } elseif (str_starts_with($symbol, 'bool@')) {
            $type = 'bool';
        } elseif (str_starts_with($symbol, 'nil@')) {
            $type = 'nil';
        }

        return array($type, substr_replace($symbol, '', 0, strlen($type)+1));
    }

    # check validity of arguments and write them in XML format
    public function write_arguments($xml, $arg, $arg_types)
    {
        # check number of arguments
        if (count($arg) != count($arg_types))
            exit(E_OTHER);

        foreach($arg_types as $value => $type) {
            $xml->startElement('arg' . $this->_arg_counter);
            $curr_arg = $arg[$this->_arg_counter-1];
            switch ($type) {
                case 'symb':
                    $this->symb_match($curr_arg);
                    $type_value = $this->get_symbol_type($curr_arg);
                    if ($type_value[0] == 'bool') {
                        $type_value[1] = strtolower($type_value[1]); # lowercase true|false
                    }
                    $xml->writeAttribute('type', $type_value[0]);
                    $xml->text($type_value[1]);
                    break;

                case 'var':
                case 'label':
                case 'type':
                    # call function called (var|label|type)_match to match argument with given regex
                    call_user_func(array($this, $type.'_match'), $curr_arg);
                    $xml->writeAttribute('type', $type);
                    $xml->text($curr_arg);
                    break;

                default:
                    exit(E_OTHER);
            }

            $xml->endElement();
            $this->_arg_counter++;
        }
    }
}
?>