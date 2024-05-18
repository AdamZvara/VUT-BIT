<?php
/**
 * @brief   Main file for parsing IPPcode22 language into XML representation
 * @date    6.3.2022
 * @author  xzvara01, xzvara01@stud.fit.vutbr.cz
 */

include_once 'classes/error_codes.php';      # error codes declarations
include_once 'classes/parse_arguments.php';  # class for parsing arguments
include_once 'classes/instruction.php';      # class for instruction parsing
include_once 'classes/statistics.php';       # class for bonus extension

ini_set('display_errors', 'stderr');

$bonus_handler = new Bonus_arg_handler($argc, $argv);
$bonus_handler->parse_args();

# create xml file using XMLWriter class, set indent and encoding
$xml = new XMLWriter();
$xml->openMemory();
$xml->setIndent(1);
$xml->setIndentString('  ');
$xml->startDocument('1.0', 'UTF-8');

# find comment in line, remove it and get rid of whitespaces
function strip_comment($line)
{
    global $stat;
    $comment_start = strpos($line, '#');
    if ($comment_start !== false) {
        $line = substr_replace($line, '', $comment_start);
        $stat->add_comment();
    }

    return trim($line);
}

$stat = new Statistics();

# look for header (.IPPCODE22)
while ($line = fgets(STDIN)) {
    $line = strip_comment($line);
    if (strlen($line) == 0)
        continue;

    # separate loaded line into array based on whitespaces
    $tokens = preg_split('/\s+/', $line);
    if (count($tokens) != 1) {
        exit(E_HEADER);
    }
    $instr = strtoupper($tokens[0]);

    # start XML document
    if ($instr === '.IPPCODE22') {
        $xml->startElement('program');
        $xml->writeAttribute('language', 'IPPcode22');
        break;
    } else {
        exit(E_HEADER);
    }
}

# continue parsing instructions
while ($line = fgets(STDIN)) {
    $line = strip_comment($line);
    if (strlen($line) == 0)
        continue;

    $stat->add_loc();
    $tokens = preg_split('/\s+/', $line);

    # extract instruction and arguments from line
    $instr = strtoupper($tokens[0]);
    $arguments = array_slice($tokens, 1);

    $instr_obj = new Instruction($instr);
    $instr_obj->start_element($xml);

    switch ($instr) {
        # type: INSTR
        case 'CREATEFRAME':
        case 'PUSHFRAME'  :
        case 'POPFRAME'   :
        case 'RETURN'     :
        case 'BREAK'      :
            $arg_types = array();
            break;

        # INSTR var
        case 'DEFVAR':
        case 'POPS'  :
            $arg_types = array('var');
            break;

        # INSTR symb
        case 'DPRINT':
        case 'EXIT'  :
        case 'WRITE' :
        case 'PUSHS' :
            $arg_types = array('symb');
            break;

        # INSTR label
        case 'LABEL':
        case 'JUMP' :
        case 'CALL' :
            $arg_types = array('label');
            break;

        # INSTR var type
        case 'READ':
            $arg_types = array('var', 'type');
            break;

        # INSTR var symbol
        case 'MOVE'    :
        case 'INT2CHAR':
        case 'STRLEN'  :
        case 'TYPE'    :
        case 'NOT'     :
            $arg_types = array('var', 'symb');
            break;

        # INSTR label symb symb
        case 'JUMPIFEQ' :
        case 'JUMPIFNEQ':
            $arg_types = array('label', 'symb', 'symb');
            break;

        # INSTR var symb symb
        case 'ADD'    :
        case 'SUB'    :
        case 'MUL'    :
        case 'IDIV'    :
        case 'LT'     :
        case 'GT'     :
        case 'EQ'     :
        case 'AND'    :
        case 'OR'     :
        case 'STRI2INT':
        case 'CONCAT' :
        case 'GETCHAR':
        case 'SETCHAR':
            $arg_types = array('var', 'symb', 'symb');
            break;

        default:
            exit(E_OPCODE);
    }

    $instr_obj->write_arguments($xml, $arguments, $arg_types);
    $instr_obj->end_element($xml);

    # label into statistics
    $jumpinstr = array('CALL' => 1, 'RETURN' => 1, 'JUMP' => 1, 'JUMPIFEQ' => 1, 'JUMPIFNEQ' => 1);
    if ($instr == 'LABEL') {
        $stat->add_label($arguments[0], $stat->get_loc());
    } elseif (array_key_exists($instr, $jumpinstr)) {
        $target = count($arguments) ? $arguments[0] : 0;
        $stat->add_jump($target, $stat->get_loc());
    }
}

$stat->calculate_fbjumps();
$bonus_handler->write($stat);

$xml->endElement(); # Root program element
$xml->endDocument();
echo $xml->outputMemory();
?>