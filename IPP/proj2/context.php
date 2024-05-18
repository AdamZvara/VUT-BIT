<?php
/**
 * @brief   Class used for parsing command line options
 * @date    12.4.2022
 * @author  xzvara01, xzvara01@stud.fit.vutbr.cz
 */

include "error_codes.php";

// Class containing options passed to script
class Context
{
    private $help_message = "USAGE: php test.php [OPT]" . PHP_EOL .
        "Test parser or interpret scripts" . PHP_EOL .
        "OPTIONS:" . PHP_EOL .
            "\t--help\t\t\tprint out this message" . PHP_EOL .
            "\t--recursive\t\ttest directory is searched recursively" . PHP_EOL .
            "\t--noclean\t\tfiles created for testing will be deleted" . PHP_EOL .
            "\t--parse-only\t\trun parser tests" . PHP_EOL .
            "\t--int-only\t\trun interpret tests" . PHP_EOL .
            "\t--directory=path\ttest directory (implicit current directory)" . PHP_EOL .
            "\t--parse-script=file\tparer.php file (implicit file \"parse.php\" in current directory)" . PHP_EOL .
            "\t--int-script=file\tinterpret.py file (implicit file \"interpret.py\" in current directory)" . PHP_EOL .
            "\t--jexampath=path\tpath to jexamxml.jar to compare output xml files" . PHP_EOL;

    private $longopt = array(
        "help",
        "directory:",
        "recursive",
        "parse-script:",
        "int-script:",
        "parse-only",
        "int-only",
        "jexampath:",
        "noclean"
        );

    // implicit values of options
    public $recursive = false;
    public $clean = true;
    public $parse_only = false;
    public $int_only = false;
    public $parse_script = "parse.php";
    public $int_script = "interpret.py";
    public $directory = null;
    public $jexampath = "/pub/courses/ipp/jexamxml/";

    // Constructor parsing arguments with getopt and checking integrity of data
    function __construct($argc)
    {
        $this->directory = dirname(__FILE__); // implicit value of directory
        $options = getopt("", $this->longopt);

        // set option variables
        foreach ($options as $key => $value) {
            switch ($key) {
                case 'help':
                    if ($argc > 2)
                        exit(E_WRONGARG);
                    echo $this->help_message . PHP_EOL;
                    exit(0);
                    break;

                case 'recursive':
                    $this->recursive = true;
                    break;

                case 'noclean':
                    $this->clean = false;
                    break;

                case 'parse-only':
                    if (key_exists("int-only", $options) || key_exists("int-script", $options))
                        exit(E_WRONGARG);
                    $this->parse_only = true;
                    break;

                case 'int-only':
                    if (key_exists("parse-only", $options) || key_exists("parse-script", $options))
                        exit(E_WRONGARG);
                    $this->int_only = true;
                    break;

                case 'directory':
                    if (!is_readable($value))
                        exit(E_NOFILE);
                    $this->directory = $value;
                    break;

                case 'parse-script':
                    if (!is_readable($value))
                        exit(E_NOFILE);
                    $this->parse_script = $value;
                    break;

                case 'int-script':
                    if (!is_readable($value))
                        exit(E_NOFILE);
                    $this->int_script = $value;
                    break;

                case 'jexampath':
                    if (!is_readable($value))
                        exit(E_NOFILE);
                    //append '/' to path if does not exist
                    if (!str_ends_with($value, '/'))
                        $value = $value . '/';
                    $this->jexampath = $value;
                    break;

                default:
                    exit(E_WRONGARG);
                    break;
            } // switch
        } // foreach
    } // constructor
} // class

?>