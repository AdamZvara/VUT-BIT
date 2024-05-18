<?php
/**
 * @brief   Testing modules
 * @date    12.4.2022
 * @author  xzvara01, xzvara01@stud.fit.vutbr.cz
 */

// Class encapsulating SingleTester and MultiTester
abstract class Tester
{
    static protected $clean;
    static protected $python = "python3";
    static protected $php = "php8.1";
    static protected $parse_script;
    static protected $int_script;
    static protected $jexampath;

    // Set paths to scripts and clean option
    static function set_options($parse_script, $int_script, $jexampath, $clean)
    {
        self::$parse_script = $parse_script;
        self::$int_script = $int_script;
        self::$jexampath = $jexampath;
        self::$clean = $clean;
    }

    // Run single test
    abstract function test($filename);
}

// Class providing test method for ParseTester and InterpretTester
abstract class SingleTester extends Tester
{
    // Execute script for parser or interpret
    abstract protected function process_file($srcfile, $tmpfile, $infile);

    // Compare results using diff or jexamxml
    abstract protected function compare_results($outfile, $tmpFile);

    function test($filename)
    {
        $fileBaseName = substr($filename, 0, -4);

        $rcfile  = $fileBaseName . '.rc';
        $srcfile = $fileBaseName . '.src' ;
        $tmpfile = $fileBaseName . '.tmp';
        $infile  = $fileBaseName . '.in';
        $outfile = $fileBaseName . '.out';

        $rc_expected = file_get_contents($rcfile);
        $rc_actual = $this->process_file($srcfile, $tmpfile, $infile);

        if ($rc_expected != 0) {
            if ($rc_expected == $rc_actual) {
                $retval = true;
            } else {
                $retval = false;
            }
        } else {
            $rc = $this->compare_results($outfile, $tmpfile);
            if ($rc == 0) {
                $retval = true;
            } else {
                $retval = false;
            }
        }

        if (parent::$clean == true) {
            unlink($tmpfile);
        }

        return $retval;
    }
}

class ParseTester extends SingleTester
{
    function process_file($srcfile, $tmpfile, $infile)
    {
        $result = null;
        $output = null;
        exec(parent::$php . ' '.parent::$parse_script.' > '. $tmpfile.' < '.$srcfile, $output, $result);
        return $result;
    }

    function compare_results($outfile, $tmpFile)
    {
        $result = null;
        $output = null;
        exec('java -jar '.parent::$jexampath.'jexamxml.jar'.' '.$tmpFile.' '.$outfile.' delta.xml '.parent::$jexampath.'options',
            $output,
            $result
        );
        return $result;
    }
}

class InterpretTester extends SingleTester
{
    function process_file($srcfile, $tmpfile, $infile)
    {
        $result = null;
        $output = null;
        exec(parent::$python . ' '.parent::$int_script.' --source='.$srcfile.' --input='.$infile.' > '.$tmpfile . ' 2> /dev/null',
            $output,
            $result
        );
        return $result;
    }

    function compare_results($outfile, $tmpFile)
    {
        $result = null;
        $output = null;
        exec('diff '.$tmpFile.' '.$outfile, $output, $result);
        return $result;
    }
}

// Class to test parser and interpret combined
class MultiTester extends Tester
{
    // Parse file using parser script
    function parse_file($srcfile, $tmpfile, $infile)
    {
        $result = null;
        $output = null;
        exec(parent::$php . ' '.parent::$parse_script.' > '. $tmpfile.' < '.$srcfile, $output, $result);
        return $result;
    }

    // Interpret file produced by parse_file function
    function interpret_file($srcfile, $tmpfile, $infile) {
        $result = null;
        $output = null;
        exec(parent::$python . ' '.parent::$int_script.' --source='.$srcfile.' --input='.$infile.' > '.$tmpfile . ' 2> /dev/null',
            $output,
            $result
        );
        return $result;
    }

    // Compare results using diff
    function compare_results($outfile, $tmpFile)
    {
        $result = null;
        $output = null;
        exec('diff '.$tmpFile.' '.$outfile, $output, $result);
        return $result;
    }

    // Test implementation for consecutive parser and interpret tests
    function test($filename)
    {
        $fileBaseName = substr($filename, 0, -4);

        $rcFile  = $fileBaseName . '.rc';
        $srcfile = $fileBaseName . '.src' ;
        $infile  = $fileBaseName . '.in';
        $intfile = $fileBaseName . '.int';
        $outfile = $fileBaseName . '.out';
        $parsefile = $fileBaseName . '.parse';

        $rc_expected = file_get_contents($rcFile);

        // parsing file ...
        $rc_actual = $this->parse_file($srcfile, $parsefile, $infile);

        if ($rc_actual != 0) {
            // parsing failed
            if ($rc_expected == $rc_actual) {
                // return codes match, it was supposed to fail!
                $retval = true;
            } else {
                // return codes do not match, oh well ...
                $retval = false;
            }
        } else {
            // parser return code was okay, go ahead and interpret file
            $rc = $this->interpret_file($parsefile, $intfile, $infile);
            if ($rc != $rc_expected) {
                // this is the final return code so if it is not equal to expected test failed
                $retval = false;
            } else {
                if ($rc == 0) {
                    // interpret return code was okay, need to compare results of interpretation!
                    if ($this->compare_results($outfile, $intfile) == 0) {
                        $retval = true;
                    } else {
                        $retval = false;
                    }
                } else {
                    // interpret return code was not okay, check if it is equal to expected return code
                    if ($rc != $rc_expected) {
                        // it is not :(
                        $retval = false;
                    } else {
                        // it is, at last the test passed
                        $retval = true;
                    }
                }
            }
        }

        if (parent::$clean == true) {
            unlink($intfile);
            unlink($parsefile);
        }

        return $retval;
    }
}
?>